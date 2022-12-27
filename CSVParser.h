#pragma once

#include <fstream>
#include <tuple>
#include <iostream>
#include <vector>
#include <string>

template<int N, typename Tuple, typename A, typename ... Types>
class CSVParserRead {
public:

    static void readLine(std::vector<std::string>& data, Tuple& tuple) {
        if (std::is_same<A, int>::value) {
            if (N <= data.size()) {
                try {
                    std::get<N - 1>(tuple) = std::stoi(data[N - 1]);
                } catch (std::invalid_argument& e) {
                    std::get<N - 1>(tuple) = 0;
                }
            } else {
                std::get<N - 1>(tuple) = 0;
            }
        }
        CSVParserRead<N - 1, Tuple, Types...>::readLine(data, tuple);
    }

};

template<typename Tuple, typename A, typename ... Types>
class CSVParserRead<1, Tuple, A, Types...> {
public:

    static void readLine(std::vector<std::string>& data, Tuple& tuple) {
        if (std::is_same<A, int>::value) {
            if (data.empty()) {
                std::get<0>(tuple) = 0;
            } else {
                try {
                    std::get<0>(tuple) = std::stoi(data[0]);
                } catch (std::invalid_argument& e) {
                    std::get<0>(tuple) = 0;
                }
            }
        }
    }

};

template<typename A, typename ... Types>
class CSVParser {
    std::ifstream &file;
public:
    class iterator;

    friend class iterator;

    explicit CSVParser(std::ifstream &file) : file(file) {
        if (!file.is_open()) {
            throw std::invalid_argument("Closed file");
        }
    }

    iterator begin() {
        return iterator(*this, false);
    }

    iterator end() {
        return iterator(*this, true);
    }

    bool isEnd() {
        return file.eof();
    }

    std::tuple<A, Types...> readLine(void) {
        std::vector<std::string> strings;
        char ch;
        int i = 0;
        while (!file.get(ch).eof()) {
            strings.emplace_back("");
            bool isSeparated = ch == '"';
            bool hasFirstEnd = false;
            if (isSeparated) {
                if (file.get(ch).eof()) {
                    break;
                }
            }
            do {
                if (ch == '"') {
                    if (isSeparated) {
                        if (hasFirstEnd) {
                            hasFirstEnd = false;
                            strings[i] += ch;
                            continue;
                        } else {
                            hasFirstEnd = true;
                            continue;
                        }
                    } else {
                        strings[i]+=ch;
                    }
                    continue;
                } else {
                    if (hasFirstEnd)
                        break;
                }

                if (ch == ',') {
                    if (isSeparated) {
                        strings[i] += ch;
                    } else {
                        break;
                    }
                }
                if (ch == '\n') {
                    if (isSeparated) {
                        strings[i] += ch;
                    } else {
                        break;
                    }
                }
                strings[i] += ch;
            } while (!file.get(ch).eof());
            i++;
            if (ch == '\n')
                break;
        }
        std::tuple<A, Types...> t;
        CSVParserRead<sizeof...(Types) + 1, std::tuple<A, Types...>, A, Types...>::readLine(strings, t);
        return t;
    }

    class iterator {
    private:
        friend CSVParser<A, Types...>;

        bool isEnd = false;
        CSVParser &parser;
        std::tuple<A, Types...> value;
    public:

        explicit iterator(CSVParser<A, Types...> &parser, bool isEnd) : parser(parser), isEnd(isEnd) {
            this->isEnd |= parser.isEnd();
            ++(*this);
        }

        bool operator==(CSVParser<A, Types...>::iterator const &it) {
            if (it.isEnd == isEnd && &(it.parser) == &(this->parser))
                return true;
            return false;
        }

        bool operator!=(CSVParser<A, Types...>::iterator const &it) {
            if (it.isEnd == isEnd && &(it.parser) == &(this->parser))
                return false;
            return true;
        }

        iterator &operator++() {
            if (isEnd)
                return *this;
            if (parser.isEnd())
                isEnd = true;
            value = parser.readLine();
            return *this;
        }

        std::tuple<A, Types...> operator*() {
            return value;
        }
    };

};
