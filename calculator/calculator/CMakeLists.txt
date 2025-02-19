set(TOKENIZE_SRC ${CMAKE_SOURCE_DIR}/calculator/tokenize/tokenize.cpp)
set(CALCULATOR_SRC calculator.cpp parser.cpp)

add_executable(calculator_test ${TOKENIZE_SRC} ${CALCULATOR_SRC} calculator_test.cpp)
target_include_directories(calculator_test PRIVATE ${CMAKE_SOURCE_DIR}/calculator/tokenize ${CMAKE_SOURCE_DIR}/calculator/polish_notation)
