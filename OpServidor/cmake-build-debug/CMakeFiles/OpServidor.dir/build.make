# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

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
CMAKE_COMMAND = /home/bernold/Desktop/clion-2020.2.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/bernold/Desktop/clion-2020.2.4/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bernold/CLionProjects/OpServidor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bernold/CLionProjects/OpServidor/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/OpServidor.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/OpServidor.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/OpServidor.dir/flags.make

CMakeFiles/OpServidor.dir/main.c.o: CMakeFiles/OpServidor.dir/flags.make
CMakeFiles/OpServidor.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bernold/CLionProjects/OpServidor/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/OpServidor.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/OpServidor.dir/main.c.o   -c /home/bernold/CLionProjects/OpServidor/main.c

CMakeFiles/OpServidor.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/OpServidor.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/bernold/CLionProjects/OpServidor/main.c > CMakeFiles/OpServidor.dir/main.c.i

CMakeFiles/OpServidor.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/OpServidor.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/bernold/CLionProjects/OpServidor/main.c -o CMakeFiles/OpServidor.dir/main.c.s

# Object files for target OpServidor
OpServidor_OBJECTS = \
"CMakeFiles/OpServidor.dir/main.c.o"

# External object files for target OpServidor
OpServidor_EXTERNAL_OBJECTS =

OpServidor: CMakeFiles/OpServidor.dir/main.c.o
OpServidor: CMakeFiles/OpServidor.dir/build.make
OpServidor: CMakeFiles/OpServidor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/bernold/CLionProjects/OpServidor/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable OpServidor"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/OpServidor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/OpServidor.dir/build: OpServidor

.PHONY : CMakeFiles/OpServidor.dir/build

CMakeFiles/OpServidor.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/OpServidor.dir/cmake_clean.cmake
.PHONY : CMakeFiles/OpServidor.dir/clean

CMakeFiles/OpServidor.dir/depend:
	cd /home/bernold/CLionProjects/OpServidor/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bernold/CLionProjects/OpServidor /home/bernold/CLionProjects/OpServidor /home/bernold/CLionProjects/OpServidor/cmake-build-debug /home/bernold/CLionProjects/OpServidor/cmake-build-debug /home/bernold/CLionProjects/OpServidor/cmake-build-debug/CMakeFiles/OpServidor.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/OpServidor.dir/depend
