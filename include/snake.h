#ifndef OS_INCLUDE_SNAKE_H
#define OS_INCLUDE_SNAKE_H
#include "bool.h"
#include "vga_color.h"

unsigned int RandomInt(unsigned int max);
void Sleep(double ms);

void CoolTransition();
void PrintInstructions();

void InitValues();

void InitSnake();
void MoveSnake();
void GrowSnake();

void DrawBorder(char c, Color color);

void ShowPaused();
void ShowUnpaused();

void HandleSnakeKeyPress();
Bool isOutOfBounds();
Bool isSnakeEatingFood();
Bool isSnakeEatingPowerUp();
Bool isSnakeHere();
Bool isSnakeGoingToBeHere();

void SpawnItem(int amount, char c, Color color);
void UpdateScore();
void PrintHighScore();
void SetSnakeColor(Color color);
void DespawnItem(char c);

void UpdateSnake();

int Snake(char* input);

#endif