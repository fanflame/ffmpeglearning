# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/momo/Documents/CLionProjects/ffmpeg

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/momo/Documents/CLionProjects/ffmpeg/cmake-build-debug-gcc

# Include any dependencies generated for this target.
include CMakeFiles/ffmpeg.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ffmpeg.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ffmpeg.dir/flags.make

CMakeFiles/ffmpeg.dir/examples/encode_video.c.o: CMakeFiles/ffmpeg.dir/flags.make
CMakeFiles/ffmpeg.dir/examples/encode_video.c.o: ../examples/encode_video.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/momo/Documents/CLionProjects/ffmpeg/cmake-build-debug-gcc/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ffmpeg.dir/examples/encode_video.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/ffmpeg.dir/examples/encode_video.c.o   -c /Users/momo/Documents/CLionProjects/ffmpeg/examples/encode_video.c

CMakeFiles/ffmpeg.dir/examples/encode_video.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ffmpeg.dir/examples/encode_video.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/momo/Documents/CLionProjects/ffmpeg/examples/encode_video.c > CMakeFiles/ffmpeg.dir/examples/encode_video.c.i

CMakeFiles/ffmpeg.dir/examples/encode_video.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ffmpeg.dir/examples/encode_video.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/momo/Documents/CLionProjects/ffmpeg/examples/encode_video.c -o CMakeFiles/ffmpeg.dir/examples/encode_video.c.s

# Object files for target ffmpeg
ffmpeg_OBJECTS = \
"CMakeFiles/ffmpeg.dir/examples/encode_video.c.o"

# External object files for target ffmpeg
ffmpeg_EXTERNAL_OBJECTS =

ffmpeg: CMakeFiles/ffmpeg.dir/examples/encode_video.c.o
ffmpeg: CMakeFiles/ffmpeg.dir/build.make
ffmpeg: CMakeFiles/ffmpeg.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/momo/Documents/CLionProjects/ffmpeg/cmake-build-debug-gcc/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ffmpeg"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ffmpeg.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ffmpeg.dir/build: ffmpeg

.PHONY : CMakeFiles/ffmpeg.dir/build

CMakeFiles/ffmpeg.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ffmpeg.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ffmpeg.dir/clean

CMakeFiles/ffmpeg.dir/depend:
	cd /Users/momo/Documents/CLionProjects/ffmpeg/cmake-build-debug-gcc && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/momo/Documents/CLionProjects/ffmpeg /Users/momo/Documents/CLionProjects/ffmpeg /Users/momo/Documents/CLionProjects/ffmpeg/cmake-build-debug-gcc /Users/momo/Documents/CLionProjects/ffmpeg/cmake-build-debug-gcc /Users/momo/Documents/CLionProjects/ffmpeg/cmake-build-debug-gcc/CMakeFiles/ffmpeg.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ffmpeg.dir/depend

