add_executable(pentobi_test test2.cpp)

target_include_directories(pentobi_test PRIVATE
    ${CMAKE_SOURCE_DIR}/libpentobi_base
    ${CMAKE_SOURCE_DIR}/libboardgame_base
    ${CMAKE_SOURCE_DIR}/libboardgame_mcts
    ${CMAKE_SOURCE_DIR}/libpentobi_mcts
)

target_link_libraries(pentobi_test
    PRIVATE
        boardgame_base
        pentobi_base
        boardgame_mcts
        pentobi_mcts
)