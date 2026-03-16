/**
 * @file bench_ringbuffer.cpp
 * @brief Performance benchmarks for RingBuffer
 */

#include <benchmark/benchmark.h>
#include "libemb/util/RingBuffer.hpp"

using namespace libemb::util;

// Push operation performance
static void BM_RingBuffer_Push(benchmark::State& state) {
    for (auto _ : state) {
        RingBuffer<uint8_t, 256> buffer;
        for (int i = 0; i < 256; ++i) {
            buffer.push(i & 0xFF);
        }
    }
    state.SetItemsProcessed(state.iterations() * 256);
}
BENCHMARK(BM_RingBuffer_Push)->Name("RingBuffer::push (256 items)");

// Pop operation performance
static void BM_RingBuffer_Pop(benchmark::State& state) {
    for (auto _ : state) {
        RingBuffer<uint8_t, 256> buffer;
        // Fill buffer
        for (int i = 0; i < 256; ++i) {
            buffer.push(i & 0xFF);
        }
        // Pop all items
        uint8_t value;
        for (int i = 0; i < 256; ++i) {
            buffer.pop(value);
        }
    }
    state.SetItemsProcessed(state.iterations() * 256);
}
BENCHMARK(BM_RingBuffer_Pop)->Name("RingBuffer::pop (256 items)");

// Throughput test: continuous push/pop
static void BM_RingBuffer_Throughput(benchmark::State& state) {
    RingBuffer<uint32_t, 128> buffer;

    for (auto _ : state) {
        // Fill half
        for (int i = 0; i < 64; ++i) {
            buffer.push(i);
        }
        // Pop half
        uint32_t value;
        for (int i = 0; i < 64; ++i) {
            buffer.pop(value);
            (void)value;
        }
    }
    state.SetItemsProcessed(state.iterations() * 128);
}
BENCHMARK(BM_RingBuffer_Throughput)->Name("RingBuffer::throughput (128 items)");

// Various buffer sizes
static void BM_RingBuffer_VariousSize(benchmark::State& state) {
    const int size = state.range(0);

    for (auto _ : state) {
        RingBuffer<uint8_t, 1024> buffer;
        // Push
        for (int i = 0; i < size; ++i) {
            buffer.push(i & 0xFF);
        }
        // Pop
        uint8_t value;
        for (int i = 0; i < size; ++i) {
            buffer.pop(value);
            (void)value;
        }
    }
    state.SetItemsProcessed(state.iterations() * size * 2);
}
BENCHMARK(BM_RingBuffer_VariousSize)
    ->Name("RingBuffer::push_pop_various")
    ->Range(1, 512)
    ->RangeMultiplier(2);

// Empty check performance
static void BM_RingBuffer_IsEmpty(benchmark::State& state) {
    RingBuffer<uint8_t, 256> buffer;

    for (auto _ : state) {
        benchmark::DoNotOptimize(buffer.empty());
    }
}
BENCHMARK(BM_RingBuffer_IsEmpty)->Name("RingBuffer::is_empty (empty)");

// Full check performance
static void BM_RingBuffer_IsFull(benchmark::State& state) {
    RingBuffer<uint8_t, 256> buffer;

    // Fill buffer
    for (int i = 0; i < 256; ++i) {
        buffer.push(i & 0xFF);
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(buffer.full());
    }
}
BENCHMARK(BM_RingBuffer_IsFull)->Name("RingBuffer::is_full (full)");

BENCHMARK_MAIN();
