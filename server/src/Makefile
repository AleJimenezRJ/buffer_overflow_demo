# Compiler and Flags
CC  := gcc
CXX := g++
CFLAGS := -g -c -std=c++17  # Enable C++17 due to using filesystem
CXXFLAGS := -g -c -std=c++17  # Enable C++17
LDFLAGS := -std=c++17  # Ensure linking uses C++17
DEPS := Socket.h Container.h
TARGETS := Server.out

# Objects to build
ALL_OBJS := Server.o VSocket.o Socket.o Container.o
$(info $$TARGETS = $(TARGETS))

# Rules for building .o files from .cpp/.cc files
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $< -o $@

%.o: %.cc $(DEPS)
	$(CXX) $(CXXFLAGS) $< -o $@

# Rule for generating the final executable
%.out: $(ALL_OBJS)
	$(info $$^ = $^)
	$(CXX) $^ $(LDFLAGS) -o $@

# Special rule for Server.out
Server.out: $(ALL_OBJS)
	$(CXX) $(ALL_OBJS) $(LDFLAGS) -o $@

# Clean rule to remove object and binary files
clean:
	rm -f *.o *.out
