#include "xbase/x_memory.h"

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

#include "xfile/private/x_path.h"
#include "xfile/private/x_directory.h"
#include "xfile/private/x_assert.h"

namespace xcore
{
    namespace xfile
    {
        // the path separator
        #if defined(TARGET_OS_WINDOWS)
        #   define PATH_SEPARATOR            '\\'
        #else
        #   define PATH_SEPARATOR            '/'
        #endif

        // is path separator?
        #if defined(TARGET_OS_WINDOWS)
        #   define path_is_separator(c)      ('/' == (c) || '\\' == (c))
        #else
        #   define path_is_separator(c)      ('/' == (c))
        #endif

        /* //////////////////////////////////////////////////////////////////////////////////////
        * implementation
        */
        #ifndef TB_CONFIG_MICRO_ENABLE
        u64 path_translate(char* path, u64 size, u64 maxn)
        {
            // check
            assert_and_check_return_val(path, 0);

            // file://?
            char* p = path;
            if (!strnicmp(p, "file:", 5)) p += 5;
            // is user directory?
            else if (path[0] == '~')
            {
                // get the home directory
                char home[cPathMaxN];
                u64 home_size = directory_home(home, sizeof(home) - 1);
                assert_and_check_return_val(home_size, 0);

                // check the path space
                u64 path_size = size? size : strlen(path);
                assert_and_check_return_val(home_size + path_size - 1 < maxn, 0);

                // move the path and ensure the enough space for the home directory
                xmem::memmove(path + home_size, path + 1, path_size - 1);

                // copy the home directory
                xmem::memcpy(path, home, home_size);
                path[home_size + path_size - 1] = '\0';
            }

            // remove repeat separator
            char*  q = path;
            u64   repeat = 0;
            for (; *p; p++)
            {
                if (path_is_separator(*p))
                {
                    // save the separator if not exists
                    if (!repeat) *q++ = PATH_SEPARATOR;

                    // repeat it
                    repeat++;
                }
                else
                {
                    // save character
                    *q++ = *p;

                    // clear repeat
                    repeat = 0;
                }
            }

            // remove the tail separator and not root: '/'
            if (q > path + 1 && *(q - 1) == PATH_SEPARATOR) q--;

            // end
            *q = '\0';

            // is windows path?
            if (q > path + 1 && is_alpha(path[0]) && path[1] == ':')
            {
                // get the upper drive prefix
                path[0] = to_upper(path[0]);

                // root? patch "x:" => "x:\"
                if (q == path + 2 && q + 1 < path + maxn)
                {
                    *q++ = PATH_SEPARATOR;
                    *q = '\0';
                }
            }

            // ok
            return q - path;
        }
        #endif

        bool path_is_absolute(char const* path)
        {
            // check
            assert_and_check_return_val(path, false);

            // is absolute?
        #ifdef TB_CONFIG_OS_WINDOWS
            return (    path[0] == '~'
        #   ifdef TB_COMPILER_LIKE_UNIX
                    ||  path[0] == '/'
                    ||  path[0] == '\\'
                    ||  !strnicmp(path, "file:", 5)
        #   endif
                    ||  (isalpha(path[0]) && path[1] == ':'));
        #else
            return (    path[0] == '/'
                    ||  path[0] == '\\'
                    ||  path[0] == '~'
                    ||  !strnicmp(path, "file:", 5));
        #endif
        }

        char const* path_absolute(char const* path, char* data, u64 maxn)
        {
            return path_absolute_to(nullptr, path, data, maxn);
        }

        char const* path_absolute_to(char const* root, char const* path, char* data, u64 maxn)
        {
            // check
            assert_and_check_return_val(path && data && maxn, nullptr);

            // empty path?
            check_return_val(path[0], nullptr);

        #ifdef TB_CONFIG_MICRO_ENABLE

            // the path is absolute?
            if (path_is_absolute(path)) return path;

            // trace
            trace_e("absolute to %s to %s failed!", path, root);
            return nullptr;
        #else

            // the path is absolute?
            if (path_is_absolute(path))
            {
                // copy it
                strlcpy(data, path, maxn);

                // translate it
                return path_translate(data, 0, maxn)? data : nullptr;
            }

            // get the root directory
            u64 size = 0;
            if (root)
            {
                // copy it
                size = strlcpy(data, root, maxn);
                assert_and_check_return_val(size < maxn, nullptr);
            }
            else
            {
                // get the current directory
                if (!(size = directory_current(data, maxn))) return nullptr;
            }

            // translate the root directory
            size = path_translate(data, size, maxn);

            // is windows path? skip the drive prefix
            char* absolute = data;
            if (size > 2 && isalpha(absolute[0]) && absolute[1] == ':' && absolute[2] == TB_PATH_SEPARATOR)
            {
                // skip it
                absolute    += 2;
                size        -= 2;
            }

            // path => data
            char const*    p = path;
            char const*    t = p;
            char*          q = absolute + size;
            char const*    e = absolute + maxn - 1;
            while (1)
            {
                if (path_is_separator(*p) || !*p)
                {
                    // the item size
                    u64 n = p - t;

                    // ..? remove item
                    if (n == 2 && t[0] == '.' && t[1] == '.')
                    {
                        // find the last separator
                        for (; q > absolute && *q != TB_PATH_SEPARATOR; q--) ;

                        // strip it
                        *q = '\0';
                    }
                    // .? continue it
                    else if (n == 1 && t[0] == '.') ;
                    // append item
                    else if (n && q + 1 + n < e)
                    {
                        // append separator
                        *q++ = TB_PATH_SEPARATOR;

                        // append item
                        strncpy(q, t, n);
                        q += n;

                        // strip it
                        *q = '\0';
                    }
                    // empty item? remove repeat
                    else if (!n) ;
                    // too small?
                    else
                    {
                        // trace
                        return nullptr;
                    }

                    // break
                    check_break(*p);

                    // next
                    t = p + 1;
                }

                // next
                p++;
            }

            // end
            if (q > absolute) *q = '\0';
            // root?
            else
            {
                *q++ = TB_PATH_SEPARATOR;
                *q = '\0';
            }

            // ok?
            return data;
        #endif
        }

