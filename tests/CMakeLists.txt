# Исполняемый файл для тестов
add_executable(test_indexer test_indexer.cpp)
target_link_libraries(test_indexer PRIVATE search_engine_lib)

# Регистрируем тест в CTest
add_test(NAME IndexerTests COMMAND test_indexer)

target_link_libraries(test_indexer PRIVATE search_engine_lib stemmer)