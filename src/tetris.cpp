#include "tetris.hpp"
#include "ultils.hpp"
#include "block.hpp"
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

extern const vector<Block> BLOCK_SET;

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

    const vector<Block> blocks;

    // const vector<Block> blocks = {
    //     {{"##", "##"}, 2, 2},
    //     {{" X ", "XXX"}, 3, 2},
    //     {{"@@@@"}, 4, 1},
    //     {{"OO", "O ", "O "}, 2, 3},
    //     {{"&&", " &", " &"}, 2, 3},
    //     {{"ZZ ", " ZZ"}, 3, 2}
    // };
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
    // preview_block = BLOCK_SET[rand() % BLOCK_SET.size()]; // Initialize preview block

    init_game();
    new_block();
    // generate_preview_block();


    startTime = std::chrono::steady_clock::now(); // <-- Add this line

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
                case 'a' : case 'A': // Move left
                    x--;
                    if (hit_wall()) x++;
                    break;
                case 'd' : case 'D': // Move right
                    x++;
                    if (hit_wall()) x--;
                    break;
                case 's' : case 'S': // Move down
                    y++;
                    if (hit_wall()) y--;
                    break;
                case 'w' : case 'W': // Rotate block
                    block_rotate();
                    play_sound("assets/sounds/sfx_rotate.wav");
                    break;
                case 'q' : case 'Q': // Quit game
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
    current = BLOCK_SET[rand() % BLOCK_SET.size()];
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
                setTextColor(getColor(c));

            }
        }
    }
}

void Tetris::draw_board() {
    // cout << "\033[2J\033[1;1H"; // Clear screen
    clearTerminal();
    clearScreen();

    setTextColor(33); // Cyan (you can change it later via color system)
    cout << R"(
                ████████╗███████╗████████╗██████╗ ██╗███████╗
                ╚══██╔══╝██╔════╝╚══██╔══╝██╔══██╗██║██╔════╝
                   ██║   █████╗     ██║   ██████╔╝██║███████╗
                   ██║   ██╔══╝     ██║   ██╔══██╗██║╚════██║
                   ██║   ███████╗   ██║   ██║  ██║██║███████║
                   ╚═╝   ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝╚══════╝
                )" << '\n';
    setTextColor(0); // Reset to default
    int timeElapsed = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count();
    // int time = 0;
    cout << "[LEVEL: " << level << "] [SCORE: " << score << "]" <<" [TIME: " << formatTime(timeElapsed) << "]" << endl;
    cout << "-----------------------------" << endl;

    

    // Top border
    cout << SYMBOL_DOUBLE_TOP_LEFT;
    for (int i = 0; i < 2 * w; i++) {
        cout << SYMBOL_DOUBLE_HORIZONTAL;
    }
    cout << SYMBOL_DOUBLE_TOP_RIGHT << endl;

    for (int j = 0; j < h; j++) {
        cout << SYMBOL_DOUBLE_VERTICAL;
        for (int i = 0; i < w; i++) {
            char ch = board[j][i];
            
            if (i >= x && i < x + current.w && j >= y && j < y + current.h) {
                char c = current.data[j - y][i - x];
                if (c != ' ') {
                    ch = c; // Use block character if within current block
                } 
            } 
            setTextColor(getColor(ch));
            cout << ch << " ";
            resetTextColor();
        }
        cout << SYMBOL_DOUBLE_VERTICAL << endl;
    }

    // Bottom border
    cout << SYMBOL_DOUBLE_BOTTOM_LEFT;
    for (int i = 0; i < 2 * w; i++) {
        cout << SYMBOL_DOUBLE_HORIZONTAL;
    }
    cout << SYMBOL_DOUBLE_BOTTOM_RIGHT << endl;

    // draw_preview_frame();

}

void Tetris::block_rotate() {
    Block b = current;
    Block rotated;
    rotated.w = b.h;
    rotated.h = b.w;
    rotated.data = vector<string>(rotated.h, string(rotated.w, ' '));

    for (int i = 0; i < b.h; i++) {
        for (int j = 0; j < b.w; j++) {
            rotated.data[j][b.h - 1 - i] = b.data[i][j];
        }
    }

    int old_x = x, old_y = y;
    x -= (rotated.w - b.w) / 2; // Center the block
    y -= (rotated.h - b.h) / 2; // Center the block
    current = rotated;
    if (hit_wall()) {
        x = old_x; 
        y = old_y;
        current = b; // Revert to old block
    }
}

void Tetris::block_gravity() {
    y++;
    if (hit_wall()) {
        y--;
        print_block();
        check_lines();
        new_block();
    }
}

void Tetris::block_fall(int row) {
    for (int j = row; j > 0; j--) {
        for (int i = 0; i < w; i++) {
            board[j][i] = board[j - 1][i];
        }
    }
    for (int i = 0; i < w; i++) {
        board[0][i] = ' ';
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
            if (board[j][i] == ' ') {
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

int Tetris::level_speed() {
    if (score > 10000) level = 7;
    else if (score > 5000) level = 5;
    else if (score > 1000) level = 4;
    else if (score > 500) level = 3;
    else if (score > 300) level = 2;
    else level = 1;

    static const int speeds[] = { 120, 90, 70, 50, 40, 30, 20 };
    return speeds[level - 1];
}

// Preview block methods
void Tetris::draw_preview_frame() {
    int px = w * 2 + 6;  // Position to the right of the board
    int py = 6;

    move_cursor(px, py - 2);
    cout << " Next: ";

    // Draw frame
    move_cursor(px, py - 1);
    cout << SYMBOL_DOUBLE_TOP_LEFT;
    for (int i = 0; i < 6; i++) cout << SYMBOL_DOUBLE_HORIZONTAL;
    cout << SYMBOL_DOUBLE_TOP_RIGHT;

    for (int j = 0; j < 4; j++) {
        move_cursor(px, py + j);
        cout << SYMBOL_DOUBLE_VERTICAL << "      " << SYMBOL_DOUBLE_VERTICAL;
    }

    move_cursor(px, py + 4);
    cout << SYMBOL_DOUBLE_BOTTOM_LEFT;
    for (int i = 0; i < 6; i++) cout << SYMBOL_DOUBLE_HORIZONTAL;
    cout << SYMBOL_DOUBLE_BOTTOM_RIGHT;

    draw_preview_block(px + 1, py + 1); // Offset inside frame
}

void Tetris::draw_preview_block(int x, int y) {
    int blockX = preview_block.h;
    int blockY = preview_block.w;

    // Adjust to center inside 5x5 preview frame
    int offsetY = y + (5 - blockY) / 2;
    int offsetX = x + (5 - blockX) / 2;

    for (int j = 0; j < blockY; ++j) {
        for (int i = 0; i < blockX; ++i) {
            char ch = preview_block.data[j][i];
            if (ch != ' ') {
                move_cursor(offsetY + j, offsetX + i * 2); // *2 for spacing
                setTextColor(getColor(ch));
                cout << ch;
                resetTextColor();
            }
        }
    }
}

void Tetris::generate_preview_block() {
    current = preview_block;
    preview_block = BLOCK_SET[rand() % BLOCK_SET.size()];
}
