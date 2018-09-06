cmake_minimum_required(VERSION 3.0)
project( TiffSplitter VERSION 0.0.1 DESCRIPTION "Command line utility for splitting a tiff file and keeping headers intact")

# Compiler stuff
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)
# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin )

# ========== Pre-reqs ==========
# ----------- opencv -----------
find_package( OpenCV REQUIRED core imgproc )

# ----------- boost -------------
find_package( Boost REQUIRED system filesystem )
if( Boost_FOUND )
	include_directories( ${Boost_INCLUDE_DIRS} )
endif()

# project headers
include_directories(include)

# ---------- libtiff -------------
find_package(TIFF REQUIRED)

add_library(ScanImageTiff SHARED src/ScanImageTiff.cpp)

set( PROJECT_LINK_LIBS libScanImageTiff.so )
link_directories(build /usr/local/lib)
# project sources
set(SOURCES src/write_tiff.cpp src/utils.cpp src/bitstrm.cpp src/main.cpp)

add_executable( TiffSplitter ${SOURCES} )
target_link_libraries( TiffSplitter ${OpenCV_LIBS} ${Boost_LIBRARIES} ${TIFF_LIBRARIES} ${PROJECT_LINK_LIBS} )