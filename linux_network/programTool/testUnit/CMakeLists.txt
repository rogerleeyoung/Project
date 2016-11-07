
SET(SOURCE_FILES TestUnit.cpp )             # 设置变量，表示所有的源文件

INCLUDE_DIRECTORIES(/usr/local/include ${PROJET_SOURCE_DIR}/testUnit)               # 相关头文件的目录

LINK_DIRECTORIES(/usr/local/lib)            # 相关库文件的目录

ADD_LIBRARY(testUnit SHARED  ${SOURCE_FILES})         # 生成静态链接库libassociation.a


SET_TARGET_PROPERTIES(unit PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/lib)  

