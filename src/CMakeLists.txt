cmake_minimum_required(VERSION 3.5)

############################################################
# Create an executable
############################################################

ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(XPublic)

message("--------Cur Src Dir: " ${CMAKE_CURRENT_LIST_DIR})
file(GLOB_RECURSE XBASE_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
message("--------TEST RESULT: " ${XBASE_SRC_LIST})


# Add an executable with the above sources
add_executable(XWeb
    ${XBASE_SRC_LIST}   
)

# link library
target_link_libraries(XWeb
    PUBLIC 
        X::TEST
)



