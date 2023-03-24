#include "sparse_ngrams.h"

#include <deque>
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
    const std::function<void(/*substring=*/std::string_view)>& consumer) {
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
      consumer(
          std::string_view(s.data() + st.back().pos, i + 2 - st.back().pos));
      // Same hashes should be glued to the left.
      while (st.size() > 1 && st.back().hash == st[st.size() - 2].hash) {
        st.pop_back();
      }
      st.pop_back();
    }
    if (!st.empty()) {
      consumer(
          std::string_view(s.data() + st.back().pos, i + 2 - st.back().pos));
    }
    st.push_back(p);
  }
}

void SparseNgramsBuilder::BuildCoveringNgrams(
    std::string_view s,
    const std::function<void(/*substring=*/std::string_view)>& consumer,
    const SparseNgramsBuilder::CoveringNgramsOptions& ngram_options) {
  struct HashAndPos {
    uint32_t hash;
    size_t pos;
  };
  std::deque<HashAndPos> st;
  // Look at increasing and decreasing sequences.
  for (size_t i = 0; i + 2 <= s.size(); ++i) {
    HashAndPos p{HashBigram(s.data() + i), i};
    if (st.size() > 1 &&
        i - st.front().pos + 3 >= ngram_options.max_ngram_length) {
      consumer(std::string_view(s.data() + st.front().pos,
                                st[1].pos + 2 - st.front().pos));
      st.pop_front();
    }
    while (!st.empty() && p.hash > st.back().hash) {
      // Glue same hashes.
      if (st.front().hash == st.back().hash) {
        consumer(
            std::string_view(s.data() + st.back().pos, i + 2 - st.back().pos));
        while (st.size() > 1) {
          size_t last_position = st.back().pos + 2;
          st.pop_back();
          consumer(std::string_view(s.data() + st.back().pos,
                                    last_position - st.back().pos));
        }
      }
      st.pop_back();
    }
    st.push_back(p);
  }
  while (st.size() > 1) {
    size_t last_position = st.back().pos + 2;
    st.pop_back();
    consumer(std::string_view(s.data() + st.back().pos,
                              last_position - st.back().pos));
  }
}

}  // namespace sparse_ngrams
