#ifndef __XFILE_FILE_H__
#define __XFILE_FILE_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "xbase/x_debug.h"

namespace xcore
{
    struct crunes_t;

    namespace xfile
    {
        enum file_mode_t
        {
            FILE_MODE_RO     = 1,
            FILE_MODE_WO     = 2,
            FILE_MODE_RW     = 4,
            FILE_MODE_CREAT  = 8,
            FILE_MODE_APPEND = 16,
            FILE_MODE_TRUNC  = 32,
        };

        /// the file seek type
        enum seek_mode_t
        {
            SEEK_MODE_BEG = 0,
            SEEK_MODE_CUR = 1,
            SEEK_MODE_END = 2
        };

        /// the file type
        enum file_type_t
        {
            FILE_TYPE_NONE      = 0,
            FILE_TYPE_DIRECTORY = 1,
            FILE_TYPE_FILE      = 2,
            FILE_TYPE_DOT       = 3,
            FILE_TYPE_DOT2      = 4
        };

        struct file_handle_t
        {
            inline file_handle_t() : m_handle(nullptr) {}
            inline file_handle_t(void* h) : m_handle(h) {}
            void* m_handle;
        };

        file_handle_t file_open(crunes_t const& path, file_mode_t mode);
        bool          file_close(file_handle_t& file);
        s64           file_read(file_handle_t& file, xbyte* data, u64 size);
        s64           file_write(file_handle_t& file, xbyte const* data, u64 size);
        s64           file_pread(file_handle_t& file, xbyte* data, u64 size, s64 offset);
        s64           file_pwrite(file_handle_t& file, xbyte const* data, u64 size, s64 offset);
        s64           file_seek(file_handle_t& file, s64 offset, seek_mode_t seek_mode);
        u64           file_size(file_handle_t file);
        s64           file_offset(file_handle_t& file);

    } // namespace xfile
} // namespace xcore

#endif // __XFILE_FILE_H__