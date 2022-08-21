#ifndef __CFILE_FILE_PUBLIC_H__
#define __CFILE_FILE_PUBLIC_H__
#include "cbase/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "cbase/c_debug.h"
#include "cbase/c_stream.h"

#include "cfile/private/c_types.h"

namespace ncore
{
	class file_stream_t : public istream_t
	{
	public:
		file_stream_t();

		bool open(crunes_t const& filepath, file_mode_t mode);
		bool isOpen() const;

	protected:
		file_stream_t& operator=(const file_stream_t&);

		virtual bool vcanSeek() const;
		virtual bool vcanRead() const;
		virtual bool vcanWrite() const;
		virtual bool vcanZeroCopy() const;
		virtual void vflush();
		virtual void vclose();
		virtual u64  vgetLength() const;
		virtual void vsetLength(u64 length);
		virtual s64  vsetPos(s64 pos);
		virtual s64  vgetPos() const;
		virtual s64 vread(u8* buffer, s64 count);
		virtual s64 vread0(u8 const*& buffer, s64 count);
		virtual s64 vwrite(const u8* buffer, s64 count);

		file_handle_t m_filehandle;
		s64 m_offset;
		u32 m_caps;
	};

} // namespace ncore

#endif ///< __CFILE_FILE_PUBLIC_H__
