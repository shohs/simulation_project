#ifndef HEATMAP_DATA_H
#define HEATMAP_DATA_H

#include <stdbool.h>

void allocate_connection_table(unsigned int max_x, unsigned int max_y);
void free_connection_table();

void set_user_connection_status(float x, float y, bool success);
float get_user_connection_percent(unsigned int x, unsigned int y);

#endif