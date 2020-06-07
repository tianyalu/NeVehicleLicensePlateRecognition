# CMakeList.txt: svm_train 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
include_directories("D:/opencv411/build/include")
  
link_directories("D:/opencv411/build/x64/vc15/lib")

# 将源代码添加到此项目的可执行文件。
file(GLOB src_files *.cpp)    

add_executable (svm_train ${src_files})

# TODO: 如有需要，请添加测试并安装目标。
target_link_libraries(svm_train opencv_world411d) 