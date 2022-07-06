cmake_minimum_required(VERSION 3.5)

############################################################
# Create a library
############################################################


message("--------Cur Src Dir: " ${CMAKE_CURRENT_LIST_DIR})
file(GLOB_RECURSE XTEST_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/*/*.cpp)
message("--------TEST RESULT: " ${XTEST_SRC_LIST})


add_library(test SHARED 
    ${XTEST_SRC_LIST}
)
add_library(X::TEST ALIAS test)

# link library
target_link_libraries(test
    PUBLIC 
        X::PUBLIB
)

target_include_directories(test
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}/XServer
)

target_include_directories(test
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}/XUserServlet
)