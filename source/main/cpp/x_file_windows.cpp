#include "xbase/x_target.h"

#if defined TARGET_PC

#    define WIN32_LEAN_AND_MEAN
#    define NOGDI
#    define NOKANJI
#    include <windows.h>
#    include <stdio.h>

#    include "xbase/x_runes.h"
#    include "xbase/x_memory.h"
#    include "xfile/x_file.h"

namespace xcore
{
    namespace xfile
    {
        const s32 cMaxPath = 512;

#    define assert_and_check_return_val(condition, returnvalue) \
        do                                                      \
        {                                                       \
            if (!(condition))                                   \
            {                                                   \
                ASSERT(false);                                  \
                return (returnvalue);                           \
            }                                                   \
        } while (false);
#    define check_return_val(check, returnvalue) \
        do                                       \
        {                                        \
            if ((check) == 0)                    \
                return (returnvalue);            \
        } while (false);

        static inline time_t filetime_to_time(FILETIME ft)
        {
            ULARGE_INTEGER ui = {{0}};
            ui.LowPart        = ft.dwLowDateTime;
            ui.HighPart       = ft.dwHighDateTime;
            return (time_t)((LONGLONG)(ui.QuadPart - 116444736000000000ull) / 10000000ul);
        }

        // get absolute path for wchar
        static inline wchar const* path_absolute_w(char const* path, wchar* full, u64 maxn)
        {
            // get absolute path
            char data[cMaxPath] = {0};
            path                = path_absolute(path, data, cMaxPath);
            tb_check_return_val(path, tb_null);

            /* we need deal with files with a name longer than 259 characters
             * @see https://stackoverflow.com/questions/5188527/how-to-deal-with-files-with-a-name-longer-than-259-characters
             */
            s32 size = ascii::strlen(path);
            if (size >= cMaxPath)
            {
                char* e = data + size - 1;
                if (e + 5 < data + sizeof(data))
                {
                    e[5] = '\0';
                    while (e >= data)
                    {
                        e[4] = *e;
                        e--;
                    }
                    data[0] = '\\';
                    data[1] = '\\';
                    data[2] = '?';
                    data[3] = '\\';
                    path    = data;
                }
                else
                    return nullptr;
            }

            // atow
            return atow(full, path, maxn) != (u64)-1 ? full : tb_null;
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
                    if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(temp))
                        CreateDirectoryW(temp, nullptr);

