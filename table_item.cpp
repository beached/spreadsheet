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

#include <array>
#include <atomic>
#include <cstdint>
#include <string>
#include <utility>

#include <daw/daw_hash_table.h>
#include <daw/json/daw_json_link.h>
#include <daw/nodepp/base_event_emitter.h>

#include "table_item.h"

namespace daw {
	namespace spreadsheet {

		table_item::id_t table_item::get_next_id( ) noexcept {
			static std::atomic <id_t> s_next_id{ 1 };
			id_t result = s_next_id++;
			return result;
		}

		table_item::~table_item( ) {
			emit_closed( );
		}

		table_item::table_item( daw::nodepp::base::EventEmitter emitter ):
				daw::json::JsonLink<table_item>{ },
				daw::nodepp::base::StandardEvents<table_item>{ std::move( emitter ) } {

			link_integral( "id", m_id );
		}

		table_item::table_item( table_item const & other ):
				daw::json::JsonLink<table_item>{ other },
				daw::nodepp::base::StandardEvents<table_item>{ other },
				m_id{ get_next_id( ) } {

			link_integral( "id", m_id );
		}

		table_item::table_item( table_item && other ):
				daw::json::JsonLink<table_item>{ std::move( other ) },
				daw::nodepp::base::StandardEvents<table_item>{ std::move( other ) },
				m_id{ std::move( other.m_id ) } {

			link_integral( "id", m_id );
		}

		table_item & table_item::operator=( table_item const & rhs ) {
			if( this != &rhs ) {
				table_item tmp{ rhs };
				using std::swap;
				swap( *this, tmp );
			}
			return *this;
		}

		table_item & table_item::operator=( table_item && rhs ) {
			if( this != &rhs ) {
				table_item tmp{ std::move( rhs ) };
				using std::swap;
				swap( *this, tmp );
			}
			return *this;
		}

		table_item::id_t table_item::id( ) const {
			return m_id;
		}

		void swap( table_item & lhs, table_item & rhs ) {
			using std::swap;
			swap( static_cast<daw::json::JsonLink<table_item> &>(lhs), static_cast<daw::json::JsonLink<table_item> &>( rhs ) );
			swap( static_cast<daw::nodepp::base::StandardEvents<table_item> &>(lhs), static_cast<daw::nodepp::base::StandardEvents<table_item> &>( rhs ) );
			swap( lhs.m_id, rhs.m_id );
		}

		void table_item::emit_closed( ) {
			std::string msg = id( ) + "_closed";
			emitter( )->emit( msg, id( ) );

		}

		void table_item::on_closed( std::function<void( id_t )> listener ) {
			std::string msg = id( ) + "_closed";
			emitter( )->on( msg, std::move( listener ) );
		}

		void table_item::emit_updated( ) {
			std::string msg = id( ) + "_updated";
			emitter( )->emit( msg, id( ) );
		}

		void table_item::on_next_updated( std::function<void( id_t )> listener ) {
			std::string msg = id( ) + "_updated";
			emitter( )->on_next( msg, std::move( listener ) );
		}

		void table_item::on_updated( std::function<void( id_t )> listener ) {
			std::string msg = id( ) + "_updated";
			emitter( )->on( msg, std::move( listener ) );
		}

		std::string to_string( table_item::table_item_type item_type ) {
			using namespace std::literals::string_literals;
			static std::array<std::string, 4> const s_items = { "Table"s, "Cell"s, "Row"s, "Column"s };
			return s_items[static_cast<size_t>(item_type)];
		}

		table_item::table_item_type table_item_type_from_string( boost::string_ref item_type ) {
			using namespace std::literals::string_literals;
			static auto const s_items = daw::create_hash_table<std::string, table_item::table_item_type>( {
					{ "Cell"s, table_item::table_item_type::Cell },
					{ "Column"s, table_item::table_item_type::Column },
					{ "Row"s, table_item::table_item_type::Row },
					{ "Table"s, table_item::table_item_type::Table }
			} );
			return s_items.at( item_type.to_string( ) );
		}

		std::ostream & operator<<( std::ostream & os, table_item::table_item_type item_type ) {
			os << to_string( item_type );
			return os;
		}

		std::istream & operator>>( std::istream & is, table_item::table_item_type & item_type ) {
			std::string tmp;
			is >> tmp;
			item_type = table_item_type_from_string( tmp );
			return is;
		}
	}    // namespace spreadsheet
}    // namespace daw

