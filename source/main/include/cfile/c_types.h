#ifndef __CFILE_FILE_TYPES_PRIVATE_H__
#define __CFILE_FILE_TYPES_PRIVATE_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_debug.h"

namespace ncore
{
    namespace nfile
    {
        struct file_handle_t
        {
            inline file_handle_t() : m_handle(nullptr) {}
            inline file_handle_t(void* h) : m_handle(h) {}

            bool  isValid() const { return m_handle != nullptr; }
            void* m_handle;
        };

        /// the file modes
        enum file_mode_t
        {
            FILE_MODE_READ         = 1,
            FILE_MODE_WRITE        = 2,
            FILE_MODE_RW           = 3,
            FILE_MODE_CREATENEW    = 8 | FILE_MODE_WRITE,
            FILE_MODE_APPEND       = 16 | FILE_MODE_WRITE,
            FILE_MODE_TRUNC        = 32 | FILE_MODE_WRITE,
            FILE_MODE_CREATE       = FILE_MODE_CREATENEW | FILE_MODE_TRUNC | FILE_MODE_WRITE,
            FILE_MODE_OPENORCREATE = FILE_MODE_CREATENEW | FILE_MODE_APPEND | FILE_MODE_WRITE,
        };

        /// the file seek modes
        enum seek_mode_t
        {
            SEEK_MODE_BEG = 0,
            SEEK_MODE_CUR = 1,
            SEEK_MODE_END = 2
        };

        /// the file type
        enum file_type_t
        {
            FILE_TYPE_NONE      = 0,
            FILE_TYPE_DIRECTORY = 1,
            FILE_TYPE_FILE      = 2,
            FILE_TYPE_DOT       = 3,
            FILE_TYPE_DOT2      = 4
        };

    } // namespace nfile
} // namespace ncore

#endif // __CFILE_FILE_TYPES_PRIVATE_H__