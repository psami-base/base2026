add_subdirectory(game)
add_subdirectory(word_generator)

find_library(TGBOT_LIB TgBot PATHS /usr/local/lib)

if(TGBOT_LIB AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
	find_package(OpenSSL REQUIRED)
	find_package(ZLIB REQUIRED)
	find_package(CURL REQUIRED)

	add_executable(bot main.cpp game/hangman.cpp bot/bot.cpp word_generator/word_generator.cpp)
	target_link_libraries(bot PRIVATE OpenSSL::SSL ZLIB::ZLIB CURL::libcurl ${TGBOT_LIB})
endif()