
SET(SOURCE_FILES WorkManager.cpp )             # 设置变量，表示所有的源文件

INCLUDE_DIRECTORIES(/usr/local/include ${PROJET_SOURCE_DIR}/workManager)               # 相关头文件的目录

LINK_DIRECTORIES(/usr/local/lib)            # 相关库文件的目录

ADD_LIBRARY(workManager SHARED  ${SOURCE_FILES})         # 生成静态链接库libassociation.a


SET_TARGET_PROPERTIES(workManager PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/lib)  

