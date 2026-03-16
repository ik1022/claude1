/**
 * @file bench_staticvector.cpp
 * @brief Performance benchmarks for StaticVector
 */

#include <benchmark/benchmark.h>
#include "libemb/util/StaticVector.hpp"

using namespace libemb::util;

// Push back operation
static void BM_StaticVector_PushBack(benchmark::State& state) {
    for (auto _ : state) {
        StaticVector<uint32_t, 256> vec;
        for (int i = 0; i < 256; ++i) {
            vec.push_back(i);
        }
    }
    state.SetItemsProcessed(state.iterations() * 256);
}
BENCHMARK(BM_StaticVector_PushBack)->Name("StaticVector::push_back (256 items)");

// Random access performance
static void BM_StaticVector_Access(benchmark::State& state) {
    StaticVector<uint32_t, 256> vec;
    for (int i = 0; i < 256; ++i) {
        vec.push_back(i);
    }

    for (auto _ : state) {
        volatile uint32_t sum = 0;
        for (size_t i = 0; i < 256; ++i) {
            sum += vec[i];
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * 256);
}
BENCHMARK(BM_StaticVector_Access)->Name("StaticVector::operator[] (256 items)");

// Iterator traversal
static void BM_StaticVector_Iterator(benchmark::State& state) {
    StaticVector<uint32_t, 256> vec;
    for (int i = 0; i < 256; ++i) {
        vec.push_back(i);
    }

    for (auto _ : state) {
        volatile uint32_t sum = 0;
        for (auto& item : vec) {
            sum += item;
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * 256);
}
BENCHMARK(BM_StaticVector_Iterator)->Name("StaticVector::iterator (256 items)");

// Various sizes
static void BM_StaticVector_VariousSize(benchmark::State& state) {
    const size_t size = state.range(0);

    for (auto _ : state) {
        StaticVector<uint32_t, 1024> vec;
        for (size_t i = 0; i < size; ++i) {
            vec.push_back(i);
        }
        // Access all
        volatile uint32_t sum = 0;
        for (size_t i = 0; i < size; ++i) {
            sum += vec[i];
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_StaticVector_VariousSize)
    ->Name("StaticVector::push_and_access")
    ->Range(1, 512)
    ->RangeMultiplier(2);

// Size check
static void BM_StaticVector_Size(benchmark::State& state) {
    StaticVector<uint32_t, 256> vec;
    for (int i = 0; i < 256; ++i) {
        vec.push_back(i);
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(vec.size());
    }
}
BENCHMARK(BM_StaticVector_Size)->Name("StaticVector::size (256 items)");

// Capacity check
static void BM_StaticVector_Capacity(benchmark::State& state) {
    StaticVector<uint32_t, 256> vec;

    for (auto _ : state) {
        benchmark::DoNotOptimize(vec.capacity());
    }
}
BENCHMARK(BM_StaticVector_Capacity)->Name("StaticVector::capacity");

// Empty check
static void BM_StaticVector_Empty(benchmark::State& state) {
    StaticVector<uint32_t, 256> vec;

    for (auto _ : state) {
        benchmark::DoNotOptimize(vec.empty());
    }
}
BENCHMARK(BM_StaticVector_Empty)->Name("StaticVector::empty");

BENCHMARK_MAIN();
