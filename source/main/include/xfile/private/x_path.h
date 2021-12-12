#ifndef __XFILE_PATH_H__
#define __XFILE_PATH_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace xcore
{
    namespace xfile
    {
        const s32 cPathMaxN = 4096;

        //! translate path to the native path
        //
        // - transform the path separator
        // - remove the repeat path separator
        // - expand the user directory with the prefix: ~
        //
        // @param path          the path
        // @param size          the path size, optional
        // @param maxn          the path maxn
        //
        // @return              true or false
        //
        xsize_t path_translate(char* path, xsize_t size, xsize_t maxn);

        //! the path is absolute?
        //
        // @param path          the path
        //
        // @return              true or false
        //
        bool path_is_absolute(char const* path);

        //! get the absolute path which relative to the current directory
        //
        // @param path          the path
        // @param data          the path data
        // @param maxn          the path maxn
        //
        // @return              the absolute path
        //
        char const* path_absolute(char const* path, char* data, xsize_t maxn);

        //! get the absolute path which relative to the given root directory
        //
        // @param root          the root path
        // @param path          the path
        // @param data          the path data
        // @param maxn          the path maxn
        //
        // @return              the absolute path
        //
        char const* path_absolute_to(char const* root, char const* path, char* data, xsize_t maxn);

        //! get the path which relative to the current directory
        //
        // @param path          the path
        // @param data          the path data
        // @param maxn          the path maxn
        //
        // @return              the relative path
        //
        char const* path_relative(char const* path, char* data, xsize_t maxn);

        //! get the path which relative to the given root directory
        //
        // @param root          the root path
        // @param path          the path
        // @param data          the path data
        // @param maxn          the path maxn
        //
        // @return              the relative path
        //
        char const* path_relative_to(char const* root, char const* path, char* data, xsize_t maxn);

        //! get the directory of path
        //
        // @param path          the path
        // @param data          the path data
        // @param maxn          the path maxn
        //
        // @return              the directory of path
        //
        char const* path_directory(char const* path, char* data, xsize_t maxn);

    } // namespace xfile
} // namespace xcore

#endif // __XFILE_PATH_H__