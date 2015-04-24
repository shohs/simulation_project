/* Required for the use of simlib.c */
#include <stdlib.h>
#include <stdarg.h>
#include "simlib.h"

#include <stdio.h>
#include <stdbool.h>

/* Event types */
#define EVENT_GLOBAL_ARRIVAL    1
#define EVENT_HOTSPOT_ARRIVAL   2
#define EVENT_DEPARTURE         3


typedef struct HotSpot {
  int id;
  double base_strength;
  double interarrival_time;
  double stay_time;
  double x, y;
  int capacity;
  int current_users;
} HotSpot;


FILE *infile = NULL, *outfile = NULL;

#define MAX_NUM_HOTSPOTS 100
#define STRENGTH_COEFFICIENT 1.0
int num_hotspots = 1;
int x_size, y_size;
double min_connection_strength = 0.1;
double global_interarrival;
double global_connection_time;
HotSpot hotspots[MAX_NUM_HOTSPOTS];

void print_all_hotspots();
void print_hotspot(struct HotSpot);

void print_all_hotspots() {
    for (int i = 0; i < num_hotspots; i++) {
        print_hotspot(hotspots[i]); 
    }
}

void print_hotspot(struct HotSpot hotspot) {
    printf("id: %i, base strength: %d \n",
            hotspot.id, hotspot.base_strength);
}

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
  if (infile) {
    fclose(infile);
  }

  if (outfile) {
    fclose(outfile);
  }
}


void cleanup_on_error(const char *message)
{
  fprintf(stderr, "%s\n", message);

  cleanup();

  exit(1);
}


void readFile()
{
  int i;

  infile = fopen("wifi.in", "r");
  if (!infile) {
    cleanup_on_error("Unable to open \"wifi.in\"\n");
  }

  outfile = fopen("wifi.out", "w");
  if (!outfile) {
    cleanup_on_error("Unable to open \"wifi.out\"\n");
  }

  /* Read in global simulation configuration parameters */
  if (fscanf(infile, "%d %d %d %lf %lf", &x_size, &y_size, &num_hotspots, &global_interarrival, &global_connection_time) != 5) {
    cleanup_on_error("Error reading input file!");
  }

  if (num_hotspots > MAX_NUM_HOTSPOTS) {
    num_hotspots = MAX_NUM_HOTSPOTS;
  }

  /* Read in configuration paramters for each hotspot */
  for (i = 0; i < num_hotspots; i++) {
    HotSpot *h = &hotspots[i];
    h->id = i + 1;
    if (fscanf(infile, "%lf %lf %lf %lf %lf %d", &h->x, &h->y, &h->base_strength, &h->interarrival_time, &h->stay_time, &h->capacity) != 6) {
      cleanup_on_error("Error reading input file!");
    }
  }

  if (fprintf(outfile, "Simulation Output:\n") < 0) {
    cleanup_on_error("Error writing to output file!");
  }
}

int main()
{
  readFile();

  print_all_hotspots();

  cleanup();

  return 0;
}