        #ifndef TB_CONFIG_MICRO_ENABLE

        char const* path_relative(char const* path, char* data, u64 maxn)
        {
            return path_relative_to(nullptr, path, data, maxn);
        }

        char const* path_relative_to(char const* root, char const* path, char* data, u64 maxn)
        {
            // check
            assert_and_check_return_val(path && data && maxn, nullptr);

            // the root is the current and the path is absolute? return path directly
            if (!root && !path_is_absolute(path))
            {
                // copy it
                strlcpy(data, path, maxn);

                // translate it
                return path_translate(data, 0, maxn)? data : nullptr;
            }

            // get the absolute path
            u64 path_size = 0;
            char path_absolute[cPathMaxN];
            u64 path_maxn = sizeof(path_absolute);
            path        = path_absolute(path, path_absolute, path_maxn);
            path_size   = strlen(path);
            assert_and_check_return_val(path && path_size && path_size < path_maxn, nullptr);

            // get the absolute root
            u64 root_size = 0;
            char root_absolute[cPathMaxN];
            u64 root_maxn = sizeof(root_absolute);
            if (root)
            {
                // get the absolute root
                root        = path_absolute(root, root_absolute, root_maxn);
                root_size   = strlen(root);
            }
            else
            {
                // get the current directory
                if (!(root_size = directory_current(root_absolute, root_maxn))) return nullptr;

                // translate it
                if (!(root_size = path_translate(root_absolute, root_size, root_maxn))) return nullptr;
                root = root_absolute;
            }
            assert_and_check_return_val(root && root_size && root_size < root_maxn, nullptr);

            // same directory? return "."
            if (path_size == root_size && !strncmp(path, root, root_size))
            {
                // check
                assert_and_check_return_val(maxn >= 2, ".");

                // return "."
                data[0] = '.';
                data[1] = '\0';
                return data;
            }

            // append separator
            if (path_size + 1 < path_maxn)
            {
                path_absolute[path_size++] = PATH_SEPARATOR;
                path_absolute[path_size] = '\0';
            }
            if (root_size + 1 < root_maxn)
            {
                root_absolute[root_size++] = PATH_SEPARATOR;
                root_absolute[root_size] = '\0';
            }

            // trace
            trace_d("path: %s, root: %s", path_absolute, root_absolute);

            // find the common leading directory
            char const*    p = path_absolute;
            char const*    q = root_absolute;
            s64           last = -1;
            for (; *p && *q && *p == *q; q++, p++)
            {
                // save the last separator
                if (*p == PATH_SEPARATOR) last = q - root_absolute;
            }

            // is different directory or outside the windows drive root? using the absolute path
            if (last <= 0 || (last == 2 && root_absolute[1] == ':' && root_size > 3))
            {
                // trace
                trace_d("no common root: %d", last);

                // the path size
                u64 size = min(path_size - 1, maxn);

                // copy it
                strncpy(data, path, size);
                data[size] = '\0';
            }
            // exists same root?
            else
            {
                // count the remaining levels in root
                u64 count = 0;
                char const* l = root_absolute + last + 1;
                for (; *l; l++)
                {
                    if (*l == PATH_SEPARATOR) count++;
                }

                // append "../" or "..\\"
                char* d = data;
                char* e = data + maxn;
                while (count--)
                {
                    if (d + 3 < e)
                    {
                        d[0] = '.';
                        d[1] = '.';
                        d[2] = PATH_SEPARATOR;
                        d += 3;
                    }
                }

                // append the left path
                l = path_absolute + last + 1;
                while (*l && d < e) *d++ = *l++;

                // remove the last separator
                if (d > data) d--;

                // end
                *d = '\0';
            }

            // trace
            trace_d("relative: %s", data);

            // ok?
            return data;
        }
        #endif

        char const* path_directory(char const* path, char* data, u64 maxn)
        {
            // check
            assert_and_check_return_val(path && data && maxn, nullptr);

            // find the last path separator
            u64 n = strlen(path);
            char const* p = path + n - 1;
            while (p >= path && *p)
            {
                // found
                if (path_is_separator(*p))
                {
                    n = p - path;
                    if (n < maxn)
                    {
                        strncpy(data, path, n);
                        data[n] = '\0';
                        return data;
                    }
                    else return nullptr;
                }
                p--;
            }

            // end
            return ".";
        }

    }
}
