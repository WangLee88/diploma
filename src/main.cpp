#include <iostream>
#include <nlohmann/json.hpp>
#include <converterjson.h>

int main(int argc, char *argv[])
{
    ConverterJSON json;

    std::vector<std::string> content = json.GetTextDocuments();


    for (int i = 0; i < content.size(); i++) std::cout << "Text No." << i + 1 << ": \n" << content[i] << "\n" << std::endl;
    int test = json.GetResponsesLimit();
    std::cout << "Max responces = " << test << std::endl;
    std::vector<std::string> requests = json.GetRequests();
    for (int i = 0; i < requests.size(); i++) std::cout << "Request No." << i + 1 << ": \n" << requests[i] << "\n" << std::endl;
    // std::vector<std::vector<std::pair<int, float>>> testAnswers;

    auto testAnswers = json.find(); //ФУНКЦИЯ ТЕСТОВАЯ, ЕЩЁ НЕ ДОРАБОТАНА.

    json.putAnswers(testAnswers);


    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.



    return 0;
}
