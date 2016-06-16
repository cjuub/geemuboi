CXX = g++
CC  = g++

CPPFLAGS =  -std=c++11
CXXFLAGS =  -O2 -Wall -Wextra -pedantic-errors -Wold-style-cast 
CXXFLAGS += -std=c++11 
CXXFLAGS += -g
LDFLAGS =   -g
LDLIBS = -lSDL2

SRCS = src/core/cpu.cpp src/core/mmu.cpp src/core/gpu.cpp src/core/input.cpp src/geemuboi/geemuboi.cpp src/video/sdl_renderer.cpp src/input/sdl_keyboard.cpp
OBJS = $(subst .cpp,.o,$(SRCS))

# Targets
PROGS = geemuboi

all: $(PROGS)

# Targets rely on implicit rules for compiling and linking
geemuboi: $(OBJS)
		$(CXX) $(LDFLAGS) -o geemuboi $(OBJS) $(LDLIBS) 

# Phony targets
.PHONY: all clean

# Standard clean
clean:
	rm -f src/*/*.o $(PROGS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

include .depend
