#include <iostream>
#include <nlohmann/json.hpp>
#include <converterjson.h>
#include <invertedindex.h>
#include <searchserver.h>

#define FORDEBUGGING


std::vector<std::vector<std::pair<int, float>>> answersConverter(std::vector<std::vector<RelativeIndex>> in_answers) {



    std::vector<std::vector<std::pair<int, float>>> result;
    for (int i = 0; i < in_answers.size(); i++) {
        std::vector<std::pair<int, float>> currentAnswer;
        for (int j = 0; j < in_answers[i].size(); j++) {
            std::pair<int, float> currentPair = std::make_pair((int) in_answers[i][j].doc_id, in_answers[i][j].rank);
            currentAnswer.push_back(currentPair);
        }
        result.push_back(currentAnswer);
    }

#ifdef FORDEBUGGING
    std::cout << "New vector" << std::endl;
    for (int i = 0; i < result.size(); i++) {
        std::cout << "I: " << i << std::endl;
        for (int j = 0; j < result[i].size(); j++){
            std::cout << "\tJ: " << j << std::endl;
            std::cout << "\t\t" << result[i][j].first << ", " << result[i][j].second << std::endl;
        }
    }
#endif

    return result;
}

int main(int argc, char *argv[])
{
    ConverterJSON json;     // Итак, этот класс только берёт из json-ов доки,
                            // берёт из json-ов запросы и берёт из json-ов кол-во ответов,
                            // но внутри себя ничего не хранит.
                            // Потом этот класс кладёт ответы в JSON-файл.
    InvertedIndex iIndex;   // Этот класс делает "предварительно заготовленную информацию" -
                            // частотный словарь, с которым ускорится поиск ответов.
                            // Внутри себя он хранит текст документов и сам частотный словарь.
    iIndex.UpdateDocumentBase(json.GetTextDocuments()); // Вот конвертер передал значение документов,
        // а индекс на их основе сделал частотный словарь.
    SearchServer sServer(iIndex);   // Это, собственно, поисковый движок.
                                    // Ему передаётся ссылка на индекс.
                                    // Далее он должен будет при помощи этого индекса произвести поиска.
    std::vector<std::string> requests = json.GetRequests(); // Это вектор запросов.
                                                            // Поскольку ни в одном из классов он не хранится,
                                                            // а в движок он передаётся по ссылке,
                                                            // придётся отдельно его сохранить здесь.
    auto answers = sServer.search(requests);    // Такс, здесь нюанс - сюда возвращается вектор векторов структур RelativeIndex,
                                                // в то время как конвертер принимает вектор векторов пар int float.
                                                // И кстати, в RelativeIndex тип док-айдишки не int, а size_t.
                                                // Для чистоты работы, нужно будет сделать функцию-конвертер.

    json.putAnswers(answersConverter(answers));


    return 0;
}
