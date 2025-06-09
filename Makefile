CXX = g++
CXXFLAGS = -Wall -Wextra -O2
PREFIX = /usr/local

LDFLAGS = -L/home/docker/gmpxx_mkII/i/GMP-6.3.0/lib -lgmp
INCLUDES = -I/home/docker/gmpxx_mkII/i/GMP-6.3.0/include -I/home/docker/gmpxx_mkII/
RPATH_FLAGS = -Wl,-rpath,/home/docker/gmpxx_mkII/i/GMP-6.3.0/lib

TARGET = test_gmpxx_mkII
TARGET_ORIG = test_gmpxx
TARGET_COMPAT = test_gmpxx_compat
TARGET_MKIISR = test_gmpxx_mkIISR
TARGET_TEST_ENV = test_env

COMMIT_HASH := $(shell git rev-parse HEAD)

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

SOURCE_TEST_ENV = test_env.cpp

ORIG_TESTS_DIR = orig_tests/cxx
ORIG_TESTS_SOURCES = orig_tests/cxx/t-assign.cc orig_tests/cxx/t-binary.cc orig_tests/cxx/t-cast.cc orig_tests/cxx/t-constr.cc \
orig_tests/cxx/t-cxx11.cc orig_tests/cxx/t-do-exceptions-work-at-all-with-this-compiler.cc orig_tests/cxx/t-headers.cc \
orig_tests/cxx/t-iostream.cc orig_tests/cxx/t-istream.cc orig_tests/cxx/t-locale.cc orig_tests/cxx/t-misc.cc orig_tests/cxx/t-mix.cc \
orig_tests/cxx/t-ops.cc orig_tests/cxx/t-ops2f.cc orig_tests/cxx/t-ops2qf.cc orig_tests/cxx/t-ops2z.cc orig_tests/cxx/t-ops3.cc \
orig_tests/cxx/t-ostream.cc orig_tests/cxx/t-ternary.cc orig_tests/cxx/t-unary.cc
#orig_tests/cxx/t-prec.cc orig_tests/cxx/t-rand.cc
ORIG_TESTS = $(filter-out $(ORIG_TESTS_DIR)/t-istream, $(ORIG_TESTS_SOURCES:$(ORIG_TESTS_DIR)/%.cc=$(ORIG_TESTS_DIR)/%))

EXAMPLES_SOURCES = examples/example01.cpp examples/example02.cpp examples/example03.cpp examples/example04.cpp
EXAMPLES_OBJECTS = $(EXAMPLES_SOURCES:.cpp=.o)
EXAMPLES_EXECUTABLES = $(EXAMPLES_SOURCES:.cpp=)

CXXFLAGS_BENCH = -O2 -fopenmp -Wall -Wextra
BENCHMARKS00_DIR = benchmarks/00_Rdot
BENCHMARKS00_0 = $(addprefix $(BENCHMARKS00_DIR)/,Rdot_gmp_C_native_01 Rdot_gmp_C_native_openmp_01)
BENCHMARKS00_1 = $(addprefix $(BENCHMARKS00_DIR)/,\
Rdot_gmp_kernel_01_orig Rdot_gmp_kernel_01_mkII Rdot_gmp_kernel_01_mkIISR \
Rdot_gmp_kernel_02_orig Rdot_gmp_kernel_02_mkII Rdot_gmp_kernel_02_mkIISR \
Rdot_gmp_kernel_03_orig Rdot_gmp_kernel_03_mkII Rdot_gmp_kernel_03_mkIISR \
Rdot_gmp_kernel_04_orig Rdot_gmp_kernel_04_mkII Rdot_gmp_kernel_04_mkIISR \
Rdot_gmp_kernel_openmp_01_orig Rdot_gmp_kernel_openmp_01_mkII Rdot_gmp_kernel_openmp_01_mkIISR \
Rdot_gmp_kernel_openmp_02_orig Rdot_gmp_kernel_openmp_02_mkII Rdot_gmp_kernel_openmp_02_mkIISR)

