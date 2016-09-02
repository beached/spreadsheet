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

#include <atomic>
#include <cstdint>
#include <istream>
#include <ostream>
#include <utility>

#include <daw/json/daw_json_link.h>
#include <daw/nodepp/base_event_emitter.h>

namespace daw {
	namespace spreadsheet {

		struct table_item: public daw::json::JsonLink<table_item>, public daw::nodepp::base::StandardEvents<table_item> {
			using id_t = size_t;
			enum class table_item_type: size_t { Table = 0, Cell = 1, Row = 2, Column = 3 };
		private:
			static id_t get_next_id( ) noexcept;

			id_t m_id;
			table_item_type m_item_type;
		public:
			virtual ~table_item( );
			
			table_item( ) = delete;
			table_item( daw::nodepp::base::EventEmitter emitter );
			table_item( table_item const & other );
			table_item( table_item && other );
			table_item & operator=( table_item const & rhs );
			table_item & operator=( table_item && rhs );

			id_t id( ) const;
			friend void swap( table_item & lhs, table_item & rhs );

			void emit_closed( );
			void on_closed( std::function<void( id_t )> listener );
			void emit_updated( );
			void on_updated( std::function<void( id_t )> listener );
			void on_next_updated( std::function<void( id_t )> listener );
		};    // table_item

		void swap( table_item & lhs, table_item & rhs );

		std::string to_string( table_item::table_item_type item_type );
		table_item::table_item_type table_item_type_from_string( boost::string_ref item_type );
		std::ostream & operator<<( std::ostream & os, table_item::table_item_type item_type );
		std::istream & operator>>( std::istream & is, table_item::table_item_type & item_type );

	}    // namespace spreadsheet
}    // namespace daw

