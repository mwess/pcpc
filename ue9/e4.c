#include <stdio.h> // for printf
#include <stdlib.h> // for rand

// create random number between min and max (inclusive)
int rand_nr(int min, int max) 
{
    int nr = (rand()/(RAND_MAX + 1.0))*(max+1-min) + min;
    return nr;
}

int main()
{
    srand(42); // initialise rng
    
    int sailorpos = 25,i;
    printf("%d\n", sailorpos + 1);
    
    for (i = 0; i < 50; i++) {
        int units_to_move = rand_nr(1,10);
        int left_or_right = rand_nr(0,1);
        if (left_or_right) {
            sailorpos += units_to_move;
        }
        else {
            sailorpos -= units_to_move;
        }

        // -1 % 51 == -1 so we just always add 51...
        sailorpos = (sailorpos + 51) % 51;
        
        printf("%d\t%d\n", i,sailorpos + 1);
    }

    return 0;
}
