#include "../include/console.h"
#include "../include/string.h"
#include "../include/bool.h"
#include "../include/math.h"
#include "../include/cmds.h"
#include "../include/snake.h"
#include "../include/space_invader.h"

static const char* validCommands[] = {
    "help",
    "clear",
    "echo",
    "exit",
    "stc",
    "sbc",
    "man",
    "snake",
    "space", // ls, cd, pwd, nano?, ping
    NULL
};

static const char* validColors[] = {
    "black",
    "blue",
    "green",
    "cyan",
    "red",
    "magenta",
    "brown",
    "light-gray",
    "dark-gray",
    "light-blue",
    "light-green",
    "light-cyan",
    "light-red",
    "light-magenta",
    "yellow",
    "white",
    NULL
};

/**
 * Purpose: Determine if a given commands first arguement is a valid one.
 * << isValidCommand >>
 * @params: 
 *      char* command -> a given command "man help 123"
 * @returns: 
 *      Bool -> true if command is in list of valid commands
 */
Bool isValidCommand(char* command)
{
    char* copy;
    StrCopy(copy, command);
    if(CompareString(Strip(Lower(copy)), "")) return False;
    StrCopy(copy, command);
    copy = Split(Strip(Lower(copy)), ' ')[0];

    int i = 0;
    while (validCommands[i] != NULL)
    {
        if (CompareString(copy, validCommands[i]))
        {
            return True;
        }
        i++;
    }
    PrintLine("Invalid command, try 'help' for a list of available commands.");
    return False;
}
/**
 * Purpose: Determine if a given color as a string is a valid VGA color.
 * << isValidColor >>
 * @params: 
 *      char* color -> a given string "green"
 * @returns: 
 *      Bool -> true if color is in list of valid colors
 */
Bool isValidColor(char* color)
{
    int i = 0;
    while (validColors[i] != NULL)
    {
        if (CompareString(Strip(Lower(color)), validColors[i]))
        {
            return True;
        }
        i++;
    }
    return False;
}
/**
 * Purpose: Convert a valid string color into the VGA color equivalent.
 * << ConvertValidStrToColor >>
 * @params: 
 *      char* color -> a given string: "green"
 * @returns: 
 *      Color -> corresponding VGA color: GREEN
 */
Color ConvertValidStrToColor(char* color)
{
    int i = 0;
    while (validColors[i] != NULL)
    {
        if (CompareString(Lower(Strip(color)), validColors[i]))
        {
            return BLACK+i;
        }
        i++;
    }
    return BLACK;
}
/**
 * Purpose: Print a list of available text color options.
 * << PrintTextColorOptions >>
 * @params: none
 * @returns: void
 */
void PrintTextColorOptions()
{
    char prev_bg = GetBackGround();
    PrintLine("------------------------------------------");
    PrintLine("\tList of Text Colors: ");
    PrintLine("------------------------------------------");
    int i = 0;
    while (validColors[i] != NULL)
    {
        PrintChar('\t');
        if(i == 15) {SetBackGroundColor(BLACK);}
        PrintStringColor(validColors[i], BLACK+i);
        SetBackGroundColor(prev_bg);
        PrintChar('\n');
        i++;
    }
    PrintLine("------------------------------------------");
}
/**
 * Purpose: Print a list of available background color options.
 * << PrintBackGroundColorOptions >>
 * @params: none
 * @returns: void
 */
void PrintBackGroundColorOptions()
{
    char prev_fg = GetForeGround();
    PrintLine("------------------------------------------");
    PrintLine("\tList of Background Colors: ");
    PrintLine("------------------------------------------");
    int i = 0;
    while (validColors[i] != NULL)
    {
        PrintChar('\t');
        if(i == 0) {SetTextColor(WHITE);}
        PrintStringHighlight(validColors[i], BLACK+i);
        SetTextColor(BLACK);
        PrintChar('\n');
        i++;
    }
    SetTextColor(prev_fg);
    PrintLine("------------------------------------------");
}
/**
 * Purpose: Test print functionality.
 * << __PrintTest__ >>
 * @params: none
 * @returns: void
 */
