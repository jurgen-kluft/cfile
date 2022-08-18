#include "cbase/c_target.h"

#if defined TARGET_PC

#    define WIN32_LEAN_AND_MEAN
#    define NOGDI
#    define NOKANJI
#    include <windows.h>
#    include <stdio.h>

#    include "cbase/c_runes.h"
#    include "cbase/c_memory.h"
#    include "xfile/private/c_file.h"

namespace ncore
{
    namespace xfile
    {
        const s32 cMaxPath = 512;

        static void mkdir(char const* path)
        {
            // make directory
            char        temp[cMaxPath] = {0};
            char const* p              = path;
            char*       t              = temp;
            char const* e              = temp + cMaxPath - 1;
            for (; t < e && *p; t++)
            {
                *t = *p;
                if (*p == L'\\' || *p == L'/')
                {
                    // make directory if not exists
                    if (INVALID_FILE_ATTRIBUTES == GetFileAttributes((LPCSTR)temp))
                        CreateDirectory((LPCSTR)temp, nullptr);

                    // skip repeat '\\' or '/'
                    while (*p && (*p == L'\\' || *p == L'/'))
                        p++;
                }
                else
                    p++;
            }
        }

        static void mkdir(wchar const* path)
        {
            // make directory
            wchar        temp[cMaxPath] = {0};
            wchar const* p              = path;
            wchar*       t              = temp;
            wchar const* e              = temp + cMaxPath - 1;
            for (; t < e && *p; t++)
            {
                *t = *p;
                if (*p == L'\\' || *p == L'/')
                {
                    // make directory if not exists
                    if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW((LPCWSTR)temp))
                        CreateDirectoryW((LPCWSTR)temp, nullptr);

                    // skip repeat '\\' or '/'
                    while (*p && (*p == L'\\' || *p == L'/'))
                        p++;
                }
                else
                    p++;
            }
        }

        file_handle_t file_open(crunes_t const& path, file_mode_t mode)
        {
            // check
            if (!path.is_valid())
                return file_handle_t();

            // init access
            DWORD access = GENERIC_READ;
            if ((mode & FILE_MODE_RW)==FILE_MODE_RW)
                access = GENERIC_READ | GENERIC_WRITE;
            else if (mode & FILE_MODE_WRITE)
                access = GENERIC_WRITE;
            else if (mode & FILE_MODE_READ)
                access = GENERIC_READ;

            // init share
            DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;

            // init flag
            DWORD cflag = 0;
            if (mode & FILE_MODE_CREATE)
            {
				access |= GENERIC_WRITE;

				if (mode & FILE_MODE_TRUNC) {
					// always create a new empty file
					cflag |= CREATE_ALWAYS;
				}
				else if (mode & FILE_MODE_APPEND) {
					// create or open and append file
					cflag |= OPEN_ALWAYS;
				}
				else {
					// create a new file only if file not exists
					cflag |= CREATE_NEW;
				}
            }
			else if (mode & FILE_MODE_TRUNC) {
				// open and truncate an existing file
				access |= GENERIC_WRITE;
				cflag |= TRUNCATE_EXISTING;
			}

            // open an existing file
            if (!cflag)
                cflag |= OPEN_EXISTING;

            // init attr
            DWORD attr = FILE_ATTRIBUTE_NORMAL;

            HANDLE file = nullptr;
            if (path.m_type == utf16::TYPE)
            {
                // init file
                file = CreateFileW((LPCWSTR)path.m_utf16.m_str, access, share, nullptr, cflag, attr, nullptr);
                if (file == INVALID_HANDLE_VALUE && (mode & FILE_MODE_CREATE))
                {
                    // make directory
                    mkdir(path.m_utf16.m_str);

                    // init it again
                    file = CreateFileW((LPCWSTR)path.m_utf16.m_str, access, share, nullptr, cflag, attr, nullptr);
                }
            }
            else if (path.m_type == ascii::TYPE)
            {
                // init file
                file = CreateFile((LPCSTR)path.m_ascii.m_str, access, share, nullptr, cflag, attr, nullptr);
                if (file == INVALID_HANDLE_VALUE && (mode & FILE_MODE_CREATE))
                {
                    // make directory
                    mkdir(path.m_ascii.m_str);

                    // init it again
                    file = CreateFile((LPCSTR)path.m_ascii.m_str, access, share, nullptr, cflag, attr, nullptr);
                }
            }
            else if (path.m_type == utf8::TYPE || path.m_type == utf32::TYPE)
            {
                // first convert to utf16 and then run the wchar functions
            }

            // append?
            if (file != INVALID_HANDLE_VALUE && (mode & FILE_MODE_APPEND))
            {
                // seek to end
                s64 size = file_size((file_handle_t)file);
                if (size)
                {
                    if (size != file_seek((file_handle_t)file, size, SEEK_MODE_BEG))
                    {
                        file_close((file_handle_t)file);
                        file = INVALID_HANDLE_VALUE;
                    }
                }
            }

            file_handle_t fh = (file != INVALID_HANDLE_VALUE) ? file_handle_t(file) : file_handle_t(nullptr);
            return fh;
        }

