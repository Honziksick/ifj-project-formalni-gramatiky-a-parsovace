# Makefile for compiling and running parsing table maker

# Compiler
CXX = g++
CXXFLAGS = -Wall -g

# Source files
LL_SRCS = lltable_maker.cpp

# Header files
HEADERS = nonterminals.h terminals.h rules.h

# Executable name
LL_TARGET = lltable_maker

.PHONY: all clean run

# Default rule
all: $(LL_TARGET)

# Rule to create the executable
$(LL_TARGET): $(LL_SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(LL_TARGET) $(LL_SRCS)

# Clean rule to remove compiled files
clean:
	rm -f $(LL_TARGET) *.out

# Run rule to execute the program
run: $(LL_TARGET) $(PREC_TARGET)
	./$(LL_TARGET) > lltable.out
