#include <tuple>
#include <vector>
#include <string>

template<int N, int size, typename Tuple, typename A, typename ... Types>
class CSVParserRead;

template<int N, int size, typename Tuple, typename ... Types>
class CSVParserRead<N, size, Tuple, int, Types...> {
public:

    static void readLine(std::vector<std::string> &data, Tuple &tuple) {
        int val = 0;
        if (size - N < data.size()) {
            try {
                val = std::stoi(data[size - N]);
            } catch (...) {}
        }
        std::get<size - N>(tuple) = val;
        CSVParserRead<N-1, size, Tuple, Types...>::readLine(data, tuple);
    }

};

template<int size, typename Tuple, typename ... Types>
class CSVParserRead<1, size, Tuple, int, Types...> {
public:

    static void readLine(std::vector<std::string> &data, Tuple &tuple) {
        int val = 0;
        if (!data.empty()) {
            try {
                val = std::stoi(data[size - 1]);
            } catch (...) {}
        }
        std::get<size-1>(tuple) = val;
    }

};
