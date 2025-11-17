#include <searchserver.h>
#include <sstream>



std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> resultRelativeIndexesVec;
    for (int i = 0; i < queries_input.size(); i++) {
        std::stringstream currentQuery;
        currentQuery << queries_input[i];
        std::string currentWord;
        std::map<std::string, int> searchedFlags;
        std::vector<std::vector<Entry>> currentWordEntries;
        while(!currentQuery.eof()){
            currentQuery >> currentWord;
            if (currentWord != "") {
                if (searchedFlags.find(currentWord) == searchedFlags.end()) {
                    searchedFlags[currentWord] = true;
                    currentWordEntries.push_back(_index.GetWordCount(currentWord));
                }
            }
        }

        for (int j = 0; j < currentWordEntries.size() - 1; j++) {
            for (int k = 0; k < currentWordEntries.size() - 1 - j; k++) {
                if (currentWordEntries[k].size() > currentWordEntries[k+1].size()) {
                    auto tempEntry = currentWordEntries[k];
                    currentWordEntries[k] = currentWordEntries[k+1];
                    currentWordEntries[k+1] = tempEntry;
                }
            }
        }
        std::vector<size_t> relevantDoc_ids;
        for (int j = 0; j < currentWordEntries.size(); j++) {
            std::vector<size_t> temp;
            for (int k = 0; k < currentWordEntries[j].size(); k++){
                if (j == 0) temp.push_back(currentWordEntries[j][k].doc_id);
                else {
                    for (int l = 0; l < relevantDoc_ids.size(); l++) {
                        if (relevantDoc_ids[l] == currentWordEntries[j][k].doc_id) temp.push_back(relevantDoc_ids[l]);
                    }
                }
            }
            relevantDoc_ids = temp;
        }
        if (relevantDoc_ids.size() == 0) resultRelativeIndexesVec.push_back({});
        else {
            std::map<size_t, int> temp; // Потом преобразую в вектор.
            int max = 0;
            for (int j = 0; j < currentWordEntries.size(); j++) {
                for (int k = 0; k < currentWordEntries[j].size(); k++) {
                    if (temp.find(currentWordEntries[j][k].doc_id) == temp.end()) {
                        temp.insert(std::make_pair(currentWordEntries[j][k].doc_id, currentWordEntries[j][k].count));
                    }
                    else {

                        int n = temp[currentWordEntries[j][k].doc_id] + currentWordEntries[j][k].count;
                        temp[currentWordEntries[j][k].doc_id] = n;
                    }
                    if (temp[currentWordEntries[j][k].doc_id] > max) max = temp[currentWordEntries[j][k].doc_id];
                }
            }
            std::vector<RelativeIndex> currentReqResult;
            for (auto it = temp.begin(); it != temp.end(); it++) {
                RelativeIndex currentRI = {it->first, (float) it->second / max};
                currentReqResult.push_back(currentRI);
            }
            for (int j = 0; j < currentReqResult.size() - 1; j++) {
                for (int k = 0; k < currentReqResult.size() - 1 - j; k++) {
                    if (currentReqResult[k].rank < currentReqResult[k + 1].rank) {

                        auto tempRes = currentReqResult[k];
                        currentReqResult[k] = currentReqResult[k + 1];
                        currentReqResult[k + 1] = tempRes;
                    }
                }
            }
            if (currentReqResult.size() > 5) currentReqResult.resize(5);
            resultRelativeIndexesVec.push_back(currentReqResult);
        }
    }
    return resultRelativeIndexesVec;
}

