#include "conversation.hpp"

Conversation::Conversation(co_http::client_ptr client)
{

}

Task<std::string> Conversation::chat(std::string_view input)
{
    co_return "";
}