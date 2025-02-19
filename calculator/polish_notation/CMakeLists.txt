set(TOKENIZE_SRC ${CMAKE_SOURCE_DIR}/calculator/tokenize/tokenize.cpp)
file(GLOB POLISH_NOTATION_SRC ./*.cpp)

add_executable(polish_notation_test ${POLISH_NOTATION_SRC} ${TOKENIZE_SRC} polish_notation_test.cpp)
target_include_directories(polish_notation_test PRIVATE ${CMAKE_SOURCE_DIR}/calculator/tokenize)
