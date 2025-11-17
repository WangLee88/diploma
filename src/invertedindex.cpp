#include <invertedindex.h>
#include <sstream>
#include <thread>
#include <mutex>

std::mutex mapLock;
std::mutex logMutex;

std::vector<std::string> InvertedIndex::getDocs(){
    return docs;
}

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    docs = input_docs;
    UpdateFreq_Dictionary(docs);
}

void InvertedIndex::Insertion(std::string in_doc, int in_docid){ // Если успею, сделать словарь мьютексов, чтобы к каждому слову создавать отдельный мьютекс.
    std::stringstream buffer;
    buffer << in_doc;
    std::map<std::string, bool> docInsertedWordsMap;
    while(!buffer.eof()) {
        std::string currentWord;
        buffer >> currentWord;
        if (!currentWord.empty()) {

            if (docInsertedWordsMap.find(currentWord) == docInsertedWordsMap.end()) {
                docInsertedWordsMap.insert(std::pair<std::string, bool>(currentWord, false));
            }
            mapLock.lock();
            if(freq_dictionary.find(currentWord) == freq_dictionary.end()) {
                Entry currentEntry{(std::size_t) in_docid, 1};
                std::vector<Entry> currentElement{currentEntry};
                freq_dictionary.insert(std::pair<std::string, std::vector<Entry>>(currentWord, currentElement));
                docInsertedWordsMap[currentWord] = true;
            }
            else if (docInsertedWordsMap[currentWord] == false) {
                freq_dictionary[currentWord].push_back(Entry{(std::size_t) in_docid, 1});
                for (int k = 0; k < (freq_dictionary[currentWord].size() - 1); k++) {
                    for (int l = 0; l < (freq_dictionary[currentWord].size() - 1 - k); l++) {
                        if (freq_dictionary[currentWord][l].doc_id > freq_dictionary[currentWord][l+1].doc_id) {
                            Entry temp = freq_dictionary[currentWord][l];
                            freq_dictionary[currentWord][l] = freq_dictionary[currentWord][l+1];
                            freq_dictionary[currentWord][l+1] = temp;
                        }
                    }
                }
                docInsertedWordsMap[currentWord] = true;
            }
            else {
                for (int i = 0; i < freq_dictionary[currentWord].size(); i++){
                    if (freq_dictionary[currentWord][i].doc_id == in_docid) freq_dictionary[currentWord][i].count++;
                }
            }
            mapLock.unlock();
        }
    }
}

void InvertedIndex::UpdateFreq_Dictionary(std::vector<std::string> in_docs){
    std::vector<std::thread*> threads;
    for (int i = 0; i < docs.size(); i++) {
        std::string currentDoc = docs[i];
        std::thread* t = new std::thread(&InvertedIndex::Insertion, this, currentDoc, i);
        threads.push_back(t);
    }
    for (int i = 0; i < threads.size(); i++) threads[i]->join();
    for (int i = 0; i < threads.size(); i++) delete threads[i];
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    if (freq_dictionary.find(word) == freq_dictionary.end()) return std::vector<Entry>{};
    else return freq_dictionary[word];
}
