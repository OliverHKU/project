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
include modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/depend.make

# Include the progress variables for this target.
include modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/progress.make

# Include the compile flags for this target's objects.
include modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/flags.make

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/flags.make
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o: ../modules/nonfree/test/test_rotation_and_scale_invariance.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -o CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o -c /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_rotation_and_scale_invariance.cpp

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.i"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -E /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_rotation_and_scale_invariance.cpp > CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.i

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.s"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -S /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_rotation_and_scale_invariance.cpp -o CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.s

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o.requires:
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o.requires

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o.provides: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o.requires
	$(MAKE) -f modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/build.make modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o.provides.build
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o.provides

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o.provides.build: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/flags.make
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o: ../modules/nonfree/test/test_detectors.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -o CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o -c /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_detectors.cpp

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.i"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -E /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_detectors.cpp > CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.i

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.s"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -S /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_detectors.cpp -o CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.s

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o.requires:
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o.requires

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o.provides: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o.requires
	$(MAKE) -f modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/build.make modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o.provides.build
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o.provides

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o.provides.build: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/flags.make
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o: ../modules/nonfree/test/test_gpu.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -o CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o -c /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_gpu.cpp

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.i"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -E /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_gpu.cpp > CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.i

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.s"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -S /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_gpu.cpp -o CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.s

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o.requires:
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o.requires

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o.provides: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o.requires
	$(MAKE) -f modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/build.make modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o.provides.build
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o.provides

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o.provides.build: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/flags.make
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o: ../modules/nonfree/test/test_surf_ocl.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -o CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o -c /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_surf_ocl.cpp

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.i"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -E /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_surf_ocl.cpp > CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.i

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.s"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -S /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_surf_ocl.cpp -o CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.s

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o.requires:
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o.requires

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o.provides: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o.requires
	$(MAKE) -f modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/build.make modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o.provides.build
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o.provides

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o.provides.build: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/flags.make
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o: ../modules/nonfree/test/test_features2d.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -o CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o -c /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_features2d.cpp

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.i"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -E /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_features2d.cpp > CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.i

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.s"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -S /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_features2d.cpp -o CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.s

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o.requires:
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o.requires

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o.provides: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o.requires
	$(MAKE) -f modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/build.make modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o.provides.build
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o.provides

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o.provides.build: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/flags.make
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o: ../modules/nonfree/test/test_main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -o CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o -c /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_main.cpp

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.i"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -E /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_main.cpp > CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.i

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.s"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -S /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_main.cpp -o CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.s

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o.requires:
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o.requires

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o.provides: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o.requires
	$(MAKE) -f modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/build.make modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o.provides.build
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o.provides

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o.provides.build: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/flags.make
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o: ../modules/nonfree/test/test_keypoints.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -o CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o -c /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_keypoints.cpp

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.i"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -E /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_keypoints.cpp > CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.i

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.s"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS)  -include "/home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/test_precomp.hpp" -Winvalid-pch  -S /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree/test/test_keypoints.cpp -o CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.s

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o.requires:
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o.requires

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o.provides: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o.requires
	$(MAKE) -f modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/build.make modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o.provides.build
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o.provides

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o.provides.build: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o

# Object files for target opencv_test_nonfree
opencv_test_nonfree_OBJECTS = \
"CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o" \
"CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o" \
"CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o" \
"CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o" \
"CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o" \
"CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o" \
"CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o"

# External object files for target opencv_test_nonfree
opencv_test_nonfree_EXTERNAL_OBJECTS =

bin/opencv_test_nonfree: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o
bin/opencv_test_nonfree: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o
bin/opencv_test_nonfree: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o
bin/opencv_test_nonfree: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o
bin/opencv_test_nonfree: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o
bin/opencv_test_nonfree: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o
bin/opencv_test_nonfree: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o
bin/opencv_test_nonfree: lib/libopencv_core.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_flann.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_imgproc.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_highgui.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_features2d.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_calib3d.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_ml.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_video.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_legacy.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_objdetect.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_photo.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_gpu.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_ocl.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_nonfree.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_ts.a
bin/opencv_test_nonfree: lib/libopencv_highgui.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_core.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_flann.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_imgproc.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_highgui.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_features2d.so.2.4.9
bin/opencv_test_nonfree: /usr/lib/x86_64-linux-gnu/libGLU.so
bin/opencv_test_nonfree: /usr/lib/x86_64-linux-gnu/libGL.so
bin/opencv_test_nonfree: /usr/lib/x86_64-linux-gnu/libSM.so
bin/opencv_test_nonfree: /usr/lib/x86_64-linux-gnu/libICE.so
bin/opencv_test_nonfree: /usr/lib/x86_64-linux-gnu/libX11.so
bin/opencv_test_nonfree: /usr/lib/x86_64-linux-gnu/libXext.so
bin/opencv_test_nonfree: lib/libopencv_gpu.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_legacy.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_photo.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_ocl.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_calib3d.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_ml.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_video.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_objdetect.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_features2d.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_flann.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_highgui.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_imgproc.so.2.4.9
bin/opencv_test_nonfree: lib/libopencv_core.so.2.4.9
bin/opencv_test_nonfree: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/build.make
bin/opencv_test_nonfree: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../bin/opencv_test_nonfree"
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/opencv_test_nonfree.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/build: bin/opencv_test_nonfree
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/build

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/requires: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_rotation_and_scale_invariance.cpp.o.requires
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/requires: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_detectors.cpp.o.requires
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/requires: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_gpu.cpp.o.requires
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/requires: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_surf_ocl.cpp.o.requires
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/requires: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_features2d.cpp.o.requires
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/requires: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_main.cpp.o.requires
modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/requires: modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/test/test_keypoints.cpp.o.requires
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/requires

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/clean:
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree && $(CMAKE_COMMAND) -P CMakeFiles/opencv_test_nonfree.dir/cmake_clean.cmake
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/clean

modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/depend:
	cd /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/liucheng/gitrepo/project/opencv/opencv-2.4.9 /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/modules/nonfree /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree /home/liucheng/gitrepo/project/opencv/opencv-2.4.9/build/modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/nonfree/CMakeFiles/opencv_test_nonfree.dir/depend

