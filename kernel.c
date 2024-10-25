#include "include/math.h"
#include "include/console.h"

void main(void)
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
    main();
}
