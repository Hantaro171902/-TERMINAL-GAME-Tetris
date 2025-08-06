#include "block.hpp"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

// Define the list of all standard Tetris blocks
const vector<Block> BLOCK_SET = {
    Block({ "##",
            "##" }, 2, 2), // O block

    Block({ " X ",
            "XXX" }, 3, 2), // T block

    Block({ "@@@@" }, 4, 1), // I block

    Block({ "OO",
            "O ",
            "O " }, 2, 3), // L block

    Block({ "&&",
            " &",
            " &" }, 2, 3), // J block

    Block({ "ZZ ",
            " ZZ" }, 3, 2) // S block
};
