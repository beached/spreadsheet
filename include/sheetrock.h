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
#include <daw/daw_optional_poly.h>

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
					daw::optional_poly<cell> first;
					daw::optional_poly<cell> last;
					virtual ~range( ) = default;
				};	// range

				struct declaration: public expression {
					daw::optional_poly<label> name;
					daw::optional_poly<expression> value;
				};	// declaration

				struct unary_operator: public expression {
					daw::optional_poly<expression> rhs;
					virtual ~unary_operator( ) = default;
				};	// unary_operator

				struct binary_operator: public expression {
					daw::optional_poly<expression> lhs;
					daw::optional_poly<expression> rhs;
					virtual ~binary_operator( ) = default;
				protected:
					template<typename ForwardIterator, typename T>
					binary_operator( ForwardIterator first, ForwardIterator last, T op ):
							expression{ },
							lhs{ },
							rhs{ } {

						auto rng = daw::parser::split_on( first, last, op );
						daw::exception::daw_throw_on_false<daw::parser::ParserException>( rng.size( ) == 2, "Expected binary expression" );
						auto l = parse_expression( rng[0].begin( ), rng[0].end( ) );
						auto r = parse_expression( rng[1].begin( ), rng[1].end( ) );
						daw::exception::daw_throw_on_false<daw::parser::ParserException>( l, "Invalid expression on lhs of +" );
						daw::exception::daw_throw_on_false<daw::parser::ParserException>( r, "Invalid expression on rhs of +" );
						lhs = *l;
						rhs = *r;
					}

					virtual int32_t op_char( ) const = 0;
				};	// binary_operator
				namespace impl {
					template<char op>
					struct bin_op_derived: public binary_operator {
						template<typename ForwardIterator>
						bin_op_derived( ForwardIterator first, ForwardIterator last ):
								binary_operator{ first, last, op } { }

						template<typename ForwardIterator>
							static daw::optional_poly<bin_op_derived> create( ForwardIterator first, ForwardIterator last ) {
								daw::optional_poly<bin_op_derived> result;
								result = bin_op_derived( first, last );
								return result;
							}

						int32_t op_char( ) const override { return static_cast<int32_t>(op); }
					};    // bin_op_derived
				}

				using bin_op_addition = impl::bin_op_derived<'+'>;
				using bin_op_subtraction = impl::bin_op_derived<'-'>;
				using bin_op_multiplication = impl::bin_op_derived<'*'>;
				using bin_op_division = impl::bin_op_derived<'/'>;
				using bin_op_modulus = impl::bin_op_derived<'%'>;
				using bin_op_power = impl::bin_op_derived<'^'>;
				using bin_op_and = impl::bin_op_derived<'&'>;
				using bin_op_or = impl::bin_op_derived<'|'>;
				using bin_op_less = impl::bin_op_derived<'<'>;
				using bin_op_greater = impl::bin_op_derived<'>'>;
				using bin_op_equal = impl::bin_op_derived<'='>;


				template<typename ForwardIterator, typename T>
				daw::optional_poly<binary_operator> get_binary_operator( ForwardIterator first, ForwardIterator last, T op ) {
					switch( op ) {
					case '+': return bin_op_addition::create<ForwardIterator>( first, last );
					case '-': return bin_op_subtraction::create<ForwardIterator>( first, last );
					case '*': return bin_op_multiplication::create<ForwardIterator>( first, last );
					case '/': return bin_op_division::create<ForwardIterator>( first, last );
					case '%': return bin_op_modulus::create<ForwardIterator>( first, last );
					case '^': return bin_op_power::create<ForwardIterator>( first, last );
					case '&': return bin_op_and::create<ForwardIterator>( first, last );
					case '|': return bin_op_or::create<ForwardIterator>( first, last );
					case '<': return bin_op_less::create<ForwardIterator>( first, last );
					case '>': return bin_op_greater::create<ForwardIterator>( first, last );
					case '=': return bin_op_equal::create<ForwardIterator>( first, last );
					default:
						return daw::optional_poly<binary_operator>{ };
					}
				}

				template<typename T>
				bool is_binary_operator( T value ) {
					return is_a( value, '+', '-', '*', '/', '%', '^', '&', '|', '<', '>', '=' );
				}

				template<typename ForwardIterator>
				daw::optional_poly<expression> create_binary_operation( ForwardIterator first, ForwardIterator last ) {
					using value_t = daw::traits::root_type_t<decltype(*first)>;
					auto lhs = daw::parser::until( first, last, &is_binary_operator<value_t> );
					daw::exception::daw_throw_on_false<daw::parser::ParserException>( lhs, "Could not find binary operator" );
					return get_binary_operator<ForwardIterator>( first, last, *lhs.last );
				}

				struct function: public expression {
					daw::optional_poly<label> name;
					daw::optional_poly<block> blk;
					virtual ~function( ) = default;
				};	// function
			}	// namespace ast	

			template<typename T>
				bool is_reserved( T value ) {
					return is_binary_operator( value ) || is_a( value, '"', '#', '~',
							'!', '(', ')', '{', '}', '[', ']', ':', ';', ',', '.', '\'' );
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
				daw::optional_poly<ast::expression> parse_expression( ForwardIterator first, ForwardIterator const last ) {
					auto result = parse_binary_operand( first, last );
					if( result ) {
						return result;
					}
				}

			template<typename ForwardIterator>
			std::vector<ast::ast_item> parse( ForwardIterator first, ForwardIterator const last ) {
				std::vector<ast::ast_item> result;

				return result;
			}
		}	// namespace sheetrock
	}    // namespace parser
}    // namespace daw

