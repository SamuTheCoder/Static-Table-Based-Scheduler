cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(STBS)

# Include main source file
target_sources(app PRIVATE src/main.c)

# Automatically include all `.c` files from the 'src' directory
file(GLOB SRC_SOURCES src/*.c)

# Automatically include all `.c` files from the 'lib' directory
file(GLOB LIB_SOURCES lib/*.h)

# Add them to the app target
target_sources(app PRIVATE ${SRC_SOURCES} ${LIB_SOURCES})

# Add the 'lib' directory to the include paths
target_include_directories(app PRIVATE lib)