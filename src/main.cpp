#include <nlohmann/json.hpp>
#include <converterjson.h>
#include <invertedindex.h>
#include <searchserver.h>

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
    return result;
}

int main(int argc, char *argv[]){
    ConverterJSON json;
    InvertedIndex iIndex;
    iIndex.UpdateDocumentBase(json.GetTextDocuments());
    SearchServer sServer(iIndex);
    std::vector<std::string> requests = json.GetRequests();
    auto answers = sServer.search(requests);
    json.putAnswers(answersConverter(answers));
    return 0;
}
