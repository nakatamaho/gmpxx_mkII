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
ORIG_TESTS = $(filter-out $(ORIG_TESTS_DIR)/t-istream, $(ORIG_TESTS_SOURCES:$(ORIG_TESTS_DIR)/%.cc=$(ORIG_TESTS_DIR)/%))

EXAMPLES_SOURCES = examples/example01.cpp examples/example02.cpp examples/example03.cpp examples/example04.cpp
EXAMPLES_OBJECTS = $(EXAMPLES_SOURCES:.cpp=.o)
EXAMPLES_EXECUTABLES = $(EXAMPLES_SOURCES:.cpp=)

BENCHMARKS00_DIR = benchmarks/00_inner_product
BENCHMARKS00_0 = $(addprefix $(BENCHMARKS00_DIR)/,inner_product_gmp_10_naive inner_product_gmp_11_openmp)
BENCHMARKS00_1 = $(addprefix $(BENCHMARKS00_DIR)/,inner_product_gmp_12_mpblas inner_product_gmp_12_mpblas_mkII inner_product_gmp_12_mpblas_compat inner_product_gmp_12_mpblas_mkIISR inner_product_gmp_13_mpblas_openmp inner_product_gmp_13_mpblas_openmp_compat inner_product_gmp_13_mpblas_openmp_mkII inner_product_gmp_13_mpblas_openmp_mkIISR)

BENCHMARKS03_DIR = benchmarks/03_gemm
BENCHMARKS03_0 = $(addprefix $(BENCHMARKS03_DIR)/,gemm_gmp_10_naive gemm_gmp_11_jli gemm_gmp_11_jli_openmp)
BENCHMARKS03_1 = $(addprefix $(BENCHMARKS03_DIR)/,gemm_gmp_30_mpblaslike_naive_ijl)

all: $(TARGET) $(TARGET_ORIG) $(TARGET_COMPAT) $(TARGET_MKIISR) $(EXAMPLES_EXECUTABLES) $(ORIG_TESTS) $(BENCHMARKS00_0) $(BENCHMARKS00_1) $(BENCHMARKS03_0) $(BENCHMARKS03_1)

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

$(ORIG_TESTS): $(ORIG_TESTS_DIR)/t-% : $(ORIG_TESTS_DIR)/t-%.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $< $(LDFLAGS)

$(ORIG_TESTS_DIR)/t-istream: $(ORIG_TESTS_DIR)/t-istream.cc $(ORIG_TESTS_DIR)/trace.c $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $^ $(LDFLAGS)

$(EXAMPLES_OBJECTS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	
$(EXAMPLES_EXECUTABLES): %: %.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS00_DIR)/%: $(BENCHMARKS00_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/inner_product_gmp_12_mpblas: $(BENCHMARKS00_DIR)/inner_product_gmp_12_mpblas.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/inner_product_gmp_12_mpblas_mkII: $(BENCHMARKS00_DIR)/inner_product_gmp_12_mpblas.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS00_DIR)/inner_product_gmp_12_mpblas_compat: $(BENCHMARKS00_DIR)/inner_product_gmp_12_mpblas.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS00_DIR)/inner_product_gmp_12_mpblas_mkIISR: $(BENCHMARKS00_DIR)/inner_product_gmp_12_mpblas.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS00_DIR)/inner_product_gmp_13_mpblas_openmp: $(BENCHMARKS00_DIR)/inner_product_gmp_13_mpblas_openmp.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/inner_product_gmp_13_mpblas_openmp_mkII: $(BENCHMARKS00_DIR)/inner_product_gmp_13_mpblas_openmp.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS00_DIR)/inner_product_gmp_13_mpblas_openmp_compat: $(BENCHMARKS00_DIR)/inner_product_gmp_13_mpblas_openmp.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS00_DIR)/inner_product_gmp_13_mpblas_openmp_mkIISR: $(BENCHMARKS00_DIR)/inner_product_gmp_13_mpblas_openmp.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS03_DIR)/%: $(BENCHMARKS03_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS)

check: ./$(TARGET) ./$(TARGET_ORIG) ./$(TARGET_COMPAT) ./$(TARGET_MKIISR) $(ORIG_TESTS)
	./$(TARGET) ./$(TARGET_ORIG) ./$(TARGET_COMPAT) ./$(TARGET_MKIISR)
	for test in $^ ; do \
		echo "./$$test"; ./$$test ; \
	done

examples: $(EXAMPLES_EXECUTABLES)

clean:
	rm -f $(TARGET) $(TARGET_ORIG) $(TARGET_COMPAT) $(TARGET_MKIISR) $(OBJECTS) $(OBJECTS_ORIG) $(OBJECTS_COMPAT) $(OBJECTS_MKIISR) $(BENCHMARKS00_0) $(BENCHMARKS00_1) $(TARGETS_TESTS) $(EXAMPLES_OBJECTS) $(EXAMPLES_EXECUTABLES) $(ORIG_TESTS)*~

.PHONY: all clean check $(TARGETS_TESTS) examples
