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

#include "big_num_t.h"

namespace daw {
	namespace spreadsheet {
		namespace number {
			big_num_t::~big_num_t( ) { };

			void big_num_t::swap( big_num_t & rhs ) noexcept {
				using std::swap;
				swap( this->value, rhs.value );
			}

			big_num_t::operator value_t&( ) {
				return value;
			}

			big_num_t::operator value_t const &( ) const {
				return value;
			}

			big_num_t & big_num_t::operator+=( big_num_t const & rhs ) {
				value += rhs.value;
				return *this;
			}

			big_num_t & big_num_t::operator-=( big_num_t const & rhs ) {
				value -= rhs.value;
				return *this;
			}

			big_num_t & big_num_t::operator*=( big_num_t const & rhs ) {
				value *= rhs.value;
				return *this;
			}

			big_num_t & big_num_t::operator/=( big_num_t const & rhs ) {
				value /= rhs.value;
				return *this;
			}

			void swap( big_num_t & lhs, big_num_t & rhs ) noexcept {
				lhs.swap( rhs );
			}

			big_num_t operator+( big_num_t lhs, big_num_t const & rhs ) {
				return lhs += rhs;
			}

			big_num_t operator-( big_num_t lhs, big_num_t const & rhs ) {
				return lhs -= rhs;
			}

			big_num_t operator*( big_num_t lhs, big_num_t const & rhs ) {
				return lhs *= rhs;
			}

			big_num_t operator/( big_num_t lhs, big_num_t const & rhs ) {
				return lhs /= rhs;
			}

			bool operator==( big_num_t const & lhs, big_num_t const & rhs ) {
				return lhs.value == rhs.value;
			}

			bool operator!=( big_num_t const & lhs, big_num_t const & rhs ) {
				return lhs.value != rhs.value;
			}

			bool operator<( big_num_t const & lhs, big_num_t const & rhs ) {
				return lhs.value < rhs.value;
			}

			bool operator>( big_num_t const & lhs, big_num_t const & rhs ) {
				return lhs.value > rhs.value;
			}

			bool operator<=( big_num_t const & lhs, big_num_t const & rhs ) {
				return lhs.value <= rhs.value;
			}

			bool operator>=( big_num_t const & lhs, big_num_t const & rhs ) {
				return lhs.value >= rhs.value;

			}

			std::ostream & operator<<( std::ostream & os, big_num_t const & rhs ) {
				os.precision( std::numeric_limits<decltype(rhs.value)>::max_digits10 );
				os << rhs.value;
				return os;
			}

			std::istream & operator>>( std::istream & is, big_num_t & rhs ) {
				is >> rhs.value;
				return is;
			}

			std::string to_string( big_num_t const & value ) {
				std::stringstream ss;
				ss << value;
				return ss.str( );
			}
		}	// namespace number
	}	// namespace spreadsheet
}	// namespace daw

