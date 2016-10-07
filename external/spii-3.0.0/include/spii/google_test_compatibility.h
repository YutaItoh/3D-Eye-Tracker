#ifndef SPII_GOOGLE_TEST_COMPATIBILITY
#define SPII_GOOGLE_TEST_COMPATIBILITY

#include <cmath>

// Google Testing macros
#define ASSERT_GE(a, b) REQUIRE((a) >= (b))
#define ASSERT_LE(a, b) REQUIRE((a) <= (b))
#define ASSERT_GT(a, b) REQUIRE((a) > (b))
#define ASSERT_LT(a, b) REQUIRE((a) < (b))
#define ASSERT_EQ(a, b) REQUIRE((a) == (b))
#define ASSERT_NE(a, b) REQUIRE((a) != (b))
#define ASSERT_FLOAT_EQ(a, b) REQUIRE(Approx(float(a)) == float(b))
#define ASSERT_DOUBLE_EQ(a, b) REQUIRE(Approx(double(a)) == double(b))
#define ASSERT_TRUE(expr) REQUIRE((expr))

#define EXPECT_GE(a, b) CHECK((a) >= (b))
#define EXPECT_LE(a, b) CHECK((a) <= (b))
#define EXPECT_GT(a, b) CHECK((a) > (b))
#define EXPECT_LT(a, b) CHECK((a) < (b))
#define EXPECT_EQ(a, b) CHECK((a) == (b))
#define EXPECT_NE(a, b) CHECK((a) != (b))
#define EXPECT_FLOAT_EQ(a, b) CHECK(Approx(float(a)) == float(b))
#define EXPECT_DOUBLE_EQ(a, b) CHECK(Approx(double(a)) == double(b))
#define EXPECT_TRUE(expr) CHECK((expr))
#define EXPECT_THROW(expr, excep) CHECK_THROWS_AS(expr, excep)
#define EXPECT_NO_THROW(expr) CHECK_NOTHROW(expr)

#define EXPECT_NEAR(a, b, tol) if (std::abs(a) >= tol || std::abs(b) >= tol) \
                                  {CHECK((std::abs((a) - (b)) / std::max(std::abs(a), std::abs(b))) <= (tol)); }

#define TEST(identifier, submethod) TEST_CASE(#identifier "/" #submethod, "")

#endif
