/* Required for the use of simlib.c */
#include <stdarg.h>
#include "simlib.h"

#include <stdio.h>

/* Event types */
#define EVENT_GLOBAL_ARRIVAL    1
#define EVENT_HOTSPOT_ARRIVAL   2
#define EVENT DEPARTURE         3

typedef struct HotSpot {
  int id;
  double base_strength;
  double interarrival_time;
  double stay_time;
  double x, y;
  int capacity;
  int current_users;
} HotSpot;

#define MAX_NUM_HOTSPOTS 100
#define STRENGTH_COEFFICIENT 1.0
int num_hotspots = 1;
int x_size, y_size;
double min_connection_strength = 0.1;
HotSpot hotspots[MAX_NUM_HOTSPOTS];

void initial_setup()
{
  int h;
  for (h = 0; h < num_hotspots; h++) {
    // Schedule an arrival event near hotspot h
  }

  // Schedule a global arrival event at random (x, y)
  // Schedule the End_Simulation event
}


double connection_strength(HotSpot *hotspot, double x, double y)
{
  double dx, dy, distance;
  dx = x - hotspot->x;
  dy = y - hotspot->y;
  distance = dx * dx + dy * dy;
  if (distance < 1) {
    // Prevent the strength approaching infinity as distance approaches 0
    return STRENGTH_COEFFICIENT * hotspot->base_strength;
  } else {
    return STRENGTH_COEFFICIENT * hotspot->base_strength / distance;
  }
}


int user_connection(double x, double y)
{
  double strengths[MAX_NUM_HOTSPOTS];
  int h;
  
  // Calculate the strengths at (x, y)
  for (h = 0; h < num_hotspots; h++) {
    strengths[h] = connection_strength(&hotspots[h], x, y);
  }

  while (1) {
    int hotspot_id = -1;

    // Find the hotspot with the next highest signal strength at (x, y)

    if (hotspot_id == -1 || strengths[hotspot_id] < min_connection_strength) {
      return -1; // Signifying a failure to connect to the network
    } else if (hotspots[hotspot_id].current_users < hotspots[hotspot_id].capacity) {
      HotSpot *h = &hotspots[hotspot_id];
      ++h->current_users;

      // Schedule a user leaving event for hotspot at hotspot_id
    }
  }
}


void global_user_arrival_handler(double x, double y)
{
  int connected_id;
  // get (x, y)
  // Schedule another user arrival event globally
  connected_id = user_connection(x, y);
}


void user_leaving_handler(int hotspot_id)
{
  --hotspots[hotspot_id].current_users;
}


void cleanup()
{

}


int main()
{
  printf("test\n");
}
