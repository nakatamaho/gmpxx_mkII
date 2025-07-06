#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <future>
#include <cstdlib>
#include "gmpxx_mkII.h"

using namespace gmpxx;
using namespace gmpxx_defaults;

// =============================================================================
// Basic Functionality Tests
// =============================================================================

class GmpxxDefaultsTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Reset to known state before each test
        this_thread::reset_to_defaults();
        global::reset_to_defaults();
    }

    void TearDown() override {
        // Clean up after each test
        this_thread::reset_to_defaults();
        global::reset_to_defaults();
    }
};

// -----------------------------------------------------------------------------
// Thread-local Configuration Tests
// -----------------------------------------------------------------------------

TEST_F(GmpxxDefaultsTest, ThreadLocalBasicFunctionality) {
    // Initially should not be using local config
    EXPECT_FALSE(this_thread::is_using_local_config());

    // Should return global defaults initially
    EXPECT_EQ(this_thread::get_precision(), default_prec_value);
    EXPECT_EQ(this_thread::get_base(), default_base_value);

    // Enable local config
    this_thread::enable_local_config(true);
    EXPECT_TRUE(this_thread::is_using_local_config());

    // Set local values
    this_thread::set_precision(1024);
    this_thread::set_base(16);

    EXPECT_EQ(this_thread::get_precision(), 1024);
    EXPECT_EQ(this_thread::get_base(), 16);

    // Disable local config
    this_thread::enable_local_config(false);
    EXPECT_FALSE(this_thread::is_using_local_config());

    // Should return to global defaults
    EXPECT_EQ(this_thread::get_precision(), default_prec_value);
    EXPECT_EQ(this_thread::get_base(), default_base_value);
}

TEST_F(GmpxxDefaultsTest, ThreadLocalSetPrecisionAutoEnables) {
    EXPECT_FALSE(this_thread::is_using_local_config());

    // Setting precision should automatically enable local config
    this_thread::set_precision(2048);

    EXPECT_TRUE(this_thread::is_using_local_config());
    EXPECT_EQ(this_thread::get_precision(), 2048);
}

TEST_F(GmpxxDefaultsTest, ThreadLocalSetBaseAutoEnables) {
    EXPECT_FALSE(this_thread::is_using_local_config());

    // Setting base should automatically enable local config
    this_thread::set_base(8);

    EXPECT_TRUE(this_thread::is_using_local_config());
    EXPECT_EQ(this_thread::get_base(), 8);
}

TEST_F(GmpxxDefaultsTest, ThreadLocalInvalidValues) {
    // Invalid precision (< 2)
    EXPECT_THROW(this_thread::set_precision(1), std::invalid_argument);
    EXPECT_THROW(this_thread::set_precision(0), std::invalid_argument);

    // Invalid base (< 2 or > 62)
    EXPECT_THROW(this_thread::set_base(1), std::invalid_argument);
    EXPECT_THROW(this_thread::set_base(63), std::invalid_argument);
    EXPECT_THROW(this_thread::set_base(-1), std::invalid_argument);
}

TEST_F(GmpxxDefaultsTest, ThreadLocalReset) {
    // Set some values
    this_thread::set_precision(1024);
    this_thread::set_base(16);
    EXPECT_TRUE(this_thread::is_using_local_config());

    // Reset should restore defaults and disable local config
    this_thread::reset_to_defaults();

    EXPECT_FALSE(this_thread::is_using_local_config());
    EXPECT_EQ(this_thread::get_precision(), default_prec_value);
    EXPECT_EQ(this_thread::get_base(), default_base_value);
}

// -----------------------------------------------------------------------------
// Global Configuration Tests
// -----------------------------------------------------------------------------

TEST_F(GmpxxDefaultsTest, GlobalBasicFunctionality) {
    // Set global values
    global::set_precision(1024);
    global::set_base(16);

    EXPECT_EQ(global::get_precision(), 1024);
    EXPECT_EQ(global::get_base(), 16);

    // Thread-local should see global values when not using local config
    EXPECT_FALSE(this_thread::is_using_local_config());
    EXPECT_EQ(this_thread::get_precision(), 1024);
    EXPECT_EQ(this_thread::get_base(), 16);
}