        bool file_close(file_handle_t& file)
        {
            // check
            if (file.m_handle == nullptr)
                return false;

            // close it
            return CloseHandle((HANDLE)file.m_handle) ? true : false;
        }

		void file_flush(file_handle_t& file)
		{
			if (file.m_handle == nullptr)
				return;

			// flush ?

		}

        s64 file_read(file_handle_t& file, u8* data, u64 size)
        {
            // check
            if (file.m_handle == nullptr || data == nullptr)
                return -1;
            if (size == 0)
                return 0;

            // read
            DWORD real_size = 0;
            return ReadFile((HANDLE)file.m_handle, data, (DWORD)size, &real_size, nullptr) ? (s64)real_size : -1;
        }

        s64 file_write(file_handle_t& file, u8 const* data, u64 size)
        {
            // check
            if (file.m_handle == nullptr || data == nullptr)
                return -1;
            if (size == 0)
                return 0;

            // write
            DWORD real_size = 0;
            return WriteFile((HANDLE)file.m_handle, data, (DWORD)size, &real_size, nullptr) ? (s64)real_size : -1;
        }

        s64 file_pread(file_handle_t& file, u8* data, u64 size, s64 offset)
        {
            // check
            if (file.m_handle == nullptr || data == nullptr)
                return -1;
            if (size == 0)
                return 0;

            // save offset
            s64 current = file_offset(file);
            if (!(current >= 0))
                return -1;

            // seek it
            if (current != offset && file_seek(file, offset, SEEK_MODE_BEG) != offset)
                return -1;

            // read it
            s64 real = file_read(file, data, size);

            // restore offset
            if (current != offset && file_seek(file, current, SEEK_MODE_BEG) != current)
                return -1;

            // ok
            return real;
        }

        s64 file_pwrite(file_handle_t& file, u8 const* data, u64 size, s64 offset)
        {
            // check
            if (file.m_handle == nullptr || data == nullptr)
                return -1;
            if (size == 0)
                return 0;

            // save offset
            s64 current = file_offset(file);
            if (!(current >= 0))
                return -1;

            // seek it
            if (current != offset && file_seek(file, offset, SEEK_MODE_BEG) != offset)
                return -1;

            // write it
            s64 real = file_write(file, data, size);

            // restore offset
            if (current != offset && file_seek(file, current, SEEK_MODE_BEG) != current)
                return -1;

            // ok
            return real;
        }


        s64 file_seek(file_handle_t& file, s64 offset, seek_mode_t mode)
        {
            // check
            if (file.m_handle == nullptr)
                return -1;

            // seek
            LARGE_INTEGER o = {{0}};
            LARGE_INTEGER p = {{0}};
            o.QuadPart      = (LONGLONG)offset;
            return SetFilePointerEx((HANDLE)file.m_handle, o, &p, (DWORD)mode) ? (s64)p.QuadPart : -1;
        }

        s64 file_offset(file_handle_t file)
        {
            // check
            if (file.m_handle == nullptr)
                return -1;

            // the file size
            return file_seek(file, (s64)0, SEEK_MODE_CUR);
        }

		s64 file_size(file_handle_t file)
        {
            // check
            if (file.m_handle == nullptr)
                return -1;

            LARGE_INTEGER size = {{0}};
            size.LowPart       = ::GetFileSize((HANDLE)file.m_handle, (LPDWORD)&size.HighPart);
            return size.QuadPart;
        }


    } // namespace xfile
} // namespace ncore

#endif
