# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/robin/Dropbox/Programming/c/TiffSplitter

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/robin/Dropbox/Programming/c/TiffSplitter/build

# Include any dependencies generated for this target.
include CMakeFiles/ScanImageTiff.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ScanImageTiff.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ScanImageTiff.dir/flags.make

CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o: CMakeFiles/ScanImageTiff.dir/flags.make
CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o: ../src/ScanImageTiff.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/robin/Dropbox/Programming/c/TiffSplitter/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o -c /home/robin/Dropbox/Programming/c/TiffSplitter/src/ScanImageTiff.cpp

CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/robin/Dropbox/Programming/c/TiffSplitter/src/ScanImageTiff.cpp > CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.i

CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/robin/Dropbox/Programming/c/TiffSplitter/src/ScanImageTiff.cpp -o CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.s

CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o.requires:

.PHONY : CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o.requires

CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o.provides: CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o.requires
	$(MAKE) -f CMakeFiles/ScanImageTiff.dir/build.make CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o.provides.build
.PHONY : CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o.provides

CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o.provides.build: CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o


# Object files for target ScanImageTiff
ScanImageTiff_OBJECTS = \
"CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o"

# External object files for target ScanImageTiff
ScanImageTiff_EXTERNAL_OBJECTS =

libScanImageTiff.so: CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o
libScanImageTiff.so: CMakeFiles/ScanImageTiff.dir/build.make
libScanImageTiff.so: CMakeFiles/ScanImageTiff.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/robin/Dropbox/Programming/c/TiffSplitter/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libScanImageTiff.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ScanImageTiff.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ScanImageTiff.dir/build: libScanImageTiff.so

.PHONY : CMakeFiles/ScanImageTiff.dir/build

CMakeFiles/ScanImageTiff.dir/requires: CMakeFiles/ScanImageTiff.dir/src/ScanImageTiff.cpp.o.requires

.PHONY : CMakeFiles/ScanImageTiff.dir/requires

CMakeFiles/ScanImageTiff.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ScanImageTiff.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ScanImageTiff.dir/clean

CMakeFiles/ScanImageTiff.dir/depend:
	cd /home/robin/Dropbox/Programming/c/TiffSplitter/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/robin/Dropbox/Programming/c/TiffSplitter /home/robin/Dropbox/Programming/c/TiffSplitter /home/robin/Dropbox/Programming/c/TiffSplitter/build /home/robin/Dropbox/Programming/c/TiffSplitter/build /home/robin/Dropbox/Programming/c/TiffSplitter/build/CMakeFiles/ScanImageTiff.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ScanImageTiff.dir/depend