TEST_F(GmpxxDefaultsTest, GlobalInvalidValues) {
    // Invalid precision
    EXPECT_THROW(global::set_precision(1), std::invalid_argument);
    EXPECT_THROW(global::set_precision(0), std::invalid_argument);

    // Invalid base
    EXPECT_THROW(global::set_base(1), std::invalid_argument);
    EXPECT_THROW(global::set_base(63), std::invalid_argument);
}

TEST_F(GmpxxDefaultsTest, GlobalReset) {
    // Set some values
    global::set_precision(1024);
    global::set_base(16);

    // Reset should restore compile-time defaults
    global::reset_to_defaults();

    EXPECT_EQ(global::get_precision(), default_prec_value);
    EXPECT_EQ(global::get_base(), default_base_value);
}

// -----------------------------------------------------------------------------
// Interaction between Global and Thread-local
// -----------------------------------------------------------------------------

TEST_F(GmpxxDefaultsTest, GlobalThreadLocalInteraction) {
    // Set global values
    global::set_precision(1024);
    global::set_base(16);

    // Thread-local should see global values
    EXPECT_EQ(this_thread::get_precision(), 1024);
    EXPECT_EQ(this_thread::get_base(), 16);

    // Enable local config with different values
    this_thread::set_precision(2048);
    this_thread::set_base(8);

    // Should see local values
    EXPECT_EQ(this_thread::get_precision(), 2048);
    EXPECT_EQ(this_thread::get_base(), 8);

    // Global values should be unchanged
    EXPECT_EQ(global::get_precision(), 1024);
    EXPECT_EQ(global::get_base(), 16);

    // Disable local config
    this_thread::enable_local_config(false);

    // Should see global values again
    EXPECT_EQ(this_thread::get_precision(), 1024);
    EXPECT_EQ(this_thread::get_base(), 16);
}

// =============================================================================
// RAII ScopedThreadLocal Tests
// =============================================================================

TEST_F(GmpxxDefaultsTest, ScopedThreadLocalBasic) {
    // Set initial values
    this_thread::set_precision(512);
    this_thread::set_base(10);

    {
        ScopedThreadLocal scope(1024, 16);

        // Should use scoped values
        EXPECT_EQ(this_thread::get_precision(), 1024);
        EXPECT_EQ(this_thread::get_base(), 16);
        EXPECT_TRUE(this_thread::is_using_local_config());
    }

    // Should restore original values
    EXPECT_EQ(this_thread::get_precision(), 512);
    EXPECT_EQ(this_thread::get_base(), 10);
    EXPECT_TRUE(this_thread::is_using_local_config());
}

TEST_F(GmpxxDefaultsTest, ScopedThreadLocalPartialChange) {
    this_thread::set_precision(512);
    this_thread::set_base(10);

    {
        // Only change precision
        ScopedThreadLocal scope(1024, 0);

        EXPECT_EQ(this_thread::get_precision(), 1024);
        EXPECT_EQ(this_thread::get_base(), 10); // Should remain unchanged
    }

    EXPECT_EQ(this_thread::get_precision(), 512);
    EXPECT_EQ(this_thread::get_base(), 10);
}

TEST_F(GmpxxDefaultsTest, ScopedThreadLocalNested) {
    this_thread::set_precision(256);
    this_thread::set_base(10);

    {
        ScopedThreadLocal scope1(512, 16);
        EXPECT_EQ(this_thread::get_precision(), 512);
        EXPECT_EQ(this_thread::get_base(), 16);

        {
            ScopedThreadLocal scope2(1024, 8);
            EXPECT_EQ(this_thread::get_precision(), 1024);
            EXPECT_EQ(this_thread::get_base(), 8);
        }

        // Should restore to scope1 values
        EXPECT_EQ(this_thread::get_precision(), 512);
        EXPECT_EQ(this_thread::get_base(), 16);
    }

    // Should restore to original values
    EXPECT_EQ(this_thread::get_precision(), 256);
    EXPECT_EQ(this_thread::get_base(), 10);
}

