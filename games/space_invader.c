#include "../include/portmap.h"
#include "../include/graphics.h"
#include "../include/random.h"
#include "../include/bool.h"
#include "../include/math.h"
#include "../include/keyboard.h"
#include "../include/console.h"
#include "../include/vga_video_color.h"
#include "../include/space_invader.h"

static int tick = 0;
static int score = 0;

static Bool isGameOver = False;
static Bool isGamePaused = False;
static Bool isShooting = False;
static Bool isRestarting = False;
static Bool isExiting = False;

static const Bool isDebug = False;

static const int x_low_bound = 20;
static const int x_high_bound = 300;
static const int y_low_bound = 150;
static const int y_high_bound = 180;

static const int speed = 1;
static const int max_enemies = 100;
static const int max_projectiles = 100;

static const int num_stars = 35;

static const int x_player_hitbox = 30;
static const int y_player_hitbox = 30;

static int lives = 3;
static int wave_num = 0;

static int vx_player = 0;
static int vy_player = 0;
static int x_player = 240;
static int y_player = 150; // bottom middle start

static int enemy_direction = 1; // 1 -> right, -1 -> left
static int num_enemies = 0;
static int num_projectiles = 0;

// need more enemy types and associated sprites / values
typedef enum {
    Basic = 0,
    CHATGPT,
    None
} EnemyType;

typedef enum {
    Bullet = 0
} ProjectileType;

struct Star 
{
    int x;
    int y;
    VideoColor color;
};

struct Projectile
{
    int x;
    int y;
    int vx;
    int vy;
    int x_hit;
    int y_hit;
    int tag; // 0 -> friendly ; 1 -> hostile
    ProjectileType type;
    VideoColor color;
};

struct Enemy
{
    int x;
    int y;
    int vx;
    int vy;
    int x_hit; // hitbox dimensions
    int y_hit;
    int hp; //hitpoints
    int points;
    EnemyType type;
    VideoColor color;
};

struct Star stars[35] = {0};
struct Enemy enemies[100] = {0};
struct Projectile projectiles[100] = {0};

void HandleSpaceInvaderKeyPress()
{
    uint8_t scan = inb(0x60);

    switch (scan)
    {
        // could probably fix this with global BOOLS for key presses -> issue of not picking up combos of key presses
        case UARROW_PRESSED: vy_player = -speed; break;
        case DARROW_PRESSED: vy_player = speed;  break;
        case LARROW_PRESSED: vx_player = -speed; break;
        case RARROW_PRESSED: vx_player = speed; break;
        case UARROW_RELEASED: vy_player = 0; break;
        case DARROW_RELEASED: vy_player = 0; break;
        case LARROW_RELEASED: vx_player = 0; break;
        case RARROW_RELEASED: vx_player = 0; break;
        case KEY_ESC: isGamePaused = True; break;
        case KEY_SPACE: isShooting = True; break;
        default: break;
    }
    
}

void HandlePauseMenuInput()
{
    uint8_t scan = inb(0x60);
    while (scan != KEY_SPACE && scan != KEY_Q && scan != KEY_R)
    {scan = inb(0x60);}
    switch (scan)
    {
        case KEY_SPACE: isGamePaused = False; break;
        case KEY_Q: isExiting = True; break;
        case KEY_R: isRestarting = True; break;
        default: break;
    }
}

void UpdatePlayerPosition()
{
    x_player += vx_player;
    y_player += vy_player;
    if(x_player > x_high_bound)
    {
        x_player = x_high_bound;
    }
    if(x_player < x_low_bound)
    {
        x_player = x_low_bound;
    }
    if(y_player > y_high_bound)
    {
        y_player = y_high_bound;
    }
    if(y_player < y_low_bound)
    {
        y_player = y_low_bound;
    }
}


void InitStars()
{
    for(int i = 0; i < num_stars; i++)
    {
        stars[i].x = RandomInt(318) + 1;
        stars[i].y = RandomInt(198) + 1;
        stars[i].color = VIDEO_WHITE;
    }

}

void InitSpaceInvader()
{
    tick = 0;
    score = 0;
    isGameOver = False;
    isGamePaused = False;
    isShooting = False;
    isRestarting = False;
    isExiting = False;
    lives = 3;
    wave_num = 0;
    vx_player = 0;
    vy_player = 0;
    x_player = 240;
    y_player = 150; // bottom right start
    enemy_direction = 1; // 1 -> right, -1 -> left
    num_enemies = 0;
    num_projectiles = 0;
    InitFrameBuffer();
    InitStars();
}

