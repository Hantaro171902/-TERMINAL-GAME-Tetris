#pragma once 
#include <vector>
#include <string>
#include <iostream>
#include "color.hpp"
struct Block {
    std::vector<std::string> data;
    int w, h; // Width and height of the block

    Block() : w(0), h(0) {}
    Block(std::vector<std::string> shape, int width, int height) 
        : data(std::move(shape)), w(width), h(height) {}
};

extern const std::vector<Block> BLOCK_SET;

TextColor getColor(char ch);

void generate_new_block();