# Adapted from class-provided Makefile with significant help from https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# NOTES TO SELF
# first target is default
# $@ = left side of ":"
# $< = first item on right of ":"
# $^ = entire right side of ":"
# -O3 = third-level optimization when compiling

CLIBDIR=CSCIx229
MLIBDIR=MainLib
STCRDIR=Structures
NOISDIR=Noise
MENUDIR=Menus
SHDEDIR=Shaders
INCLUDE=-I $(CLIBDIR) -I $(MLIBDIR) -I $(STCRDIR) -I $(NOISDIR) -I $(MENUDIR) -I $(SHDEDIR)

# Default target
all: final

BASE_CLEAN=rm -f final *.o *.a

#---------------
#  Portability
#---------------
#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLAGS=-O3 -Wall -DUSEGLEW -Wextra -Wno-unused-parameter -Wno-unused-variable
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
CLEAN=$(BASE_CLEAN) *.exe
else
#  OSX
ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r|sed -E 's/(.).*/\1/'|tr 12 21)
CFLAGS=-O3 -Wall -Wno-deprecated-declarations -Wextra -DRES=$(RES) -Wno-unused-parameter -Wno-unused-variable
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLAGS=-O3 -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-narrowing
# FIXME turned off narrowing for the manual way of setting tunnel connections. Should be turned back on once auto-gen is implemented
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=$(BASE_CLEAN)
endif

#--------------
# Dependencies
#--------------

#--- CSCIx229 ---
fatal.o: $(CLIBDIR)/fatal.c $(CLIBDIR)/CSCIx229.h
errcheck.o: $(CLIBDIR)/errcheck.c $(CLIBDIR)/CSCIx229.h
print.o: $(CLIBDIR)/print.c $(CLIBDIR)/CSCIx229.h
loadtexbmp.o: $(CLIBDIR)/loadtexbmp.c $(CLIBDIR)/CSCIx229.h
loadobj.o: $(CLIBDIR)/loadobj.c $(CLIBDIR)/CSCIx229.h

#--- MainLib ---
globals.o: $(MLIBDIR)/globals.cpp $(MLIBDIR)/globals.hpp
Vector.o: $(MLIBDIR)/Vector.cpp $(MLIBDIR)/Vector.hpp
Vertex.o: $(MLIBDIR)/Vertex.cpp $(MLIBDIR)/Vertex.hpp
# Everything else depends on some combination of the above
display.o: $(MLIBDIR)/display.cpp $(MLIBDIR)/display.hpp $(MLIBDIR)/globals.hpp $(MLIBDIR)/util.hpp
input.o: $(MLIBDIR)/input.cpp $(MLIBDIR)/input.hpp $(MLIBDIR)/globals.hpp
lighting.o: $(MLIBDIR)/lighting.cpp $(MLIBDIR)/lighting.hpp $(MLIBDIR)/globals.hpp
textures.o: $(MLIBDIR)/textures.cpp $(MLIBDIR)/textures.hpp $(MLIBDIR)/globals.hpp
util.o: $(MLIBDIR)/util.cpp $(MLIBDIR)/util.hpp $(MLIBDIR)/globals.hpp
window.o: $(MLIBDIR)/window.cpp $(MLIBDIR)/window.hpp $(MLIBDIR)/globals.hpp

#--- Structures ---
# Base structures
Structure.o: $(STCRDIR)/Structure.cpp $(STCRDIR)/Structure.hpp util.o Vertex.o
Corner.o: $(STCRDIR)/Corner.cpp $(STCRDIR)/Corner.hpp util.o Structure.o
# Composite structures
Chamber.o: $(STCRDIR)/Chamber.cpp $(STCRDIR)/Chamber.hpp util.o Structure.o
Hill.o: $(STCRDIR)/Hill.cpp $(STCRDIR)/Hill.hpp Structure.o
Tunnel.o: $(STCRDIR)/Tunnel.cpp $(STCRDIR)/Tunnel.hpp util.o Corner.o
# Buildings
Mine.o: $(STCRDIR)/Mine.cpp $(STCRDIR)/Mine.hpp Chamber.o
# Colony needs to depend on all the other Structures
Colony.o: $(STCRDIR)/Colony.cpp $(STCRDIR)/Colony.hpp Chamber.o Hill.o Tunnel.o Mine.o MainLib.a

