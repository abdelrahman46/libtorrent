/*

Copyright (c) 2004, 2007, 2009, 2011, 2016-2017, 2019-2020, Arvid Norberg
Copyright (c) 2016-2017, 2020, Alden Torres
All rights reserved.

You may use, distribute and modify this code under the terms of the BSD license,
see LICENSE file.
*/

#ifndef TORRENT_IO_HPP_INCLUDED
#define TORRENT_IO_HPP_INCLUDED

#include <cstdint>
#include <string>
#include <algorithm> // for copy
#include <cstring> // for memcpy
#include <type_traits>
#include <iterator>

#include "libtorrent/assert.hpp"
#include "libtorrent/aux_/io.hpp"

namespace lt::aux {

	// reads an integer from a byte stream
	// in big endian byte order and converts
	// it to native endianess
	template <class T, class InIt>
	inline T read_impl(InIt& start, type<T>)
	{
		T ret = 0;
		for (int i = 0; i < int(sizeof(T)); ++i)
		{
			ret <<= 8;
			ret |= static_cast<std::uint8_t>(*start);
			++start;
		}
		return ret;
	}

	template <class InIt>
	std::uint8_t read_impl(InIt& start, type<std::uint8_t>)
	{
		return static_cast<std::uint8_t>(*start++);
	}

	template <class InIt>
	std::int8_t read_impl(InIt& start, type<std::int8_t>)
	{
		return static_cast<std::int8_t>(*start++);
	}

	template <class T, class In, class OutIt>
	typename std::enable_if<(std::is_integral<In>::value
		&& !std::is_same<In, bool>::value)
		|| std::is_enum<In>::value, void>::type
	write_impl(In data, OutIt& start)
	{
		// Note: the test for [OutItT==void] below is necessary because
		// in C++11 std::back_insert_iterator::value_type is void.
		// This could change in C++17 or above
		using OutItT = typename std::iterator_traits<OutIt>::value_type;
		using Byte = typename std::conditional<
			std::is_same<OutItT, void>::value, char, OutItT>::type;
		static_assert(sizeof(Byte) == 1, "wrong iterator or pointer type");

		T val = static_cast<T>(data);
		TORRENT_ASSERT(data == static_cast<In>(val));
		for (int i = int(sizeof(T)) - 1; i >= 0; --i)
		{
			*start = static_cast<Byte>((val >> (i * 8)) & 0xff);
			++start;
		}
	}

	template <class T, class Val, class OutIt>
	typename std::enable_if<std::is_same<Val, bool>::value, void>::type
	write_impl(Val val, OutIt& start)
	{ write_impl<T>(val ? 1 : 0, start); }

	// -- adaptors

	template <class InIt>
	std::int64_t read_int64(InIt& start)
	{ return read_impl(start, type<std::int64_t>()); }

	template <class InIt>
	std::uint64_t read_uint64(InIt& start)
	{ return read_impl(start, type<std::uint64_t>()); }

	template <class InIt>
	std::uint32_t read_uint32(InIt& start)
	{ return read_impl(start, type<std::uint32_t>()); }

	template <class InIt>
	std::int32_t read_int32(InIt& start)
	{ return read_impl(start, type<std::int32_t>()); }

	template <class InIt>
	std::int16_t read_int16(InIt& start)
	{ return read_impl(start, type<std::int16_t>()); }

	template <class InIt>
	std::uint16_t read_uint16(InIt& start)
	{ return read_impl(start, type<std::uint16_t>()); }

	template <class InIt>
	std::int8_t read_int8(InIt& start)
	{ return read_impl(start, type<std::int8_t>()); }

	template <class InIt>
	std::uint8_t read_uint8(InIt& start)
	{ return read_impl(start, type<std::uint8_t>()); }


	template <class T, class OutIt>
	void write_uint64(T val, OutIt& start)
	{ write_impl<std::uint64_t>(val, start); }

	template <class T, class OutIt>
	void write_int64(T val, OutIt& start)
	{ write_impl<std::int64_t>(val, start); }

	template <class T, class OutIt>
	void write_uint32(T val, OutIt& start)
	{ write_impl<std::uint32_t>(val, start); }

	template <class T, class OutIt>
	void write_int32(T val, OutIt& start)
	{ write_impl<std::int32_t>(val, start); }

	template <class T, class OutIt>
	void write_uint16(T val, OutIt& start)
	{ write_impl<std::uint16_t>(val, start); }

	template <class T, class OutIt>
	void write_int16(T val, OutIt& start)
	{ write_impl<std::int16_t>(val, start); }

	template <class T, class OutIt>
	void write_uint8(T val, OutIt& start)
	{ write_impl<std::uint8_t>(val, start); }

	template <class T, class OutIt>
	void write_int8(T val, OutIt& start)
	{ write_impl<std::int8_t>(val, start); }

	inline int write_string(std::string const& str, char*& start)
	{
		std::memcpy(reinterpret_cast<void*>(start), str.c_str(), str.size());
		start += str.size();
		return int(str.size());
	}

	template <class OutIt>
	int write_string(std::string const& val, OutIt& out)
	{
		for (auto const c : val) *out++ = c;
		return int(val.length());
	}
} // namespace lt::aux

#endif // TORRENT_IO_HPP_INCLUDED
