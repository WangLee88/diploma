#include "converterjson.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

#define MANUAL 1

nlohmann::json GetJSON(std::string file) {
    nlohmann::json config;
    std::ifstream to_open;
    to_open.open(file);
    to_open >> config;
    to_open.close();
    return config;
}

std::string inputInfo(std::string in_dir) {
    std::string content;
    std::ifstream in_to_add;
    in_to_add.open(in_dir);
    char* app = new char[50];
    while(!in_to_add.eof()){
        in_to_add.read(app, 50);
        content.append(app, in_to_add.gcount());
    }
    delete[] app;
    in_to_add.close();
    return content;
}

std::vector<std::string> ConverterJSON::GetTextDocuments(){
    nlohmann::json config = GetJSON("../../../config/config.json");
    std::vector<std::string> content;
    std::string dir, prefix = "../../";
    for (auto it = config["files"].begin();it != config["files"].end(); it++){
        dir = prefix + (std::string)(*it);
        content.push_back(inputInfo(dir));
    }
    return content;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    nlohmann::json req = GetJSON("../../../config/Requests.json");
    std::vector<std::string> requests;
    std::string dir;
    for (auto it = req["requests"].begin();it != req["requests"].end(); it++){
        requests.push_back(*it);
    }
    return requests;
}

int ConverterJSON::GetResponsesLimit(){
    nlohmann::json config = GetJSON("../../../config/config.json");
    return config["config"]["max_responses"];
}

std::vector<std::vector<std::pair<int, float>>> ConverterJSON::find(){
#if MANUAL
    return {
        {
            {0, 0.25},
            {3, 0.89}
        },
        {},
        {
            {1, 0.965},
            {2, 0.999}
        }
    };
#elif !MANUAL

    return {};

#endif
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers){
    nlohmann::json answers_to_put;
    for (int i = 0; i < answers.size(); i++) {
        std::string name = "request";
        std::string num = std::to_string(i+1);
        for (int i = 0; i < (3 - num.length()); i++) name+= "0";
        name += num;
        if (answers[i].empty()) answers_to_put["answers"][name]["result"] = false;
        else {
            answers_to_put["answers"][name]["result"] = true;
            for (int j = 0; j < answers[i].size(); j++) {
                answers_to_put["answers"][name]["relevance"] = {"docid", answers[i][j].first, "rank" , answers[i][j].second};
            }
        }
    }

    std::ofstream testPut("../../../config/answers2.json");
    testPut << answers_to_put;
}
