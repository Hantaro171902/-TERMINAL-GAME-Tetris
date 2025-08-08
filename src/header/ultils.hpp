// ultils.hpp
#pragma once

#include <iostream>
#include <string>

void clearScreen();
void clearTerminal(); // Full terminal reset

void setTextColor(int color);
void hideCursor();

int getch();    // For key input
bool kbhit();   // For non-blocking keyboard input

void console_size(int width, int height);
void sleep_ms(int ms);
int random_range(int min, int max);

