cmake_minimum_required(VERSION 3.6)

set(default_build_type "Release")
set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
STRING "Choose the type of build." FORCE)

PROJECT(manual)

FIND_PACKAGE(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

SET(SRC
 main.cpp
)

SET(PROJECT_NAME
 a
)

SET(OpenCV_DIR
 e/opencv/opencv/build 
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  GPIO)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  wiringPi)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  pthread)
