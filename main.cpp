#include <iostream>
#include "CSVParser.h"

template<int N, typename ... Args>
struct TuplePrinter {
    static std::ostream &printTuple(std::ostream &os, int val,  std::tuple<Args...> t) {
        return TuplePrinter<N - 1, Args...>::printTuple(os, std::get<N-1>(t), t) << val << " ";
    }
    static std::ostream &printTuple(std::ostream &os, std::string val, std::tuple<Args...> t) {
        return TuplePrinter<N - 1, Args...>::printTuple(os, std::get<N-1>(t), t) << "\"" << std::get<N-1>(t) << "\" ";
    }
    static void print(std::ostream &os, std::string str) {
        os<<str;
    }
};

template<typename ... Args>
struct TuplePrinter<0, Args...> {
    static std::ostream &printTuple(std::ostream &os, int val,  std::tuple<Args...> t) {
        return os << "( " << val << " " ;
    }
    static std::ostream &printTuple(std::ostream &os, std::string val, std::tuple<Args...> t) {
        return os << "( \"" << val << "\" ";
    }
};

template<class... Args>
std::ostream& operator<<(std::ostream&os, std::tuple<Args...> const &t) {
    return TuplePrinter<sizeof...(Args) - 1, Args...>::printTuple(os, std::get<sizeof...(Args) - 1>(t), t) << ")";
}

int main() {
    std::ifstream file("table.csv");
    CSVParser<int, int, int> parser(file);
    for (auto t : parser) {
        std::cout<<t;
    }
    return 0;
}