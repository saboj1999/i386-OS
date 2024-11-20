#include <stdint.h>

#include "../include/portmap.h"
#include "../include/bool.h"
#include "../include/math.h"
#include "../include/string.h"
#include "../include/random.h"
#include "../include/time.h"
#include "../include/console.h"
#include "../include/keyboard.h"
#include "../include/snake.h"

#define SNAKE_ARRAY_SIZE 1000 // should change this

static Bool isGamePaused = False;
static Bool isGameOver = False;
static Bool isPowerUp = False;
static Bool isPowerUpSpawned = False;
static Bool isRestart = False;
static Bool isCheater = False;
static Bool wasCheater = False;
static Bool isExiting = False;

static int power_up_active_time = 50000;
static int power_up_despawn_time = 25000;

static Color snake_color = GREEN;

static int score = 0;
static int highscore = 0;
static int games_played = 0;
static int snake_size = 1;
static const char food = 0xEB;
static const char snake = 0xDB;
static const char power_up = 0xE3;

static int vx = 0;
static int vy = 0;

static const int x_min = 10; // 0-79
static const int x_max = 70; // 0-79
static const int y_min = 2;  // 0-24
static const int y_max = 22; // 0-24

static int x_length = x_max - x_min;
static int y_length = y_max - y_min;

static int x_snake[SNAKE_ARRAY_SIZE] = {0}; // this could be made into one array with structs of position
static int y_snake[SNAKE_ARRAY_SIZE] = {0};

void CoolTransition()
{
    for(int i = 0; i < 80; i++)
    {
        for(int j = 0; j < 25; j++)
        {
            SetCursorPosition(i, j);
            PrintCharFullColorAtCursor(' ', GetForeGround(), RandomInt(15));
            Sleep(0.08);
        }
    }
    for(int k = 0; k < 16; k++) // loop through colors
    {
        if(k != GetBackGround())
        {
            for(int i = 0; i < 80; i++) // x screen values
            {
                for(int j = 0; j < 25; j++) // y screen values
                {
                    SetCursorPosition(i, j);
                    if(k == GetBackGroundAtCursor())
                    {
                        PrintCharAtCursor(' ');
                        Sleep(0.24);
                    }
                }
            }
        }
    }
    SetCursorDefaultPosition();
}
void PrintInstructions()
{
    PrintLine("\t\t    -----------------------------------");
    PrintLine("\t\t\t    Instructions: ");
    PrintLine("\t\t    -----------------------------------");
    PrintLine("\t\t    1. Use Arrow Keys to Move.");
    PrintLine("\t\t    2. Avoid the Borders!");
    PrintLine("\t\t    3. Don't run into yourself!");
    PrintString("\t\t    4. Collect Apples for Points: ");
    PrintCharColor(food, RED);
    PrintChar('\n');
    PrintString("\t\t    5. Look out for Power-Ups: ");
    PrintCharColor(power_up, MAGENTA);
    PrintChar('\n');
    PrintLine("\t\t    6. Pause/Unpause with Space/Enter.");
    PrintLine("\t\t    -----------------------------------");
    PrintString("\n\n\n\n\n\n\n\n");
    HideCursor();
    Sleep(2500);
    MoveCursor(0, -5);
    PrintStringFullColor("\t\t\tPress Enter to Continue...", BLACK, YELLOW);
    HideCursor();
    while (inb(0x60) != ENTER_PRESSED){}
}

void InitValues()
{
    score = 0;
    vx = 0;
    vy = 0;
    isGameOver = False;
    isGamePaused = False;
    isPowerUp = False;
    isPowerUpSpawned = False;
    power_up_active_time = 50000;
    power_up_despawn_time = 25000;
    snake_color = GREEN;
}

void InitSnake()
{
    // initialize all spots in snake array to 0
    // avoid min x/y being 0 -> might not matter with border
    snake_size = 1;
    for(int i = 0; i < SNAKE_ARRAY_SIZE; i++)
    {
        x_snake[i] = 0;
        y_snake[i] = 0;
    }
    SetCursorPosition(40, 12);  // center cursor
    x_snake[0] = GetCursorX();
    y_snake[0] = GetCursorY();
}

