#include "../include/math.h"
#include "../include/time.h"

void Sleep(double ms)
{
    // idk say 200,000,000 is a second or something
    // FIXME: implement getsystime for better sleep
    int time = Floor((ms/1000.0)*200000000);
    while(time>0){time--;}
}