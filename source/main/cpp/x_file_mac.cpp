#include "xbase/x_target.h"

#if defined TARGET_MAC

#    include <sys/types.h>
#    include <sys/stat.h>
#    include <fcntl.h>
#    include <stdio.h>
#    include <stdlib.h>
#    include <sys/uio.h>
#    include <unistd.h>
#    include <errno.h>

#    ifdef CONFIG_POSIX_HAVE_COPYFILE
#        include <copyfile.h>
#    endif
#    ifdef CONFIG_POSIX_HAVE_SENDFILE
#        include <sys/sendfile.h>
#    endif

#    include "xbase/x_integer.h"
#    include "xbase/x_memory.h"
#    include "xbase/x_runes.h"
#    include "xfile/private/x_file.h"
#    include "xfile/private/x_path.h"
#    include "xfile/private/x_assert.h"

namespace xcore
{
    namespace xfile
    {
        const s32 cMaxPath = 512;

#    define fd2fh(fd) ((fd) >= 0 ? file_handle_t((void*)((s64)(fd) + 1)) : file_handle_t(nullptr))
#    define fh2fd(fh) (s64)((fh.m_handle) ? (((s64)(fh.m_handle)) - 1) : -1)

        file_handle_t file_open(char const* path, file_mode_t mode)
        {
            // check
            assert_and_check_return_val(path, file_handle_t());

            // the full path
            char full[cMaxPath];
            path = path_absolute(path, full, cMaxPath);
            assert_and_check_return_val(path, file_handle_t());

            // flags
            u64 flags = 0;
            if (mode & MODE_RO)
                flags |= O_RDONLY;
            else if (mode & MODE_WO)
                flags |= O_WRONLY;
            else if (mode & MODE_RW)
                flags |= O_RDWR;

            if (mode & MODE_CREAT)
                flags |= O_CREAT;
            if (mode & MODE_APPEND)
                flags |= O_APPEND;
            if (mode & MODE_TRUNC)
                flags |= O_TRUNC;

                // dma mode, no cache
#    ifdef TB_CONFIG_OS_LINUX
            if (mode & TB_FILE_MODE_DIRECT)
                flags |= O_DIRECT;
#    endif

            // noblock
            flags |= O_NONBLOCK;

            // modes
            u64 modes = 0;
            if (mode & MODE_CREAT)
            {
                // 0644: -rw-r--r--
                modes = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            }

            // open it, @note need absolute path
            s64 fd = open(path, flags, modes);
            if (fd < 0 && (mode & MODE_CREAT) && (errno != EPERM && errno != EACCES))
            {
#    ifndef TB_CONFIG_MICRO_ENABLE
                // open it again after creating the file directory
                char dir[cMaxPath];
                if (directory_create(path_directory(path, dir, sizeof(dir))))
                    fd = open(path, flags, modes);
#    endif
            }
            check_return_val(fd >= 0, file_handle_t());

            // ok?
            return fd2fh(fd);
        }

        bool file_exit(file_handle_t& file)
        {
            assert_and_check_return_val(file.m_handle, false);
            bool ok = !close(fh2fd(file)) ? true : false;
            return ok;
        }

        s64 file_read(file_handle_t& file, xbyte* data, u64 size)
        {
            // check
            assert_and_check_return_val(file.m_handle && data, -1);

            // read it
            return read(fh2fd(file), data, size);
        }

        s64 file_write(file_handle_t& file, xbyte const* data, u64 size)
        {
            // check
            assert_and_check_return_val(file.m_handle && data, -1);

            // writ it
            return write(fh2fd(file), data, size);
        }

        bool tb_file_sync(file_handle_t& file)
        {
            // check
            assert_and_check_return_val(file.m_handle, false);

            // sync
#    ifdef TB_CONFIG_POSIX_HAVE_FDATASYNC
            return !fdatasync(fh2fd(file)) ? true : false;
#    else
            return !fsync(fh2fd(file)) ? true : false;
#    endif
        }

        s64 file_seek(file_handle_t& file, s64 offset, u64 mode)
        {
            // check
            assert_and_check_return_val(file.m_handle, -1);

            // seek
            return lseek(fh2fd(file), offset, mode);
        }

        s64 file_offset(file_handle_t& file)
        {
            // check
            assert_and_check_return_val(file.m_handle, -1);

            // the offset
            return file_seek(file, (s64)0, SEEK_CUR);
        }

        u64 file_size(file_handle_t file)
        {
            // check
            assert_and_check_return_val(file.m_handle, 0);

            // the file size
            u64         size = 0;
            struct stat st   = {0};
            if (!fstat(fh2fd(file), &st))
                size = st.st_size;

            // ok?
            return size;
        }

