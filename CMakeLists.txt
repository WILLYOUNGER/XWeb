cmake_minimum_required(VERSION 3.5)

PROJECT (XWeb VERSION 1.0.0)

configure_file(Config.h.in Config.h)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

ADD_SUBDIRECTORY(src ../bin)
ADD_SUBDIRECTORY(third)
