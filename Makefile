CXX = g++-12
CXXFLAGS = -Wall -Wextra
LDFLAGS = -L/home/docker/gmpxx_mkII/i/GMP-6.3.0/lib -lgmp
INCLUDES = -I/home/docker/gmpxx_mkII/i/GMP-6.3.0/include -I/home/docker/gmpxx_mkII/
RPATH_FLAGS = -Wl,-rpath,/home/docker/gmpxx_mkII/i/GMP-6.3.0/lib

TARGET = test_gmpxx_mkII
TARGET_ORIG = test_gmpxx
TARGET_COMPAT = test_gmpxx_compat
TARGET_MKIISR = test_gmpxx_mkIISR

GMPXX_MODE_ORIGINAL = -DUSE_ORIGINAL_GMPXX
GMPXX_MODE_COMPAT = -D___GMPXX_POSSIBLE_BUGS___ -D___GMPXX_STRICT_COMPATIBILITY___
GMPXX_MODE_MKII =
GMPXX_MODE_MKIISR = -D___GMPXX_MKII_NOPRECCHANGE___

SOURCES = test_gmpxx_mkII.cpp
HEADERS = gmpxx_mkII.h
OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS_ORIG = $(SOURCES:.cpp=_orig.o)
OBJECTS_COMPAT = $(SOURCES:.cpp=_compat.o)
OBJECTS_MKIISR = $(SOURCES:.cpp=_mkiisr.o)

ORIG_TESTS_DIR = orig_tests/cxx
ORIG_TESTS_SOURCES = $(wildcard $(ORIG_TESTS_DIR)/*.cc)
ORIG_TESTS = $(ORIG_TESTS_SOURCES:$(ORIG_TESTS_DIR)/%.cc=$(ORIG_TESTS_DIR)/%)

BENCHMARKS_DIR = benchmarks/00_inner_product
BENCHMARKS0 = $(addprefix $(BENCHMARKS_DIR)/,inner_product_gmp_10_naive inner_product_gmp_11_openmp)
BENCHMARKS1 = $(addprefix $(BENCHMARKS_DIR)/,inner_product_gmp_12_mpblas inner_product_gmp_12_mpblas_mkII inner_product_gmp_12_mpblas_compat inner_product_gmp_12_mpblas_mkIISR inner_product_gmp_13_mpblas_openmp inner_product_gmp_13_mpblas_openmp_compat inner_product_gmp_13_mpblas_openmp_mkII inner_product_gmp_13_mpblas_openmp_mkIISR)

all: $(TARGET) $(TARGET_ORIG) $(TARGET_COMPAT) $(TARGET_MKIISR) $(ORIG_TESTS) $(BENCHMARKS0) $(BENCHMARKS1)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RPATH_FLAGS)

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(@:.o=.cpp)

$(TARGET_ORIG): $(OBJECTS_ORIG)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $(TARGET_ORIG) $(OBJECTS_ORIG) $(RPATH_FLAGS) -lgmpxx $(LDFLAGS)

$(OBJECTS_ORIG): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -c $(SOURCES) -o $@

$(TARGET_COMPAT): $(OBJECTS_COMPAT)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $(TARGET_COMPAT) $(OBJECTS_COMPAT) $(LDFLAGS) $(RPATH_FLAGS)

$(OBJECTS_COMPAT): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -c $(SOURCES) -o $@

$(TARGET_MKIISR): $(OBJECTS_MKIISR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $(TARGET_MKIISR) $(OBJECTS_MKIISR) $(LDFLAGS) $(RPATH_FLAGS)

$(OBJECTS_MKIISR): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_MKIISR) -c $(SOURCES) -o $@

$(ORIG_TESTS): $(ORIG_TESTS_DIR)/t-% : $(ORIG_TESTS_DIR)/t-%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $< $(LDFLAGS)

$(BENCHMARKS_DIR)/%: $(BENCHMARKS_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS)

$(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas: $(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $< $(LDFLAGS)

$(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas_mkII: $(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas_compat: $(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas_mkIISR: $(BENCHMARKS_DIR)/inner_product_gmp_12_mpblas.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS_DIR)/inner_product_gmp_13_mpblas_openmp: $(BENCHMARKS_DIR)/inner_product_gmp_13_mpblas_openmp.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $< $(LDFLAGS)

$(BENCHMARKS_DIR)/inner_product_gmp_13_mpblas_openmp_mkII: $(BENCHMARKS_DIR)/inner_product_gmp_13_mpblas_openmp.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS_DIR)/inner_product_gmp_13_mpblas_openmp_compat: $(BENCHMARKS_DIR)/inner_product_gmp_13_mpblas_openmp.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS_DIR)/inner_product_gmp_13_mpblas_openmp_mkIISR: $(BENCHMARKS_DIR)/inner_product_gmp_13_mpblas_openmp.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

check:
	./$(TARGET) ./$(TARGET_ORIG) ./$(TARGET_COMPAT) ./$(TARGET_MKIISR)

clean:
	rm -f $(TARGET) $(TARGET_ORIG) $(TARGET_COMPAT) $(TARGET_MKIISR) $(OBJECTS) $(OBJECTS_ORIG) $(OBJECTS_COMPAT) $(OBJECTS_MKIISR) $(BENCHMARKS0) $(BENCHMARKS1) $(TARGETS_TESTS) $(ORIG_TESTS)*~

.PHONY: all clean check $(TARGETS_TESTS)
