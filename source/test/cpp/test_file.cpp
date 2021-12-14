#include "xbase/x_allocator.h"
#include "xbase/x_runes.h"
#include "xfile/x_file.h"
#include "xunittest/xunittest.h"

using namespace xcore;

UNITTEST_SUITE_BEGIN(test_file)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {  }
        UNITTEST_FIXTURE_TEARDOWN() {  }

		UNITTEST_TEST(file_create)
		{
			xfile::file_handle_t fh = xfile::file_open("test.txt", FILE_MODE_CREATE);
			xfile::file_close(fh);
		}
    }
}
UNITTEST_SUITE_END
