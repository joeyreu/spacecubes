# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.11.1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.11.1/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build

# Include any dependencies generated for this target.
include examples/CMakeFiles/Simple.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/Simple.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/Simple.dir/flags.make

examples/CMakeFiles/Simple.dir/simple.c.o: examples/CMakeFiles/Simple.dir/flags.make
examples/CMakeFiles/Simple.dir/simple.c.o: ../examples/simple.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object examples/CMakeFiles/Simple.dir/simple.c.o"
	cd /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build/examples && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Simple.dir/simple.c.o   -c /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/examples/simple.c

examples/CMakeFiles/Simple.dir/simple.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Simple.dir/simple.c.i"
	cd /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build/examples && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/examples/simple.c > CMakeFiles/Simple.dir/simple.c.i

examples/CMakeFiles/Simple.dir/simple.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Simple.dir/simple.c.s"
	cd /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build/examples && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/examples/simple.c -o CMakeFiles/Simple.dir/simple.c.s

# Object files for target Simple
Simple_OBJECTS = \
"CMakeFiles/Simple.dir/simple.c.o"

# External object files for target Simple
Simple_EXTERNAL_OBJECTS =

examples/Simple.app/Contents/MacOS/Simple: examples/CMakeFiles/Simple.dir/simple.c.o
examples/Simple.app/Contents/MacOS/Simple: examples/CMakeFiles/Simple.dir/build.make
examples/Simple.app/Contents/MacOS/Simple: src/libglfw3.a
examples/Simple.app/Contents/MacOS/Simple: examples/CMakeFiles/Simple.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable Simple.app/Contents/MacOS/Simple"
	cd /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Simple.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/Simple.dir/build: examples/Simple.app/Contents/MacOS/Simple

.PHONY : examples/CMakeFiles/Simple.dir/build

examples/CMakeFiles/Simple.dir/clean:
	cd /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build/examples && $(CMAKE_COMMAND) -P CMakeFiles/Simple.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/Simple.dir/clean

examples/CMakeFiles/Simple.dir/depend:
	cd /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1 /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/examples /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build/examples /Users/joeyreu/Desktop/CS488_Project/shared/glfw-3.1.1/build/examples/CMakeFiles/Simple.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/Simple.dir/depend

