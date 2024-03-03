CXX = g++-12
CXXFLAGS = -Wall -Wextra
LDFLAGS = -L/home/docker/mpfrcxx/i/GMP-6.3.0/lib -lgmp
INCLUDES = -I/home/docker/mpfrcxx/i/GMP-6.3.0/include

TARGET = test_mpf_class

SOURCES = test_mpf_class.cpp
HEADERS = mpf_class_mkII.h
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(@:.o=.cpp)

clean:
	rm -f $(TARGET) $(OBJECTS) *~

.PHONY: all clean
