#include <iostream>
#include <nlohmann/json.hpp>
#include <converterjson.h>
#include <invertedindex.h>
#include <search.h>

int main(int argc, char *argv[])
{
    ConverterJSON json;
    InvertedIndex iIndex;
    iIndex.UpdateDocumentBase(json.GetTextDocuments());


    std::vector<std::string> requests = json.GetRequests();






    return 0;
}
