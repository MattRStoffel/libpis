# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Source files
SRCS = main.c pid.c sim.c

# Object files
OBJS = $(SRCS:.c=.o)

# Output executable
EXEC = pid_simulator

# Default target: build the executable
all: $(EXEC)

# Rule to build the executable from object files
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

# Rule to build the object files from source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove object files and the executable
clean:
	rm -f $(OBJS) $(EXEC)
	rm -f *.csv

# Run the simulator after building
run: $(EXEC)
	./$(EXEC)
	python plot.py

# Additional target to recompile and run the simulation
rebuild: clean all run

