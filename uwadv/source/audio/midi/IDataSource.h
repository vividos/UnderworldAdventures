/*
 *	IDataSource.h - DataSource type for loading data, only needs read only access
 *
 *  Copyright (C) 2002-2005 The Pentagram Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef IDATASOURCE_H
#define IDATASOURCE_H

#include <fstream>
#include <cmath>
#include "SDL_rwops.h"

class IDataSource
{
	public:
		IDataSource() {}
		virtual ~IDataSource() {}

		virtual uint8 read1()=0;
		virtual uint16 read2()=0;
		virtual uint16 read2high()=0;
		virtual uint32 read3()=0;
		virtual uint32 read4()=0;
		virtual uint32 read4high()=0;
		virtual sint32 read(void *str, sint32 num_bytes)=0;

		uint32 readX(uint32 num_bytes)
		{
			assert(num_bytes > 0 && num_bytes <= 4);
			if (num_bytes == 1) return read1();
			else if (num_bytes == 2) return read2();
			else if (num_bytes == 3) return read3();
			else return read4();
		}

		sint32 readXS(uint32 num_bytes)
		{
			assert(num_bytes > 0 && num_bytes <= 4);
			if (num_bytes == 1) return static_cast<sint8>(read1());
			else if (num_bytes == 2) return static_cast<sint16>(read2());
			else if (num_bytes == 3) return (((static_cast<sint32>(read3())) << 8)>>8);
			else return static_cast<sint32>(read4());
		}

		/* FIXME: Dubious conversion between float and int */
		float readf()
		{
#if 1
			union {
				uint32	i;
				float	f;
			} int_float;
			int_float.i = read4();
			return int_float.f;
#else
			uint32 i = read4();
			uint32 mantissa = i & 0x3FFFFF;
			sint32 exponent = ((i >> 23) & 0xFF);

			// Zero
			if (!exponent && !mantissa)
				return 0.0F;
			// Infinity and NaN (don't handle them)
			else if (exponent == 0xFF)
				return 0.0F;
			// Normalized - Add the leading one
			else if (exponent)
				mantissa |= 0x400000;
			// Denormalized - Set the exponent to 1
			else
				exponent = 1;

			float f = std::ldexp(mantissa/8388608.0,exponent-127);
			return (i >> 31)?-f:f;
#endif
		}

		void readline(std::string &str)
		{
			str.erase();
			while (!eof())
			{
				char character =  static_cast<char>(read1());

				if (character == '\r') continue;	// Skip cr
				else if (character == '\n')	break;	// break on line feed

				str+= character;
			}
		}

		virtual void seek(uint32 pos)=0;
		virtual void skip(sint32 delta)=0;
		virtual uint32 getSize()=0;
		virtual uint32 getPos()=0;
		virtual bool eof()=0;

		virtual std::ifstream *GetRawIfstream() {
			return 0;
		}

		/* SDL_RWops functions: */

		static Sint64 rw_seek(SDL_RWops *context, Sint64 offset, int whence)
		{
			IDataSource*ids = static_cast<IDataSource*>
				(context->hidden.unknown.data1);
			switch (whence) {
			case SEEK_SET:
				ids->seek(offset);
				break;
			case SEEK_CUR:
				ids->skip(offset);
				break;
			case SEEK_END:
				ids->seek(ids->getSize()+offset);
				break;
			default:
				return -1;
				break;
			}
			return ids->getPos();
		}
		static size_t rw_read(SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
		{
			IDataSource*ids = static_cast<IDataSource*>
				(context->hidden.unknown.data1);
			if (size == 0) return 0;
			sint32 nbytes = ids->read(ptr, static_cast<sint32>(maxnum*size));
			return (nbytes/size);
		}
		static size_t rw_write(SDL_RWops * /*context*/, const void * /*ptr*/,
							size_t /*size*/, size_t /*num*/)
		{
			return 0;
		}
		static int rw_close(SDL_RWops *context)
		{
			IDataSource* ids = static_cast<IDataSource*>
				(context->hidden.unknown.data1);
			delete ids;
			SDL_FreeRW(context);
			return 0;
		}

		//! Create an SDL_RWops structure from this IDataSource.
		//! It will delete the IDataSource (and itself) when closed.
		SDL_RWops* getRWops() {
			SDL_RWops* rwops = SDL_AllocRW();
			rwops->seek = rw_seek;
			rwops->read = rw_read;
			rwops->write = rw_write;
			rwops->close = rw_close;
			rwops->hidden.unknown.data1 = static_cast<void*>(this);
			return rwops;
		}
};


class IFileDataSource: public IDataSource
{
	private:
		std::ifstream *in;

	public:
	IFileDataSource(std::ifstream *data_stream)
	{
		in = data_stream;
	}

	virtual ~IFileDataSource()
	{
		FORGET_OBJECT(in);
	}

	bool good() const { return in->good(); }

	//	Read a byte value
	virtual uint8 read1()
	{
		return static_cast<uint8>(in->get());
	}

	//	Read a 2-byte value, lsb first.
	virtual uint16 read2()
	{
		uint16 val = 0;
		val |= static_cast<uint16>(in->get());
		val |= static_cast<uint16>(in->get()<<8);
		return val;
	}

	//	Read a 2-byte value, hsb first.
	virtual uint16 read2high()
	{
		uint16 val = 0;
		val |= static_cast<uint16>(in->get()<<8);
		val |= static_cast<uint16>(in->get());
		return val;
	}

	//	Read a 3-byte value, lsb first.
	virtual uint32 read3()
	{
		uint32 val = 0;
		val |= static_cast<uint32>(in->get());
		val |= static_cast<uint32>(in->get()<<8);
		val |= static_cast<uint32>(in->get()<<16);
		return val;
	}

