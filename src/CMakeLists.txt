cmake_minimum_required(VERSION 3.5)


############################################################
# Create a library
############################################################

message("--------Cur Src Dir: " ${CMAKE_CURRENT_LIST_DIR})
file(GLOB_RECURSE XPUB_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/*/*.cpp)
message("--------TEST RESULT: " ${XPUB_SRC_LIST})

#Generate the shared library from the library sources
add_library(XPUB SHARED 
    ${XPUB_SRC_LIST}
)
add_library(X::PUBLIB ALIAS XPUB)

target_link_libraries(XPUB PUBLIC pthread)
target_link_libraries(XPUB
    PUBLIC 
        X::THIRD
)

target_include_directories(XPUB PUBLIC ./XHTTP)
target_include_directories(XPUB PUBLIC ./XLog)
target_include_directories(XPUB PUBLIC ./Xml)
target_include_directories(XPUB PUBLIC ./XNet)
target_include_directories(XPUB PUBLIC ./XPthread)
target_include_directories(XPUB PUBLIC ./XResource)
target_include_directories(XPUB PUBLIC ./XRoot)
target_include_directories(XPUB PUBLIC ./XStruct)
target_include_directories(XPUB PUBLIC ./XUserServlet)
target_include_directories(XPUB PUBLIC ./XUtils)
target_include_directories(XPUB PUBLIC ../third/Xml)
target_include_directories(XPUB PUBLIC .)



############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(test
    XWebServer.cpp
    test.cpp
)

# link the new hello_library target with the hello_binary target
target_link_libraries(test
    PUBLIC 
        X::PUBLIB
)