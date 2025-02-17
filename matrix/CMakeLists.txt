set(RATIONAL_SRC ${CMAKE_SOURCE_DIR}/rational/rational.cpp)

add_executable(matrix_test ${RATIONAL_SRC} matrix_test.cpp)
target_include_directories(matrix_test PRIVATE ../rational)
