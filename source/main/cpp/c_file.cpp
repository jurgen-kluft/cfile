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

    bool file_t::open(crunes_t const& filepath, file_mode_t mode)
    {
        m_filehandle = nfile::file_open(filepath, mode);
        return m_filehandle.m_handle != nullptr;
    }

    bool file_t::isOpen() const
    {
        return false;
    }

    file_t::file_t() 
        : m_filehandle()
        , m_caps(0)
    {

    }

    file_t& file_t::operator=(const file_t& fs)
    {
        return *this;
    }

    bool file_t::vcanSeek() const
    {
        return true;
    }

    bool file_t::vcanRead() const
    {
        return m_filehandle.m_handle != nullptr;
    }

    bool file_t::vcanWrite() const
    {
        return (m_caps & FILE_CAPS_CAN_WRITE) == FILE_CAPS_CAN_WRITE;
    }

    bool file_t::vcanZeroCopy() const
    {
        return false;
    }

    void file_t::vflush()
    {
        nfile::file_flush(m_filehandle);
    }

    void file_t::vclose()
    {
        nfile::file_close(m_filehandle);
    }

    u64  file_t::vgetLength() const
    {
        return nfile::file_size(m_filehandle);
    }

    void file_t::vsetLength(u64 length)
    {
        nfile::file_seek(m_filehandle, length, SEEK_MODE_BEG);
    }

    s64  file_t::vsetPos(s64 pos)
    {
        return nfile::file_seek(m_filehandle, pos, SEEK_MODE_BEG);
    }

    s64  file_t::vgetPos() const
    {
        return nfile::file_offset(m_filehandle);
    }

    s64  file_t::vread(u8* buffer, s64 count)
    {
        return nfile::file_read(m_filehandle, buffer, count);
    }

    s64  file_t::vread0(u8 const*& buffer, s64 count)
    {
        buffer = nullptr;
        return 0;
    }

    s64  file_t::vwrite(const u8* buffer, s64 count)
    {
        return nfile::file_write(m_filehandle, buffer, count);
    }

} // namespace ncore
