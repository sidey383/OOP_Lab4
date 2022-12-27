#include <tuple>
#include <vector>
#include <string>

template<int N, int size, typename Tuple, typename A, typename ... Types>
class CSVParserRead;

template<int N, int size, typename Tuple, typename ... Types>
class CSVParserRead<N, size, Tuple, std::string, Types...> {
public:

    static void readLine(std::vector<std::string> &data, Tuple &tuple) {
        std::string val("");
        if (size - N < data.size()) {
            val = data[size - N];
        }
        std::get<size - N>(tuple) = val;
        CSVParserRead<N-1, size, Tuple, Types...>::readLine(data, tuple);
    }

};

template<int size, typename Tuple, typename ... Types>
class CSVParserRead<1, size, Tuple, std::string, Types...> {
public:

    static void readLine(std::vector<std::string> &data, Tuple &tuple) {
        std::string val("");
        if (!data.empty()) {
            val = data[size - 1];
        }
        std::get<size - 1>(tuple) = val;
    }

};
