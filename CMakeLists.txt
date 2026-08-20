cmake_minimum_required(VERSION 3.20)

project(Antivirus
  VERSION 1.0.0
  DESCRIPTION "My awesome antivirus"
  LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
  src/main.cpp
  src/error.cpp
  src/parser.cpp
  src/globals.cpp
  src/tester.cpp
  src/semantic.cpp
)

add_executable(${PROJECT_NAME}
  ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    include
)