BENCHMARKS01_DIR = benchmarks/01_Raxpy
BENCHMARKS01_0 = $(addprefix $(BENCHMARKS01_DIR)/,Raxpy_gmp_C_native_01 Raxpy_gmp_C_native_openmp_01)
BENCHMARKS01_1 = $(addprefix $(BENCHMARKS01_DIR)/,\
Raxpy_gmp_kernel_01_orig Raxpy_gmp_kernel_01_mkII Raxpy_gmp_kernel_01_mkIISR \
Raxpy_gmp_kernel_02_orig Raxpy_gmp_kernel_02_mkII Raxpy_gmp_kernel_02_mkIISR \
Raxpy_gmp_kernel_openmp_01_orig Raxpy_gmp_kernel_openmp_01_mkII Raxpy_gmp_kernel_openmp_01_mkIISR \
Raxpy_gmp_kernel_openmp_02_orig Raxpy_gmp_kernel_openmp_02_mkII Raxpy_gmp_kernel_openmp_02_mkIISR)

BENCHMARKS02_DIR = benchmarks/02_Rgemv
BENCHMARKS02_0 = $(addprefix $(BENCHMARKS02_DIR)/,Rgemv_gmp_C_native_01 Rgemv_gmp_C_native_openmp_01)
BENCHMARKS02_1 = $(addprefix $(BENCHMARKS02_DIR)/,\
Rgemv_gmp_kernel_01_orig Rgemv_gmp_kernel_01_mkII Rgemv_gmp_kernel_01_mkIISR \
Rgemv_gmp_kernel_openmp_01_orig Rgemv_gmp_kernel_openmp_01_mkII Rgemv_gmp_kernel_openmp_01_mkIISR \
Rgemv_gmp_kernel_02_orig Rgemv_gmp_kernel_02_mkII Rgemv_gmp_kernel_02_mkIISR \
Rgemv_gmp_kernel_openmp_02_orig Rgemv_gmp_kernel_openmp_02_mkII Rgemv_gmp_kernel_openmp_02_mkIISR)

BENCHMARKS03_DIR = benchmarks/03_Rgemm
BENCHMARKS03_0 = $(addprefix $(BENCHMARKS03_DIR)/,Rgemm_gmp_C_native_01 Rgemm_gmp_C_native_openmp_01 Rgemm_gmp_C_native_02 Rgemm_gmp_C_native_openmp_02)
BENCHMARKS03_1 = $(addprefix $(BENCHMARKS03_DIR)/,\
Rgemm_gmp_kernel_01_orig Rgemm_gmp_kernel_01_mkII Rgemm_gmp_kernel_01_mkIISR \
Rgemm_gmp_kernel_02_orig Rgemm_gmp_kernel_02_mkII Rgemm_gmp_kernel_02_mkIISR \
Rgemm_gmp_kernel_03_orig Rgemm_gmp_kernel_03_mkII Rgemm_gmp_kernel_03_mkIISR \
Rgemm_gmp_kernel_openmp_01_orig Rgemm_gmp_kernel_openmp_01_mkII Rgemm_gmp_kernel_openmp_01_mkIISR \
Rgemm_gmp_kernel_openmp_02_orig Rgemm_gmp_kernel_openmp_02_mkII Rgemm_gmp_kernel_openmp_02_mkIISR \
Rgemm_gmp_kernel_openmp_03_orig Rgemm_gmp_kernel_openmp_03_mkII Rgemm_gmp_kernel_openmp_03_mkIISR)

all: gmpxx_mkII.h $(TARGET) $(TARGET_ORIG) $(TARGET_COMPAT) $(TARGET_MKIISR) $(TARGET_TEST_ENV) $(EXAMPLES_EXECUTABLES) $(ORIG_TESTS) $(BENCHMARKS00_0) $(BENCHMARKS00_1) $(BENCHMARKS01_0) $(BENCHMARKS01_1) $(BENCHMARKS02_0) $(BENCHMARKS02_1) $(BENCHMARKS03_0) $(BENCHMARKS03_1) $(BENCHMARKS03_2) $(BENCHMARKS03_3)

includedir = $(PREFIX)/include

install: install-headers

