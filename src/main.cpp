#include <iostream>
#include <format>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <co_helper.hpp>
#include <co_http_client.hpp>
//#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <readline/readline.h>
#include "conversation.hpp"

using namespace std;

Task<string> chat(string_view input = "Write a story about a magic backpack");

int main(int argc, char const* argv[])
{
    try
    {
        cout << "Let's get started to chat with Google Gemini AI." << endl;

        auto prompt = argc > 1 ? argv[1] : "Write a story about a magic backpack";
        cout << format("You : {}", prompt) << endl;

        auto response = chat(prompt).get();

        cout << format("Gemini : {}", response) << endl;

        auto client = co_http::Client::create();

        auto conversation = make_shared<Conversation>(client);

        string input = readline("prompt : ");
        while (input != "exit")
        {
            /* code */
        }


    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception : " << e.what() << '\n';
    }

    return 0;
}

Task<string> chat(string_view input)
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
    contents = { object({{ "parts", parts }}) };
    obj["contents"] = contents;

    ostringstream oss;

    // cout << jv << endl;

    auto client = Client::create();
    Request request;

    request.header["Content-Type"] = "application/json";
    oss << jv;
    request.body = oss.str();
    // cout << oss.str() << endl;
    oss.str("");

    auto key = std::getenv("API_KEY");
    //auto url = format("https://generativelanguage.googleapis.com/v1beta3/models/text-bison-001:generateText?key={}", key);
    auto url = format("https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent?key={}", key);

    auto response = co_await client->await_post(url, request);
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

