

include_directories (
#  "${PROJECT_SOURCE_DIR}"
  "../external/DirectShowFrameGrabber"
  ${OpenCV_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
  ${CERES_INCLUDE_DIRS}
  ${spii_INCLUDE_DIRS}
  "../singleeyefitter"
)


file (GLOB SRCS 
  "*.cpp" 
  "../external/DirectShowFrameGrabber/*.cpp"
  "../external/DirectShowFrameGrabber/*.c"
  # "external/DirectShowFrameGrabber/*.idl"pupillabs
  )

file (GLOB HEADERS 
  *.h *.hpp 
  "../external/DirectShowFrameGrabber/*.h"
  )


add_executable( main ${SRCS} ${HEADERS} )
target_link_libraries( main 
		       ${OpenCV_LIBS}
		       ${Boost_FILESYSTEM_LIBRARY}
		       ${Boost_SYSTEM_LIBRARY}
		       ${Boost_LIBRARIES}
           ${spii_LIBRARIES}
           "singleeyefitter"
           )
