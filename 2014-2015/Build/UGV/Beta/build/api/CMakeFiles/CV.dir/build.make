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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cracker/workspace/Build/UGV/Beta/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cracker/workspace/Build/UGV/Beta/build

# Include any dependencies generated for this target.
include api/CMakeFiles/CV.dir/depend.make

# Include the progress variables for this target.
include api/CMakeFiles/CV.dir/progress.make

# Include the compile flags for this target's objects.
include api/CMakeFiles/CV.dir/flags.make

api/CMakeFiles/CV.dir/src/CV.C.o: api/CMakeFiles/CV.dir/flags.make
api/CMakeFiles/CV.dir/src/CV.C.o: /home/cracker/workspace/Build/UGV/Beta/src/api/src/CV.C
	$(CMAKE_COMMAND) -E cmake_progress_report /home/cracker/workspace/Build/UGV/Beta/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object api/CMakeFiles/CV.dir/src/CV.C.o"
	cd /home/cracker/workspace/Build/UGV/Beta/build/api && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/CV.dir/src/CV.C.o -c /home/cracker/workspace/Build/UGV/Beta/src/api/src/CV.C

api/CMakeFiles/CV.dir/src/CV.C.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CV.dir/src/CV.C.i"
	cd /home/cracker/workspace/Build/UGV/Beta/build/api && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/cracker/workspace/Build/UGV/Beta/src/api/src/CV.C > CMakeFiles/CV.dir/src/CV.C.i

api/CMakeFiles/CV.dir/src/CV.C.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CV.dir/src/CV.C.s"
	cd /home/cracker/workspace/Build/UGV/Beta/build/api && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/cracker/workspace/Build/UGV/Beta/src/api/src/CV.C -o CMakeFiles/CV.dir/src/CV.C.s

api/CMakeFiles/CV.dir/src/CV.C.o.requires:
.PHONY : api/CMakeFiles/CV.dir/src/CV.C.o.requires

api/CMakeFiles/CV.dir/src/CV.C.o.provides: api/CMakeFiles/CV.dir/src/CV.C.o.requires
	$(MAKE) -f api/CMakeFiles/CV.dir/build.make api/CMakeFiles/CV.dir/src/CV.C.o.provides.build
.PHONY : api/CMakeFiles/CV.dir/src/CV.C.o.provides

api/CMakeFiles/CV.dir/src/CV.C.o.provides.build: api/CMakeFiles/CV.dir/src/CV.C.o

# Object files for target CV
CV_OBJECTS = \
"CMakeFiles/CV.dir/src/CV.C.o"

# External object files for target CV
CV_EXTERNAL_OBJECTS =

/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: api/CMakeFiles/CV.dir/src/CV.C.o
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: api/CMakeFiles/CV.dir/build.make
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /opt/ros/indigo/lib/libroscpp.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /usr/lib/x86_64-linux-gnu/libboost_signals.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /opt/ros/indigo/lib/librosconsole.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /opt/ros/indigo/lib/librosconsole_log4cxx.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /opt/ros/indigo/lib/librosconsole_backend_interface.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /usr/lib/liblog4cxx.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /usr/lib/x86_64-linux-gnu/libboost_regex.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /opt/ros/indigo/lib/libxmlrpcpp.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /opt/ros/indigo/lib/libroscpp_serialization.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /opt/ros/indigo/lib/librostime.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /opt/ros/indigo/lib/libcpp_common.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /usr/lib/x86_64-linux-gnu/libboost_system.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /usr/lib/x86_64-linux-gnu/libboost_thread.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /usr/lib/x86_64-linux-gnu/libpthread.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: /usr/lib/x86_64-linux-gnu/libconsole_bridge.so
/home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV: api/CMakeFiles/CV.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable /home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV"
	cd /home/cracker/workspace/Build/UGV/Beta/build/api && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CV.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
api/CMakeFiles/CV.dir/build: /home/cracker/workspace/Build/UGV/Beta/devel/lib/api/CV
.PHONY : api/CMakeFiles/CV.dir/build

api/CMakeFiles/CV.dir/requires: api/CMakeFiles/CV.dir/src/CV.C.o.requires
.PHONY : api/CMakeFiles/CV.dir/requires

api/CMakeFiles/CV.dir/clean:
	cd /home/cracker/workspace/Build/UGV/Beta/build/api && $(CMAKE_COMMAND) -P CMakeFiles/CV.dir/cmake_clean.cmake
.PHONY : api/CMakeFiles/CV.dir/clean

api/CMakeFiles/CV.dir/depend:
	cd /home/cracker/workspace/Build/UGV/Beta/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cracker/workspace/Build/UGV/Beta/src /home/cracker/workspace/Build/UGV/Beta/src/api /home/cracker/workspace/Build/UGV/Beta/build /home/cracker/workspace/Build/UGV/Beta/build/api /home/cracker/workspace/Build/UGV/Beta/build/api/CMakeFiles/CV.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : api/CMakeFiles/CV.dir/depend