void UpdateStars()
{
    for(int i = 0; i < num_stars; i++)
    {
        stars[i].y++;
        if(stars[i].y > 199)
        {
            stars[i].y = 1;
            stars[i].x = RandomInt(318)+1;
        }
    }
}
void DrawStars()
{
    for(int i = 0; i < num_stars; i++)
    {
        DrawPixel(stars[i].x, stars[i].y, stars[i].color);
        if(stars[i].color != VIDEO_WHITE)
        {
            DrawPixel(stars[i].x+1, stars[i].y, stars[i].color);
            DrawPixel(stars[i].x-1, stars[i].y, stars[i].color);
            DrawPixel(stars[i].x, stars[i].y+1, stars[i].color);
            DrawPixel(stars[i].x, stars[i].y-1, stars[i].color);
        }
    }
}

void SetStarColor(VideoColor color)
{
    for(int i = 0; i < num_stars; i++)
    {
        stars[i].color = color;
    }
}

void SetStarsRandomColor()
{
    for(int i = 0; i < num_stars; i++)
    {
        stars[i].color = RandomInt(64);
    }
}

void DrawPlayer(int x, int y)
{
    // Center part
    DrawRectangle(x, y+2, 20, 7, TAN, True);
    
    // Right wing
    DrawRectangle(x+7, y+4, 8, 5, TAN, True);
    DrawRectangle(x+12, y+2, 6, 5, TAN, True);
    DrawRectangle(x+16, y+2, 4, 4, TAN, True);
    DrawRectangle(x+20, y+2, 12, 4, TAN, True);

    // left wing
    DrawRectangle(x-7, y+4, 8, 5, TAN, True);
    DrawRectangle(x-12, y+2, 6, 5, TAN, True);
    DrawRectangle(x-16, y+2, 4, 4, TAN, True);
    DrawRectangle(x-20, y+2, 12, 4, TAN, True);

    // tip
    DrawRectangle(x-4, y-5, 5, 3, TAN, True);
    DrawRectangle(x+4, y-5, 5, 3, TAN, True);
    DrawRectangle(x, y-13, 6, 4, TAN, True);
    DrawRectangle(x, y-18, 4, 2, TAN, True);

    // Debug hitbox
    if(isDebug) DrawRectangle(x, y, x_player_hitbox, y_player_hitbox, ORANGE, False);
}

void DrawEnemy(int x, int y, EnemyType type)
{
    switch (type)
    {
        case Basic: 
        {
            DrawRectangle(x, y, 30, 30, VIDEO_RED, True);
            DrawRectangle(x-5, y-5, 4, 4, VIDEO_BLACK, True);
            DrawRectangle(x+5, y-5, 4, 4, VIDEO_BLACK, True);
            DrawRectangle(x, y+5, 2, 8, VIDEO_BLACK, True);
            break;
        }
        case CHATGPT: 
        {
            x -= 10;
            y -= 10;
            // Body of the enemy
            DrawRectangle(x, y, 20, 15, VIDEO_RED, True); // Main body

            // Eyes
            DrawRectangle(x + 4, y + 3, 4, 4, VIDEO_WHITE, True);  // Left eye
            DrawRectangle(x + 12, y + 3, 4, 4, VIDEO_WHITE, True); // Right eye

            // Antennae
            DrawRectangle(x + 8, y - 5, 2, 5, VIDEO_RED, True);    // Center antenna
            DrawRectangle(x + 6, y - 5, 2, 3, VIDEO_RED, True);    // Left antenna
            DrawRectangle(x + 12, y - 5, 2, 3, VIDEO_RED, True);   // Right antenna

            // Arms
            DrawRectangle(x - 5, y + 5, 5, 3, WINE_RED, True); // Left arm
            DrawRectangle(x + 20, y + 5, 5, 3, WINE_RED, True); // Right arm

            // Legs
            DrawRectangle(x + 4, y + 15, 4, 5, WINE_RED, True); // Left leg
            DrawRectangle(x + 12, y + 15, 4, 5, WINE_RED, True); // Right leg

            break;
        }
        default: break;
    }
    // Debug hitbox
    if(isDebug)
    {
        DrawRectangle(x, y, 30, 30, ORANGE, False);
    }
    
}

void DrawProjectile(int x, int y, ProjectileType type, VideoColor color)
{
    switch (type)
    {
        case Bullet: 
        {
            DrawRectangle(x, y, 6, 2, color, True);
            break;
        }
        default: break;
    }
}

