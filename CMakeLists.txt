#cmake -S . -B build && cmake --build build && cd build
#./unit_tests
#./search_engine
#make ./scenario_1


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

if(EXISTS ${CMAKE_SOURCE_DIR}/src/main.cpp)
    add_executable(search_engine src/main.cpp)
    target_link_libraries(search_engine PRIVATE search_engine_lib)
endif()

# Поиск пакета Google Test
find_package(GTest REQUIRED)
include(GoogleTest)

# Объявление тестового исполняемого файла.
# Указываем только файлы с самими тестами (предполагается, что они лежат в папке tests/)
add_executable(unit_tests
    tests/test_query_parser.cpp
    tests/test_text_processors.cpp
    tests/test_search_engine.cpp
    tests/test_inverted_index.cpp
    tests/test_ranking.cpp
    tests/test_indexer.cpp
)

# Подключение заголовочных файлов проекта
target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Линкуем тесты с вашей основной библиотекой, GTest и дополнительными зависимостями
target_link_libraries(unit_tests
    PRIVATE
    search_engine_lib
    GTest::gtest
    GTest::gtest_main
)

# Автоматическая регистрация тестов в CTest
gtest_discover_tests(unit_tests)

add_executable(scenario_1 tests/scenarios/scenario_1.cpp)
target_link_libraries(scenario_1 PRIVATE search_engine_lib)

add_executable(scenario_2 tests/scenarios/scenario_2.cpp)
target_link_libraries(scenario_2 PRIVATE search_engine_lib)

add_executable(scenario_3 tests/scenarios/scenario_3.cpp)
target_link_libraries(scenario_3 PRIVATE search_engine_lib)

if(EXISTS "${CMAKE_SOURCE_DIR}/stopwords_config.json")
    message(STATUS "Setting up automatic copy for stopwords_config.json")

    file(COPY "${CMAKE_SOURCE_DIR}/stopwords_config.json"
         DESTINATION "${CMAKE_BINARY_DIR}"
    )
else()
    message(WARNING "stopwords_config.json not found in source directory! Auto-copy skipped.")
endif()