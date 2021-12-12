#ifndef __XFILE_ASSERT_H__
#define __XFILE_ASSERT_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "xbase/x_debug.h"

namespace xcore
{
    namespace xfile
    {
        
#    define assert_and_check_return_val(condition, returnvalue) \
        do                                                      \
        {                                                       \
            if (!(condition))                                   \
            {                                                   \
                ASSERT(false);                                  \
                return (returnvalue);                           \
            }                                                   \
        } while (false);

#    define check_return_val(condition, returnvalue) \
        do                                           \
        {                                            \
            if (!(condition))                        \
                return (returnvalue);                \
        } while (false);

#    define assert_and_check_break(cond) \
        if (!(cond))          \
            ASSERT(false);    \
            break;

#    define check_break(cond) \
        if (!(cond))          \
            break;


    }
}


#endif