#ifndef __XFILE_DIRECTORY_H__
#define __XFILE_DIRECTORY_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace xcore
{
    namespace xfile
    {
        struct file_info_t;

        //! the directory walk func type
        //
        // @param path          the file path
        // @param info          the file info
        // @param priv          the user private data
        //
        // @return              continue: true, break: false
        ///
        typedef bool (*directory_walk_func_t)(char const* path, file_info_t const* info, void* priv);


        //! create the directory
        //
        // @param path          the directory path
        //
        // @return              true or false
        ///
        bool directory_create(char const* path);

        //! remove the directory
        //
        // @param path          the directory path
        //
        // @return              true or false
        ///
        bool directory_remove(char const* path);

        //! the home directory
        //
        // @param path          the directory path data
        // @param maxn          the directory path maxn
        //
        // @return              the directory path size
        ///
        u64 directory_home(char* path, u64 maxn);

        //! the current directory
        //
        // @param path          the directory path data
        // @param maxn          the directory path maxn
        //
        // @return              the directory path size
        ///
        u64 directory_current(char* path, u64 maxn);

        //! set the current directory
        //
        // @param path          the directory path
        //
        // @return              true or false
        ///
        bool directory_current_set(char const* path);

        //! the temporary directory
        //
        // @param path          the directory path data
        // @param maxn          the directory path maxn
        //
        // @return              the directory path size
        ///
        u64 directory_temporary(char* path, u64 maxn);

        //! the directory walk
        //
        // @param path          the directory path
        // @param recursion     the recursion level, 0, 1, 2, .. or -1 (infinite)
        // @param prefix        is prefix recursion? directory is the first item
        // @param func          the callback func
        // @param priv          the callback priv
        //
        ///
        void directory_walk(char const* path, s32 recursion, bool prefix, directory_walk_func_t func, void* priv);

        //! copy directory
        //
        // @param path          the directory path
        // @param dest          the directory dest
        //
        // @return              true or false
        ///
        bool directory_copy(char const* path, char const* dest);

    } // namespace xfile
} // namespace xcore

#endif