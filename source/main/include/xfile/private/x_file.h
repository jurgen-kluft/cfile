#ifndef __XFILEIO_FILE_H__
#define __XFILEIO_FILE_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "xbase/x_debug.h"

namespace xfile
{
    const s32 FILE_DIRECT_ASIZE = 512;

/// the cached size for direct mode
#ifdef __direct_mode_small__
    const s32 FILE_DIRECT_CSIZE = (1 << 14);
#else
    const s32 TB_FILE_DIRECT_CSIZE = (1 << 17);
#endif

    enum mode_t
    {
        MODE_RO         = 1     //!< read only
    ,   MODE_WO         = 2     //!< writ only
    ,   MODE_RW         = 4     //!< read and writ
    ,   MODE_CREAT      = 8     //!< create
    ,   MODE_APPEND     = 16    //!< append
    ,   MODE_TRUNC      = 32    //!< truncate
    ,   MODE_DIRECT     = 64    //!< direct, no cache, @note data & size must be aligned by TB_FILE_DIRECT_ASIZE
    ,   MODE_EXEC       = 128   //!< executable, only for tb_file_access, not supported when creating files, not supported on windows
    };

    /// the file seek type
    enum seek_mode_t
    {
        SEEK_BEG        = 0
    ,   SEEK_CUR        = 1
    ,   SEEK_END        = 2
    };

    /// the file type
    enum file_type_t
    {
        TYPE_NONE       = 0
    ,   TYPE_DIRECTORY  = 1
    ,   TYPE_FILE       = 2
    ,   TYPE_DOT        = 3
    ,   TYPE_DOT2       = 4
    };

    /// the file info type
    struct file_info_t
    {
        size_t               type;
        size_t               size
        s64            atime;
        s64            mtime;
    };

    struct file_handle_t
    {
        void*   m_handle;
    }

    struct iovec_t
    {
        byte* m_data;
        s64 m_size;
    };

    file_handle_t      file_open(char const* path, file_mode_t mode);
    bool               file_exit(file_handle_t& file);
    s64                file_read(file_handle_t& file, byte* data, u64 size);
    s64                file_write(file_handle_t& file, byte const* data, u64 size);
    s64                file_pread(file_handle_t& file, byte* data, u64 size, u64 offset);
    s64                file_pwrite(file_handle_t& file, byte const* data, u64 size, u64 offset);
    s64                file_readv(file_handle_t& file, tb_iovec_t const* list, u64 size);
    s64                file_writev(file_handle_t& file, tb_iovec_t const* list, u64 size);
    s64                file_preadv(file_handle_t& file, tb_iovec_t const* list, u64 size, u64 offset);
    s64                file_pwritev(file_handle_t& file, tb_iovec_t const* list, u64 size, u64 offset);
    s64                file_seek(file_handle_t& file, s64 offset, seek mode);
    bool               file_sync(file_handle_t& file);
    u64                file_size(file_handle_t& file);
    s64                file_offset(file_handle_t& file);
    bool               file_info(char const* path, file_info_t* info);
    bool               file_copy(char const* path, char const* dest);
    bool               file_create(char const* path);
    bool               file_remove(char const* path);
    bool               file_rename(char const* path, char const* dest);
    bool               file_access(char const* path, u64 mode);

}

#endif // __XFILEIO_FILE_H__