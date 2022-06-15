cmake_minimum_required(VERSION 3.16)
SET (SRC_LIST test.cpp ./XNet/XBaseNet.h ./XNet/XBaseNet.cpp ./XStruct/XStruct.h ./XStruct/XStruct.cpp ./XUtils/XUtils.h ./XUtils/XUtils.cpp ./XHTTP/XHttp.h ./XHTTP/XHttp.cpp ./XLog/XLog.h ./XLog/XLog.cpp ./XPthread/XPthreadPool.h ./XPthread/XPthreadPool.cpp ./XUtils/XBlockQueue.h ./XUtils/XLock.h XWebServer.cpp XWebServer.h ./XHTTP/XServlet.h ./XHTTP/XServlet.cpp ./XStruct/XHttpStruct.h ./XUtils/XLock.h ./Xml/tinystr.h ./Xml/tinystr.cpp ./Xml/tinyxml.h ./Xml/tinyxml.cpp ./Xml/tinyxmlerror.cpp ./Xml/tinyxmlparser.cpp ./XUserServlet/XServletDefine.h ./XUserServlet/XTestServlet.h ./XUserServlet/XTestServlet.cpp)

MESSAGE(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})

ADD_EXECUTABLE(test ${SRC_LIST})
target_link_libraries(test pthread)
target_include_directories(test PUBLIC "${PROJECT_BINARY_DIR}")

target_include_directories(test PUBLIC ./XHTTP)
target_include_directories(test PUBLIC ./XLOG)
target_include_directories(test PUBLIC ./Xml)
target_include_directories(test PUBLIC ./XNet)
target_include_directories(test PUBLIC ./XPthread)
target_include_directories(test PUBLIC ./XResource)
target_include_directories(test PUBLIC ./XRoot)
target_include_directories(test PUBLIC ./XStruct)
target_include_directories(test PUBLIC ./XUserServlet)
target_include_directories(test PUBLIC ./XUtils)
target_include_directories(test PUBLIC .)



