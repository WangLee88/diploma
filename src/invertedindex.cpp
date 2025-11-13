#include <invertedindex.h>
#include <sstream>
#include <iostream>
#include <thread>
#include <mutex>
#define FORDEBUGGING

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
#ifdef FORDEBUGGING
                logMutex.lock();
                std::clog << "[DEBUG] Thread " << std::this_thread::get_id()<< ", Inserted new word: \"" << currentWord << "\", {" << freq_dictionary[currentWord][freq_dictionary[currentWord].size()-1].doc_id << ", " << freq_dictionary[currentWord][freq_dictionary[currentWord].size()-1].count << "}"<< std::endl;
                logMutex.unlock();
#endif
                docInsertedWordsMap[currentWord] = true;
            }
            else if (docInsertedWordsMap[currentWord] == false) {
                //Здесь нужен будет мьютекс, чтобы
                freq_dictionary[currentWord].push_back(Entry{(std::size_t) in_docid, 1});
#ifdef FORDEBUGGING
                logMutex.lock();
                std::clog << "[DEBUG] Thread " << std::this_thread::get_id()<< ", Inserted new word: \"" << currentWord << "\", {" << freq_dictionary[currentWord][freq_dictionary[currentWord].size()-1].doc_id << ", " << freq_dictionary[currentWord][freq_dictionary[currentWord].size()-1].count << "}"<< std::endl;
                logMutex.unlock();
#endif
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
#ifdef FORDEBUGGING
                for (int i = 0; i < freq_dictionary[currentWord].size(); i++){
                    if (freq_dictionary[currentWord][i].doc_id == in_docid) {
                        logMutex.lock();
                        std::clog << "[DEBUG] Thread " << std::this_thread::get_id()<< ", " <<  currentWord << "\" increased, {" << freq_dictionary[currentWord][in_docid].doc_id << ", " << freq_dictionary[currentWord][in_docid].count << "}"<< std::endl;
                        logMutex.unlock();
                    }
                }
#endif
            }
            mapLock.unlock();
        }

#ifdef FORDEBUGGING
        logMutex.lock();
        std::clog << "[DEBUG] Thread " << std::this_thread::get_id() << ", " << currentWord << std::endl;
        logMutex.unlock();
#endif
    }
#ifdef FORDEBUGGING
    logMutex.lock();
    std::clog << "\n[DEBUG] Thread " << std::this_thread::get_id() <<",*******************\n" <<std::endl;
    logMutex.unlock();
#endif
}

void InvertedIndex::UpdateFreq_Dictionary(std::vector<std::string> in_docs){ // Доделать в разных потоках.
    std::vector<std::thread*> threads;
    for (int i = 0; i < docs.size(); i++) {
        std::string currentDoc = docs[i];
        std::thread* t = new std::thread(&InvertedIndex::Insertion, this, currentDoc, i);
        threads.push_back(t);
    }
    for (int i = 0; i < threads.size(); i++) threads[i]->join();
    for (int i = 0; i < threads.size(); i++) delete threads[i];

#ifdef FORDEBUGGING
    std::cout << "*** RESULTS ***" << std::endl;
    for (auto it = freq_dictionary.begin(); it != freq_dictionary.end(); it++) {
        std::cout << "Word \"" << it->first << "\": " << std::endl;
        for (int i = 0; i < it->second.size(); i++) {
            std::cout << "{" << it->second[i].doc_id << ", " << it->second[i].count << "}" << std::endl;
        }
        std::cout << std::endl;
    }
#endif
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    std::vector<Entry> vec{};
    return vec;
}
