sc_begin_package(MSEED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../base/common/libs)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../base/common/libs)

subdirs(src)
