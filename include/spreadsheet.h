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

#include <memory>

#include <daw/nodepp/base_event_emitter.h>
#include <daw/json/daw_json_link.h>

#include "impl_cell_value.h"

namespace daw {
	namespace spreadsheet {

		class cell_t: public daw::json::JsonLink<cell_t> {
			std::unique_ptr<impl::cell_value> m_value;

			void link_values( );

		public:
			cell_t( daw::nodepp::base::EventEmitter emitter );
			~cell_t( );
			cell_t( cell_t const & ) = default;
			cell_t( cell_t && ) = default;
			cell_t & operator=( cell_t const & ) = default;
			cell_t & operator=( cell_t && ) = default;
			friend void swap( cell_t & lhs, cell_t & rhs ) noexcept;
		};

		void swap( cell_t & lhs, cell_t & rhs ) noexcept;
	}	// namespace spreadsheet
}	// namespace daw

