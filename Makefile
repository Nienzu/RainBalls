CC = g++

CXXFLAGS = -std=c++11

LDLIBS = -framework OpenGl -framework Cocoa -framework IOKit -framework CoreVideo -lGLEW -I/usr/local/include -lglfw3  -I/usr/local/assimp -lassimp

objects = main.o Shader.o Camera.o

.PHONY: all
all: rainballs

rainballs: $(objects)
	$(CC) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

main.o: main.cpp Shader.h Camera.h
	$(CC) -c $< $(CXXFLAGS)

Shader.o: Shader.cpp Shader.h
	$(CC) -c $< $(CXXFLAGS)

Camera.o: Camera.cpp Camera.h
	$(CC) -c $< $(CXXFLAGS)

.PHONY: clean
clean:
	rm -f rainballs $(objects)
