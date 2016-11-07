
SET(SOURCE_FILES Engine.cpp )             # 设置变量，表示所有的源文件

INCLUDE_DIRECTORIES(/usr/local/include ${PROJET_SOURCE_DIR}/engine)               # 相关头文件的目录

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/resource)

LINK_DIRECTORIES(/usr/local/lib)            # 相关库文件的目录

ADD_LIBRARY(engine STATIC  ${SOURCE_FILES})         # 生成静态链接库libassociation.a

TARGET_LINK_LIBRARIES(engine yaml-cpp dl workManager pthread unit threadpool netUnit testUnit)

SET_TARGET_PROPERTIES(engine PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/lib)  

