#include <gtest/gtest.h>


class TestTemplate : public ::testing::Test
{
protected:
  // Here initiate objects used in tests
public:
  // SetUp is called before every test
  // TearDown is called after every test
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(TestTemplate, EmptyTest) {}

TEST_F(TestTemplate, SkippedTest)
{
  GTEST_SKIP() << "Skipping single test";
}
