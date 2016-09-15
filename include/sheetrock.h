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
#include <memory>
#include <string>
#include <vector>

#include <daw/daw_parser_helper.h>
#include <daw/daw_optional.h>

namespace daw {
	namespace parser {
		namespace sheetrock {
			namespace ast {
				struct ast_item {
					virtual ~ast_item( ) = default;
				};	// ast_item

				struct block: public ast_item { };

				struct label: public ast_item {
					std::string value;
					virtual ~label( ) = default;
				};	// label

				struct expression: public ast_item { };

				struct boolean: public expression { };
				struct number: public expression { };
				struct string: public expression { };
				struct variant: public expression { };
				struct cell: public ast_item { };

				struct range: public expression {
					std::unique_ptr<cell> first;
					std::unique_ptr<cell> last;
					virtual ~range( ) = default;
				};	// range

				struct declaration: public expression {
					std::unique_ptr<label> name;
					std::unique_ptr<expression> value;
				};	// declaration

				struct unary_operator: public expression {
					std::unique_ptr<expression> lhs;
					std::unique_ptr<expression> rhs;
					virtual ~unary_operator( ) = default;
				};	// unary_operator

				struct binary_operator: public expression {
					std::unique_ptr<expression> rhs;
					virtual ~binary_operator( ) = default;
				};	// binary_operator

				struct function: public expression {
					std::unique_ptr<label> name;	
					std::unique_ptr<block> blk;
					virtual ~function( ) = default;
				};	// function
			}	// namespace ast	

			template<typename T>
				bool is_reserved( T value ) {
					return is_a( value, '"', '+', '-', '*', '/', '%', '^', '#', '&', '~',
							'|', '<', '>', '/', '=', '!', '=', '(', ')', '{', '}',
							'[', ']', ':', ';', ',', '.', '\'' );
				}

			template<typename T>
				bool is_valid_name_begin_char( T value ) {
					return  !is_number( value ) && !is_reserved( value );
				}

			template<typename T>
				bool is_valid_name_char( T value ) {
					return !is_reserved( value );
				}

			template<typename T>
				bool is_quote( T value ) {
					return is_a( value, '"', '\'' );
				}

			template<typename T>
				bool is_escape( T value ) {
					return is_a( value, '\\' );
				}

			template<typename ForwardIterator>
				auto parse_bool( ForwardIterator first, ForwardIterator last ) {


				}

			template<typename ForwardIterator>
				auto parse_name( ForwardIterator first, ForwardIterator const last ) {
					auto result = left_trim( first, last );
					result = from_to( result.first, result.last, &is_valid_name_begin_char<decltype(*first)>, &is_valid_name_char<decltype(*first)> );
					if( result.empty( ) || result.first != first ) {
						throw ParserException{ };
					}
					return result;
				}

			template<typename ForwardIterator>
				daw::optional<ast::expression> parse_expression( ForwardIterator first, ForwardIterator const last ) {
					auto result = parse_binary_operand( first, last );
					if( result ) {
						return result;
					}
				}


			template<typename ForwardIterator>
				std::vector<ast::ast_item> parse( ForwardIterator first, ForwardIterator const last ) {

				}
		}	// namespace sheetrock
	}    // namespace parser
}    // namespace daw

