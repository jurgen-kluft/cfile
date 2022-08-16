#include "cbase/c_allocator.h"
#include "cbase/c_integer.h"
#include "cbase/c_memory.h"

#include "cfile/private/c_file.h"
#include "cfile/c_file.h"

namespace ncore
{
    enum file_caps_t
    {
        FILE_CAPS_CAN_WRITE = 1,
        FILE_CAPS_CAN_SEEK = 2,
    };

    bool file_stream_t::open(crunes_t const& filepath, file_mode_t mode)
    {
        m_filehandle = xfile::file_open(filepath, mode);
        return m_filehandle.m_handle != nullptr;
    }

    bool file_stream_t::isOpen() const
    {
        return false;
    }

    file_stream_t::file_stream_t() 
        : m_filehandle()
        , m_caps(0)
    {

    }

    file_stream_t& file_stream_t::operator=(const file_stream_t& fs)
    {
        return *this;
    }

    bool file_stream_t::vcanSeek() const
    {
        return true;
    }

    bool file_stream_t::vcanRead() const
    {
        return m_filehandle.m_handle != nullptr;
    }

    bool file_stream_t::vcanWrite() const
    {
        return (m_caps & FILE_CAPS_CAN_WRITE) == FILE_CAPS_CAN_WRITE;
    }

    bool file_stream_t::vcanZeroCopy() const
    {
        return false;
    }

    void file_stream_t::vflush()
    {
        xfile::file_flush(m_filehandle);
    }

    void file_stream_t::vclose()
    {
        xfile::file_close(m_filehandle);
    }

    u64  file_stream_t::vgetLength() const
    {
        return xfile::file_size(m_filehandle);
    }

    void file_stream_t::vsetLength(u64 length)
    {
        xfile::file_seek(m_filehandle, length, SEEK_MODE_BEG);
    }

    s64  file_stream_t::vsetPos(s64 pos)
    {
        return xfile::file_seek(m_filehandle, pos, SEEK_MODE_BEG);
    }

    s64  file_stream_t::vgetPos() const
    {
        return xfile::file_offset(m_filehandle);
    }

    s64  file_stream_t::vread(u8* buffer, s64 count)
    {
        return xfile::file_read(m_filehandle, buffer, count);
    }

    s64  file_stream_t::vread0(u8 const*& buffer, s64 count)
    {
        buffer = nullptr;
        return 0;
    }

    s64  file_stream_t::vwrite(const u8* buffer, s64 count)
    {
        return xfile::file_write(m_filehandle, buffer, count);
    }

} // namespace ncore
