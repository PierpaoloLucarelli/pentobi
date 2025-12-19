add_executable(pentobi_engine server.cpp pentobi_engine.cpp)

target_include_directories(pentobi_engine PRIVATE
    ${CMAKE_SOURCE_DIR}/libpentobi_base
    ${CMAKE_SOURCE_DIR}/libboardgame_base
    ${CMAKE_SOURCE_DIR}/libboardgame_mcts
    ${CMAKE_SOURCE_DIR}/libpentobi_mcts
)

target_link_libraries(pentobi_engine
    PRIVATE
        boardgame_base
        pentobi_base
        boardgame_mcts
        pentobi_mcts
)