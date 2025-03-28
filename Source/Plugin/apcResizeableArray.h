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
    // Default constructor: initializes an empty array with zero dimensions
    apcResizeableArray() : cols(0), rows(0) {}

    // Parameterized constructor
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
        return nullptr;
    }
};


#if 0
void test_apcResizeableArray()
{// Create a 3x3 Tic-Tac-Toe board with default value ' '
    apcResizeableArray<char> board(3, 3, ' ');

    // Assign names to key positions
    board.assignName("top-left", 0, 0);
    board.assignName("center", 1, 1);
    board.assignName("bottom-right", 2, 2);

    // Place some moves
    board.at("top-left") = 'X';
    board.at("center") = 'O';
    board.at(2, 0) = 'X'; // Access by indices

    // Print the board
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            std::cout << board.at(x, y) << " ";
        }
        std::cout << std::endl;
    }
}
#endif

#endif // APCRESIZEABLEARRAY_H
