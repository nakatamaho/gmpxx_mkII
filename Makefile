CXX = g++-12
CXXFLAGS = -Wall -Wextra
LDFLAGS = -L/home/docker/gmpxx_mkII/i/GMP-6.3.0/lib -lgmp
INCLUDES = -I/home/docker/gmpxx_mkII/i/GMP-6.3.0/include -I/home/docker/gmpxx_mkII/
RPATH_FLAGS = -Wl,-rpath,/home/docker/gmpxx_mkII/i/GMP-6.3.0/lib

TARGET = test_gmpxx_mkII
TARGET_ORIG = test_gmpxx
TARGET_MKIISR = test_gmpxx_mkIISR

SOURCES = test_gmpxx_mkII.cpp
HEADERS = gmpxx_mkII.h
OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS_ORIG = $(SOURCES:.cpp=_orig.o)
OBJECTS_MKIISR = $(SOURCES:.cpp=_mkiisr.o)

ORIG_TESTS_DIR = tests/cxx
ORIG_TESTS_SOURCES = $(wildcard $(ORIG_TESTS_DIR)/*.cc)
ORIG_TESTS = $(ORIG_TESTS_SOURCES:$(ORIG_TESTS_DIR)/%.cc=$(ORIG_TESTS_DIR)/%)

BENCHMARKS_DIR = benchmarks/00_inner_product
BENCHMARKS0 = $(addprefix $(BENCHMARKS_DIR)/,inner_product_gmp_10_naive inner_product_gmp_11_openmp)
BENCHMARKS1 = $(addprefix $(BENCHMARKS_DIR)/,inner_product_gmp_12_mpblas inner_product_gmp_12_mpblas_mkII inner_product_gmp_12_mpblas_mkIISR)

all: $(TARGET) $(TARGET_ORIG) $(TARGET_MKIISR) $(ORIG_TESTS) $(BENCHMARKS0) $(BENCHMARKS1)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RPATH_FLAGS)

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(@:.o=.cpp)

$(TARGET_ORIG): $(OBJECTS_ORIG)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET_ORIG) $(OBJECTS_ORIG) $(RPATH_FLAGS) -lgmpxx $(LDFLAGS)

$(OBJECTS_ORIG): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -D___GMPXX_STRICT_COMPATIBILITY___ -c $(SOURCES) -o $@

$(TARGET_MKIISR): $(OBJECTS_MKIISR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -D___GMPXX_MKII_NOPRECCHANGE___ -o $(TARGET_MKIISR) $(OBJECTS_MKIISR) $(LDFLAGS) $(RPATH_FLAGS)

$(OBJECTS_MKIISR): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -D___GMPXX_MKII_NOPRECCHANGE___ -c $(SOURCES) -o $@

$(ORIG_TESTS): $(ORIG_TESTS_SOURCES) $(HEADERS)
	sed -i 's/#include "gmpxx\.h"/#include "gmpxx_mkII\.h"\nusing namespace gmp;/' $<
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS)

$(BENCHMARKS_DIR)/%: $(BENCHMARKS_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS)

$(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas: $(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS)

$(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas_mkII: $(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -D___GMPXX_STRICT_COMPATIBILITY___ -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas_mkIISR: $(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -D__GMPXX_MKII_NOPRECCHANGE__ -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

check:
	./$(TARGET) ./$(TARGET_ORIG) ./$(TARGET_NOPRECCHANGE) ./$(TARGET_MKIISR)

clean:
	rm -f $(TARGET) $(TARGET_ORIG) $(TARGET_NOPRECCHANGE) $(TARGET_MKIISR) $(OBJECTS) $(OBJECTS_ORIG) $(OBJECTS_NOPRECCHANGE) $(OBJECTS_MKIISR) $(BENCHMARKS0) $(BENCHMARKS1) $(TARGETS_TESTS) *~

.PHONY: all clean check $(TARGETS_TESTS)
