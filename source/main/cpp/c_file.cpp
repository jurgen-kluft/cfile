#include "cfile/c_file.h"

namespace ncore
{
    namespace nfile
    {
        enum file_caps_t
        {
            FILE_CAPS_CAN_WRITE = 1,
            FILE_CAPS_CAN_SEEK  = 2,
        };

        file_t::file_t() : m_filehandle(), m_caps(0) {}

        bool file_t::open(const char* filepath, file_mode_t mode)
        {
            m_filehandle = file_open(filepath, mode);
            return m_filehandle.m_handle != nullptr;
        }
        bool file_t::isOpen() const { return false; }
        bool file_t::v_canSeek() const { return true; }
        bool file_t::v_canRead() const { return m_filehandle.m_handle != nullptr; }
        bool file_t::v_canWrite() const { return (m_caps & FILE_CAPS_CAN_WRITE) == FILE_CAPS_CAN_WRITE; }
        bool file_t::v_canView() const { return false; }
        void file_t::v_flush() { file_flush(m_filehandle); }
        void file_t::v_close() { file_close(m_filehandle); }
        u64  file_t::v_getLength() const { return file_size(m_filehandle); }
        void file_t::v_setLength(u64 length) { file_seek(m_filehandle, length, SEEK_MODE_BEG); }
        s64  file_t::v_setPos(s64 pos) { return file_seek(m_filehandle, pos, SEEK_MODE_BEG); }
        s64  file_t::v_getPos() const { return file_offset(m_filehandle); }
        s64  file_t::v_read(u8* buffer, s64 count) { return file_read(m_filehandle, buffer, count); }
        s64  file_t::v_view(u8 const*& buffer, s64 count)
        {
            buffer = nullptr;
            return 0;
        }
        s64 file_t::v_write(const u8* buffer, s64 count) { return file_write(m_filehandle, buffer, count); }

        file_t& file_t::operator=(const file_t& fs) { return *this; }

    } // namespace nfile
} // namespace ncore
