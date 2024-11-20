#include "include/console.h"
#include "include/portmap.h"
#include "include/keyboard.h"
#include "include/bool.h"
#include "include/cmds.h"

/*
 * TODO: 
 *  FIXME: add pages support for scroll up/down
 *  FIXME: seperate command line and text editor functionality and way to change between them
 */
void main(void)
{
    ClearScreen(); 
    SetTextColor(GREEN);
    SetFullBackGroundColor(BLACK);
    PrintStringColor("Warning: The 'space' command will corrupt the operating system.\n\n", RED);
    while(True)
    {
        // TODO: implement PWD
        PrintString("> ");
        char* input = GetInput();
        if (isValidCommand(input))
        {
            ProcessCommand(input);
        }
    }
}
