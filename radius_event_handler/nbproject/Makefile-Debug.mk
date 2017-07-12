#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=g++
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Configuration.o \
	${OBJECTDIR}/PacketReceiver.o \
	${OBJECTDIR}/RadiusEventHandler.o \
	${OBJECTDIR}/ThriftSender.o \
	${OBJECTDIR}/Utils.o \
	${OBJECTDIR}/Worker.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/lib/x86_64-linux-gnu -L../tbb-2017_U7/build/linux_intel64_gcc_cc4.9.2_libc2.19_kernel3.16.0_release

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/radius_event_handler

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/radius_event_handler: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/radius_event_handler ${OBJECTFILES} ${LDLIBSOPTIONS} -lpthread -lboost_system -ltbb -lboost_serialization

${OBJECTDIR}/Configuration.o: Configuration.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/include -I/usr/include/c++/4.9.2 -I../tbb-2017_U7/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Configuration.o Configuration.cpp

${OBJECTDIR}/PacketReceiver.o: PacketReceiver.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/include -I/usr/include/c++/4.9.2 -I../tbb-2017_U7/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PacketReceiver.o PacketReceiver.cpp

${OBJECTDIR}/RadiusEventHandler.o: RadiusEventHandler.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/include -I/usr/include/c++/4.9.2 -I../tbb-2017_U7/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadiusEventHandler.o RadiusEventHandler.cpp

${OBJECTDIR}/ThriftSender.o: ThriftSender.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/include -I/usr/include/c++/4.9.2 -I../tbb-2017_U7/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ThriftSender.o ThriftSender.cpp

${OBJECTDIR}/Utils.o: Utils.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/include -I/usr/include/c++/4.9.2 -I../tbb-2017_U7/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Utils.o Utils.cpp

${OBJECTDIR}/Worker.o: Worker.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/include -I/usr/include/c++/4.9.2 -I../tbb-2017_U7/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Worker.o Worker.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
