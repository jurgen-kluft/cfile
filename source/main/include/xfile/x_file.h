#ifndef __XFILE_FILE_PUBLIC_H__
#define __XFILE_FILE_PUBLIC_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "xbase/x_debug.h"
#include "xbase/x_stream.h"

#include "xfile/private/x_enums.h"
#include "xfile/private/x_file.h"

namespace xcore
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
		virtual void vflush();
		virtual void vclose();
		virtual u64  vgetLength() const;
		virtual void vsetLength(u64 length);
		virtual s64  vsetPos(s64 pos);
		virtual s64  vgetPos() const;
		virtual s64 vread(xbyte* buffer, s64 count);
		virtual s64 vwrite(const xbyte* buffer, s64 count);

		xfile::file_handle_t m_filehandle;
		s64 m_offset;
		u32 m_caps;
	};

} // namespace xcore

#endif ///< __XFILE_FILE_PUBLIC_H__