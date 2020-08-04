##
# Copyright (C) 2020 Maxime Robeyns <maximerobeyns@gmail.com>
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

# Constants --------------------------------------------------------------------

PROG			= optk

SOURCES			= src
INCDIR			= includes
BUILDDIR		= build
TARGETDIR		= bin
#RESDIR			= resources

PROJECT_SOURCES = $(shell find ${SOURCES} -type f -name *.cpp)
PROJECT_OBJECTS = $(patsubst ${SOURCES}/%,${BUILDDIR}/%,${PROJECT_SOURCES:.cpp=.o})

INCLUDES		= -I${INCDIR} -I/usr/local/include
LIBS            =
INCDEP			= -I${INCDIR}
CFLAGS			= -O2 -Wall -std=c++17
CC				= g++

# Targets ----------------------------------------------------------------------

all: directories ${PROG}

remake: cleaner all

# resources: directories
#	 @cp ${RESDIR}/* ${TARGETDIR}/

directories:
	@mkdir -p ${TARGETDIR}
	@mkdir -p ${BUILDDIR}

clean:
	${RM} -rf ${BUILDDIR}

cleaner: clean
	${RM} -rf ${TARGETDIR}

# gets dependencies for existing object files
-include ${PROJECT_OBJECTS:.o=.d}

# linking
${PROG}: ${PROJECT_OBJECTS}
	${CC} -o ${TARGETDIR}/${PROG} $^ ${LIBS}

# compilation
${BUILDDIR}/%.o: ${SOURCES}/%.cpp
	mkdir -p $(dir $@)
	${CC} ${CFLAGS} ${INCLUDES} -c -o $@ $<
	${CC} ${CFLAGS} ${INCDEP} -MM ${SOURCES}/$*.cpp > ${BUILDDIR}/$*.d
	@cp -f ${BUILDDIR}/$*.d ${BUILDDIR}/$*.d.tmp
	sed -e 's|.*:|${BUILDDIR}/$*.o:|' < ${BUILDDIR}/$*.d.tmp > ${BUILDDIR}/$*.d
	sed -e 's/.*://' -e 's/\\$$//' < ${BUILDDIR}/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> ${BUILDDIR}/$*.d
	rm -f ${BUILDDIR}/$*.d.tmp

cmds:
	compiledb make

.PHONY: all remake clean cleaner resources cmds

# end
