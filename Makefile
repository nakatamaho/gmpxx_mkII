CXX = g++-12
CXXFLAGS = -Wall -Wextra
LDFLAGS = -L/home/docker/gmpxx_mkII/i/GMP-6.3.0/lib -lgmp
INCLUDES = -I/home/docker/gmpxx_mkII/i/GMP-6.3.0/include

TARGET = test_gmpxx_mkII

SOURCES = test_gmpxx_mkII.cpp
HEADERS = gmpxx_mkII.h
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(@:.o=.cpp)

clean:
	rm -f $(TARGET) $(OBJECTS) *~

.PHONY: all clean
