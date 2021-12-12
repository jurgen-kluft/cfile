#ifndef __XFILE_FILE_H__
#define __XFILE_FILE_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "xbase/x_debug.h"

namespace xcore
{
    namespace xfile
    {
        const s32 FILE_DIRECT_ASIZE = 512;

#ifdef __direct_mode_small__
        const s32 FILE_DIRECT_CSIZE = (1 << 14);
#else
        const s32 TB_FILE_DIRECT_CSIZE = (1 << 17);
#endif

        enum file_mode_t
        {
            MODE_RO     = 1,
            MODE_WO     = 2,
            MODE_RW     = 4,
            MODE_CREAT  = 8,
            MODE_APPEND = 16,
            MODE_TRUNC  = 32,
            MODE_DIRECT = 64, //!< direct, no cache, @note data & size must be aligned by FILE_DIRECT_ASIZE
            MODE_EXEC   = 128 //!< executable, only for file_access, not supported when creating files, not supported on windows
        };

        /// the file seek type
        enum seek_mode_t
        {
            SEEK_BEG = 0,
            SEEK_CUR = 1,
            SEEK_END = 2
        };

        /// the file type
        enum file_type_t
        {
            TYPE_NONE      = 0,
            TYPE_DIRECTORY = 1,
            TYPE_FILE      = 2,
            TYPE_DOT       = 3,
            TYPE_DOT2      = 4
        };

        /// the file info type
        struct file_info_t
        {
            u64 type;
            u64 size;
            s64 atime;
            s64 mtime;
        };

        struct file_handle_t
        {
            inline file_handle_t() : m_handle(nullptr) {}
            inline file_handle_t(void* h) : m_handle(h) {}
            void* m_handle;
        };

        struct iovec_t
        {
            xbyte* m_data;
            s64    m_size;
        };

        file_handle_t file_open(char const* path, file_mode_t mode);
        bool          file_exit(file_handle_t& file);
        s64           file_read(file_handle_t& file, xbyte* data, u64 size);
        s64           file_write(file_handle_t& file, xbyte const* data, u64 size);
        s64           file_pread(file_handle_t& file, xbyte* data, u64 size, u64 offset);
        s64           file_pwrite(file_handle_t& file, xbyte const* data, u64 size, u64 offset);
        s64           file_readv(file_handle_t& file, iovec_t const* list, u64 size);
        s64           file_writev(file_handle_t& file, iovec_t const* list, u64 size);
        s64           file_preadv(file_handle_t& file, iovec_t const* list, u64 size, u64 offset);
        s64           file_pwritev(file_handle_t& file, iovec_t const* list, u64 size, u64 offset);
        s64           file_seek(file_handle_t& file, s64 offset, seek_mode_t seek_mode);
        bool          file_sync(file_handle_t& file);
        u64           file_size(file_handle_t file);
        s64           file_offset(file_handle_t& file);
        bool          file_info(char const* path, file_info_t* info);
        bool          file_copy(char const* path, char const* dest);
        bool          file_create(char const* path);
        bool          file_remove(char const* path);
        bool          file_rename(char const* path, char const* dest);
        bool          file_access(char const* path, u64 mode);

    } // namespace xfile
} // namespace xcore

#endif // __XFILE_FILE_H__