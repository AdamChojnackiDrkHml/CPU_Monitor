# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/adam/Pers/CPU_Monitor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/adam/Pers/CPU_Monitor/build

# Include any dependencies generated for this target.
include CMakeFiles/CPU_Monitor.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/CPU_Monitor.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/CPU_Monitor.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CPU_Monitor.dir/flags.make

CMakeFiles/CPU_Monitor.dir/src/analyzer.c.o: CMakeFiles/CPU_Monitor.dir/flags.make
CMakeFiles/CPU_Monitor.dir/src/analyzer.c.o: ../src/analyzer.c
CMakeFiles/CPU_Monitor.dir/src/analyzer.c.o: CMakeFiles/CPU_Monitor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adam/Pers/CPU_Monitor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/CPU_Monitor.dir/src/analyzer.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/CPU_Monitor.dir/src/analyzer.c.o -MF CMakeFiles/CPU_Monitor.dir/src/analyzer.c.o.d -o CMakeFiles/CPU_Monitor.dir/src/analyzer.c.o -c /home/adam/Pers/CPU_Monitor/src/analyzer.c

CMakeFiles/CPU_Monitor.dir/src/analyzer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CPU_Monitor.dir/src/analyzer.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/adam/Pers/CPU_Monitor/src/analyzer.c > CMakeFiles/CPU_Monitor.dir/src/analyzer.c.i

CMakeFiles/CPU_Monitor.dir/src/analyzer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CPU_Monitor.dir/src/analyzer.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/adam/Pers/CPU_Monitor/src/analyzer.c -o CMakeFiles/CPU_Monitor.dir/src/analyzer.c.s

CMakeFiles/CPU_Monitor.dir/src/logger.c.o: CMakeFiles/CPU_Monitor.dir/flags.make
CMakeFiles/CPU_Monitor.dir/src/logger.c.o: ../src/logger.c
CMakeFiles/CPU_Monitor.dir/src/logger.c.o: CMakeFiles/CPU_Monitor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adam/Pers/CPU_Monitor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/CPU_Monitor.dir/src/logger.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/CPU_Monitor.dir/src/logger.c.o -MF CMakeFiles/CPU_Monitor.dir/src/logger.c.o.d -o CMakeFiles/CPU_Monitor.dir/src/logger.c.o -c /home/adam/Pers/CPU_Monitor/src/logger.c

CMakeFiles/CPU_Monitor.dir/src/logger.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CPU_Monitor.dir/src/logger.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/adam/Pers/CPU_Monitor/src/logger.c > CMakeFiles/CPU_Monitor.dir/src/logger.c.i

CMakeFiles/CPU_Monitor.dir/src/logger.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CPU_Monitor.dir/src/logger.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/adam/Pers/CPU_Monitor/src/logger.c -o CMakeFiles/CPU_Monitor.dir/src/logger.c.s

CMakeFiles/CPU_Monitor.dir/src/main.c.o: CMakeFiles/CPU_Monitor.dir/flags.make
CMakeFiles/CPU_Monitor.dir/src/main.c.o: ../src/main.c
CMakeFiles/CPU_Monitor.dir/src/main.c.o: CMakeFiles/CPU_Monitor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adam/Pers/CPU_Monitor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/CPU_Monitor.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/CPU_Monitor.dir/src/main.c.o -MF CMakeFiles/CPU_Monitor.dir/src/main.c.o.d -o CMakeFiles/CPU_Monitor.dir/src/main.c.o -c /home/adam/Pers/CPU_Monitor/src/main.c

CMakeFiles/CPU_Monitor.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CPU_Monitor.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/adam/Pers/CPU_Monitor/src/main.c > CMakeFiles/CPU_Monitor.dir/src/main.c.i

CMakeFiles/CPU_Monitor.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CPU_Monitor.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/adam/Pers/CPU_Monitor/src/main.c -o CMakeFiles/CPU_Monitor.dir/src/main.c.s

CMakeFiles/CPU_Monitor.dir/src/printer.c.o: CMakeFiles/CPU_Monitor.dir/flags.make
CMakeFiles/CPU_Monitor.dir/src/printer.c.o: ../src/printer.c
CMakeFiles/CPU_Monitor.dir/src/printer.c.o: CMakeFiles/CPU_Monitor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adam/Pers/CPU_Monitor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/CPU_Monitor.dir/src/printer.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/CPU_Monitor.dir/src/printer.c.o -MF CMakeFiles/CPU_Monitor.dir/src/printer.c.o.d -o CMakeFiles/CPU_Monitor.dir/src/printer.c.o -c /home/adam/Pers/CPU_Monitor/src/printer.c

CMakeFiles/CPU_Monitor.dir/src/printer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CPU_Monitor.dir/src/printer.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/adam/Pers/CPU_Monitor/src/printer.c > CMakeFiles/CPU_Monitor.dir/src/printer.c.i

CMakeFiles/CPU_Monitor.dir/src/printer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CPU_Monitor.dir/src/printer.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/adam/Pers/CPU_Monitor/src/printer.c -o CMakeFiles/CPU_Monitor.dir/src/printer.c.s

CMakeFiles/CPU_Monitor.dir/src/queue.c.o: CMakeFiles/CPU_Monitor.dir/flags.make
CMakeFiles/CPU_Monitor.dir/src/queue.c.o: ../src/queue.c
CMakeFiles/CPU_Monitor.dir/src/queue.c.o: CMakeFiles/CPU_Monitor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adam/Pers/CPU_Monitor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/CPU_Monitor.dir/src/queue.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/CPU_Monitor.dir/src/queue.c.o -MF CMakeFiles/CPU_Monitor.dir/src/queue.c.o.d -o CMakeFiles/CPU_Monitor.dir/src/queue.c.o -c /home/adam/Pers/CPU_Monitor/src/queue.c

