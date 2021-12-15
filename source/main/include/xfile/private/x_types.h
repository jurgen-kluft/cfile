#ifndef __XFILE_FILE_TYPES_PRIVATE_H__
#define __XFILE_FILE_TYPES_PRIVATE_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "xbase/x_debug.h"

namespace xcore
{
    struct file_handle_t
    {
        inline file_handle_t() : m_handle(nullptr) {}
        inline file_handle_t(void* h) : m_handle(h) {}
        void* m_handle;
    };

	/// the file modes
	enum file_mode_t
	{
		FILE_MODE_RO        = 1,
		FILE_MODE_WO        = 2,
		FILE_MODE_RW        = 4,
		FILE_MODE_CREATENEW = 8,
		FILE_MODE_APPEND    = 16,
		FILE_MODE_TRUNC     = 32,
		FILE_MODE_CREATE    = FILE_MODE_CREATENEW | FILE_MODE_TRUNC,
		FILE_MODE_OPENORCREATE = FILE_MODE_CREATENEW | FILE_MODE_APPEND,
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


} // namespace xcore

#endif // __XFILE_FILE_TYPES_PRIVATE_H__