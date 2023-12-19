#pragma once

#include <string>
#include <string_view>
#include <co_helper.hpp>
#include <co_http_client.hpp>
#include <boost/json.hpp>

class Conversation
{
public:
    Conversation(co_http::client_ptr client);

    Task<std::string> chat(std::string_view input);

private:
    co_http::client_ptr _client;
    boost::json::value _history;        
};