                    // skip repeat '\\' or '/'
                    while (*p && (*p == L'\\' || *p == L'/'))
                        p++;
                }
                else
                    p++;
            }
        }

        file_handle_t file_open(char const* path, file_mode_t mode)
        {
            // check
            assert_and_check_return_val(path, nullptr);

            // the full path
            wchar full[cMaxPath];
            if (!path_absolute_w(path, full, cMaxPath))
                return nullptr;

            // init access
            DWORD access = GENERIC_READ;
            if (mode & MODE_RO)
                access = GENERIC_READ;
            else if (mode & MODE_WO)
                access = GENERIC_WRITE;
            else if (mode & MODE_RW)
                access = GENERIC_READ | GENERIC_WRITE;

            // init share
            DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;

            // init flag
            DWORD cflag = 0;
            if (mode & MODE_CREAT)
            {
                // always create a new empty file
                if (mode & MODE_TRUNC)
                    cflag |= CREATE_ALWAYS;
                // create or open and append file
                else if (mode & MODE_APPEND)
                    cflag |= OPEN_ALWAYS;
                // create a new file only if file not exists
                else
                    cflag |= CREATE_NEW;
            }
            // open and truncate an existing file
            else if (mode & MODE_TRUNC)
                cflag |= TRUNCATE_EXISTING;
            // open an existing file
            if (!cflag)
                cflag |= OPEN_EXISTING;

            // init attr
            DWORD attr = FILE_ATTRIBUTE_NORMAL;
            if (mode & MODE_DIRECT)
                attr |= FILE_FLAG_NO_BUFFERING;

            // init file
            HANDLE file = CreateFileW(full, access, share, nullptr, cflag, attr, nullptr);
            if (file == INVALID_HANDLE_VALUE && (mode & MODE_CREAT))
            {
                // make directory
                mkdir(full);

                // init it again
                file = CreateFileW(full, access, share, nullptr, cflag, attr, nullptr);
            }

            // append?
            if (file != INVALID_HANDLE_VALUE && (mode & MODE_APPEND))
            {
                // seek to end
                s64 size = file_size((file_handle_t)file);
                if (size)
                {
                    if (size != file_seek((file_handle_t)file, size, SEEK_BEG))
                    {
                        file_exit((file_handle_t)file);
                        file = INVALID_HANDLE_VALUE;
                    }
                }
            }

            file_handle_t handle;
            handle.m_handle = file != INVALID_HANDLE_VALUE ? (file_handle_t)file : nullptr;

            return handle;
        }

        bool file_exit(file_handle_t& file)
        {
            // check
            assert_and_check_return_val(file, false);

            // close it
            return CloseHandle((HANDLE)file.m_handle) ? true : false;
        }
        s64 file_read(file_handle_t& file, byte* data, u64 size)
        {
            // check
            assert_and_check_return_val(file && data, -1);
            check_return_val(size, 0);

            // read
            DWORD real_size = 0;
            return ReadFile((HANDLE)file.m_handle, data, (DWORD)size, &real_size, nullptr) ? (s64)real_size : -1;
        }
        s64 file_write(file_handle_t& file, byte const* data, u64 size)
        {
            // check
            assert_and_check_return_val(file && data, -1);
            check_return_val(size, 0);

            // write
            DWORD real_size = 0;
            return WriteFile((HANDLE)file.m_handle, data, (DWORD)size, &real_size, nullptr) ? (s64)real_size : -1;
        }
        s64 file_pread(file_handle_t& file, byte* data, u64 size, s64 offset)
        {
            // check
            assert_and_check_return_val(file && data, -1);

            // save offset
            s64 current = file_offset(file);
            assert_and_check_return_val(current >= 0, -1);

            // seek it
            if (current != offset && file_seek(file, offset, SEEK_BEG) != offset)
                return -1;

            // read it
            s64 real = file_read(file, data, size);

            // restore offset
            if (current != offset && file_seek(file, current, SEEK_BEG) != current)
                return -1;

            // ok
            return real;
        }
        s64 file_pwrite(file_handle_t& file, byte const* data, u64 size, s64 offset)
        {
            // check
            assert_and_check_return_val(file && data, -1);

            // save offset
            s64 current = file_offset(file);
            assert_and_check_return_val(current >= 0, -1);

            // seek it
            if (current != offset && file_seek(file, offset, SEEK_BEG) != offset)
                return -1;

            // write it
            s64 real = file_write(file, data, size);

            // restore offset
            if (current != offset && file_seek(file, current, SEEK_BEG) != current)
                return -1;

            // ok
            return real;
        }
        s64 file_readv(file_handle_t& file, iovec_t const* list, u64 size)
        {
            // check
            assert_and_check_return_val(file && list && size, -1);

            // walk read
            u64 i    = 0;
            u64 read = 0;
            for (i = 0; i < size; i++)
            {
                // the data & size
                byte* data = list[i].data;
                u64   need = list[i].size;
                check_break(data && need);

                // read it
                s64 real = file_read(file, data, need);

                // full? next it
                if (real == need)
                {
                    read += real;
                    continue;
                }

                // failed?
                check_return_val(real >= 0, -1);

                // ok?
                if (real > 0)
                    read += real;

                // end
                break;
            }

            // ok?
            return read;
        }
        s64 file_writev(file_handle_t& file, iovec_t const* list, u64 size)
        {
            // check
            assert_and_check_return_val(file && list && size, -1);

            // walk write
            u64 i    = 0;
            u64 write = 0;
            for (i = 0; i < size; i++)
            {
                // the data & size
                byte* data = list[i].data;
                u64   need = list[i].size;
                check_break(data && need);

                // write it
                s64 real = file_write(file, data, need);

                // full? next it
                if (real == need)
                {
                    write += real;
                    continue;
                }

                // failed?
                check_return_val(real >= 0, -1);

                // ok?
                if (real > 0)
                    write += real;

                // end
                break;
            }

            // ok?
            return write;
        }

        s64 file_preadv(file_handle_t& file, iovec_t const* list, u64 size, s64 offset)
        {
            // check
            assert_and_check_return_val(file && list && size, -1);

            // save offset
            s64 current = file_offset(file);
            assert_and_check_return_val(current >= 0, -1);

            // seek it
            if (current != offset && file_seek(file, offset, SEEK_BEG) != offset)
                return -1;

            // read it
            s64 real = file_readv(file, list, size);

            // restore offset
            if (current != offset && file_seek(file, current, SEEK_BEG) != current)
                return -1;

            // ok
            return real;
        }

        s64 file_pwritev(file_handle_t& file, iovec_t const* list, u64 size, s64 offset)
        {
            // check
            assert_and_check_return_val(file && list && size, -1);

            // save offset
            s64 current = file_offset(file);
            assert_and_check_return_val(current >= 0, -1);

            // seek it
            if (current != offset && file_seek(file, offset, SEEK_BEG) != offset)
                return -1;

            // write it
            s64 real = file_writev(file, list, size);

            // restore offset
            if (current != offset && file_seek(file, current, SEEK_BEG) != current)
                return -1;

            // ok
            return real;
        }

        bool file_sync(file_handle_t& file)
        {
            // check
            assert_and_check_return_val(file, false);

            // sync it
            return FlushFileBuffers((HANDLE)file.m_handle) ? true : false;
        }

        s64 file_seek(file_handle_t& file, s64 offset, u64 mode)
        {
            // check
            assert_and_check_return_val(file, -1);

            // seek
            LARGE_INTEGER o = {{0}};
            LARGE_INTEGER p = {{0}};
            o.QuadPart      = (LONGLONG)offset;
            return SetFilePointerEx((HANDLE)file.m_handle, o, &p, (DWORD)mode) ? (s64)p.QuadPart : -1;
        }

        s64 file_offset(file_handle_t& file)
        {
            // check
            assert_and_check_return_val(file, -1);

            // the file size
            return file_seek(file, (s64)0, SEEK_CUR);
        }

        s64 file_size(file_handle_t& file)
        {
            // check
            assert_and_check_return_val(file, 0);
            LARGE_INTEGER size = {{0}};
            size.LowPart       = ::GetFileSize((HANDLE)file.m_handle, &size.HighPart);
            return size.QuadPart;
        }

        bool file_info(char const* path, file_info_t* info)
        {
            // check
            assert_and_check_return_val(path, false);

            // the full path
            wchar full[cMaxPath];
            if (!path_absolute_w(path, full, cMaxPath))
                return false;

            // get attributes
            WIN32_FILE_ATTRIBUTE_DATA st = {0};
            if (!GetFileAttributesExW(full, GetFileExInfoStandard, &st))
                return false;

            // get info
            if (info)
            {
                // init info
                memset(info, 0, sizeof(file_info_t));

                // file type
                if (st.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    info->type = TYPE_DIRECTORY;
                else if (st.dwFileAttributes != 0xffffffff)
                    info->type = TYPE_FILE;

                // file size
                info->size = ((s64)st.nFileSizeHigh << 32) | (s64)st.nFileSizeLow;

                // the last access time
                info->atime = filetime_to_time(st.ftLastAccessTime);

                // the last modify time
                info->mtime = filetime_to_time(st.ftLastWriteTime);
            }

            // ok
            return true;
        }
        bool file_copy(char const* path, char const* dest)
        {
            // check
            assert_and_check_return_val(path && dest, false);

            // the full path
            wchar full0[cMaxPath];
            if (!path_absolute_w(path, full0, cMaxPath))
                return false;

            // the dest path
            wchar full1[cMaxPath];
            if (!path_absolute_w(dest, full1, cMaxPath))
                return false;

            // copy it
            if (!CopyFileW(full0, full1, FALSE))
            {
                // make directory
                mkdir(full1);

                // copy it again
                return (bool)CopyFileW(full0, full1, FALSE);
            }

            // ok
            return true;
        }
        bool file_create(char const* path)
        {
            // check
            assert_and_check_return_val(path, false);

            // make it
            file_handle_t file = file_init(path, MODE_CREAT | MODE_WO | MODE_TRUNC);
            if (file)
                file_exit(file);

            // ok?
            return file ? true : false;
        }
        bool file_remove(char const* path)
        {
            // check
            assert_and_check_return_val(path, false);

            // the full path
            wchar full[cMaxPath];
            if (!path_absolute_w(path, full, cMaxPath))
                return false;

            // remove readonly first
            DWORD attrs = GetFileAttributesW(full);
            if (attrs & FILE_ATTRIBUTE_READONLY)
                SetFileAttributesW(full, attrs & ~FILE_ATTRIBUTE_READONLY);

            // remove it
            return DeleteFileW(full) ? true : false;
        }
        bool file_rename(char const* path, char const* dest)
        {
            // check
            assert_and_check_return_val(path && dest, false);

            // the full path
            wchar full0[cMaxPath];
            if (!path_absolute_w(path, full0, cMaxPath))
                return false;

            // the dest path
            wchar full1[cMaxPath];
            if (!path_absolute_w(dest, full1, cMaxPath))
                return false;

            // rename it
            if (!MoveFileExW(full0, full1, MOVEFILE_REPLACE_EXISTING))
            {
                // make directory
                mkdir(full1);

                // rename it again
                return MoveFileExW(full0, full1, MOVEFILE_REPLACE_EXISTING);
            }

            // ok
            return true;
        }
        bool file_access(char const* path, u64 mode)
        {
            // check
            assert_and_check_return_val(path, false);

            // the full path
            wchar full[cMaxPath];
            if (!path_absolute_w(path, full, cMaxPath))
                return false;

            // init permission
            DWORD perm = FILE_TRAVERSE | SYNCHRONIZE;
            if (mode & MODE_RW)
                perm = GENERIC_READ | GENERIC_WRITE;
            else
            {
                if (mode & MODE_RO)
                    perm |= GENERIC_READ;
                if (mode & MODE_WO)
                    perm |= GENERIC_WRITE;
            }

            // The windows POSIX implementation: _access_s and _waccess_s don't work well.
            // For example, _access_s reports the folder "C:\System Volume Information" can be accessed.
            // So using the win32 API "CreateFile" here which works much better.

            HANDLE h = CreateFileW(full, perm, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
            check_return_val(h != INVALID_HANDLE_VALUE, false);
            CloseHandle(h);

            return true;
        }
    } // namespace xfile
} // namespace xcore

#endif