TEST_F(GmpxxDefaultsTest, ScopedThreadLocalFromGlobal) {
    // Start with global config only
    global::set_precision(512);
    global::set_base(10);
    EXPECT_FALSE(this_thread::is_using_local_config());

    {
        ScopedThreadLocal scope(1024, 16);
        EXPECT_TRUE(this_thread::is_using_local_config());
        EXPECT_EQ(this_thread::get_precision(), 1024);
        EXPECT_EQ(this_thread::get_base(), 16);
    }

    // Should restore to global config
    EXPECT_FALSE(this_thread::is_using_local_config());
    EXPECT_EQ(this_thread::get_precision(), 512);
    EXPECT_EQ(this_thread::get_base(), 10);
}

// =============================================================================
// Multi-threading Tests
// =============================================================================

TEST_F(GmpxxDefaultsTest, ThreadIndependence) {
    const int num_threads = 4;
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([i, &success_count]() {
            // Each thread sets different values
            mp_bitcnt_t expected_prec = 512 + i * 256;
            int expected_base = 10 + i;

            this_thread::set_precision(expected_prec);
            this_thread::set_base(expected_base);

            // Verify values are correctly set and isolated
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            if (this_thread::get_precision() == expected_prec && this_thread::get_base() == expected_base && this_thread::is_using_local_config()) {
                success_count++;
            }
        });
    }

    for (auto& t: threads) {
        t.join();
    }

    EXPECT_EQ(success_count.load(), num_threads);
}

TEST_F(GmpxxDefaultsTest, GlobalConfigThreadSafety) {
    const int num_threads = 8;
    const int iterations = 100;
    std::vector<std::thread> threads;
    std::atomic<bool> start_flag{false};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&start_flag, iterations]() {
            // Wait for all threads to be ready
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            // Rapidly change global settings
            for (int j = 0; j < iterations; ++j) {
                mp_bitcnt_t prec = 512 + (j % 1000);
                int base = 2 + (j % 60);

                global::set_precision(prec);
                global::set_base(base);

                // Verify we can read back valid values
                mp_bitcnt_t read_prec = global::get_precision();
                int read_base = global::get_base();

                EXPECT_GE(read_prec, 2);
                EXPECT_GE(read_base, 2);
                EXPECT_LE(read_base, 62);
            }
        });
    }

    // Start all threads simultaneously
    start_flag.store(true);

    for (auto& t: threads) {
        t.join();
    }
}

// =============================================================================
// Environment Variable Tests
// =============================================================================

class EnvironmentTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Save original environment
        original_prec_env_ = std::getenv("GMPXX_MKII_DEFAULT_PREC");
        original_base_env_ = std::getenv("GMPXX_MKII_DEFAULT_BASE");

        // Reset initialization state before each test
        env::reset_initialization();
        global::reset_to_defaults();
    }

    void TearDown() override {
        // Restore original environment
        if (original_prec_env_) {
            setenv("GMPXX_MKII_DEFAULT_PREC", original_prec_env_, 1);
        } else {
            unsetenv("GMPXX_MKII_DEFAULT_PREC");
        }

        if (original_base_env_) {
            setenv("GMPXX_MKII_DEFAULT_BASE", original_base_env_, 1);
        } else {
            unsetenv("GMPXX_MKII_DEFAULT_BASE");
        }

        // Reset everything to clean state
        env::reset_initialization();
        global::reset_to_defaults();
    }

  private:
    const char* original_prec_env_;
    const char* original_base_env_;
};

TEST_F(EnvironmentTest, ValidEnvironmentVariables) {
    // Clear any existing environment variables first
    unsetenv("GMPXX_MKII_DEFAULT_PREC");
    unsetenv("GMPXX_MKII_DEFAULT_BASE");

    // Set valid environment variables
    setenv("GMPXX_MKII_DEFAULT_PREC", "1024", 1);
    setenv("GMPXX_MKII_DEFAULT_BASE", "16", 1);

    // Force initialization with new environment
    bool success = env::initialize_from_environment(true);
    EXPECT_TRUE(success);

    // Check values were applied
    EXPECT_EQ(global::get_precision(), 1024);
    EXPECT_EQ(global::get_base(), 16);

    auto [init_success, warning] = env::get_initialization_status();
    EXPECT_TRUE(init_success);
    EXPECT_TRUE(warning.empty());
}

