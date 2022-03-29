cmake_minimum_required(VERSION 3.16)
SET (SRC_LIST test.cpp ./XNet/XBaseNet.h ./XNet/XBaseNet.cpp ./XStruct/XStruct.h ./XStruct/XStruct.cpp ./XUtils/XUtils.h ./XUtils/XUtils.cpp ./XHTTP/XHttp.h ./XHTTP/XHttp.cpp ./XLog/XLog.h ./XLog/XLog.cpp ./XPthread/XPthreadPool.h ./XPthread/XPthreadPool.cpp ./XUtils/XBlockQueue.h ./XUtils/XLock.h XWebServer.cpp XWebServer.h ./XHTTP/XServlet.h ./XHTTP/XServlet.cpp ./XStruct/XHttpStruct.h ./XUtils/XLock.h ./Xml/tinystr.h ./Xml/tinystr.cpp ./Xml/tinyxml.h ./Xml/tinyxml.cpp ./Xml/tinyxmlerror.cpp ./Xml/tinyxmlparser.cpp ./XUserServlet/XServletDefine.h ./XUserServlet/XTestServlet.h ./XUserServlet/XTestServlet.cpp)
ADD_EXECUTABLE(test ${SRC_LIST})
target_link_libraries(test pthread)

