cmake_minimum_required(VERSION 3.0)
project(main)

# 1.编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 2.头文件搜索路径
# include_directories()

# 3.库文件搜索路径
# link_directories()

# 4.需要编译的源文件列表
set(SRC_LIST muduo_server.cpp)

# 5.把指定路径下的所有源文件名字放入变量名SRC_LIST中
# aux_source_directory(. SRC_LIST)

# 6.编译后的可执行文件名
add_executable(server ${SRC_LIST})

# 7.需要链接的库
target_link_libraries(server muduo_net muduo_base pthread)

# 8.设置可执行文件最终的存储路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


