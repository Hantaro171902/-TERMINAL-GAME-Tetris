#pragma once
#include <vector>
#include <string>

struct Block {
    std::vector<std::string> data;
    int w, h;
};

class Tetris {

public:
    Tetris();
    ~Tetris();

    void start_game();
private:
    int w, h, level, score, x, y;
    bool game_over;
    std ::vector<std::vector<char>> board;
    Block current;

    void tetris_init();
    void new_block();
    void print_block();
    void draw_board();
    void block_rotate();
    void block_gravity();
    bool hit_test();
    void block_fall(int row);
    void check_lines();
    int level_speed();
}