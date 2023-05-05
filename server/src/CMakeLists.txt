# 1.定义了一个SRC_LIST变量 包含了该目录下所有的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./dao DAO_LIST)
aux_source_directory(./service SERVICE_LIST)

# 2.编译后的可执行文件名
add_executable(myServer ${SRC_LIST} ${DB_LIST} ${DAO_LIST} ${SERVICE_LIST})

# 3.需要包含的头文件
include_directories(${PROJECT_SOURCE_DIR}/server/include)
include_directories(${PROJECT_SOURCE_DIR}/server/include/db)
include_directories(${PROJECT_SOURCE_DIR}/server/include/dao)
include_directories(${PROJECT_SOURCE_DIR}/server/include/projo)
include_directories(${PROJECT_SOURCE_DIR}/server/include/service)
include_directories(${PROJECT_SOURCE_DIR}/server/include/utils)

# 4.需要链接的库
target_link_libraries(myServer muduo_net muduo_base mysqlclient pthread)



