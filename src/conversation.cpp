#include <format>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/algorithm/string/replace.hpp>
#include "conversation.hpp"

using namespace std;

Conversation::Conversation(co_http::client_ptr client, bool is_debug) : _client(client), _debug(is_debug)
{
    this->reset();
}

void Conversation::reset()
{
    _history = {
        {"contents", boost::json::array()}};
}

Task<std::string> Conversation::chat(std::string_view input)
{
    using namespace co_http;
    using namespace boost::json;
    using jarray = boost::json::array;

    object user_content;
    user_content["role"] = "user";
    user_content["parts"] = jarray({object({{"text", input}})});
    auto &contents = _history["contents"].as_array();
    contents.emplace_back(user_content);

    ostringstream oss;
    oss << _history;

    if (_debug)
        cout << "INFO : " << oss.str() << endl;

    Request request;

    request.header["Content-Type"] = "application/json";
    request.body = oss.str();

    auto key = std::getenv("API_KEY");
    // auto url = format("https://generativelanguage.googleapis.com/v1beta3/models/text-bison-001:generateText?key={}", key);
    auto url = format("https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent?key={}", key);

    auto response = co_await _client->await_post(url, request);
    // cout << response.body << endl;

    auto value = boost::json::parse(response.body).as_object();
    if (value["error"].is_object())
    {
        oss << value << endl;
        throw runtime_error(oss.str());
    }

    // auto output = value["candidates"].as_array()[0].as_object()["output"];
    auto output = value["candidates"].as_array()[0].as_object()
                      ["content"]
                          .as_object()
                              ["parts"]
                          .as_array()[0]
                          .as_object()
                              ["text"];

    object model_content;
    model_content["role"] = "model";
    model_content["parts"] = jarray({object({{"text", output}})});

    contents.emplace_back(model_content);
    // cout << _history << endl;

    oss.str("");
    oss << output.as_string().c_str();
    
    co_return boost::replace_all_copy(oss.str(), "\\n", "\n ");
}