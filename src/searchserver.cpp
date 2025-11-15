#include <searchserver.h>
#include <sstream>
#define FORDEBUGGING
#ifdef FORDEBUGGING
#include <iostream>
#endif


std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> resultRelativeIndexesVec;
    // Это контейнер для возвращаемого значения.
    // Начинаем обрабатывать входной вектор запросов.

    for (int i = 0; i < queries_input.size(); i++) { // каждый запрос проходит через верхний икл.
        std::stringstream currentQuery; // Буфер для хранения текущего запроса.
        currentQuery << queries_input[i]; // Текущий (i-й) запрос вывели в строковый буфер.

        std::string currentWord; // Локальная переменная для каждого слова в запросе

        // А вот дальше я сам не понял, что я пытался сделать.
        // По-идее, для начала через частотный словарь посмотреть, которое из слов запроса встречается реже всего,
        // причём под "реже всего"... Вот здесь я ошибся.
        // Под "Реже всего подразумевается" не "абсолютное наименьшее число повторений слова во всех документах итого",
        // а просто "встречается в наименьшем количестве документов".

        // Сначала я пробовал использовать словарь. Попробую на сей раз вектор. Сначала из всех слов, потом отсортирую.
        // Хотя... Можно сразу запускать поиск в частотном словаре.
        // А чтобы не делать это лишний раз для слов в запросе, которые встречаются более одного раза,
        // я как раз воспользуюсь словарём.

        std::map<std::string, int> searchedFlags; // Вот словарь для пометки уже проверенных по частотному словарю слов.
        std::vector<std::vector<Entry>> currentWordEntries; // Сюда буду векторы вхождения слов сохранять.
        while(!currentQuery.eof()){ // Цикл для обработки текущего (i-го) запроса.
            currentQuery >> currentWord; // Вводим из строкового буфера каждое слово.


            if (currentWord != "") { // Проверка на случай, если были лишние пробелы или считался конец строки

                if (searchedFlags.find(currentWord) == searchedFlags.end()) {


                    searchedFlags[currentWord] = true;
                    // Вот и проверили - если слово встречается впервые, помечаем, что провели поиск.

                    // Такс, ну теперь можно прямо из частотного словаря брать вектор вхождений и сохранить его вектор.

                    currentWordEntries.push_back(_index.GetWordCount(currentWord));
                    // Всё! Сохранили вектор вхождений в вектор векторов вхождений.
                }
                // Ответвлений не требуется - если слово уже было найдено в словаре флагов, то ничего делать не нужно.
            }

        }

        // Вот к этому моменту по текущему (i-тому) запросу по каждому слову в векторе currentWordEntries должны были
        // сохраниться вектора Entry.
        // Теперь нужно отсортировать вектор векторов. Можно пузырьком с конца.

        for (int j = currentWordEntries.size() - 1; j > 0; j--) {

            if (!currentWordEntries[j].empty()) {
                if (currentWordEntries[j - 1].empty() || currentWordEntries[j].size() < currentWordEntries[j - 1].size()) {
                    std::vector<Entry> temp = currentWordEntries[j];
                    currentWordEntries[j] = currentWordEntries[j - 1];
                    currentWordEntries[j - 1] = currentWordEntries[j];

                }
            }

        }


        // Теперь currentWordEntries отсортирован по частоте слов.
        // Первый элемент - самый маленький. При этом пустые - в конце.

        // Пора производить индексирование (то есть создавать вектор индексов релевантности).

        // Важно! Сами слова здесь уже не так важны.
        // Мы итак сохранили вхождения этих слов.
        // Этих данных достаточно, чтобы посчитать релевантность запроса как такового.

        // Важно понять задание - по-видимому, если нет ни одного документа, в котором встречаются все слова запроса, то в ответ записывается false；


        // Как рассчитывается релевантность?
        // Шаг первый - определить перечень документов, в которых встречается не только самое редкое слово.
        // При этом должен быть хотя бы один документ, в котором встречаются все слова.
        // Шаг второй - посчитать по этим документам абсолютную релевантность,
        // паралельно выявить максимальную абсолютную релевантность.
        // Шаг третий - рассчитать относительную релевантность для каждого документа.

        // Попробую самый примитивный способ: пройдусь циклом по всему вектору векторов,
        // сплошняком, и паралельно брать док-айдишки, по каждой считать абсолютную релевантность.
        // Но если выяснится, что в документе только одно слово встречается -
        // то в вектор релевантностей сохранять пустое значение.
        std::vector<size_t> relevantDoc_ids; // Сюда

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
        // Если нет ни одного документа, в котором встречаются все слова,
        // то в результирующий вектор векторов сохраняем пустой вектор.

// ВОТ ЗДЕСЬ ОШИБКА, НАДО ЗДЕСЬ ПЕРЕПРОВЕРИТЬ ВСЕ ЭНТРИСЫ, А ПОТОМ ОСТАВШИЙСЯ КОД.




        else {
            // И вот самое интересное. Если размер не 0, то будем считать релевантность.
#ifdef FORDEBUGGING
            std::cout << "I: " << i << std::endl;
            for (int m = 0; m < currentWordEntries.size(); m++){
                std::cout << "\tM (word): " << m << std::endl;
                for (int n = 0; n < currentWordEntries[m].size(); n++) {
                    std::cout << "\t\tN (entry): " << n
                              << ", DocID: " << currentWordEntries[m][n].doc_id
                              << ", count: " << currentWordEntries[m][n].count << std::endl;
                }
            }

#endif
            std::map<size_t, int> temp; // Потом преобразую в вектор.
            int max = 0;
            for (int j = 0; j < currentWordEntries.size(); j++) {
                for (int k = 0; k < currentWordEntries[j].size(); k++) {
                    if (temp.find(currentWordEntries[j][k].doc_id) == temp.end()) {
                        temp.insert(std::make_pair(currentWordEntries[j][k].doc_id, currentWordEntries[j][k].count));
                    }
                    else {
                        temp[currentWordEntries[j][k].doc_id]+= currentWordEntries[j][k].count;

                    }
                    if (temp[currentWordEntries[j][k].doc_id] > max) max = temp[currentWordEntries[j][k].doc_id];
                }
            }



            std::vector<RelativeIndex> currentReqResult;
            for (auto it = temp.begin(); it != temp.end(); it++) {
                RelativeIndex currentRI = {it->first, (float) it->second / max};
                currentReqResult.push_back(currentRI);
            }




            // Осталось отсортировать пузырьком.

            // Так, ну словарь вроде готов.
            // Осталось преобразовать его в вектор, отсортировать и добавить в результирующий вектор векторов.
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

