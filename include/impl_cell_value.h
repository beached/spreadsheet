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

#include <boost/utility/string_ref.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <cstdint>
#include <memory>
#include <vector>

#include <daw/json/daw_json_link.h>
#include <daw/nodepp/base_event_emitter.h>
#include <daw/daw_variant.h>

#include "big_num_t.h"

namespace daw {
	namespace spreadsheet {
		namespace impl {
			class cell_value;

			class column: public daw::json::JsonLink<column> {
				std::vector<cell_value> m_values;
			};
			enum class expected_value_t { Text, Number, Timestamp, Time, Boolean };

			using cell_variant_t = daw::variant_t<std::string, daw::spreadsheet::number::big_num_t, boost::posix_time::ptime, bool>;

			std::function<cell_variant_t( )> eval( boost::string_ref cell_value );

			class cell_value: public daw::json::JsonLink<cell_value>, public daw::nodepp::base::StandardEvents<cell_value> {
				std::string m_id;
				void link_values( );
			public:
				cell_value( daw::nodepp::base::EventEmitter emitter, std::string id );
				virtual ~cell_value( );
				virtual std::string evaluate( ) const = 0;
				virtual void set_value( boost::string_ref ) = 0;
				virtual std::string value_type( ) const = 0;
				std::string & id( );
				std::string const & id( ) const;

				cell_value( cell_value const & ) = default;
				cell_value( cell_value && ) = default;
				cell_value & operator=( cell_value const & ) = default;
				cell_value & operator=( cell_value && ) = default;

				///
				/// \param cb A callback function that takes the string id of the updated cell
				void on_data_updated( std::function<void( std::string )> cb );

				void emit_data_updated( );
			};	// cell_value

			class cell_value_string: public cell_value {
				std::string m_value;
				void link_values( );
			public:
				cell_value_string( daw::nodepp::base::EventEmitter emitter, std::string id, std::string value );
				std::string evaluate( ) const override;
				void set_value( boost::string_ref value_string ) override;
				std::string value_type( ) const override;

				cell_value_string( cell_value_string const & ) = default;
				cell_value_string( cell_value_string && ) = default;
				cell_value_string & operator=( cell_value_string const & ) = default;
				cell_value_string & operator=( cell_value_string && ) = default;
			};	// cell_value_string

			struct cell_value_number: public cell_value {
				using value_t = daw::spreadsheet::number::big_num_t;
			private:
				value_t m_value;
				void link_values( );
			public:
				cell_value_number( daw::nodepp::base::EventEmitter emitter, std::string id, value_t value );
				std::string evaluate( ) const override;
				void set_value( boost::string_ref value_number ) override;
				std::string value_type( ) const override;

				cell_value_number( cell_value_number const & ) = default;
				cell_value_number( cell_value_number && ) = default;
				cell_value_number & operator=( cell_value_number const & ) = default;
				cell_value_number & operator=( cell_value_number && ) = default;

				value_t & value( );
				value_t const & value( ) const;
			};	// cell_value_number

			std::string to_string( cell_value_number::value_t const & value );
		}	// namespace impl
	}	// namespace spreadsheet
}	// namespace daw

