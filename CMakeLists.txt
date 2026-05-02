cmake_minimum_required(VERSION 3.16)
project(SearchEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Папки с заголовками
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/Indexing
    ${CMAKE_SOURCE_DIR}/include/TextProcessing
    ${CMAKE_SOURCE_DIR}/include/Ranking
)

# Ищем все .cpp файлы рекурсивно в src/
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Основная библиотека
add_library(search_engine_lib ${SOURCES})

# Исполняемый файл (если есть main.cpp)
if(EXISTS ${CMAKE_SOURCE_DIR}/src/main.cpp)
    add_executable(search_engine src/main.cpp)
    target_link_libraries(search_engine PRIVATE search_engine_lib)
endif()

# Тесты
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

target_link_libraries(search_engine_lib PUBLIC stemmer)