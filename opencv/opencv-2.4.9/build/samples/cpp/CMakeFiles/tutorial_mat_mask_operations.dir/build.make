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
include samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/depend.make

# Include the progress variables for this target.
include samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/progress.make

# Include the compile flags for this target's objects.
include samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/flags.make

samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o: samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/flags.make
samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o: ../samples/cpp/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o -c /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/samples/cpp/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp

samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.i"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/samples/cpp/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp > CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.i

samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.s"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/samples/cpp/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp -o CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.s

samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o.requires:
.PHONY : samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o.requires

samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o.provides: samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o.requires
	$(MAKE) -f samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/build.make samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o.provides.build
.PHONY : samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o.provides

samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o.provides.build: samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o

# Object files for target tutorial_mat_mask_operations
tutorial_mat_mask_operations_OBJECTS = \
"CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o"

# External object files for target tutorial_mat_mask_operations
tutorial_mat_mask_operations_EXTERNAL_OBJECTS =

bin/cpp-tutorial-mat_mask_operations: samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o
bin/cpp-tutorial-mat_mask_operations: /usr/lib/x86_64-linux-gnu/libGLU.so
bin/cpp-tutorial-mat_mask_operations: /usr/lib/x86_64-linux-gnu/libGL.so
bin/cpp-tutorial-mat_mask_operations: /usr/lib/x86_64-linux-gnu/libSM.so
bin/cpp-tutorial-mat_mask_operations: /usr/lib/x86_64-linux-gnu/libICE.so
bin/cpp-tutorial-mat_mask_operations: /usr/lib/x86_64-linux-gnu/libX11.so
bin/cpp-tutorial-mat_mask_operations: /usr/lib/x86_64-linux-gnu/libXext.so
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_core.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_flann.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_imgproc.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_highgui.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_ml.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_video.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_objdetect.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_photo.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_nonfree.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_features2d.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_calib3d.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_legacy.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_contrib.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_stitching.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_videostab.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_gpu.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_ocl.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_nonfree.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_ocl.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_gpu.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_objdetect.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_photo.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_legacy.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_ml.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_video.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_calib3d.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_features2d.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_flann.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_highgui.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_imgproc.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: lib/libopencv_core.so.2.4.9
bin/cpp-tutorial-mat_mask_operations: samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/build.make
bin/cpp-tutorial-mat_mask_operations: samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../bin/cpp-tutorial-mat_mask_operations"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tutorial_mat_mask_operations.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/build: bin/cpp-tutorial-mat_mask_operations
.PHONY : samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/build

samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/requires: samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/tutorial_code/core/mat_mask_operations/mat_mask_operations.cpp.o.requires
.PHONY : samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/requires

samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/clean:
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp && $(CMAKE_COMMAND) -P CMakeFiles/tutorial_mat_mask_operations.dir/cmake_clean.cmake
.PHONY : samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/clean

samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/depend:
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/liucheng/gitrepo/project/opencv/opencv-2.4.9 /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/samples/cpp /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : samples/cpp/CMakeFiles/tutorial_mat_mask_operations.dir/depend