install-headers: $(HEADERS)
	@mkdir -p $(includedir)
	@install -m 644 $(HEADERS) $(includedir)
	@echo "Installed headers to $(includedir)"

gmpxx_mkII.h: gmpxx_mkII.h.in
	sed 's/@GIT_COMMIT_HASH@/$(COMMIT_HASH)/g' gmpxx_mkII.h.in > gmpxx_mkII.h

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

$(TARGET_TEST_ENV): $(SOURCE_TEST_ENV) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET_TEST_ENV) $(SOURCE_TEST_ENV) $(LDFLAGS) $(RPATH_FLAGS)

$(OBJECTS_MKIISR): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_MKIISR) -c $(SOURCES) -o $@

$(ORIG_TESTS): $(ORIG_TESTS_DIR)/t-% : $(ORIG_TESTS_DIR)/t-%.cc $(HEADERS)
	$(CXX) -g $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $< $(LDFLAGS)

$(ORIG_TESTS_DIR)/t-istream: $(ORIG_TESTS_DIR)/t-istream.cc $(ORIG_TESTS_DIR)/trace.c $(HEADERS)
	$(CXX) -g $(CXXFLAGS) $(INCLUDES) $(GMPXX_MODE_COMPAT) -o $@ $^ $(LDFLAGS)