#--- Noise ---
Noise.o: $(NOISDIR)/Noise.cpp $(NOISDIR)/Noise.hpp
Perlin.o: $(NOISDIR)/Perlin.cpp $(NOISDIR)/Perlin.hpp

#--- Menus ---
Menus.o: $(MENUDIR)/Menus.cpp $(MENUDIR)/Menus.hpp

#--- Shaders ---
Shaders.o: $(SHDEDIR)/Shaders.cpp $(SHDEDIR)/Shaders.hpp

#----------------
# Create archive
#----------------
CSCIx229.a: fatal.o errcheck.o print.o loadtexbmp.o loadobj.o
	ar -rcs $@ $^
MainLib.a: globals.o display.o lighting.o input.o textures.o util.o Vector.o Vertex.o window.o
	ar -rcs $@ $^
Structures.a: Structure.o Corner.o Chamber.o Hill.o Tunnel.o Colony.o Mine.o
	ar -rcs $@ $^
Noise.a: Noise.o Perlin.o
	ar -rcs $@ $^
Menus.a: Menus.o
	ar -rcs $@ $^
Shaders.a: Shaders.o
	ar -rcs $@ $^

#---------------
# Compile rules
#---------------
%.o: $(CLIBDIR)/%.c
	gcc -c $(CFLAGS) $< $(INCLUDE)
%.o: $(CLIBDIR)/%.cpp
	g++ -c $(CFLAGS) $< $(INCLUDE)

%.o: $(MLIBDIR)/%.c
	gcc -c $(CFLAGS) $< $(INCLUDE)
%.o: $(MLIBDIR)/%.cpp
	g++ -c $(CFLAGS) $< $(INCLUDE)

%.o: $(STCRDIR)/%.c
	gcc -c $(CFLAGS) $< $(INCLUDE)
%.o: $(STCRDIR)/%.cpp
	g++ -c $(CFLAGS) $< $(INCLUDE)

%.o: $(NOISDIR)/%.c
	gcc -c $(CFLAGS) $< $(INCLUDE)
%.o: $(NOISDIR)/%.cpp
	g++ -c $(CFLAGS) $< $(INCLUDE)

%.o: $(MENUDIR)/%.c
	gcc -c $(CFLAGS) $< $(INCLUDE)
%.o: $(MENUDIR)/%.cpp
	g++ -c $(CFLAGS) $< $(INCLUDE)

%.o: $(SHDEDIR)/%.c
	gcc -c $(CFLAGS) $< $(INCLUDE)
%.o: $(SHDEDIR)/%.cpp
	g++ -c $(CFLAGS) $< $(INCLUDE)

%.o: %.c
	gcc -c $(CFLAGS) $< $(INCLUDE)
%.o: %.cpp
	g++ -c $(CFLAGS) $< $(INCLUDE)

#-----------
#  Targets
#-----------
# Link (and final.o dependency to keep them together)
final.o: final.cpp final.hpp $(CLIBDIR)/CSCIx229.h MainLib.a Structures.a Noise.a Menus.a Shaders.a
# FIXME I have no clue why Vertex.o needs to be here, but apparently it does
final:               final.o            CSCIx229.a MainLib.a Structures.a Noise.a Menus.a Shaders.a Vertex.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS) $(INCLUDE)

noise: $(NOISDIR)/main.cpp Noise.a CSCIx229.a MainLib.a
	g++ -o final $^ $(CFLAGS) $(LIBS) $(INCLUDE)
.SILENT: noiseRun
noiseRun: noise
	./final

menus: $(MENUDIR)/main.cpp Menus.a CSCIx229.a MainLib.a
	g++ -o final $^ $(CFLAGS) $(LIBS) $(INCLUDE)
.SILENT: menusRun
menusRun: menus
	./final

# Clean
clean:
	$(CLEAN)

# Make and run
.SILENT: run
run: final
	./final || (echo "Exit code: $$?")