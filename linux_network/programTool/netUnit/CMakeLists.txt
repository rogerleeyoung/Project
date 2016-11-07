
SET(SOURCE_FILES NetUnit.cpp Address.cpp NetManager.cpp NetServer.cpp)              # 设置变量，表示所有的源文件

INCLUDE_DIRECTORIES(/usr/local/include ${PROJET_SOURCE_DIR}/netUnit)               # 相关头文件的目录

LINK_DIRECTORIES(/usr/local/lib)            # 相关库文件的目录

ADD_LIBRARY(netUnit SHARED  ${SOURCE_FILES})         # 生成静态链接库libassociation.a

TARGET_LINK_LIBRARIES(netUnit util unit)            # 依赖的库文件

SET_TARGET_PROPERTIES(netUnit PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/lib)  

