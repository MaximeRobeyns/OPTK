##
# Copyright (C) 2020 Maxime Robeyns <maximerobeyns@gmail.com>
#
# Written for the ACRC, University of Bristol
#
# Licensed under the Educational Community License, Version 2.0
# (the "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
# http://www.osedu.org/licenses/ECL-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# @file
# @version 0.1

# GNU Make Configuration -------------------------------------------------------

SHELL := bash
.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += -j8

# Constants --------------------------------------------------------------------

PROG			= optk
TESTPROG		= optk_tests

SOURCES			= src
TESTSRC			= test

BUILDDIR		= build
TESTBUILDDIR	= testbuild

INCDIR			= includes
TARGETDIR		= bin

PROJECT_SOURCES	= $(shell find ${SOURCES} -type f -name *.cpp)
PROJECT_HEADERS	= $(shell find ${INCDIR} -type f -name *.hpp)
PROJECT_OBJECTS	= $(patsubst ${SOURCES}/%,${BUILDDIR}/%,${PROJECT_SOURCES:.cpp=.o})
PROJECT_SUBDIRS = $(shell find ${SOURCES} -type d | cut -d'/' -f2-)

TEST_SOURCES	= $(shell find ${TESTSRC} -type f -name *.cpp)
TEST_OBJECTS_i	= $(patsubst ${TESTSRC}/%,${TESTBUILDDIR}/test/%,${TEST_SOURCES:.cpp=.o})
TEST_OBJECTS_ii	= $(patsubst ${SOURCES}/%,${TESTBUILDDIR}/src/%,${PROJECT_SOURCES:.cpp=.o})
TEST_OBJECTS	= ${TEST_OBJECTS_i} ${TEST_OBJECTS_ii}
TEST_SUBDIRS    = $(shell find ${TESTSRC} -type d | cut -d'/' -f2-)

INCLUDES		= -I${INCDIR} -I/usr/local/include #$(shell python3-config --embed --includes)
LIBS			= #$(shell python3-config --embed --ldflags)
INCDEP			= -I${INCDIR}
CFLAGS			= -O2 -Wall -std=c++17 -pthread -fPIC
TESTFLAGS		= -D__OPTK_TESTING -g -Wall -std=c++17 -fsanitize=address -pthread
TESTLDFLAGS		= -fsanitize=address
CC				= g++

# Targets ----------------------------------------------------------------------

all: directories ${PROG} lib${PROG}.so

test: testdirs ${TESTPROG}

remake: cleaner all

directories:
	@mkdir -p ${TARGETDIR}
	@mkdir -p ${BUILDDIR}

testdirs:
	@mkdir -p ${TARGETDIR}
	@mkdir -p $(addprefix ${TESTBUILDDIR}/, ${PROJECT_SUBDIRS})
	@mkdir -p $(addprefix ${TESTBUILDDIR}/, ${TEST_SUBDIRS})

clean:
	@${RM} -rf ${BUILDDIR} ${TESTBUILDDIR}

cleaner: clean
	@${RM} -rf ${TARGETDIR}/*
	@${RM} -rf docs/html
	@${RM} -rf docs/latex

# gets dependencies for existing object files
-include ${TEST_OBJECTS:.o=.d}

lib${PROG}.so: ${PROJECT_OBJECTS}
	@${CC} -shared -o ${TARGETDIR}/$@ $^ ${LIBS}

# linking
${PROG}: ${PROJECT_OBJECTS}
	@${CC} -o ${TARGETDIR}/${PROG} $^ ${LIBS}

${TESTPROG}: ${TEST_OBJECTS}
	@${CC} -o ${TARGETDIR}/${TESTPROG} ${TESTLDFLAGS} $^ ${LIBS}

# The following pattern is due in large part to the work of Scott McPeak:
# http://scottmcpeak.com/autodepend/autodepend.html

# compilation
${BUILDDIR}/%.o: ${SOURCES}/%.cpp
	@mkdir -p $(dir $@)
	${CC} ${CFLAGS} ${INCLUDES} -c -o $@ $<
	@${CC} ${CFLAGS} ${INCDEP} -MM ${SOURCES}/$*.cpp > ${BUILDDIR}/$*.d
	@cp -f ${BUILDDIR}/$*.d ${BUILDDIR}/$*.d.tmp
	@sed -e 's|.*:|${BUILDDIR}/$*.o:|' < ${BUILDDIR}/$*.d.tmp > ${BUILDDIR}/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < ${BUILDDIR}/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> ${BUILDDIR}/$*.d
	@rm -f ${BUILDDIR}/$*.d.tmp

# test compilation
${TESTBUILDDIR}/src/%.o: ${SOURCES}/%.cpp
	@mkdir -p $(dir $@)
	${CC} ${TESTFLAGS} ${INCLUDES} -c -o $@ $<
	@${CC} ${TESTFLAGS} ${INCDEP} -MM ${SOURCES}/$*.cpp > ${TESTBUILDDIR}/$*.d
	@cp -f ${TESTBUILDDIR}/$*.d ${TESTBUILDDIR}/$*.d.tmp
	@sed -e 's|.*:|${TESTBUILDDIR}/$*.o:|' < ${TESTBUILDDIR}/$*.d.tmp > ${TESTBUILDDIR}/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < ${TESTBUILDDIR}/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> ${TESTBUILDDIR}/$*.d
	@rm -f ${TESTBUILDDIR}/$*.d.tmp

${TESTBUILDDIR}/test/%.o: ${TESTSRC}/%.cpp
	@mkdir -p $(dir $@)
	${CC} ${TESTFLAGS} ${INCLUDES} -c -o $@ $<
	@${CC} ${TESTFLAGS} ${INCDEP} -MM ${TESTSRC}/$*.cpp > ${TESTBUILDDIR}/$*.d
	@cp -f ${TESTBUILDDIR}/$*.d ${TESTBUILDDIR}/$*.d.tmp
	@sed -e 's|.*:|${TESTBUILDDIR}/$*.o:|' < ${TESTBUILDDIR}/$*.d.tmp > ${TESTBUILDDIR}/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < ${TESTBUILDDIR}/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> ${TESTBUILDDIR}/$*.d
	@rm -f ${TESTBUILDDIR}/$*.d.tmp

cmds: ${PROJECT_SOURCES} ${PROJECT_HEADERS}
	@compiledb make

docs: docs/html

docs/html: ${PROJECT_SOURCES} ${PROJECT_HEADERS}
	@doxygen docs/Doxyfile

.PHONY: all remake clean cleaner docs
# end
