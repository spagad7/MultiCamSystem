################################################################################
# Trigger Makefile
################################################################################

################################################################################
# Key paths and settings
################################################################################
CFLAGS += -std=c++11
CVFLAGS = `pkg-config --cflags opencv`
#SOURCES = main.cpp CameraConfig.cpp TriggerConfig.cpp ImageAcquisition.cpp Miscellaneous.cpp
CC = g++ ${CFLAGS} -ggdb ${CVFLAGS} 
OUTPUTNAME = MultiCamHTBuffer${D}

OUTDIR = bin

################################################################################
# Dependencies
################################################################################
# Spinnaker deps
SPINNAKER_LIB = -L../../lib -lSpinnaker${D}
CV_LIB = `pkg-config --libs opencv`${D}

################################################################################
# Master inc/lib/obj/dep settings
################################################################################
OBJS = main.o CameraConfig.o TriggerConfig.o ImageAcquisition.o Miscellaneous.o
INC = -I../../include
LIB += -Wl,-Bdynamic ${SPINNAKER_LIB}
LIB += ${CV_LIB}
LIB += -Wl,-rpath-link=../../lib 

################################################################################
# Rules/recipes
################################################################################
# Final binary
${OUTPUTNAME}: ${OBJS}
	${CC} -o ${OUTPUTNAME} ${OBJS} ${LIB}
	mv ${OUTPUTNAME} ${OUTDIR} 

# Intermediate objects
# %.o: %.cpp
#	${CC} ${CFLAGS} ${INC} -Wall -c -D LINUX $*.cpp

main.o: main.cpp
	${CC} ${CFLAGS} ${INC} -Wall -c -D LINUX main.cpp

TriggerConfig.o: TriggerConfig.cpp ../headers/TriggerConfig.h
	${CC} ${CFLAGS} ${INC} -Wall -c -D LINUX TriggerConfig.cpp

CameraConfig.o: CameraConfig.cpp ../headers/CameraConfig.h
	${CC} ${CFLAGS} ${INC} -Wall -c -D LINUX CameraConfig.cpp

ImageAcquisition.o: ImageAcquisition.cpp ../headers/ImageAcquisition.h
	${CC} ${CFLAGS} ${INC} -Wall -c -D LINUX ImageAcquisition.cpp

Miscellaneous.o: Miscellaneous.cpp ../headers/Miscellaneous.h
	${CC} ${CFLAGS} ${INC} -Wall -c -D LINUX Miscellaneous.cpp

# Clean up intermediate objects
clean_obj:
	rm -f ${OBJS}	@echo "all cleaned up!"

# Clean up everything.
clean:
	rm -f ${OUTDIR}/${OUTPUTNAME} ${OBJS}	@echo "all cleaned up!"
