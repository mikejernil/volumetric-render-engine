TARGET = VolumeApp.exe

CXX = g++
WINDRES = windres

CXXFLAGS = -std=c++17 -Wall -DM_PI=3.14159265358979323846 -I./dependencies/glew/include/GL
LDFLAGS = -L./dependencies/glew/lib/Release/x64
LDLIBS = -luser32 -lgdi32 -lcomdlg32 -lglew32 -lopengl32

SRC = VolumeApp.cpp \
      $(wildcard src/*.cpp) \
      $(wildcard src/*/*.cpp) \
      $(wildcard src/*/*/*.cpp)

OBJS = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS) OGL.o
	$(CXX) $(OBJS) OGL.o $(LDFLAGS) $(LDLIBS) -mwindows -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

OGL.o: OGL.rc
	$(WINDRES) $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) OGL.o $(TARGET)

.PHONY: all run clean