#pragma once

#include <string>
#include <string_view>
#include <co_helper.hpp>
#include <co_http_client.hpp>
#include <boost/json.hpp>

class Conversation
{
public:
    Conversation(co_http::client_ptr client, bool is_debug = false);

    Task<std::string> chat(std::string_view input);

    void reset();

private:
    co_http::client_ptr _client;
    boost::json::object _history;
    bool _debug = false;
};