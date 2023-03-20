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
  NgramsTest() : builder_({}) {}

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
  EXPECT_THAT(
      CollectNgrams("hello world"),
      UnorderedElementsAre("hel", "ell", "llo", "lo ", "o w", "lo w", " wo",
                           "lo wo", "wor", "orl", "worl", "rld"));
}

TEST_F(NgramsTest, TestSimpleCovering) {
  EXPECT_THAT(CollectCoveringNgrams("he"), UnorderedElementsAre());
  EXPECT_THAT(CollectCoveringNgrams("hel"), UnorderedElementsAre("hel"));
  EXPECT_THAT(CollectCoveringNgrams("hell"),
              UnorderedElementsAre("hel", "ell"));
  EXPECT_THAT(
      CollectCoveringNgrams("hello world"),
      UnorderedElementsAre("hel", "ell", "llo", "rld", "worl", "lo wo"));
}

TEST_F(NgramsTest, SplitGithubCodesearch) {
  EXPECT_THAT(CollectNgrams("chester "),
              UnorderedElementsAre("che", "hes", "ches", "est", "chest", "ste",
                                   "ter", "ster", "er "));
  EXPECT_THAT(CollectCoveringNgrams("chester"),
              UnorderedElementsAre("chest", "ster"));
}

TEST_F(NgramsTest, SplitForLoop) {
  EXPECT_THAT(
      CollectNgrams("for(int i=42"),
      UnorderedElementsAre("for", "or(", "for(", "r(i", "for(i", "(in", "int",
                           "(int", "nt ", "t i", " i=", "t i=", "i=4", "t i=4",
                           "nt i=4", "(int i=4", "=42"));
  EXPECT_THAT(CollectCoveringNgrams("for(int i=42"),
              UnorderedElementsAre("for(i", "(int i=4", "=42"));
}

}  // namespace
}  // namespace sparse_ngrams
