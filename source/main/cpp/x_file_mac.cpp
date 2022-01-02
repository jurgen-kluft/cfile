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

#    include "xbase/x_integer.h"
#    include "xbase/x_memory.h"
#    include "xbase/x_runes.h"
#    include "xfile/private/x_file.h"

namespace xcore
{
    namespace xfile
    {
        const s32 cMaxPath = 512;

#    define fd2fh(fd) ((fd) >= 0 ? file_handle_t((void*)((s64)(fd) + 1)) : file_handle_t(nullptr))
#    define fh2fd(fh) (s64)((fh.m_handle) ? (((s64)(fh.m_handle)) - 1) : -1)

        file_handle_t file_open(crunes_t const& path, file_mode_t mode)
        {
            // check
            if (!path.is_valid())
                return file_handle_t();

            if (!path.is_valid())
                return file_handle_t();

            // flags
            u64 flags = 0;
            if ((mode & FILE_MODE_RW) == FILE_MODE_RW)
                flags |= O_RDWR;
            else if (mode & FILE_MODE_WRITE)
                flags |= O_WRONLY;
            else if (mode & FILE_MODE_READ)
                flags |= O_RDONLY;

            if (mode & FILE_MODE_CREATE)
                flags |= O_CREAT;
            if (mode & FILE_MODE_APPEND)
                flags |= O_APPEND;
            if (mode & FILE_MODE_TRUNC)
                flags |= O_TRUNC;

            // dma mode, no cache
#    ifdef PLATFORM_LINUX
            if (mode & FILE_MODE_DIRECT)
                flags |= O_DIRECT;
#    endif

            // noblock
            flags |= O_NONBLOCK;

            // modes
            u64 modes = 0;
            if (mode & FILE_MODE_CREATE)
            {
                // 0644: -rw-r--r--
                modes = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            }

            // if path.m_type != utf8::TYPE
            // convert to utf8?

            // open it, @note need absolute path
            s64 fd = open(path.m_ascii.m_str, flags, modes);
            if (fd < 0 && (mode & FILE_MODE_CREATE) && (errno != EPERM && errno != EACCES))
            {
                // open it again after creating the file directory
                //char dir[cMaxPath];
                //if (directory_create(path_directory(path, dir, sizeof(dir))))
                //    fd = open(path, flags, modes);
            }

            if (!(fd >= 0))
                return file_handle_t();

            // ok?
            return fd2fh(fd);
        }

        bool file_close(file_handle_t& file)
        {
            if (file.m_handle == nullptr)
                return -1;

            bool ok = !close(fh2fd(file)) ? true : false;
            return ok;
        }

		void file_flush(file_handle_t& file)
		{
			if (file.m_handle == nullptr)
				return;

			// flush?
		}

        s64 file_read(file_handle_t& file, xbyte* data, u64 size)
        {
            // check
            if (file.m_handle == nullptr || data == nullptr)
                return -1;

            // read it
            return read(fh2fd(file), data, size);
        }

        s64 file_write(file_handle_t& file, xbyte const* data, u64 size)
        {
            // check
            if (file.m_handle == nullptr || data == nullptr)
                return -1;

            // writ it
            return write(fh2fd(file), data, size);
        }


		s64 file_pread(file_handle_t& file, xbyte* data, u64 size, s64 offset)
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

		s64 file_pwrite(file_handle_t& file, xbyte const* data, u64 size, s64 offset)
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
            return lseek(fh2fd(file), offset, mode);
        }

        s64 file_offset(file_handle_t file)
        {
            // check
            if (file.m_handle == nullptr)
                return -1;
            // the offset
            return file_seek(file, (s64)0, SEEK_MODE_CUR);
        }

        s64 file_size(file_handle_t file)
        {
            // check
            if (file.m_handle==nullptr)
                return 0;

            // the file size
            u64         size = 0;
            struct stat st   = {0};
            if (!fstat(fh2fd(file), &st))
                size = st.st_size;

            // ok?
            return size;
        }


    } // namespace xfile
} // namespace xcore

#endif
