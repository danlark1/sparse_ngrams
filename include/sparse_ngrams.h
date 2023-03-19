#pragma once

#include <cstdint>
#include <functional>
#include <string_view>

namespace sparse_ngrams {

class SparseNgramsBuilder {
 public:
  struct Options {
    size_t max_ngram_size = 32;
    bool case_insensitive = true;
  };
  // Constructs a class. max_ngram_size is suggested to be in between [16, 32]
  // as real cases don't include the search for more.
  SparseNgramsBuilder(const Options& options);

  // O(s.size()) algorithm to construct at most 2n - 2 ngrams. On a substring,
  // returns a subset of these ngrams. consumer returns begin and end of a
  // string. Ignores max_ngram_size. Used by indexing.
  void BuildAllNgrams(std::string_view s,
                      const std::function<void(/*begin=*/const char*,
                                               /*end=*/const char*)>& consumer);

  // O(s.size()) algorithm to construct at most n-2 ngrams. Covers with the
  // minimal amount of ngrams from the BuildAllNgrams. Useful for reducing
  // the amount of ngram search and making it an intersection of all found docs
  // Returns ngrams of at most max_ngram_size. Useful for a substring search
  // and retrieval.
  void BuildCoveringNgrams(
      std::string_view s,
      const std::function<void(/*begin=*/const char*, /*end=*/const char*)>&
          consumer);

 private:
  Options options_;
};

}  // namespace sparse_ngrams
