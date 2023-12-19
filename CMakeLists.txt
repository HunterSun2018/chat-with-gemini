cmake_minimum_required(VERSION 3.2.1)

project(Gemini)

set(CMAKE_CXX_STANDARD 20)

include_directories(~/boost_1_84_0)

aux_source_directory(src SRC)

add_executable(chat_with_gemini ${SRC})
target_link_libraries(chat_with_gemini co_http_client)

install(TARGETS chat_with_gemini
    LIBRARY DESTINATION lib 
    RUNTIME DESTINATION bin)