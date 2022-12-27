#include <iostream>
#include "CSVParser.h"

int main() {
    std::ifstream file("table.csv");

    CSVParser<int, std::string, int, std::string> parser(file, 2);
    for (auto t : parser) {
        std::cout<<t<<std::endl;
    }
    return 0;
}