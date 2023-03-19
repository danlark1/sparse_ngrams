#include "sparse_ngrams.h"

#include <deque>
#include <vector>
#include <string>
#include <string_view>

namespace sparse_ngrams {
namespace {

// Some hashing algorithm.
inline uint32_t HashBigram(const char* begin) {
  // Improving the efficiency of ^ and $ searches.
  if (begin[0] == '\n' || begin[1] == '\n') return 0;
  const uint64_t kMul1 = 0xc6a4a7935bd1e995ULL;
  const uint64_t kMul2 = 0x228876a7198b743ULL;
  uint64_t a =
      static_cast<uint64_t>(begin[0]) * kMul1 + static_cast<uint64_t>(begin[1]) * kMul2;
  return a + (~a >> 47);
}

}  // namespace

SparseNgramsBuilder::SparseNgramsBuilder(
    const SparseNgramsBuilder::Options& options)
    : options_(options) {}

void SparseNgramsBuilder::BuildAllNgrams(
    std::string_view s,
    const std::function<void(/*begin=*/const char*, /*end=*/const char*)>&
        consumer) {
  struct HashAndPos {
    uint32_t hash;
    size_t pos;
  };
  std::vector<HashAndPos> st;
  for (size_t i = 0; i + 2 <= s.size(); ++i) {
    HashAndPos p{HashBigram(s.data() + i), i + 1};
    while (!st.empty() && p.hash < st.back().hash) {
      consumer(s.data() + st.back().pos - 1, s.data() + i + 2);
      while (st.size() > 1 && st.back().hash == (st.end() - 2)->hash) {
        st.pop_back();
      }
      st.pop_back();
    }
    if (!st.empty()) {
      consumer(s.data() + st.back().pos - 1, s.data() + i + 2);
    }
    st.push_back(p);
  }
}

void SparseNgramsBuilder::BuildCoveringNgrams(
    std::string_view s,
    const std::function<void(/*begin=*/const char*, /*end=*/const char*)>&
        consumer) {
  struct HashAndPos {
    uint32_t hash;
    size_t pos;
  };
  std::deque<HashAndPos> st;
  // Look at increasing and decreasing sequences.
  for (size_t i = 0; i + 2 <= s.size(); ++i) {
    HashAndPos p{HashBigram(s.data() + i), i + 1};
    // Filter out big ngrams.
    if (!st.empty() && i - st.front().pos + 3 >= options_.max_ngram_size) {
      consumer(s.data() + st[0].pos - 1, s.data() + st[1].pos + 1);
      st.pop_front();
    }
    while (!st.empty() && p.hash < st.back().hash) {
      if (st.front().hash == st.back().hash) {
        consumer(s.data() + st.back().pos - 1, s.data() + i + 2);
        while (st.size() > 1) {
          size_t last_position = st.back().pos;
          st.pop_back();
          consumer(s.data() + st.back().pos - 1, s.data() + last_position + 1);
        }
      }
      st.pop_back();
    }
    st.push_back(p);
  }
  while (st.size() > 1) {
    size_t last_position = st.back().pos;
    st.pop_back();
    consumer(s.data() + st.back().pos - 1,
             s.data() + last_position + 1);
  }
}

}  // namespace sparse_ngrams
