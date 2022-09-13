#include "cbase/c_allocator.h"
#include "cbase/c_runes.h"
#include "cfile/private/c_file.h"
#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(test_nfile)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {  }
        UNITTEST_FIXTURE_TEARDOWN() {  }

		UNITTEST_TEST(file_create)
		{
			file_handle_t fh = nfile::file_open("test.txt", FILE_MODE_CREATE);
			nfile::file_close(fh);
		}
    }
}
UNITTEST_SUITE_END
