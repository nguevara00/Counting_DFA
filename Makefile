CXX = c++
CXXFLAGS = -g -std=c++20 -Wall -Wextra -Wpedantic
LDFLAGS = -lgmpxx -lgmp

TARGET = main
OBJECTS = main.o

$(TARGET): $(OBJECTS)
    $(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
    $(CXX) $(CXXFLAGS) -c $< -o $@

main.o: main.cpp

.PHONY: clean
clean:
    rm -f $(OBJECTS) $(TARGET)
