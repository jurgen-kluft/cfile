#include "xbase/x_target.h"

#if defined TARGET_PC

#    include "ole32.h"
#    include "user32.h"
#    include "ws2_32.h"
#    include "mswsock.h"
#    include "shell32.h"
#    include "dbghelp.h"
#    include "kernel32.h"
#    include "iphlpapi.h"

#elif defined TARGET_MAC

#    include <sys/types.h>
#    include <sys/stat.h>
#    include <fcntl.h>
#    include <stdio.h>
#    include <dirent.h>
#    include <unistd.h>
#    include <errno.h>

#endif

#include "xfile/private/x_directory.h"
#include "xfile/private/x_path.h"
#include "xfile/private/x_file.h"
#include "xfile/private/x_assert.h"

namespace xcore
{
    namespace xfile
    {

#if defined TARGET_PC
        bool directory_create(char const* path)
        {
            // check
            assert_and_check_return_val(path, false);

            // the absolute path
            wchar full[cPathMaxN];
            if (!path_absolute_w(path, full, cPathMaxN)) return false;

            // make it
            bool ok = CreateDirectoryW(full, nullptr)? true : false;
            if (!ok)
            {
                // make directory
                wchar_t          temp[cPathMaxN] = {0};
                wchar_t const*   p = full;
                wchar_t*         t = temp;
                wchar_t const*   e = temp + cPathMaxN - 1;
                for (; t < e && *p; t++)
                {
                    *t = *p;
                    if (*p == L'\\' || *p == L'/')
                    {
                        // make directory if not exists
                        if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(temp)) CreateDirectoryW(temp, nullptr);

                        // skip repeat '\\' or '/'
                        while (*p && (*p == L'\\' || *p == L'/')) p++;
                    }
                    else p++;
                }

                // make it again
                ok = CreateDirectoryW(full, nullptr)? true : false;
            }

            // ok?
            return ok;
        }


        size_t directory_home(char_t* path, size_t maxn)
        {
            // check
            assert_and_check_return_val(path && maxn, 0);

            // the home directory
            bool_t   ok = false;
            handle_t pidl = null;
            wchar_t  home[TB_PATH_MAXN] = {0};
            do
            {
                /* get the local appdata folder location
                *
                * CSIDL_APPDATA 0x1a
                * CSIDL_LOCAL_APPDATA 0x1c
                */
                if (S_OK != shell32()->SHGetSpecialFolderLocation(null, 0x1c /* CSIDL_LOCAL_APPDATA */, &pidl)) break;
                check_break(pidl);

                // get the home directory
                if (!shell32()->SHGetPathFromIDListW(pidl, home)) break;

                // ok
                ok = true;

            } while (0);

            // exit pidl
            if (pidl) GlobalFree(pidl);
            pidl = null;

            // wtoa
            xsize_t size = ok? wtoa(path, home, maxn) : 0;

            // ok?
            return size != -1? size : 0;
        }        
#endif

#if defined TARGET_MAC

        bool directory_create(char const* path)
        {
            // check
            assert_and_check_return_val(path, false);

            // the full path
            char full[cPathMaxN];
            path = path_absolute(path, full, cPathMaxN);
            assert_and_check_return_val(path, false);

            // make it (0755: drwxr-xr-x)
            bool ok = !mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
            if (!ok && (errno != EPERM && errno != EACCES))
            {
                // make directory
                char        temp[cPathMaxN] = {0};
                char const* p               = full;
                char*       t               = temp;
                char const* e               = temp + cPathMaxN - 1;
                for (; t < e && *p; t++)
                {
                    *t = *p;
                    if (*p == '/')
                    {
                        // make directory if not exists
                        if (!file_info(temp, nullptr))
                        {
                            if (mkdir(temp, S_IRWXU | S_IRWXG | S_IRWXO) != 0)
                                return false;
                        }

                        // skip repeat '/'
                        while (*p && *p == '/')
                            p++;
                    }
                    else
                        p++;
                }

                // make it again
                ok = !mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
            }
            return ok;
        }

#endif

    } // namespace xfile
} // namespace xcore