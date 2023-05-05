cmake_minimum_required(VERSION 3.0)
project(distributed-chatServer)

# 1.编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 2.设置可执行文件最终的存储路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 3.指定需要编译搜索的子目录
add_subdirectory(server/src)
add_subdirectory(client/src)


