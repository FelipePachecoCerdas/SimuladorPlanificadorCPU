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
CMAKE_COMMAND = /home/jdtm23/Downloads/CLion-2020.2.4/clion-2020.2.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/jdtm23/Downloads/CLion-2020.2.4/clion-2020.2.4/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/OpCliente.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/OpCliente.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/OpCliente.dir/flags.make

CMakeFiles/OpCliente.dir/main.c.o: CMakeFiles/OpCliente.dir/flags.make
CMakeFiles/OpCliente.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/OpCliente.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/OpCliente.dir/main.c.o   -c /home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/main.c

CMakeFiles/OpCliente.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/OpCliente.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/main.c > CMakeFiles/OpCliente.dir/main.c.i

CMakeFiles/OpCliente.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/OpCliente.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/main.c -o CMakeFiles/OpCliente.dir/main.c.s

# Object files for target OpCliente
OpCliente_OBJECTS = \
"CMakeFiles/OpCliente.dir/main.c.o"

# External object files for target OpCliente
OpCliente_EXTERNAL_OBJECTS =

OpCliente: CMakeFiles/OpCliente.dir/main.c.o
OpCliente: CMakeFiles/OpCliente.dir/build.make
OpCliente: CMakeFiles/OpCliente.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable OpCliente"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/OpCliente.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/OpCliente.dir/build: OpCliente

.PHONY : CMakeFiles/OpCliente.dir/build

CMakeFiles/OpCliente.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/OpCliente.dir/cmake_clean.cmake
.PHONY : CMakeFiles/OpCliente.dir/clean

CMakeFiles/OpCliente.dir/depend:
	cd /home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos /home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos /home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/cmake-build-debug /home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/cmake-build-debug /home/jdtm23/Documents/SimuladorPlanificadorCPU/operativos/cmake-build-debug/CMakeFiles/OpCliente.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/OpCliente.dir/depend

