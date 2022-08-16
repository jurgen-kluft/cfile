#include "cbase/c_allocator.h"
#include "cbase/c_runes.h"
#include "cfile/c_file.h"
#include "cunittest/xunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(test_file_stream)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {  }
        UNITTEST_FIXTURE_TEARDOWN() {  }

		UNITTEST_TEST(open)
		{
			file_stream_t fs;
			fs.open("test.txt", FILE_MODE_CREATE);
			fs.close();
		}
    }
}
UNITTEST_SUITE_END
