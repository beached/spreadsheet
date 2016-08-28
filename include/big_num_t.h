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

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <istream>
#include <ostream>
#include <string>

namespace daw {
	namespace spreadsheet {
		namespace number {
			struct big_num_t {
				using value_t = boost::multiprecision::cpp_dec_float_50;
				value_t value;

				template<typename... Args>
					big_num_t( Args && ... args ):
							value{ std::forward<Args>( args )... } { }

				~big_num_t( );

				big_num_t( big_num_t const & ) = default;

				big_num_t( big_num_t && ) = default;

				big_num_t & operator=( big_num_t const & ) = default;

				big_num_t & operator=( big_num_t && ) = default;

				void swap( big_num_t & rhs ) noexcept;
				operator value_t&( );
				operator value_t const &( ) const;
				big_num_t & operator+=( big_num_t const & rhs );
				big_num_t & operator-=( big_num_t const & rhs );
				big_num_t & operator*=( big_num_t const & rhs );
				big_num_t & operator/=( big_num_t const & rhs );
			};    // big_num_t

			void swap( big_num_t & lhs, big_num_t & rhs ) noexcept;
			big_num_t operator+( big_num_t lhs, big_num_t const & rhs );
			big_num_t operator-( big_num_t lhs, big_num_t const & rhs );
			big_num_t operator*( big_num_t lhs, big_num_t const & rhs );
			big_num_t operator/( big_num_t lhs, big_num_t const & rhs );
			bool operator==( big_num_t const & lhs, big_num_t const & rhs );
			bool operator!=( big_num_t const & lhs, big_num_t const & rhs );
			bool operator<( big_num_t const & lhs, big_num_t const & rhs );
			bool operator>( big_num_t const & lhs, big_num_t const & rhs );
			bool operator<=( big_num_t const & lhs, big_num_t const & rhs );
			bool operator>=( big_num_t const & lhs, big_num_t const & rhs );
			std::ostream & operator<<( std::ostream & os, big_num_t const & rhs );
			std::istream & operator>>( std::istream & is, big_num_t & rhs );
			std::string to_string( big_num_t const & value );
		}	// namespace number
	}	// namespace spreadsheet
}	// namespace daw

