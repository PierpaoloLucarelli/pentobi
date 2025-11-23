add_executable(pentobi_test test.cpp)

target_include_directories(pentobi_test PRIVATE
    ${CMAKE_SOURCE_DIR}/libpentobi_base
    ${CMAKE_SOURCE_DIR}/libboardgame_base
)

target_link_libraries(pentobi_test
    PRIVATE
        pentobi_base
        boardgame_base
)