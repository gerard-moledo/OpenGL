INCLUDES = -I./external/glad/include -I./external/glfw-3.4-win64/include
LIBS = -L./external/glfw-3.4-win64/lib-mingw-w64

LINKER_FLAGS = -lglfw3 -lgdi32 -luser32 -lkernel32

./builds/mingw/program.exe : ./builds/mingw/main.o ./builds/mingw/glad.o
	g++ $^ $(LIBS) $(LINKER_FLAGS) -o $@

./builds/mingw/main.o : ./src/main.cpp
	g++ $(INCLUDES) -c $< -o $@

./builds/mingw/glad.o : ./external/glad/src/glad.c
	g++ $(INCLUDES) -c $< -o $@

clean :
	rm ./builds/mingw/*.o 
	
all : clean ./builds/mingw/program.exe