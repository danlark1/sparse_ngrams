[![License](https://img.shields.io/badge/License-Boost%201.0-lightblue.svg)](https://www.boost.org/LICENSE_1_0.txt)

sparse_ngrams: Prefiltering algorithm for indexing GitHub code
==============================================================

**Work in progress**

`sparse_ngrams` is a C++ library that contains a search substring and regexp
algorithms that are scalable for code search indexing and used in [GitHub Codesearch](https://github.blog/2023-02-06-the-technology-behind-githubs-new-code-search/). It's indended to
reduce the indexing and query response times compared to [zoekt (which is used by Sourcegraph)](https://github.com/google/zoekt) and [Russ Cox's](https://swtch.com/~rsc/regexp/regexp4.html) trigram search. The solution is meant to be scalable to billions lines of code with <100ms latency. More on code search project is TBD.

* **Easy:** First-class, easy to use dependency and carefully documented APIs.
* **Fast:** We do care about speed of the algorithms and provide reasonable implementations.
* **Well tested:** We test all algorithms with a unified framework, under sanitizers and fuzzing.
* **Benchmarked:** We gather benchmarks for all implementations to better understand good and bad spots.

Table of Contents
-----------------

* [Quick Start](#quick-start)
* [Testing](#testing)
* [Documentation](#documentation)
* [License](#license)

Quick Start
-----------

You can use cmake with `add_subdirectory`. Includes are in `include`,
sources are in `src` folders.


We support all C++17 compliant modern compilers (GCC, Clang, MSVC).

Testing
-------

To test and benchmark, we use [Google benchmark](https://github.com/google/benchmark) library.
Simply do in the root directory:

```console
# Check out the libraries.
$ git clone https://github.com/google/benchmark.git
$ git clone https://github.com/google/googletest.git
$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DSPARSE_NGRAMS_TESTING=on -DBENCHMARK_ENABLE_GTEST_TESTS=off -DBENCHMARK_ENABLE_TESTING=off ..
$ make -j
$ ctest -j4 --output-on-failure
```

Documentation
-------------

TBD.

License
-------

The code is made available under the [Boost License 1.0](https://boost.org/LICENSE_1_0.txt).
