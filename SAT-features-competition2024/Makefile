###################################################################
#
# You should be able to just fill in the vars here for any project.
# You will need dirs "obj" and "debugobj" each containing
# Makefiles containing only the following:
#
# VPATH = ..
#
# all : ${OBJ}
#
###################################################################

COMMONOBJ = SATinstance.o  stopwatch.o  BinSolver.o BuildSolvers.o

FEATOBJ = features.o $(COMMONOBJ)

EXECNAME = features

INCLUDE =

LIB = -l:liblpk5.a -lvarsat -ldl  -lz

LIBDIRS = -Llp_solve_5.0 -LVARSAT 

RELEASEFLAGS = -O5 -DDEBUG -g -lz 

DEBUGFLAGS = -static -Wall -DDEBUG -g

SOLVERLIB =  

release:
	cd obj; make all "OBJ = ${FEATOBJ}" "CPPFLAGS = -DFEATURES ${RELEASEFLAGS} ${INCLUDE}";
	make allfeat "VPATH = obj" "OBJ = $(FEATOBJ)" "FLAGS = -o features ${RELEASEFLAGS}"

libubcsat.a:
	cd UBCSAT; make;
	
libvarsat.a:
	cd VARSAT; make;

liblpk.a:
	cd lp_solve_5.0/lpsolve5; ./ccc; cp liblpk5.a ../liblpk5.a


allfeat : $(OBJ)
	make libvarsat.a; make liblpk.a; g++  ${LIBDIRS} $^ ${FLAGS} $(LIB)

feature:
	cd obj; make all "OBJ = ${FEATOBJ}" "CPPFLAGS = -DFEATURES ${RELEASEFLAGS} ${INCLUDE}";
	make allfeat "VPATH = obj" "OBJ = $(FEATOBJ)" "FLAGS = -o features ${RELEASEFLAGS}"


.PHONY: clean depend again

clean-all:
	rm -f obj/*.o features;  cd VARSAT; make clean ; cd ../lp_solve_5.0;  make clean

clean:
	rm -f obj/*.o features

depend:
	makedepend -Y -fobj/Makefile *.cc *.cpp >& /dev/null
	makedepend -Y -fdebugobj/Makefile *.cc *.cpp >& /dev/null

again:
	make clean; make depend; make
