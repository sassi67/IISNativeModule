#include <gtest/gtest.h>
#include <utils/smoke.h>

namespace utils {
    TEST(TestUtils, TestSmokeGetHello)
    {
        EXPECT_EQ("Hello IISNativeModule!", utils::Smoke::getHello());
    }
}
