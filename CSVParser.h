#pragma once

#include <fstream>
#include <tuple>
#include <vector>
#include <string>
#include "parsers/CSVIntParser.h"
#include "parsers/CSVStringParser.h"

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
        return os << val << " " ;
    }
    static std::ostream &printTuple(std::ostream &os, std::string val, std::tuple<Args...> t) {
        return os << val << "\" ";
    }
};

template<class... Args>
std::ostream& operator<<(std::ostream&os, std::tuple<Args...> const &t) {
    return TuplePrinter<sizeof...(Args) - 1, Args...>::printTuple(os, std::get<sizeof...(Args) - 1>(t), t);
}

template<>
std::ostream& operator<<(std::ostream&os, std::tuple<> const &t) {
    return os;
}

template<int N, int size, typename Tuple, typename A, typename ... Types>
class CSVParserRead;



template<typename ... Types>
class CSVParser {
    std::ifstream &file;
    void readLine(std::vector<std::string>& strings) {
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
    }
public:
    class iterator;

    friend class iterator;

    explicit CSVParser(std::ifstream &file) : file(file) {
        if (!file.is_open()) {
            throw std::invalid_argument("Closed file");
        }
    }

    CSVParser(std::ifstream &file, int skip) : file(file) {
        if (!file.is_open()) {
            throw std::invalid_argument("Closed file");
        }
        for (int i = 0; i < skip; i++) {
            readLine();
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

    std::tuple<Types...> readLine(void) {
        std::vector<std::string> strings;
        readLine(strings);
        std::tuple<Types...> t;
        CSVParserRead<sizeof...(Types), sizeof...(Types) , std::tuple<Types...>, Types...>::readLine(strings, t);
        return t;
    }

    class iterator {
    private:
        friend CSVParser<Types...>;

        bool isEnd = false;
        CSVParser &parser;
        std::tuple<Types...> value;
    public:

        explicit iterator(CSVParser<Types...> &parser, bool isEnd) : parser(parser), isEnd(isEnd) {
            this->isEnd |= parser.isEnd();
            ++(*this);
        }

        bool operator==(CSVParser<Types...>::iterator const &it) {
            if (it.isEnd == isEnd && &(it.parser) == &(this->parser))
                return true;
            return false;
        }

        bool operator!=(CSVParser<Types...>::iterator const &it) {
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

        std::tuple<Types...> operator*() {
            return value;
        }
    };

};

template<>
class CSVParser<> {
    std::ifstream &file;
    void readLine(std::vector<std::string>& strings) {
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
    }
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

    std::tuple<> readLine(void) {
        std::vector<std::string> strings;
        readLine(strings);
        return {};
    }

    class iterator {
    private:
        friend CSVParser<>;

        bool isEnd = false;
        CSVParser &parser;
        std::tuple<> value;
    public:

        explicit iterator(CSVParser<> &parser, bool isEnd) : parser(parser), isEnd(isEnd) {
            this->isEnd |= parser.isEnd();
            ++(*this);
        }

        bool operator==(CSVParser<>::iterator const &it) {
            if (it.isEnd == isEnd && &(it.parser) == &(this->parser))
                return true;
            return false;
        }

        bool operator!=(CSVParser<>::iterator const &it) {
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

        std::tuple<> operator*() {
            return value;
        }
    };

};
