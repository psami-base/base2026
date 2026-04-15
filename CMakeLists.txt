cmake_minimum_required(VERSION 3.16)
project(oop-assignments)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSanitizer" ON)
add_compile_options(
	-Wall
	-Wextra
	-pedantic
	-Werror
	-Wno-self-assign-overloaded
	-Wno-missing-braces
)

if(ENABLE_SANITIZERS)
	add_compile_options(-fsanitize=address,undefined)
	add_link_options(-fsanitize=address,undefined)
endif()

include_directories(.)

add_subdirectory(any)
add_subdirectory(array)
# add_subdirectory(array_traits)
add_subdirectory(calculator)
# add_subdirectory(ct_prime)
add_subdirectory(frozen_set)
add_subdirectory(gcd)
add_subdirectory(geometry)
add_subdirectory(inserter)
# add_subdirectory(iterator_traits)
# add_subdirectory(iterops)
add_subdirectory(itertools)
# add_subdirectory(lru)
add_subdirectory(matrix)
add_subdirectory(move_iterator)
add_subdirectory(optional)
add_subdirectory(quantity)
add_subdirectory(random)
add_subdirectory(rational)
add_subdirectory(reverse_iterator)
add_subdirectory(shared_ptr)
# add_subdirectory(stack)
add_subdirectory(string)
add_subdirectory(string_view)
add_subdirectory(sum)
# add_subdirectory(type_vector)
add_subdirectory(unique_ptr)
add_subdirectory(unordered_set)
add_subdirectory(vector)
add_subdirectory(vector2d)