        bool file_info(char const* path, file_info_t* info)
        {
            // check
            assert_and_check_return_val(path, false);

            // the full path (need translate "~/")
            char full[cMaxPath];
            path = path_absolute(path, full, cMaxPath);
            assert_and_check_return_val(path, false);

            // exists?
            check_return_val(!access(path, F_OK), false);

            // get info
            if (info)
            {
                // init info
                xmem::memset(info, 0, sizeof(file_info_t));

                // get stat
#    ifdef TB_CONFIG_POSIX_HAVE_STAT64
                struct stat64 st = {0};
                if (!stat64(path, &st))
#    else
                struct stat st = {0};
                if (!stat(path, &st))
#    endif
                {
                    // file type
                    if (S_ISDIR(st.st_mode))
                        info->type = TYPE_DIRECTORY;
                    else
                        info->type = TYPE_FILE;

                    // file size
                    info->size = st.st_size >= 0 ? (u64)st.st_size : 0;

                    // the last access time
                    info->atime = (time_t)st.st_atime;

                    // the last modify time
                    info->mtime = (time_t)st.st_mtime;
                }
            }

            // ok
            return true;
        }

#    ifndef TB_CONFIG_MICRO_ENABLE
        s64 file_pread(file_handle_t& file, xbyte* data, u64 size, u64 offset)
        {
            // check
            assert_and_check_return_val(file.m_handle, -1);

            // read it
#        ifdef TB_CONFIG_POSIX_HAVE_PREAD64
            return pread64(fh2fd(file), data, (size_t)size, offset);
#        else
            return pread(fh2fd(file), data, (size_t)size, offset);
#        endif
        }

        s64 file_pwrite(file_handle_t& file, xbyte const* data, u64 size, u64 offset)
        {
            // check
            assert_and_check_return_val(file.m_handle, -1);

            // writ it
#        ifdef TB_CONFIG_POSIX_HAVE_PWRITE64
            return pwrite64(fh2fd(file), data, (size_t)size, offset);
#        else
            return pwrite(fh2fd(file), data, (size_t)size, offset);
#        endif
        }

        s64 file_readv(file_handle_t& file, iovec_t const* list, u64 size)
        {
            // check
            assert_and_check_return_val(file.m_handle && list && size, -1);

            // check iovec
            ASSERTCT(sizeof(iovec_t) == sizeof(struct iovec));
            ASSERT(memberof_eq(iovec_t, data, struct iovec, iov_base));
            ASSERT(memberof_eq(iovec_t, size, struct iovec, iov_len));

            // read it
            return readv(fh2fd(file), (struct iovec const*)list, size);
        }

        s64 file_writev(file_handle_t& file, iovec_t const* list, u64 size)
        {
            // check
            assert_and_check_return_val(file.m_handle && list && size, -1);

            // check iovec
            ASSERTCT(sizeof(iovec_t) == sizeof(struct iovec));
            ASSERT(memberof_eq(iovec_t, data, struct iovec, iov_base));
            ASSERT(memberof_eq(iovec_t, size, struct iovec, iov_len));

            // writ it
            return writev(fh2fd(file), (struct iovec const*)list, size);
        }

        s64 file_preadv(file_handle_t& file, iovec_t const* list, u64 size, u64 offset)
        {
            // check
            assert_and_check_return_val(file.m_handle && list && size, -1);

            // check iovec
            ASSERTCT(sizeof(iovec_t) == sizeof(struct iovec));
            ASSERT(memberof_eq(iovec_t, data, struct iovec, iov_base));
            ASSERT(memberof_eq(iovec_t, size, struct iovec, iov_len));

            // read it
#        ifdef TB_CONFIG_POSIX_HAVE_PREADV
            return preadv(fh2fd(file), (struct iovec const*)list, size, offset);
#        else

            // FIXME: lock it

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
#        endif
        }

        s64 file_pwritev(file_handle_t& file, iovec_t const* list, u64 size, u64 offset)
        {
            // check
            assert_and_check_return_val(file.m_handle && list && size, -1);

            // check iovec
            ASSERTCT(sizeof(iovec_t) == sizeof(struct iovec));
            ASSERT(memberof_eq(iovec_t, data, struct iovec, iov_base));
            ASSERT(memberof_eq(iovec_t, size, struct iovec, iov_len));

            // writ it
#        ifdef TB_CONFIG_POSIX_HAVE_PWRITEV
            return pwritev(fh2fd(file), (struct iovec const*)list, size, offset);
#        else

            // FIXME: lock it

            // save offset
            s64 current = file_offset(file);
            assert_and_check_return_val(current >= 0, -1);

            // seek it
            if (current != offset && file_seek(file, offset, SEEK_BEG) != offset)
                return -1;

            // writ it
            s64 real = file_writev(file, list, size);

            // restore offset
            if (current != offset && file_seek(file, current, SEEK_BEG) != current)
                return -1;

            // ok
            return real;
#        endif
        }

