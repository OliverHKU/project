# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/liucheng/gitrepo/project/opencv/opencv-2.4.9

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build

# Include any dependencies generated for this target.
include samples/cpp/CMakeFiles/example_stereo_match.dir/depend.make

# Include the progress variables for this target.
include samples/cpp/CMakeFiles/example_stereo_match.dir/progress.make

# Include the compile flags for this target's objects.
include samples/cpp/CMakeFiles/example_stereo_match.dir/flags.make

samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o: samples/cpp/CMakeFiles/example_stereo_match.dir/flags.make
samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o: ../samples/cpp/stereo_match.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o -c /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/samples/cpp/stereo_match.cpp

samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/example_stereo_match.dir/stereo_match.cpp.i"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/samples/cpp/stereo_match.cpp > CMakeFiles/example_stereo_match.dir/stereo_match.cpp.i

samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/example_stereo_match.dir/stereo_match.cpp.s"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/samples/cpp/stereo_match.cpp -o CMakeFiles/example_stereo_match.dir/stereo_match.cpp.s

samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o.requires:
.PHONY : samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o.requires

samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o.provides: samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o.requires
	$(MAKE) -f samples/cpp/CMakeFiles/example_stereo_match.dir/build.make samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o.provides.build
.PHONY : samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o.provides

samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o.provides.build: samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o

# Object files for target example_stereo_match
example_stereo_match_OBJECTS = \
"CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o"

# External object files for target example_stereo_match
example_stereo_match_EXTERNAL_OBJECTS =

bin/cpp-example-stereo_match: samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o
bin/cpp-example-stereo_match: /usr/lib/x86_64-linux-gnu/libGLU.so
bin/cpp-example-stereo_match: /usr/lib/x86_64-linux-gnu/libGL.so
bin/cpp-example-stereo_match: /usr/lib/x86_64-linux-gnu/libSM.so
bin/cpp-example-stereo_match: /usr/lib/x86_64-linux-gnu/libICE.so
bin/cpp-example-stereo_match: /usr/lib/x86_64-linux-gnu/libX11.so
bin/cpp-example-stereo_match: /usr/lib/x86_64-linux-gnu/libXext.so
bin/cpp-example-stereo_match: lib/libopencv_core.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_flann.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_imgproc.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_highgui.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_ml.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_video.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_objdetect.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_photo.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_nonfree.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_features2d.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_calib3d.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_legacy.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_contrib.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_stitching.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_videostab.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_gpu.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_ocl.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_nonfree.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_ocl.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_gpu.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_objdetect.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_photo.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_legacy.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_ml.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_video.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_calib3d.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_features2d.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_flann.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_highgui.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_imgproc.so.2.4.9
bin/cpp-example-stereo_match: lib/libopencv_core.so.2.4.9
bin/cpp-example-stereo_match: samples/cpp/CMakeFiles/example_stereo_match.dir/build.make
bin/cpp-example-stereo_match: samples/cpp/CMakeFiles/example_stereo_match.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../bin/cpp-example-stereo_match"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example_stereo_match.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
samples/cpp/CMakeFiles/example_stereo_match.dir/build: bin/cpp-example-stereo_match
.PHONY : samples/cpp/CMakeFiles/example_stereo_match.dir/build

samples/cpp/CMakeFiles/example_stereo_match.dir/requires: samples/cpp/CMakeFiles/example_stereo_match.dir/stereo_match.cpp.o.requires
.PHONY : samples/cpp/CMakeFiles/example_stereo_match.dir/requires

samples/cpp/CMakeFiles/example_stereo_match.dir/clean:
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp && $(CMAKE_COMMAND) -P CMakeFiles/example_stereo_match.dir/cmake_clean.cmake
.PHONY : samples/cpp/CMakeFiles/example_stereo_match.dir/clean

samples/cpp/CMakeFiles/example_stereo_match.dir/depend:
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/liucheng/gitrepo/project/opencv/opencv-2.4.9 /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/samples/cpp /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp/CMakeFiles/example_stereo_match.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : samples/cpp/CMakeFiles/example_stereo_match.dir/depend

