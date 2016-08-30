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
#include "table_item.h"

namespace daw {
	namespace spreadsheet {
		namespace impl {
			class cell_value;

			class column: public table_item {
				std::vector<cell_value> m_values;
			public:
				column( daw::nodepp::base::EventEmitter emitter );
				column( column const & other );
				column( column && other );
				column & operator=( column const & rhs );
				column & operator=( column && rhs );
				friend void swap( column & lhs, column & rhs );
			};	// column

			void swap( column & lhs, column & rhs );

			struct cell_value: public table_item {
				enum class expected_value_t: size_t { General, Text, Number, Timestamp, Time, Boolean };
				using string_t = std::string;
				using number_t = daw::spreadsheet::number::big_num_t;
				using timestamp_t = boost::posix_time::ptime;
				using bool_t = bool;
				using cell_variant_t = daw::variant_t<string_t, number_t, timestamp_t, bool_t >;
				using eval_func_t = std::function<cell_variant_t( )>;
			private:
				void link_values( );
				// State values
				expected_value_t m_value_type;
				std::string m_string_value;
				// Non-state values
				eval_func_t m_evaluated;

				eval_func_t eval( boost::string_ref cell_value );
			public:
				cell_value( );

				cell_value( daw::nodepp::base::EventEmitter emitter, expected_value_t value_type = expected_value_t::General, std::string string_value = "", eval_func_t eval_func = nullptr );
				virtual ~cell_value( );
				std::string evaluate( );
				void set_value( boost::string_ref );

				expected_value_t & value_type( );
				expected_value_t const & value_type( ) const;

				cell_value( cell_value const & other );
				cell_value( cell_value && other );
				cell_value & operator=( cell_value const & rhs );
				cell_value & operator=( cell_value && rhs );

				friend void swap( cell_value & lhs, cell_value & rhs ) noexcept;
				///
				/// \param cb A callback function that takes the string id of the updated cell
				void on_data_updated( std::function<void( std::string )> cb );

				bool empty( ) const;

				void emit_data_updated( );
			};	// cell_value

			cell_value::expected_value_t expected_value_from_string( boost::string_ref value_type );
			std::string to_string( cell_value::expected_value_t const & value_type );
			std::ostream & operator<<( std::ostream & os, cell_value::expected_value_t const & expected_value );
			std::istream & operator>>( std::istream & os, cell_value::expected_value_t & expected_value );

		}	// namespace impl
	}	// namespace spreadsheet
}	// namespace daw

namespace std {
	template<>
	struct hash<daw::spreadsheet::impl::cell_value::expected_value_t> {
		size_t operator()( daw::spreadsheet::impl::cell_value::expected_value_t const & value ) const noexcept {
			return static_cast<size_t>( value );
		}
	};	// hash
}

