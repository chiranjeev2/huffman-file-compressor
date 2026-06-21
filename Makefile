CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -Iinclude
SRCS = src/BitReaderWriter.cpp src/Encoder.cpp src/Decoder.cpp src/main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = huffman

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)