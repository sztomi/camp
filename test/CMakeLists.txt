###############################################################################
##
## Copyright (C) 2009-2014 TEGESO/TEGESOFT and/or its subsidiary(-ies) and mother company.
## Contact: Tegesoft Information (contact@tegesoft.com)
##
## This file is part of the CAMP library.
##
## The MIT License (MIT)
##
## Copyright (c) 2009-2014 TEGESO/TEGESOFT
##
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
##
## The above copyright notice and this permission notice shall be included in
## all copies or substantial portions of the Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
## THE SOFTWARE.
##
###############################################################################


cmake_minimum_required(VERSION 2.6)

# set project's name
project(CAMP_TEST)

# all source files
set(CAMP_TEST_SRCS
    main.cpp
    arrayproperty.cpp
    arrayproperty.hpp
    class.cpp
    class.hpp
    enum.cpp
    enum.hpp
    enumobject.cpp
    enumobject.hpp
    enumproperty.cpp
    enumproperty.hpp
    tagholder.cpp
    tagholder.hpp
    value.cpp
    value.hpp
    property.cpp
    property.hpp
    classvisitor.cpp
    classvisitor.hpp
    constructor.cpp
    constructor.hpp
    mapper.cpp
    mapper.hpp
    inheritance.cpp
    inheritance.hpp
    userobject.cpp
    userobject.hpp
    propertyaccess.cpp
    propertyaccess.hpp
    functionaccess.cpp
    functionaccess.hpp
    serialization.hpp
    serialization.cpp
)

# find Boost
find_package(Boost 1.38.0)

# include files search paths
include_directories(
    ${CAMP_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
)

# linker search paths
link_directories(
    ${CAMP_BINARY_DIR}
)

# no implicit link to boost.test library
add_definitions(-DBOOST_TEST_NO_LIB)

# instruct CMake to build an executable from all of the source files
add_executable(camptest ${CAMP_TEST_SRCS})

# required standard level (needed to pass -std=c++11 to gcc and clang)
set_property(TARGET camptest PROPERTY CXX_STANDARD 11)

# define d suffix on windows
if(WIN32)
    set_target_properties(camptest PROPERTIES DEBUG_POSTFIX d)
endif()

# last thing we have to do is to tell CMake what libraries our executable needs,
target_link_libraries(camptest camp)

# - Add the executable as a CTest
add_test(camptest camptest)

# add the qt subdirectory, but do not build it by default
if(BUILD_TEST_QT)
    add_subdirectory(qt)
endif()

