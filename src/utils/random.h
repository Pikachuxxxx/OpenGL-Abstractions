#pragma once

#include <time.h>
#include <cstdlib>
#include <ctime>

/* Returns a random float in the range of 0 to 1 */
float GetRandomFloatNormalized()
{
    srand((unsigned) time(NULL));
    return ((float) (rand() / RAND_MAX));
}

/* Returns either 0 or 1 in a random fashion */
int GetRandomBool()
{
    srand(time(NULL));
    return rand() % 2;
}

/* Returns a ransom integer in the specified range (inclusive of the range) */
int GetRandomIntInc(int min, int max)
{
    // srand((unsigned) time(0));
    return (min + (rand() % (int)(max - min + 1)));
}

/* Returns a ransom float in the specified range (inclusive of the range) */
float GetRandomFloatInc(float min, float max)
{
    // srand((unsigned) time(0));
    return (min + static_cast<float>(rand()) /( static_cast <float> (RAND_MAX/(max-min))));
}
