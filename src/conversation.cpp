#include <format>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/algorithm/string/replace.hpp>
#include "conversation.hpp"

using namespace std;

Conversation::Conversation(co_http::client_ptr client) : _client(client)
{

}

Task<std::string> Conversation::chat(std::string_view input)
{
    using namespace co_http;
    using namespace boost::json;

    value jv = {
        { "contents", { // array
            {
                { "parts", {   // array
                {{"text", input}}
                }
                }
            }
            }
        }
    };

    boost::json::object obj;
    boost::json::array contents, parts;
    // obj["prompt"].emplace_object()["text"] = input;    
    parts = { object({{"text", input}}) };
    contents = { object({ {"role", "user"}, { "parts", parts }}) };
    obj["contents"] = contents;

    ostringstream oss;

    cout << obj << endl;
    Request request;

    request.header["Content-Type"] = "application/json";
    oss << jv;
    request.body = oss.str();
    // cout << oss.str() << endl;
    oss.str("");

    auto key = std::getenv("API_KEY");
    //auto url = format("https://generativelanguage.googleapis.com/v1beta3/models/text-bison-001:generateText?key={}", key);
    auto url = format("https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent?key={}", key);

    auto response = co_await _client->await_post(url, request);
    // cout << response.body << endl;

    auto value = boost::json::parse(response.body).as_object();
    if (value["error"].is_object())
    {
        oss << value << endl;
        throw runtime_error(oss.str());
    }

    //auto output = value["candidates"].as_array()[0].as_object()["output"];
    auto output = value["candidates"].as_array()[0].as_object()
        ["content"].as_object()
        ["parts"].as_array()[0].as_object()
        ["text"];

    oss << output.as_string().c_str();

    co_return boost::replace_all_copy(oss.str(), "\\n", "\n ");    
}