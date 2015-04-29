#include "heatmapdata.h"

#include <stdlib.h>

unsigned int size_x, size_y;
unsigned int *connected = NULL, *attempted = NULL;


unsigned int convert_2d_index(unsigned int x, unsigned int y)
{
    return y * size_x + x;
}


bool allocate_connection_table(unsigned int max_x, unsigned int max_y)
{
    size_x = max_x;
    size_y = max_y;
    
    /* Avoid a potential memory leak. */
    if (connected) {
        free(connected);
    }
    if (attempted) {
        free (attempted);
    }

    connected = calloc(size_x * size_y, sizeof(unsigned int));
    attempted = calloc(size_x * size_y, sizeof(unsigned int));

    if (!connected || !attempted)
    {
        return false;
    }
    return true;
}


void free_connection_table()
{
    if (connected) {
        free(connected);
    }
    if (attempted) {
        free (attempted);
    }  
    connected = NULL;
    attempted = NULL;
}


void set_user_connection_status(float x, float y, bool success)
{
    unsigned int ix, iy;
    ix = (unsigned int)x;
    iy = (unsigned int)y;

    ++attempted[convert_2d_index(ix, iy)];
    if (success) {
        ++connected[convert_2d_index(ix, iy)];
    }
}


float get_user_connection_percent(unsigned int x, unsigned int y)
{
    int index = convert_2d_index(x, y);
    if (!attempted[index]) {
        return 0.0f;
    } else { 
        return (float)connected[index] / attempted[index];
    }
}