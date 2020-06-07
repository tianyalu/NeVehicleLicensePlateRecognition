# CMakeList.txt: NeVehicleLicensePlateRecognition 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

include_directories("D:/CommonDev/opencv/opencv4.1.2/opencv/build/include")

link_directories("D:/CommonDev/opencv/opencv4.1.2/opencv/build/x64/vc15/lib")

file(GLOB srcs *.cpp) 

# 将源代码添加到此项目的可执行文件。
# add_executable (NeVehicleLicensePlateRecognition "NeVehicleLicensePlateRecognition.cpp" "NeVehicleLicensePlateRecognition.h" "PlateRecognize.cpp" "PlateLocate.cpp" "SobelLocate.cpp")
add_executable (NeVehicleLicensePlateRecognition ${srcs})

# TODO: 如有需要，请添加测试并安装目标。
target_link_libraries(NeVehicleLicensePlateRecognition opencv_world412d)