void SpawnProjectile(int x, int y, int vx, int vy, ProjectileType type, int tag)
{
    if (num_projectiles >= max_projectiles) return;
    projectiles[num_projectiles].x = x;
    projectiles[num_projectiles].y = y;
    projectiles[num_projectiles].vx = vx;
    projectiles[num_projectiles].vy = vy;
    projectiles[num_projectiles].x_hit = 2;
    projectiles[num_projectiles].y_hit = 6; // add switch statement for type
    projectiles[num_projectiles].tag = tag;
    projectiles[num_projectiles].type = type;
    switch (tag)
    {
        case 0: projectiles[num_projectiles].color = BRIGHT_LIME; break;
        case 1: projectiles[num_projectiles].color = HOT_BRIGHT_RED; break;
        default: projectiles[num_projectiles].color = VIDEO_BLUE; break;
    }
    num_projectiles++;
}

void SpawnEnemy(int x, int y, int vx, int vy, EnemyType type)
{
    if (num_enemies >= max_enemies) return;
    enemies[num_enemies].x = x;
    enemies[num_enemies].y = y;
    enemies[num_enemies].vx = vx;
    enemies[num_enemies].vy = vy;
    enemies[num_enemies].x_hit = 30;
    enemies[num_enemies].y_hit = 30; // add switch statement for type
    enemies[num_enemies].hp = 5;
    enemies[num_enemies].points = 10;
    enemies[num_enemies].type = type;
    enemies[num_enemies].color = VIDEO_RED;
    num_enemies++;
}

void DespawnProjectile(int i)
{
    projectiles[i].x = projectiles[num_projectiles-1].x;
    projectiles[i].y = projectiles[num_projectiles-1].y;
    projectiles[i].vx = projectiles[num_projectiles-1].vx;
    projectiles[i].vy = projectiles[num_projectiles-1].vy;
    projectiles[i].x_hit = projectiles[num_projectiles-1].x_hit;
    projectiles[i].y_hit = projectiles[num_projectiles-1].y_hit;
    projectiles[i].tag = projectiles[num_projectiles-1].tag;
    projectiles[i].type = projectiles[num_projectiles-1].type;
    projectiles[i].color = projectiles[num_projectiles-1].color;
    num_projectiles--;
}

void DespawnEnemy(int i)
{
    enemies[i].x = enemies[num_enemies-1].x;
    enemies[i].y = enemies[num_enemies-1].y;
    enemies[i].vx = enemies[num_enemies-1].vx;
    enemies[i].vy = enemies[num_enemies-1].vy;
    enemies[i].x_hit = enemies[num_enemies-1].x_hit;
    enemies[i].y_hit = enemies[num_enemies-1].y_hit;
    enemies[i].hp = enemies[num_enemies-1].hp;
    enemies[i].points = enemies[num_enemies-1].points;
    enemies[i].type = enemies[num_enemies-1].type;
    enemies[i].color = enemies[num_enemies-1].color;
    num_enemies--;
}

void DrawProjectiles()
{
    for(int i  = 0; i < num_projectiles; i++)
    {
        DrawProjectile(projectiles[i].x, projectiles[i].y, projectiles[i].type, projectiles[i].color);
    }
}

void DrawEnemies()
{
    for(int i  = 0; i < num_enemies; i++)
    {
        DrawEnemy(enemies[i].x, enemies[i].y, enemies[i].type);
    }
}

void DrawPauseMenu()
{
    DrawRectangle(160, 108, 140, 160, VIDEO_BLACK, True);
    DrawRectangle(160, 108, 130, 150, VIDEO_WHITE, False);

    DrawString(100, 64, "     PAUSED\n\n===============\n\n   Q -> Quit\n\n   R-> Restart\n\n Space-> Resume", VIDEO_WHITE);
}

void DrawFinalScoreMenu(int wave)
{
    SetStarColor(VIDEO_RED);
    int x = 90;
    int y = 100;

    DrawString(x, y, "Final Score: ", VIDEO_WHITE);
    DrawInt(x+112, y, score, VIDEO_WHITE);

    if(wave > 1)
    {
        DrawString(x, y+16, "You Completed ", VIDEO_WHITE);
        DrawInt(x+112, y+16, wave-1, VIDEO_WHITE);
        DrawString(x+112+GetIntLength(wave-1)*8/*8->FontWidth*/, y+16, " Waves!", VIDEO_WHITE);
    }

    DrawString(x, y+32, "To Retry: ", VIDEO_WHITE);
    DrawString(x+32, y+48, "Press R!", VIDEO_WHITE);
    UpdateScreen();
    
    while(inb(0x60) != KEY_R) {}
    isRestarting = True;
}

