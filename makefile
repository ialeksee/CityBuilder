# the compiler to use
CC = clang++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -fcolor-diagnostics -fansi-escape-codes  -std=c++20 -mmacosx-version-min=13.0 -framework OpenGL -framework GLUT -framework Carbon -v -DUSE_OPENAL 
  
#files to link:
LFLAGS =  -L/opt/homebrew/Cellar/libpng/1.6.39/lib/ -lpng  
  
# the name to use for both the target source file, and the output file:
TARGET = city_builder
  
all: $(TARGET)
  
$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp $(LFLAGS)
