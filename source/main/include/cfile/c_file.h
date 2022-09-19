#ifndef __CFILE_FILE_PUBLIC_H__
#define __CFILE_FILE_PUBLIC_H__
#include "cbase/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "cbase/c_stream.h"
#include "cfile/c_types.h"

namespace ncore
{
    struct crunes_t;

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

    class file_t : public istream_t
    {
    public:
        file_t();

        bool open(crunes_t const& filepath, file_mode_t mode);
        bool isOpen() const;

    protected:
        file_t& operator=(const file_t&);

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
        virtual s64  vread(u8* buffer, s64 count);
        virtual s64  vread0(u8 const*& buffer, s64 count);
        virtual s64  vwrite(const u8* buffer, s64 count);

        file_handle_t m_filehandle;
        s64           m_offset;
        u32           m_caps;
    };

} // namespace ncore

#endif ///< __CFILE_FILE_PUBLIC_H__