void __PrintTest__()
{
    char* str = "Hello World!";
    char* tab_str = "HELLO\t\tW\tO\tR\tL\tD\t!";
    ClearScreen();
    SetFullBackGroundColor(LIGHT_GRAY);
    SetTextColor(GREEN);
    for(int i = 0; i < 100; i++)
    {
        if(i%2==0)
        { 
            PrintString(str);
            PrintString("\t");
            PrintLine(str);
        }
        else
        { 
            PrintLineColor(tab_str, RED);
            PrintIntColor(1234, MAGENTA);
            PrintString("\n");
            PrintIntHighlight(-4321, YELLOW);
            PrintChar('\n');
            PrintDoubleFullColor(-1234.5678, BLACK, YELLOW);
            PrintLine("");
        }
    }

    int x = 1000000000;
    while (x > 0) {x--;}

    PrintLineHighlight("END", YELLOW);
    SetFullBackGroundColor(WHITE);
    SetAllTextColor(LIGHT_BLUE);
    Printf("Hello %s, Welcome to your OS!\nHigh %d! \nThe Temperature is: %f%c\n", "Jake", 5, 82.501, 0xF8);
    Printf("Is %d even? %s", 4, isEven(4) ? "Yes" : "No");
    
    x = 1000000000;
    while (x > 0) {x--;}

    SetAllTextColor(WHITE);
    SetFullBackGroundColor(BLACK);
    RemoveTextColors();
    RemoveHighlights();

    x = 1000000000;
    while (x > 0) {x--;}
}
/**
 * Purpose: Print a list of available commands.
 * << Help >>
 * @params: none
 * @returns: void
 */
void Help()
{
    PrintLine("------------------------------------------");
    PrintLine("\tList of Available Commands: ");
    PrintLine("------------------------------------------");
    PrintLine("\thelp  -> List available commands.");
    PrintLine("\tclear -> Clear the screen.");
    PrintLine("\techo  -> Echo text in console.");
    PrintLine("\texit  -> Exit kernel main.");
    PrintLine("\tstc   -> Set text color.");
    PrintLine("\tsbc   -> Set background color.");
    PrintLine("\tsnake -> Snake...?");
    PrintLine("\tspace -> Defend Earth!");
    PrintLine("------------------------------------------");
    PrintLine("\tman @ -> Manual for @ command.");
    PrintLine("------------------------------------------");
}
/**
 * Purpose: Print a manual for a specified command.
 * << Man >>
 * @params: 
 *      char* input -> verified input: "man @"
 * @returns: void
 */
void Man(char* input)
{
    char** tokens = Split(Strip(Lower(input)), ' ');

    if (!tokens[1] || tokens[1][0] == '\0')
    {
        PrintLine("No argument supplied for 'man' command, type 'man man' for usage.");
        return;
    }

    if (CompareString(tokens[1], "help")) {PrintLine("\nThe 'help' command is used to list all of the commands available to the user.\nType 'help' for a list of commands.\n"); return;}
    if (CompareString(tokens[1], "clear")) {PrintLine("\nThe 'clear' command is used to clear the screen of all characters.\nType 'clear' to clear the screen.\n"); return;}
    if (CompareString(tokens[1], "echo")) {PrintLine("\nThe 'echo' command is used to print to the console from command line.\nType 'echo' followed by any number of arguments to print arguments to screen.\nUsage: 'echo Hello World!'\nHello World!\n"); return;}
    if (CompareString(tokens[1], "exit")) {PrintLine("\nThe 'exit' command is used to halt the CPU and effectively exit the kernel.\nType 'exit' to end your session.\n"); return;}
    if (CompareString(tokens[1], "stc")) {PrintLine("\nThe 'stc' command is used to set the color of text on the screen.\nType 'stc' followed by a color to change the text color.\nUsage: 'stc green'\nFor a list of colors type 'stc colors'.\n"); return;}
    if (CompareString(tokens[1], "sbc")) {PrintLine("\nThe 'sbc' command is used to set the color of background on the screen.\nType 'sbc' followed by a color to change the text color.\nUsage: 'sbc blue'\nFor a list of colors type 'sbc colors'.\n"); return;}
    if (CompareString(tokens[1], "man")) {PrintLine("\nThe 'man' command is used to provide a manual for a given command.\nType 'man' followed by a command to view a manual.\nUsage: 'man help'\n"); return;}
    if (CompareString(tokens[1], "snake")) {PrintLine("\nThe 'snake' command is used to eat apples.\nSnakes like apples.\nUsage: 'snake'\n"); return;}
    if (CompareString(tokens[1], "space")) {PrintLine("\nThe 'space' command is used to shoot invaders.\nInvaders from Space!\nUsage: 'space'\n"); return;}
    PrintLine("Invalid argument for 'man' command, type 'help' for a list of commands.");

}
/**
 * Purpose: Echo input to console. Will not print echo in console.
 * << Echo >>
 * @params: 
 *      char* input -> verified input: "echo @"
 * @returns: void
 */
