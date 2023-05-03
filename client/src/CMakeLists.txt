# 1.定义了一个SRC_LIST变量 包含了该目录下所有的源文件
aux_source_directory(. SRC_LIST)

# 2.编译后的可执行文件名
add_executable(myClient ${SRC_LIST})

# # 3.需要链接的库
target_link_libraries(myClient muduo_net muduo_base pthread)




