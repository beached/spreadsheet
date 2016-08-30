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

#include <daw/daw_parser_helper.h>
#include <unordered_map>

#include "impl_cell_value.h"


namespace daw {
	namespace spreadsheet {
		namespace impl {
			void cell_value::link_values( ) {
				link_streamable( "value_type", m_value_type );
				link_string( "string_value", m_string_value );
			}

			cell_value::cell_value( daw::nodepp::base::EventEmitter emitter, cell_value::expected_value_t value_type, std::string string_value, cell_value::eval_func_t eval_func ):
					table_item{ std::move( emitter ) },
					m_value_type{ value_type },
					m_string_value{ std::move( string_value ) },
					m_evaluated{ std::move( eval_func ) } {

				link_values( );
			}

			cell_value::cell_value( ):
					cell_value{ daw::nodepp::base::create_event_emitter( ) } { }

			cell_value::cell_value( cell_value const & other ):
					table_item{ other },
					m_value_type{ other.m_value_type },
					m_string_value{ other.m_string_value },
					m_evaluated{ nullptr } {

				link_values( );
			}

			cell_value::cell_value( cell_value && other ):
					table_item{ std::move( other ) },
					m_value_type{ std::exchange( other.m_value_type, expected_value_t::General ) },
					m_string_value{ std::exchange( other.m_string_value, "" ) },
					m_evaluated{ std::exchange( other.m_evaluated, nullptr ) } {

				link_values( );
			}

			cell_value & cell_value::operator=( cell_value const & rhs ) {
				if( this != &rhs ) {
					cell_value tmp{ rhs };
					using std::swap;
					swap( *this, tmp );
				}
				return *this;
			}

			cell_value & cell_value::operator=( cell_value && rhs ) {
				if( this != &rhs ) {
					cell_value tmp{ std::move( rhs ) };
					using std::swap;
					swap( *this, tmp );
				}
				return *this;
			}

			void swap( cell_value & lhs, cell_value & rhs ) noexcept {
				using std::swap;
				swap( static_cast<table_item &>(lhs), static_cast<table_item &>(rhs) );
				swap( lhs.m_value_type, rhs.m_value_type );
				lhs.m_string_value.swap( rhs.m_string_value );
				lhs.m_evaluated.swap( rhs.m_evaluated );
			}

			cell_value::~cell_value( ) {
				try {
					std::string event_id = id( ) + "_data_updated";
					this->emitter( )->remove_all_listeners( event_id );
				} catch( ... ) {
					// I don't think I should care
				}
			}

			void cell_value::on_data_updated( std::function<void( std::string )> cb ) {
				std::string message = id( ) + "_data_updated";
				this->emitter( )->on( message, cb );
			}

			void cell_value::emit_data_updated( ) {
				std::string message = id( ) + "_data_updated";
				this->emitter( )->emit( message );
			}

			template<typename ForwardIterator>
			boost::optional<daw::spreadsheet::number::big_num_t> try_numeric( ForwardIterator first, ForwardIterator last ) {
				try {
					return daw::spreadsheet::number::big_num_t{ first };
				} catch( std::exception const & ) {
					return boost::optional<daw::spreadsheet::number::big_num_t>{ };
				}
			}

			template<typename ForwardIterator>
				cell_value::cell_variant_t to_variant( ForwardIterator first, ForwardIterator last ) {
					auto result = daw::parser::find_numeric( first, last );
					if( result ) {
						auto opt_num = try_numeric( result.first, result.last );
						assert( opt_num );
						return cell_value::cell_variant_t{ }.store( *opt_num );
					}
					return cell_value::cell_variant_t{ daw::parser::make_find_result( first, last ).as_string( ) };
				}

			cell_value::eval_func_t cell_value::eval( boost::string_ref cell_value ) {
				auto rng = daw::parser::trim( cell_value.begin( ), cell_value.end( ) );
				if( !daw::parser::is_a( *rng.first, '=' ) ) {
					// Evaluation needed
					return std::function<cell_variant_t( )>( );
				}
				// A value
				auto result = to_variant( rng.first, rng.last );
				return [res = std::move( result )]() {
					return res;
				};
			}

			bool cell_value::empty( ) const {
				return m_string_value.empty( );
			}

			cell_value::expected_value_t expected_value_from_string( boost::string_ref value_type ) {
				static std::unordered_map<std::string, cell_value::expected_value_t> const val_map = {
						{ "Boolean", cell_value::expected_value_t::Boolean },
						{ "Number", cell_value::expected_value_t::Number },
						{ "General", cell_value::expected_value_t::General },
						{ "Text", cell_value::expected_value_t::Text },
						{ "Time", cell_value::expected_value_t::Time },
						{ "Timestamp", cell_value::expected_value_t::Timestamp }
				};
				return val_map.at( value_type.to_string( ) );
			}

			std::string to_string( cell_value::expected_value_t const & value_type ) {
				static std::unordered_map<cell_value::expected_value_t, std::string> const val_map = {
						{ cell_value::expected_value_t::Boolean, "Boolean" },
						{ cell_value::expected_value_t::Number, "Number" },
						{ cell_value::expected_value_t::General, "General" },
						{ cell_value::expected_value_t::Text, "Text" },
						{ cell_value::expected_value_t::Time, "Time" },
						{ cell_value::expected_value_t::Timestamp, "Timestamp" }
				};
				return val_map.at( value_type );
			}

			std::ostream & operator<<( std::ostream & os, cell_value::expected_value_t const & expected_value ) {
				os << to_string( expected_value );
				return os;
			}

			std::istream & operator>>( std::istream & is, cell_value::expected_value_t & expected_value ) {
				std::string val;
				is >> val;
				expected_value = expected_value_from_string( val );
				return is;
			}

			column::column( daw::nodepp::base::EventEmitter emitter ):
					table_item{ std::move( emitter ) },
					m_values{ } {


				link_array( "values", m_values );
			}

			column::column( column const & other ):
					table_item{ other },
					m_values{ other.m_values } {

				link_array( "values", m_values );
			}

			column::column( column && other ):
					table_item{ std::move( other ) },
					m_values{ std::move( other.m_values ) } {

				link_array( "values", m_values );
			}

			column & column::operator=( column const & rhs ) {
				if( this != &rhs ) {
					column tmp{ rhs };
					using std::swap;
					swap( *this, tmp );
				}
				return *this;
			}

			column & column::operator=( column && rhs ) {
				if( this != &rhs ) {
					column tmp{ std::move( rhs ) };
					using std::swap;
					swap( *this, tmp );
				}
				return *this;
			}

			void swap( column & lhs, column & rhs ) {
				using std::swap;
				swap( static_cast<table_item &>(lhs), static_cast<table_item &>(rhs) );
				lhs.m_values.swap( rhs.m_values );
			}
		}    // namespace impl
	}    // namespace spreadsheet
}    // namespace daw