void MoveSnake()
{
    int prev_cx = GetCursorX();
    int prev_cy = GetCursorY();
    SetCursorPosition(x_snake[0], y_snake[0]);
    PrintCharAtCursor(' ');
    SetCursorPosition(prev_cx, prev_cy);
    for(int i = 0; i < snake_size - 1; i++)
    {
        x_snake[i] = x_snake[i+1];
        y_snake[i] = y_snake[i+1]; // shifting everything toward index zero -> tail at zero
    }
    x_snake[snake_size-1] = GetCursorX();
    y_snake[snake_size-1] = GetCursorY();
}

void GrowSnake()
{
    snake_size++;
    x_snake[snake_size-1] = GetCursorX();
    y_snake[snake_size-1] = GetCursorY();
}

void DrawBorder(char c, Color color)
{
    HideCursor();
    SetTextColor(color);
    SetCursorPosition(x_min, y_min);
    for(int i = 0; i < x_max - x_min; i++)
    {
        PrintChar(c);
        MoveCursor(-1, y_max - y_min);
        Sleep(5);
        PrintChar(c);
        MoveCursor(0, -(y_max - y_min));
        Sleep(5);
    }
    SetCursorPosition(x_min, y_min);
    for(int i = 0; i < y_max - y_min; i++)
    {
        PrintCharAtCursor(c);
        MoveCursor(x_max - x_min, 0);
        Sleep(5);
        PrintCharAtCursor(c);
        MoveCursor(-(x_max - x_min), 1);
        Sleep(5);
    }
    SetCursorPosition(x_max, y_max);
    PrintCharAtCursor(c);
    ShowCursor();
}

void ShowPaused()
{
    int prev_cx = GetCursorX();
    int prev_cy = GetCursorY();
    SetCursorPosition(1, 0);
    PrintString("                \n                        ");
    SetCursorPosition(1, 0);
    PrintStringFullColor(" ** PAUSED ** \n Press Enter to Resume. Press ESC to Exit. ", LIGHT_CYAN, BLACK);
    SetCursorPosition(prev_cx, prev_cy);
}

void ShowUnpaused()
{
    int prev_cx = GetCursorX();
    int prev_cy = GetCursorY();
    SetCursorPosition(1, 0);
    PrintString("              \n                                            ");
    SetCursorPosition(1, 0);
    PrintStringFullColor(" ** UNPAUSED ** \n Press Space to Pause. ", LIGHT_CYAN, BLACK);
    SetCursorPosition(prev_cx, prev_cy);
}

void HandleSnakeKeyPress()
{
    uint8_t scan = inb(0x60);
    
    switch (scan)
    {
        case UARROW_PRESSED: vx = 0; vy = -1; break;
        case DARROW_PRESSED: vx = 0; vy = 1; break;
        case LARROW_PRESSED: vx = -1; vy = 0; break;
        case RARROW_PRESSED: vx = 1; vy = 0; break;
        case SPACE_PRESSED: {isGamePaused = True; ShowPaused(); ShowCursor(); break;}
        case ENTER_PRESSED: {isGamePaused = False; ShowUnpaused(); HideCursor(); break;}
        case ESCAPE_PRESSED: {if(isGamePaused) {isExiting = True;} break;}
        default: return;
    }
}

Bool isOutOfBounds()
{
    return GetCursorX() >= x_max || GetCursorX() <= x_min || GetCursorY() >= y_max || GetCursorY() <= y_min;
}

Bool isSnakeEatingFood()
{
    // could update this to handle different food types
    // would need an array of food chars
    return GetCharAtCursor() == food;
}

Bool isSnakeEatingPowerUp()
{
    // could update this to handle different food types
    // would need an array of food chars
    return GetCharAtCursor() == power_up;
}

Bool isSnakeHere()
{
    return GetCharAtCursor() == snake;
}