Bool isProjectileCollidingWithEnemy(int i, int j)
{
    return projectiles[i].x <= ((enemies[j].x%320) + enemies[j].x_hit/2) &&
           projectiles[i].x >= ((enemies[j].x%320) - enemies[j].x_hit/2) &&
           projectiles[i].y <= ((enemies[j].y%200) + enemies[j].y_hit/2) &&
           projectiles[i].y >= ((enemies[j].y%200) - enemies[j].y_hit/2);
}

Bool isProjectileCollidingWithPlayer(int i)
{
    return projectiles[i].x <= (x_player + x_player_hitbox/2) &&
           projectiles[i].x >= (x_player - x_player_hitbox/2) &&
           projectiles[i].y <= (y_player + y_player_hitbox/2) &&
           projectiles[i].y >= (y_player - y_player_hitbox/2);
}

void UpdateProjectiles()
{
    int p_to_despawn[25] = {0};
    int num_p_to_despawn = 0;
    int e_to_despawn[25] = {0};
    int num_e_to_despawn = 0;
    Bool hasTakenDamage = False;
    for(int i = 0; i < num_projectiles; i++)
    {
        if (projectiles[i].x >= 315 || projectiles[i].x <= 5 || projectiles[i].y >= 195 || projectiles[i].y <= 5)
        {
            p_to_despawn[num_p_to_despawn] = i;
            num_p_to_despawn++;
        }
        for(int j = 0; j < num_enemies; j++)
        {
            if(projectiles[i].tag == 0 && isProjectileCollidingWithEnemy(i, j))
            {
                p_to_despawn[num_p_to_despawn] = i;
                num_p_to_despawn++;

                enemies[j].hp--;
                if(enemies[j].hp <= 0)
                {
                    e_to_despawn[num_e_to_despawn] = j;
                    num_e_to_despawn++;
                    score += enemies[j].points;
                }
            }
            if(projectiles[i].tag == 1 && isProjectileCollidingWithPlayer(i))
            {
                p_to_despawn[num_p_to_despawn] = i;
                num_p_to_despawn++;
                if(!hasTakenDamage) {lives--; hasTakenDamage = True;}
            }
        }
        if(projectiles[i].tag == 1 && tick % 2000 == 0)
        {
            projectiles[i].x += projectiles[i].vx;
            projectiles[i].y += projectiles[i].vy;
        }
        else if(projectiles[i].tag == 0)
        {
            projectiles[i].x += projectiles[i].vx;
            projectiles[i].y += projectiles[i].vy;
        }
    }

    // FIXME: getting hit deletes all projectiles??? WORKING AFTER INGAME RESET??????
    for(int k = 0; k < num_p_to_despawn; k++)
    {
        DespawnProjectile(p_to_despawn[k]);
    }
    for(int w = 0; w < num_e_to_despawn; w++)
    {
        DespawnEnemy(e_to_despawn[w]);
    }
}

void UpdateEnemies()
{
    int to_despawn[25] = {0}; // use chars to use 0 as null
    int num_to_despawn = 0;
    int rand_enemy = RandomInt(num_enemies); //FIXME: poor representation of staircasing
    Bool atInterval = enemies[rand_enemy].x % 100 == 0;
    for(int i = 0; i < num_enemies; i++)
    {
        if(atInterval)
        {
            enemies[i].y += enemies[i].vy;
            enemy_direction *= -1;
        }
        enemies[i].x += enemy_direction * enemies[i].vx;

        if(RandomInt(100) < 20)
        {
            // FIXME: this logic could be put elsewhere so enemies fire when not moving also
            SpawnProjectile(enemies[i].x, enemies[i].y, 0, enemies[i].vy/8, Bullet, 1);  
        }

        if(enemies[i].y > 160) isGameOver = True;
    }

    for(int j = 0; j < num_to_despawn; j++)
    {
        DespawnEnemy(to_despawn[j]);
    }
}

void UpdateSpaceInvader()
{
    UpdateStars();
    UpdatePlayerPosition();
    
    if (isShooting && tick % 30000 == 0)
    {
        // spawn projectile in front of player to avoid suicide
        SpawnProjectile(x_player, y_player-(y_player_hitbox/2)-1, 0, vy_player-speed*2, Bullet, 0);
        isShooting = False;
    }
    // should check collision in projectiles
    UpdateProjectiles();
    // should update enemies less frequently to get desired effect of staircasing down
    if(tick % 100000 == 0)
    {
        UpdateEnemies();
    }
}

