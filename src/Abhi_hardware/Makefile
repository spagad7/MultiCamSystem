################################################################################
# Trigger Makefile
################################################################################

################################################################################
# Key paths and settings
################################################################################
CFLAGS += -std=c++11
CVFLAGS = `pkg-config --cflags opencv`
CC = g++ ${CFLAGS} -ggdb ${CVFLAGS}
OUTPUTNAME = Abhi_hardware${D}

OUTDIR = ../../bin

################################################################################
# Dependencies
################################################################################
# Spinnaker deps
SPINNAKER_LIB = -L../../lib -lSpinnaker${D}
CV_LIB = `pkg-config --libs opencv`${D}

################################################################################
# Master inc/lib/obj/dep settings
################################################################################
OBJ = Abhi_hardware.o
INC = -I../../include
LIB += -Wl,-Bdynamic ${SPINNAKER_LIB}
LIB += ${CV_LIB}
LIB += -Wl,-rpath-link=../../lib 

################################################################################
# Rules/recipes
################################################################################
# Final binary
${OUTPUTNAME}: ${OBJ}
	${CC} -o ${OUTPUTNAME} ${OBJ} ${LIB}
	mv ${OUTPUTNAME} ${OUTDIR}

# Intermediate objects
%.o: %.cpp
	${CC} ${CFLAGS} ${INC} -Wall -c -D LINUX $*.cpp

# Clean up intermediate objects
clean_obj:
	rm -f ${OBJ}	@echo "all cleaned up!"

# Clean up everything.
clean:
	rm -f ${OUTDIR}/${OUTPUTNAME} ${OBJ}	@echo "all cleaned up!"
