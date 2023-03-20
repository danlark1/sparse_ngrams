#include "sparse_ngrams.h"

#include <string>
#include <string_view>
#include <vector>

namespace sparse_ngrams {
namespace {

// Some hashing algorithm.
inline uint32_t HashBigram(const char* begin) {
  // Maybe use CRC? Use fingerprint from or_tools.
  const uint64_t kMul1 = 0xc6a4a7935bd1e995ULL;
  const uint64_t kMul2 = 0x228876a7198b743ULL;
  uint64_t a = static_cast<uint64_t>(begin[0]) * kMul1 +
               static_cast<uint64_t>(begin[1]) * kMul2;
  return a + (~a >> 47);
}

}  // namespace

SparseNgramsBuilder::SparseNgramsBuilder(
    const SparseNgramsBuilder::Options& options)
    : options_(options) {
  (void)options_;
}

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
    // Take a hash and position.
    HashAndPos p{HashBigram(s.data() + i), i};
    // Remove from the end until hashes are bigger.
    while (!st.empty() && p.hash > st.back().hash) {
      // Consume all while removing bigger hashes as it may be relevant for a
      // substring search.
      consumer(s.data() + st.back().pos, s.data() + i + 2);
      // Same hashes should be glued to the left.
      while (st.size() > 1 && st.back().hash == st[st.size() - 2].hash) {
        st.pop_back();
      }
      st.pop_back();
    }
    if (!st.empty()) {
      consumer(s.data() + st.back().pos, s.data() + i + 2);
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
  std::vector<HashAndPos> st;
  // Look at increasing and decreasing sequences.
  for (size_t i = 0; i + 2 <= s.size(); ++i) {
    HashAndPos p{HashBigram(s.data() + i), i};
    while (!st.empty() && p.hash > st.back().hash) {
      // Glue same hashes.
      if (st.front().hash == st.back().hash) {
        consumer(s.data() + st.back().pos, s.data() + i + 2);
        while (st.size() > 1) {
          size_t last_position = st.back().pos + 2;
          st.pop_back();
          consumer(s.data() + st.back().pos, s.data() + last_position);
        }
      }
      st.pop_back();
    }
    st.push_back(p);
  }
  while (st.size() > 1) {
    size_t last_position = st.back().pos + 2;
    st.pop_back();
    consumer(s.data() + st.back().pos, s.data() + last_position);
  }
}

}  // namespace sparse_ngrams
