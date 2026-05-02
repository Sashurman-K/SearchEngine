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

# Исходники библиотеки (без main.cpp)
set(LIB_SOURCES
    src/Indexing/Indexer.cpp
    src/Indexing/InvertedIndex.cpp
    src/QueryParser.cpp
    src/SearchEngine.cpp
    src/Ranking/TFIDFStrategy.cpp
    src/Ranking/BM25Strategy.cpp
    src/Ranking/Ranker.cpp
    src/TextProcessing/BaseTextProcessor.cpp
    src/TextProcessing/TextDecorator.cpp
    src/TextProcessing/StopWordsDecorator.cpp
    src/TextProcessing/StopWordsConfig.cpp
    src/TextProcessing/Stemmer.cpp
    src/TextProcessing/StemmingDecorator.cpp
)

# Библиотека
add_library(search_engine_lib ${LIB_SOURCES})

# Линковка libstemmer (если установлена)
find_library(STEMMER_LIB stemmer)
if(STEMMER_LIB)
    target_link_libraries(search_engine_lib PUBLIC ${STEMMER_LIB})
    message(STATUS "Found libstemmer: ${STEMMER_LIB}")
else()
    message(WARNING "libstemmer not found — stemming will not work")
endif()

# nholmann/json (header-only)
find_package(nlohmann_json QUIET)
if(nlohmann_json_FOUND)
    message(STATUS "Found nlohmann_json")
else()
    message(WARNING "nlohmann_json not found — stopwords config will not work")
endif()

# Исполняемый файл (опционально, если main.cpp дозреет)
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