void Echo(char* input)
{
    char** tokens = Split(Strip(input), ' ');
    int i = 1;
    while(tokens[i] != 0)
    {
        Printf("%s ", tokens[i]);
        i++;
    }
    PrintChar('\n');
}
/**
 * Purpose: Set the color of the VGA foreground or text.
 * << STC >>
 * @params: 
 *      char* input -> verified input: "stc @"
 * @returns: void
 */
void STC(char* input)
{
    char** tokens = Split(Strip(Lower(input)), ' ');
    if (!tokens[1])
    {
        PrintLine("No argument supplied for 'stc' command, type 'man stc' for usage.");
        return;
    }
    if(CompareString(tokens[1], "colors")) {PrintTextColorOptions(); return;}
    if (!isValidColor(tokens[1]))
    {
        PrintLine("Invalid color for 'stc' command, type 'stc colors' for a list of colors.");
        return;
    }
    Color new_color = ConvertValidStrToColor(tokens[1]);
    SetAllTextColor(new_color);
}
/**
 * Purpose: Set the color of the VGA background.
 * << SBC >>
 * @params: 
 *      char* input -> verified input: "sbc @"
 * @returns: void
 */
void SBC(char* input)
{
    char** tokens = Split(Strip(Lower(input)), ' ');
    if (!tokens[1])
    {
        PrintLine("No argument supplied for 'sbc' command, type 'man sbc' for usage.");
        return;
    }
    if(CompareString(tokens[1], "colors")) {PrintBackGroundColorOptions(); return;}
    if (!isValidColor(tokens[1]))
    {
        PrintLine("Invalid color for 'sbc' command, type 'sbc colors' for a list of colors.");
        return;
    }
    Color new_color = ConvertValidStrToColor(tokens[1]);
    SetFullBackGroundColor(new_color);
    RemoveHighlights();
}
/**
 * Purpose: Run a given verified command from user input.
 * << ProcessCommand >>
 * @params: 
 *      char* str -> verified input: starts with valid command
 * @returns: void
 */
void ProcessCommand(char* str)
{
    char* copy;
    StrCopy(copy, str);
    char* command = Strip(Split(Strip(Lower(copy)), ' ')[0]);
    
    if (CompareString(command, "help")) {Help(); return;}
    if (CompareString(command, "clear")) {ClearScreen(); return;}
    if (CompareString(command, "echo")) {Echo(str); return;}
    if (CompareString(command, "exit")) {PrintLine("Bye!"); while(1){}} // FIXME: this should exit qemu
    if (CompareString(command, "stc")) {STC(str); return;}
    if (CompareString(command, "sbc")) {SBC(str); return;}
    if (CompareString(command, "man")) {Man(str); return;}
    if (CompareString(command, "snake")) {Printf("Your Highscore: %d\n", Snake(str)); return;}
    if (CompareString(command, "space")) {SpaceInvader(); return;}
}