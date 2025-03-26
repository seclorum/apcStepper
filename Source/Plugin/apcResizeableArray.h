#ifndef APCRESIZEABLEARRAY_H
#define APCRESIZEABLEARRAY_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

template<typename T>
class apcResizeableArray {
private:
    std::vector<std::vector<T>> data;
    std::unordered_map<std::string, std::pair<int, int>> nameToIndex;
    int cols, rows;

public:
    apcResizeableArray(int initialCols, int initialRows, T defaultValue = T())
        : cols(initialCols), rows(initialRows) {
        data.resize(rows, std::vector<T>(cols, defaultValue));
    }

    // Resize the array
    void resize(int newCols, int newRows, T defaultValue = T()) {
        data.resize(newRows, std::vector<T>(newCols, defaultValue));
        cols = newCols;
        rows = newRows;
    }

    // Access by integer indices
    T& at(int x, int y) {
        return data[y][x];
    }

    // Assign a string to a specific (x, y)
    void assignName(const std::string& name, int x, int y) {
        nameToIndex[name] = {x, y};
    }

    // Access by string
    T& at(const std::string& name) {
        auto it = nameToIndex.find(name);
        if (it != nameToIndex.end()) {
            return data[it->second.second][it->second.first];
        }
        throw std::out_of_range("Key not found");
    }
};

#endif // APCRESIZEABLEARRAY_H