#include "xbase/x_allocator.h"
#include "xbase/x_integer.h"
#include "xbase/x_memory.h"

#include "xfile/private/x_file.h"
//#include "xfile/x_stream.h"

namespace xcore
{
    namespace xfile
    {
        class stream_t
        {
        public:
            bool open(const char* filename, file_mode_t mode);
            bool close();
            u64  read(xbyte* buffer, u64 size);
            u64  write(xbyte const* buffer, u64 size);
            bool seek();

            bool is_open() const;
            bool can_write() const;
        };
    }

} // namespace xcore
