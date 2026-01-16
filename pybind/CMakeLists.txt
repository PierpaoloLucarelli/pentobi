add_executable(pentobi_engine server.cpp pentobi_engine.cpp main.cpp crash_handler.cpp logger.cpp validation.cpp)
add_executable(generate_map move_convert.cpp)

target_include_directories(pentobi_engine PRIVATE
    ${CMAKE_SOURCE_DIR}/libpentobi_base
    ${CMAKE_SOURCE_DIR}/libboardgame_base
    ${CMAKE_SOURCE_DIR}/libboardgame_mcts
    ${CMAKE_SOURCE_DIR}/libpentobi_mcts
)
target_include_directories(generate_map PRIVATE
    ${CMAKE_SOURCE_DIR}/libpentobi_base
    ${CMAKE_SOURCE_DIR}/libboardgame_base
    ${CMAKE_SOURCE_DIR}/libboardgame_mcts
    ${CMAKE_SOURCE_DIR}/libpentobi_mcts
)


target_link_libraries(generate_map
    PRIVATE
        boardgame_base
        pentobi_base
        boardgame_mcts
        pentobi_mcts
)

add_library(pentobi_cached_moves
    cached_moves.cpp
)

target_include_directories(pentobi_cached_moves PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(pentobi_cached_moves PUBLIC
    boardgame_base
    pentobi_base
    boardgame_mcts
    pentobi_mcts
)

target_link_libraries(pentobi_engine
    PRIVATE
        boardgame_base
        pentobi_base
        boardgame_mcts
        pentobi_mcts
        pentobi_cached_moves
)
