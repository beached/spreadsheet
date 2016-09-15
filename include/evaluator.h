// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/utility/string_ref.hpp>

#include <daw/daw_parser_helper.h>

#include "impl_cell_value.h"

namespace daw {
	namespace spreadsheet {
		using result_t = std::function<daw::variant_t<daw::spreadsheet::impl::cell_value::string_t, daw::spreadsheet::impl::cell_value::number_t, daw::spreadsheet::impl::cell_value::timestamp_t, daw::spreadsheet::impl::cell_value::duration_t, daw::spreadsheet::impl::cell_value::bool_t>( )>;

		/// @brief Only use for values in lower ASCII range of UTF-8.
		/// @param first1 Iterattor for the start of first string
		/// @param last1 Iterattor for the end of first string
		/// @param first2 Iterattor for the start of second string
		/// @param last2 Iterattor for the end of second string
		/// @return true if both strings are equal without accounting for case, false otherwise
		template<typename ForwardIterator1, typename ForwardIterator2>
			bool is_equal_nc( ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2 ) {
				static auto const equal_nc = []( auto lhs, auto rhs ) {
					lhs = lhs | 0b00100000;	// makes
					rhs = rhs | 0b00100000;
					return lhs == rhs;
				};
				return std::equal( first1, last1, first2, last2, equal_nc );
			};

		template<typename ForwardIterator>
			auto eval_string( ForwardIterator first, ForwardIterator last ) {
				return result_t{ daw::parser::make_find_result( first, last ).as_string( ) };
			}

		template<typename ForwardIterator>
			auto eval_bool( ForwardIterator first, ForwardIterator last ) {
				auto rng = daw::parser::trim_right( first, last );

				static const std::string t = "true";
				bool result = is_equal_nc( rng.first, rng.last, t.begin( ), t.last( ) );
				if( !result ) {
					static const std::string f = "false";
					result = is_equal_nc( rng.first, rng.last, f.begin( ), f.last( ) );
					if( !result ) {
						// not a bool
						throw daw::parser::ParserError{ };
					}
				}
				return result_t{ result };
			}


		template<typename ForwardIterator>
			auto eval_timestamp( ForwardIterator first, ForwardIterator last ) {
				auto rng = daw::parser::trim( fist, last );
				try {
					return result_t{ boost::posix_time::from_iso_string( rng.as_string( ) ) };
				} catch( std::exception const & ) {
					throw daw::parser::ParserException{ };
				}
			}

		template<typename ForwardIterator>
			auto eval_number( ForwardIterator first, ForwardIterator last ) {
				try {
					boost::string_ref val{ first, last };
					cell_value::number_t result{ val.to_string( ) };
					return result_t{ std::move( result ) };
				} catch( std::exception const & ) {
					throw daw::parser::ParserException{ };
				}
			}

		template<typename ForwardIterator>
			auto eval_number_timestamp_or_duration( ForwardIterator first, ForwardIterator last ) {
				try {
					return eval_timestamp( first, last );
				} catch( daw::parser::ParserException const & ) { }
				try {
					return eval_duration( first, last );
				} catch( daw::parser::ParserException const & ) { }
				try {
					return eval_number( first, last );
				} catch( daw::parser::ParserException const & ) { }
				return eval_string( first, last );
			}

		template<typename ForwardIterator>
			auto eval_func( ForwardIterator first, ForwardIterator last ) {
				auto rng = daw::parser::until( first, last, &daw::parser::not_space<decltype(first)> );
				switch( *rng.first ) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': return eval_number_or_timestamp( rng.first, rng.last );
				case '+':
				case '-':
				case '*':
				case '/':
				case '^':
				case '%':
				case '(':
				case ')':
				case '"':
				}
			}

		template<typename ForwardIterator>
			auto evaluator( ForwardIterator first, ForwardIterator last ) {
				auto rng = daw::parser::trim_left( first, last );
				try {
					switch( *rng.first ) {
					case '=': return eval_func( std::next( rng.first ), rng.last );
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': return eval_number_or_timestamp( rng.first, rng.last );
					case 't':
					case 'T':
					case 'f':
					case 'F': return eval_bool( rng.first, rng.last );
					}
				} catch( daw::parser::ParserError const & ) { }
				return eval_string( rng.first, rng.last );
			}

	}    // namespace spreadsheet
}    // namespace daw

