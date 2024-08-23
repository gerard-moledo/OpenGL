# Compiler flags
C = g++

# Project files
SRCS = ./src/main.cpp ./src/Renderer.cpp ./include/Renderer.hpp ./external/glad/src/glad.c
OBJS = $(patsubst ./src/%.cpp, %.o, $(filter %.cpp, $(SRCS))) glad.o
EXE = program.exe

# Debug settings
DBGDIR = ./builds/mingw/debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/objs/, $(OBJS))
DBGFLAGS = -g -DDEBUG

# Release settings
RELDIR = ./builds/mingw/release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/objs/, $(OBJS))
RELFLAGS = -O3 -DNDEBUG

# Search Directories
INCDIRS = -I./include -I./external/glad/include -I./external/glfw-3.4-win64/include -I./external/glm-1.0.1
LIBDIRS = -L./external/glfw-3.4-win64/lib-mingw-w64

# Compiler Flags
COMPFLAGS = -c $(INCDIRS)

# Linker flags
LINKFLAGS = $(LIBDIRS) -lglfw3 -lgdi32 -luser32 -lkernel32

.PHONY: all prep clean debug release assets

default: debug release

# Debug Rules
# ==========================================
debug: $(DBGEXE) assets

$(DBGEXE): $(DBGOBJS)
	$(C) $^ $(DBGFLAGS) $(LINKFLAGS) -o $@

$(DBGDIR)/objs/glad.o: ./external/glad/src/glad.c
	$(C) $^ $(DBGFLAGS) $(COMPFLAGS) -o $@
$(DBGDIR)/objs/%.o: ./src/%.cpp
	$(C) $^ $(DBGFLAGS) $(COMPFLAGS) -o $@
# ==========================================

# Release Rules
# ==========================================
release: $(RELEXE) assets

$(RELEXE): $(RELOBJS)
	$(C) $^ $(RELFLAGS) $(LINKFLAGS) -o $@

$(RELDIR)/objs/glad.o: ./external/glad/src/glad.c
	$(C) $^ $(RELFLAGS) $(COMPFLAGS) -o $@
$(RELDIR)/objs/%.o: ./src/%.cpp
	$(C) $^ $(RELFLAGS) $(COMPFLAGS) -o $@
# ==========================================

clean :
	rm -f $(DBGDIR)/objs/*
	rm -f $(DBGDIR)/program.exe
	rm -f $(RELDIR)/objs/*
	rm -f $(RELDIR)/program.exe

prep :
	mkdir -p $(DBGDIR)/objs
	mkdir -p $(RELDIR)/objs

assets :
	cp -r ./assets $(DBGDIR)
	cp -r ./assets $(RELDIR)

all : clean prep release debug assets