

add_library (
    singleeyefitter
    SingleEyeFitter.cpp
    cvx.cpp
    utils.cpp
)

if (MSVC)
    add_definitions(-DNOMINMAX)
endif()

include_directories(
    SYSTEM
	${Boost_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN_INCLUDE_DIRS}
    ${CERES_INCLUDE_DIRS}
    ${spii_INCLUDE_DIRS}
)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    singleeyefitter
    ${Boost_LIBRARIES}
    ${OpenCV_LIBS}
    ${CERES_LIBRARIES}
    ${spii_LIBRARIES}
)
