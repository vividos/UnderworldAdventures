/*
 *	ODataSource.h - DataSource type for writing data
 *
 *  Copyright (C) 2002-2003 The Pentagram Team
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

#ifndef ODATASOURCE_H
#define ODATASOURCE_H

#include "common_types.h"
#include <fstream>
#include <deque>
#include <vector>

class ODataSource
{
	public:
		ODataSource() {}
		virtual ~ODataSource() {}

		virtual void write1(uint32)=0;
		virtual void write2(uint16)=0;
		virtual void write2high(uint16)=0;
		virtual void write3(uint32)=0;
		virtual void write4(uint32)=0;
		virtual void write4high(uint32)=0;
		virtual void write(const void *str, uint32 num_bytes)=0;

		void writeX(uint32 val, uint32 num_bytes)
		{
			assert(num_bytes > 0 && num_bytes <= 4);
			if (num_bytes == 1) write1(static_cast<uint8>(val));
			else if (num_bytes == 2) write2(static_cast<uint16>(val));
			else if (num_bytes == 3) write3(val);
			else write4(val);
		}

		void writef(float f)
		{
			// FIXME: dubious...
			union {
				uint32	i;
				float	f;
			} int_float;
			int_float.f = f;
			write4(int_float.i);
		}
		
		virtual void seek(uint32 pos)=0;
		virtual void skip(sint32 delta)=0;
		virtual uint32 getSize()=0;
		virtual uint32 getPos()=0;
};


class OFileDataSource : public ODataSource
{
	private:
		std::ofstream *out;

	public:
	OFileDataSource(std::ofstream *data_stream)
	{
		out = data_stream;
	}

	virtual ~OFileDataSource()
	{
		FORGET_OBJECT(out);
	}

	bool good() const { return out->good(); }

	virtual void write1(uint32 val)         
	{ 
		out->put(static_cast<char> (val&0xff));
	}

	virtual void write2(uint16 val)         
	{ 
		out->put(static_cast<char> (val&0xff));
		out->put(static_cast<char> ((val>>8)&0xff));
	}

	virtual void write2high(uint16 val)     
	{ 
		out->put(static_cast<char> ((val>>8)&0xff));
		out->put(static_cast<char> (val&0xff));
	}

	virtual void write3(uint32 val)         
	{ 
		out->put(static_cast<char> (val&0xff));
		out->put(static_cast<char> ((val>>8)&0xff));
		out->put(static_cast<char> ((val>>16)&0xff));
	}

	virtual void write4(uint32 val)         
	{ 
		out->put(static_cast<char> (val&0xff));
		out->put(static_cast<char> ((val>>8)&0xff));
		out->put(static_cast<char> ((val>>16)&0xff));
		out->put(static_cast<char> ((val>>24)&0xff));
	}

	virtual void write4high(uint32 val)     
	{ 
		out->put(static_cast<char> ((val>>24)&0xff));
		out->put(static_cast<char> ((val>>16)&0xff));
		out->put(static_cast<char> ((val>>8)&0xff));
		out->put(static_cast<char> (val&0xff));
	}

	virtual void write(const void *b, uint32 len) 
	{ 
		out->write(static_cast<const char *>(b), len); 
	}

	virtual void seek(uint32 pos) { out->seekp(pos); }

	virtual void skip(sint32 pos) { out->seekp(pos, std::ios::cur); }

	virtual uint32 getSize()
	{
		long pos = out->tellp();
		out->seekp(0, std::ios::end);
		long len = out->tellp();
		out->seekp(pos);
		return len;
	}

	virtual uint32 getPos() { return out->tellp(); }
};

class OBufferDataSource: public ODataSource
{
protected:
	uint8 *buf;
	uint8 *buf_ptr;
	uint32 size;
public:
	OBufferDataSource(void *data, uint32 len)
	{
		assert(data==0 || len==0);
		buf = buf_ptr = reinterpret_cast<uint8*>(data);
		size = len;
	};
	
	void load(char *data, uint32 len)
	{
		assert(data==0 || len==0);
		buf = buf_ptr = reinterpret_cast<uint8*>(data);
		size = len;
	};
	
	virtual ~OBufferDataSource() {};
	
	virtual void write1(uint32 val)
	{
		*buf_ptr++ = val & 0xff;
	};
	
	virtual void write2(uint16 val)
	{
		*buf_ptr++ = val & 0xff;
		*buf_ptr++ = (val>>8) & 0xff;
	};

	virtual void write2high(uint16 val)
	{
		*buf_ptr++ = (val>>8) & 0xff;
		*buf_ptr++ = val & 0xff;
	};
	
	virtual void write3(uint32 val)
	{
		*buf_ptr++ = val & 0xff;
		*buf_ptr++ = (val>>8) & 0xff;
		*buf_ptr++ = (val>>16)&0xff;
	};
	
	virtual void write4(uint32 val)
	{
		*buf_ptr++ = val & 0xff;
		*buf_ptr++ = (val>>8) & 0xff;
		*buf_ptr++ = (val>>16)&0xff;
		*buf_ptr++ = (val>>24)&0xff;
	};
	
	virtual void write4high(uint32 val)
	{
		*buf_ptr++ = (val>>24)&0xff;
		*buf_ptr++ = (val>>16)&0xff;
		*buf_ptr++ = (val>>8) & 0xff;
		*buf_ptr++ = val & 0xff;
	};

	virtual void write(const void *b, uint32 len)
	{
		std::memcpy(buf_ptr, b, len);
		buf_ptr += len;
	};
	
	virtual void seek(uint32 pos) { buf_ptr = const_cast<unsigned char *>(buf)+pos; };
	
	virtual void skip(sint32 pos) { buf_ptr += pos; };
	
	virtual uint32 getSize() { return size; };
	
	virtual uint32 getPos() { return static_cast<uint32>(buf_ptr-buf); };
};

class ODequeDataSource : public ODataSource
{
	private:
		std::deque<char> out;

	public:
	ODequeDataSource() {}

	virtual ~ODequeDataSource() {}

	const std::deque<char> &buf() const { return out; }

	virtual void write1(uint32 val)
	{
		out.push_back(static_cast<char> (val&0xff));
	}

	virtual void write2(uint16 val)
	{
		out.push_back(static_cast<char> (val&0xff));
		out.push_back(static_cast<char> ((val>>8)&0xff));
	}

	virtual void write2high(uint16 val)
	{
		out.push_back(static_cast<char> ((val>>8)&0xff));
		out.push_back(static_cast<char> (val&0xff));
	}

	virtual void write3(uint32 val)
	{
		out.push_back(static_cast<char> (val&0xff));
		out.push_back(static_cast<char> ((val>>8)&0xff));
		out.push_back(static_cast<char> ((val>>16)&0xff));
	}

	virtual void write4(uint32 val)
	{
		out.push_back(static_cast<char> (val&0xff));
		out.push_back(static_cast<char> ((val>>8)&0xff));
		out.push_back(static_cast<char> ((val>>16)&0xff));
		out.push_back(static_cast<char> ((val>>24)&0xff));
	}

	virtual void write4high(uint32 val)
	{
		out.push_back(static_cast<char> ((val>>24)&0xff));
		out.push_back(static_cast<char> ((val>>16)&0xff));
		out.push_back(static_cast<char> ((val>>8)&0xff));
		out.push_back(static_cast<char> (val&0xff));
	}

	virtual void write(const void *b, uint32 length)
	{
		write(b, length, length);
	}
	
	virtual void write(const void *b, uint32 length, uint32 pad_length)
	{
		for(uint32 i=0; i<length; i++)
			out.push_back(static_cast<const char *>(b)[i]);
		if(pad_length>length)
			for(pad_length-=length; pad_length>0; --pad_length)
				out.push_back(static_cast<char>(0x00));
	}

	virtual void clear()          { out.clear(); }

	virtual void seek(uint32 /*pos*/) { /*out->seekp(pos); FIXME: Do something here. */ }
	virtual void skip(sint32 /*pos*/) { /*out->seekp(pos, std::ios::cur); FIXME: Do something here. */ }

	virtual uint32 getSize()      { return static_cast<uint32>(out.size()); }
	
	virtual uint32 getPos() { return static_cast<uint32>(out.size()); /*return out->tellp(); FIXME: Do something here. */ }

};

