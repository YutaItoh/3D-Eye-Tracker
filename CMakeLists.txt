cmake_minimum_required(VERSION 2.8)

project( 3d_eye_tracker )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_INSTALL_PREFIX "./install") 

# Your custom paths to Boost and OpenCV libraries
set(BOOST_ROOT "C:/SDK/boost_1_62_0")
set(OpenCV_DIR "C:/SDK/opencv-3.1.0/build")

# Bundled external libraries
set(CMAKE_PREFIX_PATH  "${PROJECT_SOURCE_DIR}/external/Ceres-1.11/CMake")
set(SPII_INSTALL_DIR   "${PROJECT_SOURCE_DIR}/external/spii-3.0.0")
set(EIGEN_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/external/Eigen-3.2.7")
set(EIGEN3_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/external/Eigen-3.2.7")



###################################
### Setup Boost path
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
unset(Boost_INCLUDE_DIR CACHE)
unset(Boost_LIBRARY_DIRS CACHE)
IF (MSVC14)
  SET (_BOOST_COMPILER 14.0)
ELSEIF (MSVC12)
  SET (_BOOST_COMPILER 12.0)
ELSEIF (MSVC11)
  SET (_BOOST_COMPILER 11.0)
ELSEIF (MSVC10)
  SET (_BOOST_COMPILER 10.0)
ELSEIF (MSVC90)
  SET (_BOOST_COMPILER 9.0)
ELSEIF (MSVC80)
  SET (_BOOST_COMPILER 8.0)
ELSEIF (WIN32)
  SET (_BOOST_COMPILER _mt)
ENDIF (MSVC14)

IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET (BOOST_LIBRARYDIR ${BOOST_ROOT}/lib64-msvc-${_BOOST_COMPILER})
ELSE (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET (BOOST_LIBRARYDIR ${BOOST_ROOT}/lib32-msvc-${_BOOST_COMPILER})
ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 8)

#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME OFF)
###################################

#find_package ( Boost 1.58 COMPONENTS filesystem system timer REQUIRED )
find_package ( Boost 1.62 COMPONENTS filesystem system timer thread date_time chrono regex serialization REQUIRED )
find_package ( OpenCV REQUIRED )
#find_package ( TBB REQUIRED )
find_package ( Eigen3 REQUIRED )
find_package (Ceres REQUIRED)


### Find spii
#ExternalProject_Get_Property(spii SPII_INSTALL_DIR)
#set(SPII_INSTALL_DIR "C:/SDK/Spii")
#set(SPII_INSTALL_DIR "C:/SDK/spii-3.0.0/vc14_x64")
set(spii_INCLUDE_DIRS ${SPII_INSTALL_DIR}/include)
find_library (spii_LIBRARY_RELEASE
              spii
              PATHS ${SPII_INSTALL_DIR}/lib/ ${SPII_INSTALL_DIR}/lib/Release)
find_library (meschach_LIBRARY_RELEASE
              meschach
              PATHS ${SPII_INSTALL_DIR}/lib/ ${SPII_INSTALL_DIR}/lib/Release)
if (spii_LIBRARY_RELEASE AND meschach_LIBRARY_RELEASE)
    set(spii_LIBRARIES ${spii_LIBRARY_RELEASE} ${meschach_LIBRARY_RELEASE})
else()
    set(spii_LIBRARIES "")
endif()
message(STATUS "spii_LIBRARIES:\"${spii_LIBRARIES}\"")
# copy dll
file(COPY ${SPII_INSTALL_DIR}/bin/spii.dll DESTINATION ${PROJECT_BINARY_DIR}/main)

# copy OpenCV dlls
file(GLOB OPENCV_DLLS
  "${OpenCV_DIR}/x64/vc14/bin/*.dll"
)
file(COPY ${OPENCV_DLLS} DESTINATION ${PROJECT_BINARY_DIR}/main)

message(STATUS "")
message(STATUS "--- Include directories ---" )
message(STATUS " OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}" )
message(STATUS " Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}" )
message(STATUS "---------------------------" )
message(STATUS "")


add_subdirectory (main)
add_subdirectory (singleeyefitter)