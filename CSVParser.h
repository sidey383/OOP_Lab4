#pragma once
#include <fstream>
#include <tuple>
#include <iostream>
#include <tuple>

#define BUF_SIZE 4096

static bool readNext(std::ifstream &file, char *buffer, unsigned int *pose) {
    (*pose) = 0;
    file.read(buffer, BUF_SIZE);
    return file.peek() == EOF;
}

static int readInt(std::ifstream &file, char *buffer, unsigned int *pose, bool* isEnd) {
    int i = 0;
    while(buffer[*pose] != ',') {
        if (buffer[*pose] == '\n') {
            (*isEnd) = true;
            break;
        }
        if (std::isdigit(buffer[*pose])) {
            i += i*10 + buffer[*pose] - '0';
        }
        if(*pose + 1 == BUF_SIZE) {
            if (readNext(file, buffer, pose)) {
                (*isEnd) = true;
                break;
            }
        }
    }
    return i;
}

template<int N, typename A, typename ... Types>
class CSVParserRead {
public:

    static std::tuple<A, Types...> readLine(std::ifstream &file, char *buffer, unsigned int *pose, bool* isEnd) {
        if(std::is_same<A, int>::value) {
            if((*isEnd)) {
                return std::tuple<A, Types...>(0 , CSVParserRead<N-1, Types...>::readLine(file, buffer, pose, isEnd));
            }
            return std::tuple<A, Types...>(readInt(file, buffer, pose, isEnd) , CSVParserRead<N-1, Types...>::readLine(file, buffer, pose, isEnd));
        }
    }

};

template<typename A, typename ... Types>
class CSVParserRead<1, A, Types...> {
public:

    static std::tuple<A, Types...> readLine(std::ifstream &file, char *buffer, unsigned int *pose, bool* isEnd) {
        if(std::is_same<A, int>::value) {
            if((*isEnd)) {
                return std::tuple<A, Types...>(0);
            }
            return std::tuple<std::string, A, Types...>(readInt(file, buffer, pose, isEnd));
        }
    }

};

template<typename A, typename ... Types>
class CSVParser {
    std::ifstream& file;
    char buffer[BUF_SIZE];
    unsigned int pose;
public:
    class iterator;
    friend class iterator;

    explicit CSVParser(std::ifstream& file) : file(file) {
        if(!file.is_open()) {
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
        return file.peek() == EOF;
    }

    std::tuple<A, Types...> readLine(void) {
        bool isEnd[1];
        isEnd[0] = file.peek() == EOF;
        return CSVParserRead<sizeof...(Types), A, Types...>::readLine(file, buffer, &pose, isEnd);
    }

    class iterator {
    private:
        friend CSVParser<A, Types...>;

        bool isEnd = false;
        CSVParser& parser;
        std::tuple<A, Types...> value;
    public:

        explicit iterator(CSVParser<A, Types...>& parser, bool isEnd) : parser(parser), isEnd(isEnd) {
            this->isEnd |= parser.isEnd();
        }

        bool operator==(CSVParser<A, Types...>::iterator const &it) {
            if(it.isEnd == isEnd && &(it.parser) == &(this->parser))
                return true;
            return false;
        }

        bool operator!= (CSVParser<A, Types...>::iterator const &it) {
            if(it.isEnd == isEnd && &(it.parser) == &(this->parser))
                return false;
            return true;
        }

        iterator& operator++ () {
            if (isEnd)
                return *this;
            if (parser.isEnd())
                isEnd = true;
            value = parser.readLine();
            return *this;
        }

        std::tuple<A, Types...> operator* () {
            return value;
        }
    };

};
