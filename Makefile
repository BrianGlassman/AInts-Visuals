# Adapted from class-provided Makefile with significant help from https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# NOTES TO SELF
# first target is default
# $@ = left side of ":"
# $< = first item on right of ":"
# $^ = entire right side of ":"
# -O3 = third-level optimization when compiling

CLIBDIR=CSCIx229
MLIBDIR=MainLib
INCLUDE=-I $(CLIBDIR) -I $(MLIBDIR)

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
CFLAGS=-O3 -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=$(BASE_CLEAN)
endif

#--------------
# Dependencies
#--------------
# CSCIx229
final.o: final.cpp final.hpp $(CLIBDIR)/CSCIx229.h
fatal.o: $(CLIBDIR)/fatal.c $(CLIBDIR)/CSCIx229.h
errcheck.o: $(CLIBDIR)/errcheck.c $(CLIBDIR)/CSCIx229.h
print.o: $(CLIBDIR)/print.c $(CLIBDIR)/CSCIx229.h
loadtexbmp.o: $(CLIBDIR)/loadtexbmp.c $(CLIBDIR)/CSCIx229.h
loadobj.o: $(CLIBDIR)/loadobj.c $(CLIBDIR)/CSCIx229.h
# MainLib
display.o: $(MLIBDIR)/display.cpp $(MLIBDIR)/display.hpp
input.o: $(MLIBDIR)/input.cpp $(MLIBDIR)/input.hpp
window.o: $(MLIBDIR)/window.cpp $(MLIBDIR)/window.hpp

#----------------
# Create archive
#----------------
CSCIx229.a:fatal.o errcheck.o print.o loadtexbmp.o loadobj.o
	ar -rcs $@ $^
MainLib.a:window.o display.o input.o
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
%.o: %.c
	gcc -c $(CFLAGS) $< $(INCLUDE)
%.o: %.cpp
	g++ -c $(CFLAGS) $< $(INCLUDE)

#-----------
#  Targets
#-----------
# Link
final: final.o CSCIx229.a MainLib.a
	g++ -o $@ $^ $(CFLAGS) $(LIBS) $(INCLUDE)

# Clean
clean:
	$(CLEAN)