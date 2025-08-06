#pragma once
#include <vector>
#include <string>
#include "block.hpp"

// struct Block {
//     std::vector<std::string> data;
//     int w, h;
// };

class Tetris {

public:
    Tetris(int width, int height);
    ~Tetris();

    void run();
private:
    int w, h, level, score, x, y;
    bool game_over;
    std ::vector<std::vector<char>> board;
    Block current;

    void init_game();
    void new_block();
    void print_block();
    void draw_board();
    void block_rotate();
    void block_gravity();
    void block_fall(int row);
    bool hit_wall();
    void check_lines();
    int level_speed();
};