Bool isSnakeGoingToBeHere()
{
    return GetCursorX() == x_snake[snake_size-1] + vx && GetCursorY() == y_snake[snake_size-1] + vy;
}

void SpawnItem(int amount, char c, Color color)
{
    int prev_cx = GetCursorX();
    int prev_cy = GetCursorY();
    int rand_x, rand_y;
    for(int i = 0; i < amount; i++)
    {
        do {
            rand_x = RandomInt(x_max - x_min) + x_min + 1;
            rand_y = RandomInt(y_max - y_min) + y_min + 1;
            SetCursorPosition(rand_x, rand_y);
        } while(isSnakeHere() || isSnakeGoingToBeHere());

        PrintCharColor(c, color);
    }
    SetCursorPosition(prev_cx, prev_cy); // put cursor back
    if(c == power_up) isPowerUpSpawned = True;
}

void UpdateScore()
{
    Color prev_fg = GetForeGround();
    Color prev_bg = GetBackGround();
    int prev_cx = GetCursorX();
    int prev_cy = GetCursorY();
    SetBackGroundColor(BLACK);
    SetTextColor(LIGHT_CYAN);
    SetCursorPosition(65, 0);
    Printf(" Score: %d ", score);
    SetCursorPosition(prev_cx, prev_cy); // put cursor back
    SetBackGroundColor(prev_bg);
    SetTextColor(prev_fg);
}

void PrintHighScore()
{
    Color prev_fg = GetForeGround();
    Color prev_bg = GetBackGround();
    int prev_cx = GetCursorX();
    int prev_cy = GetCursorY();
    SetBackGroundColor(BLACK);
    SetTextColor(LIGHT_CYAN);
    SetCursorPosition(35, 23); // bottom middle
    Printf(" Highscore: %d ", highscore);
    SetCursorPosition(prev_cx, prev_cy); // put cursor back
    SetBackGroundColor(prev_bg);
    SetTextColor(prev_fg);
}

void SetSnakeColor(Color color)
{
    SetTextColor(color);
    int i = 0;
    while(x_snake[i] != 0)
    {
        SetCursorPosition(x_snake[i], y_snake[i]);
        PrintCharAtCursor(snake);
        i++;
    }
}

void DespawnItem(char c)
{
    // could improve this by keeping track of all item locations
    // only needing to loop through them
    int prev_cx = GetCursorX();
    int prev_cy = GetCursorY();
    for(int i = x_min; i < x_max; i++)
    {
        for(int j = y_min; j < y_max; j++)
        {
            SetCursorPosition(i, j);
            if(GetCharAtCursor() == c)
            {
                PrintCharAtCursor(' ');
            }
        }
    }
    SetCursorPosition(prev_cx, prev_cy);
}

void UpdateSnake()
{
    // move cursor and check conditions
    MoveCursor(vx, vy);
    if(isSnakeEatingFood())
    {
        SpawnItem(1, food, RED);
        score += 10;
        UpdateScore();
        GrowSnake();
    }
    else if(isSnakeEatingPowerUp())
    {
        SpawnItem(RandomInt(10)+5, food, RED);
        score += 50;
        MoveSnake();
        isPowerUp = True;
        isPowerUpSpawned = False;
        power_up_despawn_time = 25000;
        power_up_active_time = 50000;
    }
    else
    {
        MoveSnake();
    }
    // check if head is out of bounds or overself -> gameover
    if(isOutOfBounds() || isSnakeHere())
    {
        isGameOver = True;
    }
    PrintCharAtCursor(snake);
}

void Exit(Color fg)
{
    CoolTransition();
    ShowCursor();
    SetTextColor(fg);
}