TEST_F(EnvironmentTest, InvalidEnvironmentVariables) {
    // Clear any existing environment variables first
    unsetenv("GMPXX_MKII_DEFAULT_PREC");
    unsetenv("GMPXX_MKII_DEFAULT_BASE");

    // Set invalid environment variables
    setenv("GMPXX_MKII_DEFAULT_PREC", "invalid", 1);
    setenv("GMPXX_MKII_DEFAULT_BASE", "999", 1);

    // Force initialization with invalid environment
    bool success = env::initialize_from_environment(true);
    EXPECT_FALSE(success);

    // Should fall back to defaults
    EXPECT_EQ(global::get_precision(), default_prec_value);
    EXPECT_EQ(global::get_base(), default_base_value);

    auto [init_success, warning] = env::get_initialization_status();
    EXPECT_FALSE(init_success);
    EXPECT_FALSE(warning.empty());
    EXPECT_TRUE(warning.find("Invalid") != std::string::npos);
}

TEST_F(EnvironmentTest, PartialInvalidEnvironment) {
    // Clear any existing environment variables first
    unsetenv("GMPXX_MKII_DEFAULT_PREC");
    unsetenv("GMPXX_MKII_DEFAULT_BASE");

    // Set one valid, one invalid
    setenv("GMPXX_MKII_DEFAULT_PREC", "2048", 1);
    setenv("GMPXX_MKII_DEFAULT_BASE", "invalid", 1);

    bool success = env::initialize_from_environment(true);
    EXPECT_FALSE(success);

    // Valid one should be applied, invalid one should use default
    EXPECT_EQ(global::get_precision(), 2048);
    EXPECT_EQ(global::get_base(), default_base_value);

    auto [init_success, warning] = env::get_initialization_status();
    EXPECT_FALSE(init_success);
    EXPECT_FALSE(warning.empty());
    EXPECT_TRUE(warning.find("Invalid") != std::string::npos);
    EXPECT_TRUE(warning.find("GMPXX_MKII_DEFAULT_BASE") != std::string::npos);
}

TEST_F(EnvironmentTest, EdgeCaseEnvironmentValues) {
    // Test edge cases for precision
    unsetenv("GMPXX_MKII_DEFAULT_PREC");
    unsetenv("GMPXX_MKII_DEFAULT_BASE");

    // Test minimum valid precision
    setenv("GMPXX_MKII_DEFAULT_PREC", "2", 1);
    setenv("GMPXX_MKII_DEFAULT_BASE", "2", 1);

    bool success = env::initialize_from_environment(true);
    EXPECT_TRUE(success);
    EXPECT_EQ(global::get_precision(), 2);
    EXPECT_EQ(global::get_base(), 2);

    // Test maximum valid base
    env::reset_initialization();
    setenv("GMPXX_MKII_DEFAULT_BASE", "62", 1);

    success = env::initialize_from_environment(true);
    EXPECT_TRUE(success);
    EXPECT_EQ(global::get_base(), 62);

    // Test invalid precision (too small)
    env::reset_initialization();
    setenv("GMPXX_MKII_DEFAULT_PREC", "1", 1);

    success = env::initialize_from_environment(true);
    EXPECT_FALSE(success);
    EXPECT_EQ(global::get_precision(), default_prec_value);

    // Test invalid base (too large)
    env::reset_initialization();
    setenv("GMPXX_MKII_DEFAULT_PREC", "512", 1);
    setenv("GMPXX_MKII_DEFAULT_BASE", "63", 1);

    success = env::initialize_from_environment(true);
    EXPECT_FALSE(success);
    EXPECT_EQ(global::get_precision(), 512);           // valid precision should be applied
    EXPECT_EQ(global::get_base(), default_base_value); // invalid base should use default
}

