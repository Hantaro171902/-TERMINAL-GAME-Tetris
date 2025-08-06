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

void Tetris::run() {
    set_non_blocking_input();
    init_game();
    new_block();


    int tick = 0;
    while (!game_over) {
        // draw_board();
        // this_thread::sleep_for(chrono::milliseconds(level_speed()));
        this_thread::sleep_for(chrono::milliseconds(10));
        tick++;

        if (tick % 5 == 0) {
            draw_board();
        }

        if (tick % level_speed() == 0) {
            block_gravity();
            draw_board();
        }

        int ch = getchar();
        if (ch > 0) {
            switch (ch) {
                case 'a' || 'A': // Move left
                    x--;
                    if (hit_wall()) x++;
                    break;
                case 'd' || 'D': // Move right
                    x++;
                    if (hit_wall()) x--;
                    break;
                case 's' || 'S': // Move down
                    y++;
                    if (hit_wall()) y--;
                    break;
                case 'w' || 'W': // Rotate block
                    block_rotate();
                    break;
                case 'q' || 'Q': // Quit game
                    game_over = true;
                    break;
            }
        }
    }
    draw_board();
    // reset_input();
    cout << "Game Over! Your score: " << score << endl;
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

void Tetris::block_rotate() {
    Block b = current;
    Block rotated;
    rotated.w = b.h;
    rotated.h = b.w;
    rotated.data = vector<string>(rotated.h, string(rotated.w, ' '));

    for (int i = 0; i < b.w; i++) {
        for (int j = 0; j < b.h; j++) {
            rotated.data[j][b.w - 1 - i] = b.data[i][j];
        }
    }

    int old_x = x, old_y = y;
    x -= (rotated.w - b.w) / 2; // Center the block
    y -= (rotated.h - b.h) / 2; // Center the block
    current = rotated;
    if (hit_wall()) {
        x = old_x; 
        y = old_y;
        // current = b; // Revert to old block
    }
}

void Tetris::block_gravity() {
    y++;
    if (hit_wall()) {
        y--;
        print_block();
        // check_lines();
        new_block();
    }
}

void Tetris::block_fall(int row) {
    for (int j = row; j > 0; j--) {
        for (int i = 0; i < w; i++) {
            board[i][j] = board[i][j - 1];
        }
    }
    for (int i = 0; i < w; i++) {
        board[i][0] = ' ';
    }
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

void Tetris::check_lines() {
    int bonus = 100;
    for (int j = h - 1; j >= 0; j--) {
        bool full = true;
        for (int i = 0; i < w; i++) {
            if (board[i][j] == ' ') {
                full = false;
                break;
            }
        }
        if (full) {
            score += 10;
            bonus *= 2; 
            block_fall(j);
            j++;   // re-check same line
        }
    }
}
