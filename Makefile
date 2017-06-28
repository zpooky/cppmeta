# Declaration of variables
CC = g++
CC_FLAGS = -ggdb -std=c++14 -pedantic

# File names
EXEC = main
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

# To obtain object files
# $@ Contains the target file name.
# $< Contains the first dependency file name.
%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
