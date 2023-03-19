#include "sparse_ngrams.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string_view>
#include <unordered_set>

using ::testing::UnorderedElementsAre;

namespace sparse_ngrams {
namespace {

class NgramsTest : public ::testing::Test {
 protected:
  NgramsTest() : builder_({32, true}) {}

  std::unordered_set<std::string> CollectNgrams(std::string_view s) {
    std::unordered_set<std::string> set;
    builder_.BuildAllNgrams(s, [&set](const char* begin, const char* end) {
      set.insert(std::string(begin, end));
    });
    return set;
  }

  std::unordered_set<std::string> CollectCoveringNgrams(std::string_view s) {
    std::unordered_set<std::string> set;
    builder_.BuildCoveringNgrams(s, [&set](const char* begin, const char* end) {
      set.insert(std::string(begin, end));
    });
    return set;
  }

  SparseNgramsBuilder builder_;
};

TEST_F(NgramsTest, TestSimple) {
  EXPECT_THAT(CollectNgrams("he"), UnorderedElementsAre());
  EXPECT_THAT(CollectNgrams("hel"), UnorderedElementsAre("hel"));
  EXPECT_THAT(CollectNgrams("hell"), UnorderedElementsAre("hel", "ell"));
  EXPECT_THAT(CollectNgrams("hello world"),
              UnorderedElementsAre("hel", "ell", " wo", " world", "llo", "o w",
                                   "lo ", "llo ", "ello ", "hello ", " wor",
                                   "rld", "wor", "o world", "orl", "orld"));
}

TEST_F(NgramsTest, TestSimpleCovering) {
  EXPECT_THAT(CollectCoveringNgrams("he"), UnorderedElementsAre());
  EXPECT_THAT(CollectCoveringNgrams("hel"), UnorderedElementsAre("hel"));
  EXPECT_THAT(CollectCoveringNgrams("hell"), UnorderedElementsAre("hel", "ell"));
  EXPECT_THAT(CollectCoveringNgrams("hello world"),
      UnorderedElementsAre("o world", "hello "));
}

}  // namespace
}  // namespace sparse_ngrams
