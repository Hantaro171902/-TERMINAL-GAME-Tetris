#include "tetris.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <memory>

using namespace std;

namespace {
    termios saved_termios;

    void set_non_blocking_input() {
        termios t;
        tcgetattr(STDIN_FILENO, &saved_termios);
        t = saved_termios;
        t.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // Set non-blocking mode
    }
    
    void reset_input() {
        tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios); // Restore original terminal
    }

    const vector<Block> blocks = {
        {{"##", "##"}, 2, 2},
        {{" X ", "XXX"}, 3, 2},
        {{"@@@@"}, 4, 1},
        {{"OO", "O ", "O "}, 2, 3},
        {{"&&", " &", " &"}, 2, 3},
        {{"ZZ ", " ZZ"}, 3, 2}
    };
}

Tetris::Tetris(int width, int height) : w(width), h(height), level(1), score(0), x(0), y(0), game_over(false) {
    board.resize(h, vector<char>(w, ' '));
    srand(time(nullptr));
}

Tetris::~Tetris() {
    reset_input();
}

void Tetris::init_game() {
    x = w / 2;
    y = 0;
}

void Tetris::new_block() {
    current = blocks[rand() % blocks.size()];
    x = w / 2 - current.w / 2;
    y = 0;
    if (hit_wall()) {
        game_over = true; 
    }
}

void Tetris::print_block() {
    for (int i = 0; i < current.w; i++) {
        for (int j = 0; j < current.h; j++) {
            char c = current.data[j][i];
            if (c != ' ') {
                board[y + j][x + i] = c;
            }
        }
    }
}

void Tetris::draw_board() {
    cout << "\033[2J\033[1;1H"; // Clear screen
    cout << "[LEVEL: " << level << "] [SCORE: " << score << "]" << endl;
    cout << "-----------------------------" << endl;

    for (int i = 0; i < 2 * w + 2; i++) {
        cout << "-";
    }
    cout << endl;

    for (int j = 0; j < h; j++) {
        cout << "!";
        for (int i = 0; i < w; i++) {
            if (i >= x && i < x + current.w && j >= y && j < y + current.h) {
                char c = current.data[j - y][i - x];
                cout << (c != ' ' ? c : board[j][i]) << " ";
            } else {
                cout << board[j][i] << " ";
            }
        }
        cout << "!" << endl;
    }

    for (int i = 0; i < 2 * w + 2; i++) {
        cout << "-";
    }
    cout << endl;
}

bool Tetris::hit_wall() {
    for (int i = 0; i < current.w; i++) {
        for (int j = 0; j < current.h; j++) {
            if (current.data[j][i] != ' ') {
                int board_x = x + i;
                int board_y = y + j;
                if (board_x < 0 || board_x >= w || board_y < 0 || board_y >= h || board[board_y][board_x] != ' ') {
                    return true;
                }
            }
        }
    }
    return false;
}