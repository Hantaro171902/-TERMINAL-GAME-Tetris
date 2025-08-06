#include <iostream>
#include "tetris.hpp"
#include "block.hpp"

using namespace std;

int main() {
    Tetris game(10, 20);
    game.run();
    return 0;
}