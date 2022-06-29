cmake_minimum_required(VERSION 3.5)

############################################################
# Create a library
############################################################


message("--------Cur Src Dir: " ${CMAKE_CURRENT_LIST_DIR})
file(GLOB_RECURSE THIRD_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/Xml/*.cpp)
message("--------TEST RESULT: " ${THIRD_SRC_LIST})


add_library(third SHARED 
    ${THIRD_SRC_LIST}
)
add_library(X::THIRD ALIAS third)

target_include_directories(third
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}/third/Xml
)