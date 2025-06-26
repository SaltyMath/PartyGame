#ifndef MENUSCREEN_HPP
#define MENUSCREEN_HPP

#include "../GameState.hpp"

void InitMenuScreen();
void UpdateMenuScreen(GameState& state);
void DrawMenuScreen();
void UnloadMenuScreen();

extern float menuCooldown;

#endif
