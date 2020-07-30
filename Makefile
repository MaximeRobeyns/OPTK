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

# Targets ----------------------------------------------------------------------

PROJECT_PATHS   = ./src/optk ./src/benchmarks
PROJECT_SOURCES = $(shell find ${PROJECT_PATHS} -name *.cpp)

INCLUDES = -I ./src/includes -L ./src/includes

build:
	g++ -o optk ${INCLUDES} ${PROJECT_SOURCES}

cmds:
	compiledb make

# end
