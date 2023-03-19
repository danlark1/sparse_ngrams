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
      while (st.size() > 1 && st.back().hash == st[st.size() - 2].hash) {
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
  std::deque<HashAndPos> deque;
  // Look at increasing and decreasing sequences.
  for (size_t i = 0; i + 2 <= s.size(); ++i) {
    HashAndPos p{HashBigram(s.data() + i), i + 1};
    // Filter out big ngrams.
    if (!deque.empty() && i - deque.front().pos + 3 >= options_.max_ngram_size) {
      consumer(s.data() + deque[0].pos - 1, s.data() + deque[1].pos + 1);
      deque.pop_front();
    }
    while (!deque.empty() && p.hash < deque.back().hash) {
      if (deque.front().hash == deque.back().hash) {
        consumer(s.data() + deque.back().pos - 1, s.data() + i + 2);
        while (deque.size() > 1) {
          size_t last_position = deque.back().pos;
          deque.pop_back();
          consumer(s.data() + deque.back().pos - 1, s.data() + last_position + 1);
        }
      }
      deque.pop_back();
    }
    deque.push_back(p);
  }
  while (deque.size() > 1) {
    size_t last_position = deque.back().pos;
    deque.pop_back();
    consumer(s.data() + deque.back().pos - 1,
             s.data() + last_position + 1);
  }
}

}  // namespace sparse_ngrams