int Snake(char* input)
{
    if(CompareString(Split(Strip(Lower(input)), ' ')[1], "-c"))
    {
        isCheater = True;
        wasCheater = False;
        highscore = 0;
        input = "snake -c";
    }
    int i = 0;
    games_played++;
    InitValues();

    Color prev_fg = GetForeGround();
    Color prev_bg = GetBackGround();

    if(!isRestart)
    {
        CoolTransition();
        PrintInstructions();
        CoolTransition();
    }
    isRestart = False;
    ClearScreen();
    DrawBorder(0xB1, BLUE);
    InitSnake();
    SetTextColor(snake_color); // snake color
    UpdateScore();
    PrintHighScore();
    ShowUnpaused(); // bad naming convention for this one
    HideCursor();

    while(!isGameOver)
    {
        // spawn initial food
        if (i == 0) SpawnItem(1, food, RED);

        // check if recent arrow key pressed has changed
        HandleSnakeKeyPress();
        while(isGamePaused){HandleSnakeKeyPress(); if(isExiting) {isExiting = False; isCheater = False; Exit(prev_fg); return highscore;}}

        if (i % 360 == 0)
        {
            if (RandomInt(1000) == 0 && !isPowerUpSpawned) SpawnItem(1, power_up, MAGENTA);
            if (isCheater && RandomInt(100) < 25) {SpawnItem(2, power_up, RandomInt(15));}
            // we update the screen much less frequently than checking for key presses
            UpdateSnake();
            if(score > highscore) 
            {
                highscore = score; 
                if(wasCheater)
                {
                    highscore = -999999;
                }
                PrintHighScore();}
        }

        if(isPowerUpSpawned)
        {
            power_up_despawn_time--;
            if(power_up_despawn_time < 0)
            {
                DespawnItem(power_up);
                isPowerUpSpawned = False;
                power_up_despawn_time = 25000;
            }
        }

        if(isPowerUp)
        {
            if(power_up_active_time >= 7500 && i % 1000 == 0)
            {
                snake_color = (snake_color + 1) % 15;
                SetSnakeColor(snake_color);
            }
            if (power_up_active_time < 7500 && i % 1000 == 0)
            {
                SetSnakeColor(RED);
            }
            power_up_active_time--;
            if(power_up_active_time < 0)
            {
                DespawnItem(food);
                SpawnItem(1, food, RED);
                isPowerUp = False;
                power_up_active_time = 50000;;
                SetSnakeColor(GREEN);
            }
        }

        // wait DeltaTime?
        Sleep(0.4);
        i++;
    }

    SetCursorPosition(40, 12);  // center cursor
    MoveCursor(-4, -4);
    PrintStringFullColor(" Score: ", LIGHT_MAGENTA, LIGHT_BLUE);
    PrintIntFullColor(score, LIGHT_MAGENTA, LIGHT_BLUE);
    PrintCharHighlight(' ', LIGHT_BLUE);
    HideCursor();
    if(score > highscore) highscore = score;
    if(wasCheater || isCheater)
    {
        highscore = -999999;
        wasCheater = True;
    }

    MoveCursor(-16, 2);
    PrintStringFullColor(" Press R to Reset... ", BLACK, YELLOW);
    MoveCursor(-21, 2);
    PrintStringFullColor(" Press ESC to Exit... ", BLACK, YELLOW);
    HideCursor();

    uint8_t scan;
    if(!wasCheater && games_played > 10)
    {
        MoveCursor(-22, 2);
        PrintStringFullColor(" Press C to Cheat... ", BLACK, YELLOW);
        scan = inb(0x60);
        while(scan != KEY_R && scan != KEY_ESC && scan != KEY_C) 
        {
            scan = inb(0x60);
        }
    }
    else
    {
        if (wasCheater) {MoveCursor(-22, 2); PrintStringFullColor(" Press C to Cheat... ", DARK_GRAY, LIGHT_GRAY);}
        scan = inb(0x60);
        while(scan != KEY_R && scan != KEY_ESC) 
        {
            scan = inb(0x60);
        }
    }

    if(scan == KEY_R) {isRestart = True; isCheater = False; SetTextColor(prev_fg); return Snake(input);}
    if(scan == KEY_C) {isRestart = True; isCheater = True; SetTextColor(prev_fg); return Snake(input);}
    isCheater = False;

    Exit(prev_fg);

    return highscore;
}