CMakeFiles/CPU_Monitor.dir/src/queue.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CPU_Monitor.dir/src/queue.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/adam/Pers/CPU_Monitor/src/queue.c > CMakeFiles/CPU_Monitor.dir/src/queue.c.i

CMakeFiles/CPU_Monitor.dir/src/queue.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CPU_Monitor.dir/src/queue.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/adam/Pers/CPU_Monitor/src/queue.c -o CMakeFiles/CPU_Monitor.dir/src/queue.c.s

CMakeFiles/CPU_Monitor.dir/src/reader.c.o: CMakeFiles/CPU_Monitor.dir/flags.make
CMakeFiles/CPU_Monitor.dir/src/reader.c.o: ../src/reader.c
CMakeFiles/CPU_Monitor.dir/src/reader.c.o: CMakeFiles/CPU_Monitor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adam/Pers/CPU_Monitor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/CPU_Monitor.dir/src/reader.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/CPU_Monitor.dir/src/reader.c.o -MF CMakeFiles/CPU_Monitor.dir/src/reader.c.o.d -o CMakeFiles/CPU_Monitor.dir/src/reader.c.o -c /home/adam/Pers/CPU_Monitor/src/reader.c

CMakeFiles/CPU_Monitor.dir/src/reader.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CPU_Monitor.dir/src/reader.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/adam/Pers/CPU_Monitor/src/reader.c > CMakeFiles/CPU_Monitor.dir/src/reader.c.i

CMakeFiles/CPU_Monitor.dir/src/reader.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CPU_Monitor.dir/src/reader.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/adam/Pers/CPU_Monitor/src/reader.c -o CMakeFiles/CPU_Monitor.dir/src/reader.c.s

CMakeFiles/CPU_Monitor.dir/src/watchdog.c.o: CMakeFiles/CPU_Monitor.dir/flags.make
CMakeFiles/CPU_Monitor.dir/src/watchdog.c.o: ../src/watchdog.c
CMakeFiles/CPU_Monitor.dir/src/watchdog.c.o: CMakeFiles/CPU_Monitor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adam/Pers/CPU_Monitor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/CPU_Monitor.dir/src/watchdog.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/CPU_Monitor.dir/src/watchdog.c.o -MF CMakeFiles/CPU_Monitor.dir/src/watchdog.c.o.d -o CMakeFiles/CPU_Monitor.dir/src/watchdog.c.o -c /home/adam/Pers/CPU_Monitor/src/watchdog.c

CMakeFiles/CPU_Monitor.dir/src/watchdog.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CPU_Monitor.dir/src/watchdog.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/adam/Pers/CPU_Monitor/src/watchdog.c > CMakeFiles/CPU_Monitor.dir/src/watchdog.c.i

CMakeFiles/CPU_Monitor.dir/src/watchdog.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CPU_Monitor.dir/src/watchdog.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/adam/Pers/CPU_Monitor/src/watchdog.c -o CMakeFiles/CPU_Monitor.dir/src/watchdog.c.s

# Object files for target CPU_Monitor
CPU_Monitor_OBJECTS = \
"CMakeFiles/CPU_Monitor.dir/src/analyzer.c.o" \
"CMakeFiles/CPU_Monitor.dir/src/logger.c.o" \
"CMakeFiles/CPU_Monitor.dir/src/main.c.o" \
"CMakeFiles/CPU_Monitor.dir/src/printer.c.o" \
"CMakeFiles/CPU_Monitor.dir/src/queue.c.o" \
"CMakeFiles/CPU_Monitor.dir/src/reader.c.o" \
"CMakeFiles/CPU_Monitor.dir/src/watchdog.c.o"

# External object files for target CPU_Monitor
CPU_Monitor_EXTERNAL_OBJECTS =

CPU_Monitor: CMakeFiles/CPU_Monitor.dir/src/analyzer.c.o
CPU_Monitor: CMakeFiles/CPU_Monitor.dir/src/logger.c.o
CPU_Monitor: CMakeFiles/CPU_Monitor.dir/src/main.c.o
CPU_Monitor: CMakeFiles/CPU_Monitor.dir/src/printer.c.o
CPU_Monitor: CMakeFiles/CPU_Monitor.dir/src/queue.c.o
CPU_Monitor: CMakeFiles/CPU_Monitor.dir/src/reader.c.o
CPU_Monitor: CMakeFiles/CPU_Monitor.dir/src/watchdog.c.o
CPU_Monitor: CMakeFiles/CPU_Monitor.dir/build.make
CPU_Monitor: CMakeFiles/CPU_Monitor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/adam/Pers/CPU_Monitor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking C executable CPU_Monitor"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CPU_Monitor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CPU_Monitor.dir/build: CPU_Monitor
.PHONY : CMakeFiles/CPU_Monitor.dir/build

CMakeFiles/CPU_Monitor.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CPU_Monitor.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CPU_Monitor.dir/clean

CMakeFiles/CPU_Monitor.dir/depend:
	cd /home/adam/Pers/CPU_Monitor/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/adam/Pers/CPU_Monitor /home/adam/Pers/CPU_Monitor /home/adam/Pers/CPU_Monitor/build /home/adam/Pers/CPU_Monitor/build /home/adam/Pers/CPU_Monitor/build/CMakeFiles/CPU_Monitor.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CPU_Monitor.dir/depend

