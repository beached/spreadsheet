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

#include "impl_cell_value.h"
#include "../.CLion2016.2/system/cmake/generated/spreadsheet-a5caca25/a5caca25/Debug/install/include/daw/daw_parser_helper.h"

namespace daw {
	namespace spreadsheet {
		namespace impl {
			void cell_value::link_values( ) {
				link_string( "id", m_id );
			}

			cell_value::cell_value( daw::nodepp::base::EventEmitter emitter, std::string id ):
					daw::json::JsonLink<cell_value>{ },
					daw::nodepp::base::StandardEvents<cell_value>{ std::move( emitter ) },
					m_id{ std::move( id ) } {

				link_values( );
			}

			cell_value::~cell_value( ) {
				try {
					std::string event_id = id( ) + "_data_updated";
					this->emitter( )->remove_all_listeners( event_id );
				} catch(...) {
					// I don't think I should care
				}
			}
			
			std::string & cell_value::id( ) {
				return m_id;
			}

			std::string const & cell_value::id( ) const {
				return m_id;
			}

			void cell_value::on_data_updated( std::function<void( std::string )> cb ) {
				std::string message = id( ) + "_data_updated";
				this->emitter( )->on( message, cb );
			}

			void cell_value::emit_data_updated( ) {
				std::string message = id( ) + "_data_updated";
				this->emitter( )->emit( message );
			}

			//
			// cell_value_string
			//
			void cell_value_string::link_values( ) {
				link_string( "value", m_value );
			}

			cell_value_string::cell_value_string( daw::nodepp::base::EventEmitter emitter, std::string id, std::string value ):
					cell_value{ std::move( emitter ), std::move( id ) },
					m_value{ std::move( value ) } {

				link_values( );
				this->emit_data_updated( );
			}

			std::string cell_value_string::evaluate( ) const {
				return m_value;
			}

			void cell_value_string::set_value( boost::string_ref value_string ) {
				m_value = value_string.to_string( );
				this->emit_data_updated( );
			}

			std::string cell_value_string::value_type( ) const {
				return "string";
			}

			//
			// cell_value_number
			//

			cell_value_number::cell_value_number( daw::nodepp::base::EventEmitter emitter, std::string id, cell_value_number::value_t value ):
					cell_value{ std::move( emitter ), std::move( id ) },
					m_value{ value } {

				link_values( );
				this->emit_data_updated( );
			}

			void cell_value_number::link_values( ) {
				link_streamable( "value", m_value );
			}

			std::string cell_value_number::evaluate( ) const {
				using std::to_string;
				return to_string( m_value );
			}

			void cell_value_number::set_value( boost::string_ref value_number ) {
				daw::parser::to_int( value_number.begin( ), value_number.end( ), m_value );
				this->emit_data_updated( );
			}

			std::string cell_value_number::value_type( ) const {
				return "number";
			}

			cell_value_number::value_t & cell_value_number::value( ) {
				return m_value;
			}

			cell_value_number::value_t const & cell_value_number::value( ) const {
				return m_value;
			}

			boost::optional<daw::spreadsheet::number::big_num_t> try_numeric( boost::string_ref value ) {
				try {
					return daw::spreadsheet::number::big_num_t{ value.data( ) };
				} catch( std::runtime_error const & ) {
					return boost::optional<daw::spreadsheet::number::big_num_t>{ };
				}
			}

			template<typename ForwardIterator>
			    cell_variant_t to_variant( ForwardIterator first, ForwardIterator last ) {
					auto result = daw::parser::find_numeric( first, last );
					if( result ) {
						auto opt_num = try_numeric( result.first, result.last );
						assert( opt_num );
						return cell_variant_t{ }.store( *opt_num );
					}
					return cell_variant_t{ daw::parser::make_find_result( first, last ).as_string( ) };
				}

			template<typename ForwardIterator>
			std::function<cell_variant_t( )> eval( ForwardIterator first, ForwardIterator last ) {
				auto rng = daw::parser::trim( first, last );
				if( !daw::parser::is_a( *rng.first, '=' ) ) {
					// Evaluation needed
					return std::function<cell_variant_t( )>( );
				}
				// A value
				auto result = to_variant( rng.first, rng.last );
				return [res=std::move(result)]( ) {
					return res;
				};
			}
		}	// namespace impl
	}	// namespace spreadsheet
}	// namespace daw