void DrawSpaceInvader()
{
    DrawStars();
    DrawPlayer(x_player, y_player);
    DrawEnemies();
    DrawProjectiles();

    // DrawString(5, 5, "abcdefghijklmnopqrstuvqxyz\nABCDEFGHIJKLMNOPQRSTUVWXYZ\n0123456789\n`~@#$%&*()[]{}|/?<>,.-=_+", HOT_PINK);
    DrawString(5, 5, "Lives: ", VIDEO_WHITE);
    DrawChar(61, 5, lives+'0', VIDEO_WHITE);

    DrawString(220, 5, "Score: ", VIDEO_WHITE);
    DrawInt(280, 5, score, VIDEO_WHITE);

    DrawString(116, 5, "Wave: ", VIDEO_WHITE);
    DrawInt(164, 5, wave_num, VIDEO_WHITE);
}

void DrawWaveInfo(int wave, Bool start)
{
    if(start) SetStarsRandomColor();
    UpdateStars();
    DrawStars();
    int x = 90;
    int y = 75;
    // TODO: add info about gained lives
    if(wave >= 2)
    {
        DrawString(x, y, "Wave ", VIDEO_WHITE);
        DrawInt(x+40, y, wave-1, VIDEO_WHITE);
        DrawString(x+40+GetIntLength(wave-1)*8/*8->FontWidth*/, y, " Completed!", VIDEO_WHITE);
    }
    DrawString(x, y+16, "To Begin Wave ", VIDEO_WHITE);
    DrawInt(x+112, y+16, wave, VIDEO_WHITE);
    DrawString(x+32, y+32, "Press C!", VIDEO_WHITE);
    UpdateScreen();
    int i = 0;
    while(inb(0x60) != KEY_C) {if(i%1000==0) {DrawWaveInfo(wave, False);} i++;}
    SetStarColor(VIDEO_WHITE);
}

// TODO: Establish wave progression and difficulty
void SpawnWave(int wave)
{
    DrawWaveInfo(wave, True);
    if(wave%3==0){lives += 1+RandomInt(2);} // FIXME: maybe change this to be only every X rounds and maybe its random chance for extra lives
    int i = 50;
    int j = 50;
    for(int x = 0; x < (wave/2+1 > 6 ? 6 : wave/2+1); x++)
    {
        j = 50;
        for(int y = 0; y < (wave/5+1 > 3 ? 3 : wave/5+1); y++)
        {
            // FIXME: this is HACKY FIX for varied projectile speed -> 
            // to properly implement, add projectile_speed to enemy struct,
            // this can vary enemy to enemy and maybe even round to round
            SpawnEnemy(i, j, 4, 8+wave, CHATGPT); // subject to change
            j += 40;
        }
        i += 40;
    }
    x_player = 240;
    y_player = 150;
    vx_player = 0;
    vy_player = 0;
    num_projectiles = 0; // ignore data in projectiles array pretend empty -> data to be overwritten anyway
    isShooting = False;
}

void SpaceInvader()
{
    SetVideoMode();  // Enter graphics mode 13h
    
    InitSpaceInvader();

    // SpawnEnemy(200, 40, 4, 8, CHATGPT); // for testing

    while(!isGameOver)
    {
        if(num_enemies <= 0) {score+= 100*wave_num; wave_num++; SpawnWave(wave_num); }
        HandleSpaceInvaderKeyPress();
        if(tick % 1000 == 0)
        {            
            UpdateSpaceInvader();
            DrawSpaceInvader();
            if(lives<=0) isGameOver = True;
            if(isGamePaused)
            {
                DrawPauseMenu();
            }
            if(!isGameOver) UpdateScreen();
            if(isGamePaused)
            {
                HandlePauseMenuInput();
                if(isExiting) {UpdateScreen(); SetTextMode(); ClearScreen(); return;}
                if(isRestarting) {SpaceInvader(); return;}
            }
        }
        tick++;
    }
    DrawFinalScoreMenu(wave_num); // TODO: could look cool like wave change
    if(isRestarting)
    {
        SpaceInvader(); 
        return;
    }

    UpdateScreen(); // one final time to clear pixels
    SetTextMode();  // Return to text mode
    ClearScreen();
}