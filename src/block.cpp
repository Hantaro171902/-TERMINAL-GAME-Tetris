#include "block.hpp"
#include "color.hpp"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

TextColor getColor(char ch) {
    switch (ch) {
        case '#': return YELLOW;   // O block
        case 'X': return MAGENTA;  // T block
        case '@': return CYAN;     // I block
        case 'O': return BLUE;     // L block
        case '&': return GREEN;    // J block
        case 'Z': return RED;      // S block
        case 'S': return GRAY;    // Z block
        default:  return WHITE;    // fallback
    }
}


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
            " ZZ" }, 3, 2), // Z block
    
    Block({ " SS",
            "SS " }, 3, 2) // S block

};