class OAutoBufferDataSource: public ODataSource
{
protected:
	uint8 *buf;
	uint8 *buf_ptr;
	uint32 size;
	uint32 loc;
	uint32 allocated;

	void checkResize(uint32 num_bytes)
	{
		// Increment loc
		loc+=num_bytes;

		// Don't need to resize
		if (loc <= size) return; 

		// Reallocate the buffer
		if (loc > allocated)
		{
			// The old pointer position
			uint32 pos = static_cast<uint32>(buf_ptr-buf);

			// The new buffer and size (2 times what is needed)
			allocated = loc*2;
			uint8 *new_buf = new uint8[allocated];

			memcpy(new_buf, buf, size);
			delete [] buf;

			buf = new_buf;
			buf_ptr = buf + pos;
		}

		// Update size
		size = loc;
	}

public:
	OAutoBufferDataSource(uint32 initial_len)
	{
		allocated = initial_len;
		size = 0;

		// Make the min allocated size 16 bytes
		if (allocated < 16) allocated = 16;
		buf = buf_ptr = new uint8[allocated];
		loc = 0;
	};

	//! Get a pointer to the data buffer.
	const uint8 * getBuf() { return buf; }

	virtual ~OAutoBufferDataSource()
	{
		delete [] buf_ptr;
	}
	