	//	Read a 4-byte long value, lsb first.
	virtual uint32 read4()
	{
		uint32 val = 0;
		val |= static_cast<uint32>(in->get());
		val |= static_cast<uint32>(in->get()<<8);
		val |= static_cast<uint32>(in->get()<<16);
		val |= static_cast<uint32>(in->get()<<24);
		return val;
	}

	//	Read a 4-byte long value, hsb first.
	virtual uint32 read4high()
	{
		uint32 val = 0;
		val |= static_cast<uint32>(in->get()<<24);
		val |= static_cast<uint32>(in->get()<<16);
		val |= static_cast<uint32>(in->get()<<8);
		val |= static_cast<uint32>(in->get());
		return val;
	}

	sint32 read(void *b, sint32 len)
	{
		in->read(static_cast<char *>(b), len);
		sint32 count = in->gcount();
		if (in->eof()) in->clear(); // clear eof state
		return count;
	}

	virtual void seek(uint32 pos)  { in->seekg(pos); }

	virtual void skip(sint32 pos)  { in->seekg(pos, std::ios::cur); }

	virtual uint32 getSize()
	{
		long pos = in->tellg();
		in->seekg(0, std::ios::end);
		long len = in->tellg();
		in->seekg(pos);
		return len;
	}

	virtual uint32 getPos() { return in->tellg(); }

	virtual bool eof() { in->get(); bool ret = in->eof(); if (!ret) in->unget(); return ret; }

	virtual std::ifstream *GetRawIfstream() {
		return in;
	}
};

class IBufferDataSource : public IDataSource
{
protected:
	const uint8* buf;
	const uint8* buf_ptr;
	bool free_buffer;
	uint32 size;

	void ConvertTextBuffer()
	{
#ifdef WIN32
		uint8* new_buf = new uint8[size];
		uint8* new_buf_ptr = new_buf;
		uint32 new_size = 0;

		// What we want to do is convert all 0x0D 0x0A to just 0x0D

		// Do for all but last byte
		while (size > 1)
		{
			if (*(uint16*)buf_ptr == 0x0A0D)
			{
				buf_ptr++;
				size--;
			}

			*new_buf_ptr = *buf_ptr;

			new_buf_ptr++;
			new_size++;
			buf_ptr++;
			size--;
		}

		// Do last byte
		if (size) *new_buf_ptr = *buf_ptr;

		// Delete old buffer if requested
		if (free_buffer) delete[] buf;

		buf_ptr = buf = new_buf;
		size = new_size;
		free_buffer = true;
#endif
	}

public:
	IBufferDataSource(const void* data, unsigned int len, bool is_text = false,
					  bool delete_data = false) {
		assert(data != 0 || len == 0);
		buf = buf_ptr = static_cast<const uint8 *>(data);
		size = len;
		free_buffer = delete_data;

		if (is_text) ConvertTextBuffer();
	}

	virtual void load(const void* data, unsigned int len, bool is_text = false,
					  bool delete_data = false) {
		if (free_buffer && buf) delete [] const_cast<uint8 *>(buf);
		free_buffer = false;
		buf = buf_ptr = 0;

		assert(data != 0 || len == 0);
		buf = buf_ptr = static_cast<const uint8 *>(data);
		size = len;
		free_buffer = delete_data;

		if (is_text) ConvertTextBuffer();
	}

	virtual ~IBufferDataSource() {
		if (free_buffer && buf) delete [] const_cast<uint8 *>(buf);
		free_buffer = false;
		buf = buf_ptr = 0;
	}

	virtual uint8 read1() {
		uint8 b0;
		b0 = *buf_ptr++;
		return (b0);
	}

	virtual uint16 read2() {
		uint8 b0, b1;
		b0 = *buf_ptr++;
		b1 = *buf_ptr++;
		return (b0 | (b1 << 8));
	}

	virtual uint16 read2high() {
		uint8 b0, b1;
		b1 = *buf_ptr++;
		b0 = *buf_ptr++;
		return (b0 | (b1 << 8));
	}

	virtual uint32 read3() {
		uint8 b0, b1, b2;
		b0 = *buf_ptr++;
		b1 = *buf_ptr++;
		b2 = *buf_ptr++;
		return (b0 | (b1 << 8) | (b2 << 16));
	}

	virtual uint32 read4() {
		uint8 b0, b1, b2, b3;
		b0 = *buf_ptr++;
		b1 = *buf_ptr++;
		b2 = *buf_ptr++;
		b3 = *buf_ptr++;
		return (b0 | (b1<<8) | (b2<<16) | (b3<<24));
	}

	virtual uint32 read4high() {
		uint8 b0, b1, b2, b3;
		b3 = *buf_ptr++;
		b2 = *buf_ptr++;
		b1 = *buf_ptr++;
		b0 = *buf_ptr++;
		return (b0 | (b1<<8) | (b2<<16) | (b3<<24));
	}

	virtual sint32 read(void *str, sint32 num_bytes) {
		if (buf_ptr >= buf + size) return 0;
		sint32 count = num_bytes;
		if (buf_ptr + num_bytes > buf + size)
			count = static_cast<sint32>(buf - buf_ptr + size);
		std::memcpy(str, buf_ptr, count);
		buf_ptr += count;
		return count;
	}

	virtual void seek(uint32 pos) {
		buf_ptr = buf + pos;
	}

	virtual void skip(sint32 delta) {
		buf_ptr += delta;
	}

	virtual uint32 getSize() {
		return size;
	}

	virtual uint32 getPos() {
		return static_cast<uint32>(buf_ptr - buf);
	}

	virtual bool eof() { return (static_cast<uint32>(buf_ptr-buf))>=size; }

};


#endif
