#ifndef __CFILE_FILE_PUBLIC_H__
#define __CFILE_FILE_PUBLIC_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_stream.h"
#include "cfile/c_types.h"

namespace ncore
{
    namespace nfile
    {
        file_handle_t file_open(const char* path, file_mode_t mode);
        bool          file_close(file_handle_t file);
        void          file_flush(file_handle_t file);
        s64           file_read(file_handle_t file, u8* data, u64 size);
        s64           file_write(file_handle_t file, u8 const* data, u64 size);
        s64           file_pread(file_handle_t file, u8* data, u64 size, s64 offset);
        s64           file_pwrite(file_handle_t file, u8 const* data, u64 size, s64 offset);
        s64           file_seek(file_handle_t file, s64 offset, seek_mode_t seek_mode);
        s64           file_size(file_handle_t file);
        s64           file_offset(file_handle_t file);

        class file_t : public istream_t
        {
        public:
            file_t();

            bool open(const char* filepath, file_mode_t mode);
            bool isOpen() const;

        protected:
            file_t& operator=(const file_t&);

            virtual bool v_canSeek() const;
            virtual bool v_canRead() const;
            virtual bool v_canWrite() const;
            virtual bool v_canView() const;
            virtual void v_flush();
            virtual void v_close();
            virtual u64  v_getLength() const;
            virtual void v_setLength(u64 length);
            virtual s64  v_setPos(s64 pos);
            virtual s64  v_getPos() const;
            virtual s64  v_read(u8* buffer, s64 count);
            virtual s64  v_view(u8 const*& buffer, s64 count);
            virtual s64  v_write(const u8* buffer, s64 count);

            file_handle_t m_filehandle;
            s64           m_offset;
            u32           m_caps;
        };

    } // namespace nfile
} // namespace ncore

#endif ///< __CFILE_FILE_PUBLIC_H__
