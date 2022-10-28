# Adapted from class-provided Makefile with significant help from https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# NOTES TO SELF
# first target is default
# $@ = left side of ":"
# $< = first item on right of ":"
# $^ = entire right side of ":"
# -O3 = third-level optimization when compiling

LIBDIR=CSCIx229

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
final.o: final.cpp final.hpp $(LIBDIR)/CSCIx229.h
fatal.o: $(LIBDIR)/fatal.c $(LIBDIR)/CSCIx229.h
errcheck.o: $(LIBDIR)/errcheck.c $(LIBDIR)/CSCIx229.h
print.o: $(LIBDIR)/print.c $(LIBDIR)/CSCIx229.h
loadtexbmp.o: $(LIBDIR)/loadtexbmp.c $(LIBDIR)/CSCIx229.h
loadobj.o: $(LIBDIR)/loadobj.c $(LIBDIR)/CSCIx229.h

#----------------
# Create archive
#----------------
CSCIx229.a:fatal.o errcheck.o print.o loadtexbmp.o loadobj.o
	ar -rcs $@ $^

#---------------
# Compile rules
#---------------
%.o: $(LIBDIR)/%.c
	gcc -c $(CFLAGS) $< -I $(LIBDIR)
%.o: $(LIBDIR)/%.cpp
	g++ -c $(CFLAGS) $< -I $(LIBDIR)
%.o: %.c
	gcc -c $(CFLAGS) $< -I $(LIBDIR)
%.o: %.cpp
	g++ -c $(CFLAGS) $< -I $(LIBDIR)

#-----------
#  Targets
#-----------
# Link
final: final.o CSCIx229.a
	g++ -o $@ $^ $(CFLAGS) $(LIBS) -I $(LIBDIR)

# Clean
clean:
	$(CLEAN)