TEST_F(EnvironmentTest, InitializationResetFunctionality) {
    // Set valid environment
    setenv("GMPXX_MKII_DEFAULT_PREC", "1024", 1);
    setenv("GMPXX_MKII_DEFAULT_BASE", "16", 1);

    bool success = env::initialize_from_environment(true);
    EXPECT_TRUE(success);
    EXPECT_EQ(global::get_precision(), 1024);
    EXPECT_EQ(global::get_base(), 16);

    // Reset initialization
    env::reset_initialization();

    // Values should be back to defaults
    EXPECT_EQ(global::get_precision(), default_prec_value);
    EXPECT_EQ(global::get_base(), default_base_value);

    auto [init_success, warning] = env::get_initialization_status();
    EXPECT_TRUE(init_success); // reset clears warnings
    EXPECT_TRUE(warning.empty());

    // Should be able to initialize again
    success = env::initialize_from_environment();
    EXPECT_TRUE(success);
    EXPECT_EQ(global::get_precision(), 1024); // should pick up environment again
    EXPECT_EQ(global::get_base(), 16);
}

// =============================================================================
// Integration Tests with mpf_class
// =============================================================================

TEST_F(GmpxxDefaultsTest, IntegrationWithMpfClass) {
    // Test that settings actually affect mpf_class creation
    this_thread::set_precision(1024);

    gmpxx::mpf_class x(3.14159);
    EXPECT_EQ(x.get_prec(), 1024);

    // Change precision and create another
    this_thread::set_precision(2048);
    gmpxx::mpf_class y(2.71828);
    EXPECT_EQ(y.get_prec(), 2048);

    // Original should retain its precision
    EXPECT_EQ(x.get_prec(), 1024);
}

TEST_F(GmpxxDefaultsTest, IntegrationWithScopedSettings) {
    this_thread::set_precision(512);

    gmpxx::mpf_class x(1.0);
    EXPECT_EQ(x.get_prec(), 512);

    {
        ScopedThreadLocal scope(2048);
        gmpxx::mpf_class y(2.0);
        EXPECT_EQ(y.get_prec(), 2048);
    }

    // After scope, new objects should use original precision
    gmpxx::mpf_class z(3.0);
    EXPECT_EQ(z.get_prec(), 512);
}

// =============================================================================
// Performance Tests
// =============================================================================

TEST_F(GmpxxDefaultsTest, PerformanceThreadLocalAccess) {
    this_thread::set_precision(1024);

    auto start = std::chrono::high_resolution_clock::now();

    // Many rapid accesses
    for (int i = 0; i < 100000; ++i) {
        volatile mp_bitcnt_t prec = this_thread::get_precision();
        volatile int base = this_thread::get_base();
        (void)prec;
        (void)base; // Suppress unused warnings
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Should be reasonably fast (< 100ms for 100k accesses)
    EXPECT_LT(duration.count(), 100000);
}

// =============================================================================
// Edge Cases and Stress Tests
// =============================================================================

TEST_F(GmpxxDefaultsTest, ExtremePrecisionValues) {
    // Test with very large precision values
    mp_bitcnt_t large_prec = 1000000; // 1 million bits

    EXPECT_NO_THROW(this_thread::set_precision(large_prec));
    EXPECT_EQ(this_thread::get_precision(), large_prec);

    EXPECT_NO_THROW(global::set_precision(large_prec));
    EXPECT_EQ(global::get_precision(), large_prec);
}

TEST_F(GmpxxDefaultsTest, RapidScopedChanges) {
    // Rapidly create and destroy scoped configurations
    for (int i = 0; i < 1000; ++i) {
        ScopedThreadLocal scope(512 + i, 10 + (i % 53));
        EXPECT_EQ(this_thread::get_precision(), 512 + i);
        EXPECT_EQ(this_thread::get_base(), 10 + (i % 53));
    }

    // Should be back to defaults
    EXPECT_EQ(this_thread::get_precision(), default_prec_value);
    EXPECT_EQ(this_thread::get_base(), default_base_value);
}

// =============================================================================
// Main function
// =============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