        bool file_copy(char const* path, char const* dest)
        {
            // check
            assert_and_check_return_val(path && dest, false);

#        ifdef TB_CONFIG_POSIX_HAVE_COPYFILE

            // the full path
            char full0[cMaxPath];
            path = tb_path_absolute(path, full0, cMaxPath);
            assert_and_check_return_val(path, false);

            // the dest path
            char full1[cMaxPath];
            dest = tb_path_absolute(dest, full1, cMaxPath);
            assert_and_check_return_val(dest, false);

            // attempt to copy it directly
            if (!copyfile(path, dest, 0, COPYFILE_ALL))
                return true;
            else if (errno != EPERM && errno != EACCES)
            {
                // attempt to copy it again after creating directory
                char dir[cMaxPath];
                if (tb_directory_create(tb_path_directory(dest, dir, sizeof(dir))))
                    return !copyfile(path, dest, 0, COPYFILE_ALL);
            }

            // failed
            return false;
#        else
            s32  ifd = -1;
            s32  ofd = -1;
            bool ok  = false;
            do
            {
                // get the absolute source path
                char data[8192];
                path = path_absolute(path, data, sizeof(data));
                assert_and_check_break(path);

                // get stat.st_mode first
#            ifdef TB_CONFIG_POSIX_HAVE_STAT64
                struct stat64 st = {0};
                if (stat64(path, &st))
                    break;
#            else
                struct stat st = {0};
                if (stat(path, &st))
                    break;
#            endif

                // open source file
                ifd = open(path, O_RDONLY);
                check_break(ifd >= 0);

                // get the absolute source path
                dest = path_absolute(dest, data, sizeof(data));
                assert_and_check_break(dest);

                // open destinate file and copy file mode
                ofd = open(dest, O_RDWR | O_CREAT | O_TRUNC, st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
                if (ofd < 0 && (errno != EPERM && errno != EACCES))
                {
                    // attempt to open it again after creating directory
                    char dir[cMaxPath];
                    if (directory_create(path_directory(dest, dir, sizeof(dir))))
                        ofd = open(dest, O_RDWR | O_CREAT | O_TRUNC, st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
                }
                check_break(ofd >= 0);

                // get file size
                u64 size = file_size(fd2fh(ifd));

                // init write size
                u64 writ = 0;

                // attempt to copy file using `sendfile`
#            ifdef CONFIG_POSIX_HAVE_SENDFILE
                while (writ < size)
                {
                    off_t seek = writ;
                    s64   real = sendfile(ofd, ifd, &seek, (size_t)(size - writ));
                    if (real > 0)
                        writ += real;
                    else
                        break;
                }

                /* attempt to copy file directly if sendfile failed
                 *
                 * sendfile() supports regular file only after "since Linux 2.6.33".
                 */
                if (writ != size)
                {
                    lseek(ifd, 0, SEEK_SET);
                    lseek(ofd, 0, SEEK_SET);
                }
                else
                {
                    ok = true;
                    break;
                }
#            endif

                // copy file using `read` and `write`
                writ = 0;
                while (writ < size)
                {
                    // read some data
                    s32 real = read(ifd, data, (size_t)xmin(size - writ, sizeof(data)));
                    if (real > 0)
                    {
                        real = write(ofd, data, real);
                        if (real > 0)
                            writ += real;
                        else
                            break;
                    }
                    else
                        break;
                }

                // ok?
                ok = (writ == size);

            } while (0);

            // close source file
            if (ifd >= 0)
                close(ifd);
            ifd = -1;

            // close destinate file
            if (ofd >= 0)
                close(ofd);
            ofd = -1;

            // ok?
            return ok;
#        endif
        }

        bool file_create(char const* path)
        {
            // check
            assert_and_check_return_val(path, false);

            // make it
            file_handle_t file = file_open(path, file_mode_t(MODE_CREAT | MODE_WO | MODE_TRUNC));
            if (file.m_handle)
                file_exit(file);

            // ok?
            return file.m_handle ? true : false;
        }

        bool file_remove(char const* path)
        {
            // check
            assert_and_check_return_val(path, false);

            // the full path
            char full[cMaxPath];
            path = path_absolute(path, full, cMaxPath);
            assert_and_check_return_val(path, false);

            // remove it
            return !remove(path) ? true : false;
        }

        bool file_rename(char const* path, char const* dest)
        {
            // check
            assert_and_check_return_val(path && dest, false);

            // the full path
            char full0[cMaxPath];
            path = path_absolute(path, full0, cMaxPath);
            assert_and_check_return_val(path, false);

            // the dest path
            char full1[cMaxPath];
            dest = path_absolute(dest, full1, cMaxPath);
            assert_and_check_return_val(dest, false);

            // attempt to rename it directly
            if (!rename(path, dest))
                return true;
            else if (errno != EPERM && errno != EACCES)
            {
                // attempt to rename it again after creating directory
                char dir[cMaxPath];
                if (directory_create(path_directory(dest, dir, sizeof(dir))))
                    return !rename(path, dest);
            }
            return false;
        }

        bool file_access(char const* path, u64 mode)
        {
            // check
            assert_and_check_return_val(path, false);

            // the full path
            char full[cMaxPath];
            path = path_absolute(path, full, cMaxPath);
            assert_and_check_return_val(path, false);

            // flags
            u64 flags = 0;
            if (mode & MODE_RW)
                flags = R_OK | W_OK;
            else
            {
                if (mode & MODE_RO)
                    flags |= R_OK;
                if (mode & MODE_WO)
                    flags |= W_OK;
            }
            if (mode & MODE_EXEC)
                flags |= X_OK;

            return !access(full, flags);
        }
#    endif

    } // namespace xfile
} // namespace xcore

#endif