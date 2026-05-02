# Тест индексатора
add_executable(test_indexer test_indexer.cpp)
target_link_libraries(test_indexer PRIVATE search_engine_lib)
add_test(NAME IndexerTests COMMAND test_indexer)

# Тест поиска
add_executable(test_search test_search.cpp)
target_link_libraries(test_search PRIVATE search_engine_lib)
add_test(NAME SearchTests COMMAND test_search)
set_tests_properties(IndexerTests SearchTests PROPERTIES
    FAIL_REGULAR_EXPRESSION "FAILED"
)