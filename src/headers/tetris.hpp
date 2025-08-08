#pragma once
#include <vector>
#include <string>
#include "block.hpp"
#include <iostream>
#include <chrono>

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
    std::chrono::steady_clock::time_point startTime; // Start time for the game
    int w, h, level, score, x, y;
    bool game_over;
    std::vector<std::vector<char>> board;
    Block current;
    Block preview_block;

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

    //preview block
    void draw_preview_frame();
    void draw_preview_block(int x, int y);
    void generate_preview_block();

};