$(EXAMPLES_OBJECTS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(EXAMPLES_EXECUTABLES): %: %.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS00_DIR)/%: $(BENCHMARKS00_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_01_orig: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< $(LDFLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_01_mkII: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_01_mkIISR: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_02_orig: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< $(LDFLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_02_mkII: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_02_mkIISR: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_03_orig: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_03.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< $(LDFLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_03_mkII: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_03.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_03_mkIISR: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_03.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_04_orig: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_04.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< $(LDFLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_04_mkII: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_04.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_04_mkIISR: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_04.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_01_orig: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_01_mkII: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_01_mkIISR: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_02_orig: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< $(LDFLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_02_mkII: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_02_mkIISR: $(BENCHMARKS00_DIR)/Rdot_gmp_kernel_openmp_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS01_DIR)/%: $(BENCHMARKS01_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_01_orig: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< -lgmpxx $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_01_mkII: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_01_mkIISR: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_02_orig: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< -lgmpxx $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_02_mkII: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_02_mkIISR: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_01_orig: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< -lgmpxx $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_01_mkII: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_01_mkIISR: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_02_orig: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< -lgmpxx $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_02_mkII: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_02_mkIISR: $(BENCHMARKS01_DIR)/Raxpy_gmp_kernel_openmp_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/%: $(BENCHMARKS02_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_01_orig: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< -lgmpxx $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_01_mkII: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_01_mkIISR: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_01_orig: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< -lgmpxx $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_01_mkII: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_01_mkIISR: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_02_orig: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< -lgmpxx $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_02_mkII: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_02_mkIISR: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_02_orig: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< -lgmpxx $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_02_mkII: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_02_mkIISR: $(BENCHMARKS02_DIR)/Rgemv_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/%: $(BENCHMARKS03_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_01_orig: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_01_mkII: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKII) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_01_mkIISR: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_02_orig: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_02_mkII: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKII) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_02_mkIISR: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_03_orig: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_03.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_03_mkII: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_03.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKII) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_03_mkIISR: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_03.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_01_orig: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< -lgmpxx $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_01_mkII: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_01_mkIISR: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_01.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_02_orig: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< -lgmpxx $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_02_mkII: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_02_mkIISR: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_02.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_03_orig: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_03.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -o $@ $< -lgmpxx $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_ORIGINAL) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_03_mkII: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_03.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

$(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_03_mkIISR: $(BENCHMARKS03_DIR)/Rgemm_gmp_kernel_openmp_03.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -o $@ $< $(LDFLAGS) $(RPATH_FLAGS)
	$(CXX) $(CXXFLAGS_BENCH) $(INCLUDES) $(GMPXX_MODE_MKIISR) -S -fverbose-asm -g -o $@.s $< $(LDFLAGS)

check: ./$(TARGET) ./$(TARGET_ORIG) ./$(TARGET_COMPAT) ./$(TARGET_MKIISR) $(ORIG_TESTS)
	./$(TARGET) ./$(TARGET_ORIG) ./$(TARGET_COMPAT) ./$(TARGET_MKIISR)
	for test in $^ ; do \
		echo "./$$test"; ./$$test ; \
	done
	make check_env

check_env: $(TARGET)
	@unset GMPXX_MKII_DEFAULT_PREC; \
	 ./$(TARGET) > output.txt; \
	 grep "Default precision: 512" output.txt && echo "Test 1 passed!" || echo "Test 1 failed!"
	@export GMPXX_MKII_DEFAULT_PREC=256; \
	 ./$(TARGET) > output.txt; \
	 grep "Default precision: 256" output.txt && echo "Test 2 passed!" || echo "Test 2 failed!"
	@export GMPXX_MKII_DEFAULT_PREC=1024; \
	 ./$(TARGET) > output.txt; \
	 grep "Default precision: 1024" output.txt && echo "Test 3 passed!" || echo "Test 3 failed!"
	@export GMPXX_MKII_DEFAULT_PREC=0.5; \
	 ./$(TARGET) > output.txt 2>&1 || true; \
	 grep "Error: Invalid GMPXX_MKII_DEFAULT_PREC value" output.txt && echo "Test 4 passed!" || echo "Test 4 failed!"

examples: $(EXAMPLES_EXECUTABLES)

OS_NAME = $(shell uname -a | awk '{print $$1}')
LOG00_NAME = log.$(shell basename $(BENCHMARKS00_DIR)).$(OS_NAME).`python ../../cpumodel.py`
LOG01_NAME = log.$(shell basename $(BENCHMARKS01_DIR)).$(OS_NAME).`python ../../cpumodel.py`
LOG02_NAME = log.$(shell basename $(BENCHMARKS02_DIR)).$(OS_NAME).`python ../../cpumodel.py`
LOG03_NAME = log.$(shell basename $(BENCHMARKS03_DIR)).$(OS_NAME).`python ../../cpumodel.py`

benchmark: $(BENCHMARKS00_0) $(BENCHMARKS00_1) $(BENCHMARKS01_0) $(BENCHMARKS01_1) $(BENCHMARKS02_0) $(BENCHMARKS02_1) $(BENCHMARKS03_0) $(BENCHMARKS03_1)
	cd $(BENCHMARKS00_DIR); bash go.sh 2>&1 | tee ../../$(LOG00_NAME) | tee $(LOG00_NAME) ; python plot.py $(LOG00_NAME)
	cd $(BENCHMARKS01_DIR); bash go.sh 2>&1 | tee ../../$(LOG01_NAME) | tee $(LOG01_NAME) ; python plot.py $(LOG01_NAME)
	cd $(BENCHMARKS02_DIR); bash go.sh 2>&1 | tee ../../$(LOG02_NAME) | tee $(LOG02_NAME) ; python plot.py $(LOG02_NAME)
	cd $(BENCHMARKS03_DIR); bash go.sh 2>&1 | tee ../../$(LOG03_NAME) | tee $(LOG03_NAME) ; python plot.py $(LOG03_NAME)

clean:
	rm -f $(TARGET) $(TARGET_ORIG) $(TARGET_COMPAT) $(TARGET_MKIISR) $(OBJECTS) $(OBJECTS_ORIG) $(OBJECTS_COMPAT) $(OBJECTS_MKIISR) $(BENCHMARKS00_0) $(BENCHMARKS00_1) $(BENCHMARKS00_DIR)/gmon* $(BENCHMARKS00_DIR)/gprof* $(BENCHMARKS03_DIR)/gmon* $(BENCHMARKS03_DIR)/gprof* $(BENCHMARKS01_0) $(BENCHMARKS01_1) $(BENCHMARKS03_0) $(BENCHMARKS03_1) $(BENCHMARKS03_2) $(BENCHMARKS03_3) $(TARGETS_TESTS) $(EXAMPLES_OBJECTS) $(EXAMPLES_EXECUTABLES) $(ORIG_TESTS)*~

.PHONY: all clean check $(TARGETS_TESTS) examples
