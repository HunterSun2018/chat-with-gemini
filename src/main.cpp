#include <iostream>
#include <string>
#include <string_view>
#include <co_helper.hpp>
#include <co_http_client.hpp>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/json/src.hpp>   //inline boost::json
#include "conversation.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    try
    {
        cout << "Let's get started to chat with Google Gemini AI." << endl;

        bool is_debug = false;
        if (argc > 1 && argv[1] == string("debug=on"))
            is_debug = true;
        // auto prompt = argc > 1 ? argv[1] : "Write a story about a magic backpack";
        // cout << format("You : {}", prompt) << endl;

        // auto response = chat(prompt).get();

        // cout << format("Gemini : {}", response) << endl;

        auto client = co_http::Client::create();

        auto conversation = make_shared<Conversation>(client, is_debug);

        for (string prompt = readline("prompt : ");
             prompt != ".exit";
             prompt = readline("prompt : "))
        {
            if (prompt == ".reset")
            {
                 conversation->reset();
                 continue;
            }   

            auto response = conversation->chat(prompt).get();
            cout << "Gemini :\n"
                 << response << endl;

            add_history(prompt.c_str());
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception : " << e.what() << '\n';
    }

    return 0;
}