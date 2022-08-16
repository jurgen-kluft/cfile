#ifndef __XFILE_FILE_PRIVATE_H__
#define __XFILE_FILE_PRIVATE_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "xbase/x_debug.h"
#include "xfile/private/x_types.h"

namespace ncore
{
    struct crunes_t;

    namespace xfile
    {
        file_handle_t file_open(crunes_t const& path, file_mode_t mode);
        bool          file_close(file_handle_t& file);
		void          file_flush(file_handle_t& file);
        s64           file_read(file_handle_t& file, u8* data, u64 size);
        s64           file_write(file_handle_t& file, u8 const* data, u64 size);
        s64           file_pread(file_handle_t& file, u8* data, u64 size, s64 offset);
        s64           file_pwrite(file_handle_t& file, u8 const* data, u64 size, s64 offset);
        s64           file_seek(file_handle_t& file, s64 offset, seek_mode_t seek_mode);
        s64           file_size(file_handle_t file);
        s64           file_offset(file_handle_t file);

    } // namespace xfile
} // namespace ncore

#endif // __XFILE_FILE_PRIVATE_H__