	virtual void write1(uint32 val)
	{
		checkResize(1);
		*buf_ptr++ = val & 0xff;
	};
	
	virtual void write2(uint16 val)
	{
		checkResize(2);
		*buf_ptr++ = val & 0xff;
		*buf_ptr++ = (val>>8) & 0xff;
	};

	virtual void write2high(uint16 val)
	{
		checkResize(2);
		*buf_ptr++ = (val>>8) & 0xff;
		*buf_ptr++ = val & 0xff;
	};
	
	virtual void write3(uint32 val)
	{
		checkResize(3);
		*buf_ptr++ = val & 0xff;
		*buf_ptr++ = (val>>8) & 0xff;
		*buf_ptr++ = (val>>16)&0xff;
	};
	
	virtual void write4(uint32 val)
	{
		checkResize(4);
		*buf_ptr++ = val & 0xff;
		*buf_ptr++ = (val>>8) & 0xff;
		*buf_ptr++ = (val>>16)&0xff;
		*buf_ptr++ = (val>>24)&0xff;
	};
	
	virtual void write4high(uint32 val)
	{
		checkResize(4);
		*buf_ptr++ = (val>>24)&0xff;
		*buf_ptr++ = (val>>16)&0xff;
		*buf_ptr++ = (val>>8) & 0xff;
		*buf_ptr++ = val & 0xff;
	};

	virtual void write(const void *b, uint32 len)
	{
		checkResize(len);
		std::memcpy(buf_ptr, b, len);
		buf_ptr += len;
	};
	
	virtual void seek(uint32 pos) 
	{ 
		// No seeking past the end of the buffer
		if (pos<=size) loc = pos;
		else loc = size;

		buf_ptr = const_cast<unsigned char *>(buf)+loc;
	};
	
	virtual void skip(sint32 pos) 
	{ 
		// No seeking past the end
		if (pos>=0)
		{
			loc += pos;
			if (loc > size) loc = size;
		}
		// No seeking past the start
		else
		{
			uint32 invpos = -pos;
			if (invpos > loc) invpos = loc;
			loc -= invpos;
		}
		buf_ptr = const_cast<unsigned char *>(buf)+loc;
	};
	
	virtual uint32 getSize() { return size; };
	
	virtual uint32 getPos() { return static_cast<uint32>(buf_ptr-buf); };

	// Don't actually do anything substantial
	virtual void clear()          
	{ 
		buf_ptr = buf; 
		size = 0;
		loc = 0;
	}

};


#endif
