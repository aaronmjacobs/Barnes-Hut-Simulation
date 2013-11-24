CC = g++
COMPILE_FLAGS = -w
LINK_FLAGS = -DGL_GLEXT_PROTOTYPES -framework OpenGL -framework GLUT -w
EXECUTABLE = a.out
SOURCES = $(wildcard *.cpp **/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

BUILD = $(SOURCES) $(EXECUTABLE)

all: $(BUILD)

debug: LINK_FLAGS += -g
debug: COMPILE_FLAGS += -g
debug: $(BUILD)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LINK_FLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) -c $< -o $@ $(COMPILE_FLAGS)

clean:
	find . -name '*.o' -type f -delete
	rm $(EXECUTABLE)