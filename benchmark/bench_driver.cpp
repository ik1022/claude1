/**
 * @file bench_driver.cpp
 * @brief Performance benchmarks for drivers
 */

#include <benchmark/benchmark.h>
#include "libemb/driver/ssd1306/Ssd1306.hpp"
#include "port/mock/hal/MockI2c.hpp"

using namespace libemb::driver;
using namespace libemb::port::mock;

// Ssd1306 initialization
static void BM_Ssd1306_Init(benchmark::State& state) {
    for (auto _ : state) {
        MockI2c i2c;
        Ssd1306 display;
        benchmark::DoNotOptimize(display.init(&i2c));
    }
}
BENCHMARK(BM_Ssd1306_Init)->Name("Ssd1306::init");

// Frame buffer clear
static void BM_Ssd1306_Clear(benchmark::State& state) {
    MockI2c i2c;
    Ssd1306 display;
    display.init(&i2c);

    for (auto _ : state) {
        display.clear();
    }
}
BENCHMARK(BM_Ssd1306_Clear)->Name("Ssd1306::clear");

// Frame buffer display (I2C write)
static void BM_Ssd1306_Display(benchmark::State& state) {
    MockI2c i2c;
    Ssd1306 display;
    display.init(&i2c);
    display.clear();

    for (auto _ : state) {
        benchmark::DoNotOptimize(display.display());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Ssd1306_Display)->Name("Ssd1306::display (I2C transfer)");

// Pixel drawing - horizontal line
static void BM_Ssd1306_DrawHLine(benchmark::State& state) {
    MockI2c i2c;
    Ssd1306 display;
    display.init(&i2c);

    for (auto _ : state) {
        display.clear();
        display.drawHLine(0, 32, 128, true);
    }
    state.SetItemsProcessed(state.iterations() * 128);
}
BENCHMARK(BM_Ssd1306_DrawHLine)->Name("Ssd1306::drawHLine (128 pixels)");

// Pixel drawing - vertical line
static void BM_Ssd1306_DrawVLine(benchmark::State& state) {
    MockI2c i2c;
    Ssd1306 display;
    display.init(&i2c);

    for (auto _ : state) {
        display.clear();
        display.drawVLine(64, 0, 64, true);
    }
    state.SetItemsProcessed(state.iterations() * 64);
}
BENCHMARK(BM_Ssd1306_DrawVLine)->Name("Ssd1306::drawVLine (64 pixels)");

// Complex pattern drawing
static void BM_Ssd1306_ComplexPattern(benchmark::State& state) {
    MockI2c i2c;
    Ssd1306 display;
    display.init(&i2c);

    for (auto _ : state) {
        display.clear();
        // Draw grid
        for (int x = 0; x < 128; x += 16) {
            display.drawVLine(x, 0, 64, true);
        }
        for (int y = 0; y < 64; y += 16) {
            display.drawHLine(0, y, 128, true);
        }
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Ssd1306_ComplexPattern)->Name("Ssd1306::complex_pattern (grid)");

// I2C transaction count
static void BM_Ssd1306_I2CTransactions(benchmark::State& state) {
    for (auto _ : state) {
        MockI2c i2c;
        Ssd1306 display;
        display.init(&i2c);
        display.clear();
        display.display();

        size_t txn_count = i2c.getTransactionCount();
        benchmark::DoNotOptimize(txn_count);
    }
}
BENCHMARK(BM_Ssd1306_I2CTransactions)->Name("Ssd1306::i2c_transaction_count");

BENCHMARK_MAIN();
