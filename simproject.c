/******************************************************
 * Simulating Traffic on a mult-hotspot WiFi Network  *
 *                                                    *
 * CS 480/580 - Steven Hohs, Tarif Haque, Lauren Wood *
 * Final Project                                      *
 ******************************************************/

/* Required for the use of simlib.c */
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "simlib.h"

#include <stdio.h>
#include <stdbool.h>
#include "heatmapdata.h"

/* Event types */
#define EVENT_GLOBAL_ARRIVAL    1
#define EVENT_HOTSPOT_ARRIVAL   2
#define EVENT_DEPARTURE         3
#define EVENT_END_SIMULATION    4

#define STREAM_GLOBAL 1
#define STREAM_DEPARTURE 2
#define STREAM_COORDINATE_GENERATION 3

#define HOTSPOT_ID 3
#define ARRIVAL_X  4
#define ARRIVAL_Y  5

#define MATH_PI 3.14159265358979323846 

typedef struct HotSpot {
    int id;
    double base_strength;
    double interarrival_time;
    double stay_time;
    double x, y;
    int capacity;
    int current_users;
} HotSpot;

typedef struct Point {
     double x;
     double y;
} Point;

typedef struct StrengthIdPair {
    double strength;
    int id;
} StrengthIdPair;


FILE *infile = NULL, *outfile = NULL;

#define MAX_NUM_HOTSPOTS 100
#define STRENGTH_COEFFICIENT 1.0
#define NEAR_RADIUS 25.0f /* Make this based on strength */
#define TOTAL_SIMULATION_TIME 100000

int num_hotspots = 1;
int x_size, y_size;
double min_connection_strength = 0.1;
double global_interarrival;
double global_connection_time;
HotSpot hotspots[MAX_NUM_HOTSPOTS];

int successful_connections, attempted_connections;

/* Non sim-lib prototypes */
void print_all_hotspots();
void print_hotspot(struct HotSpot);
int getStream(int);
void cleanup();
void cleanup_on_error(const char*);

/* Get the random event stream ID for a given hotspot */
int getStream(int hotspotID) 
{
    return hotspotID + 3;
}


void print_all_hotspots() 
{
    for (int i = 0; i < num_hotspots; i++) {
        print_hotspot(hotspots[i]); 
    }
}


void print_hotspot(struct HotSpot hotspot) 
{
    printf("id: %i, base strength: %d \n,  ",
            hotspot.id, hotspot.base_strength);
}


bool event_list_empty()
{
    return list_size[LIST_EVENT] <= 0;
}


Point generate_random_coordinate() 
{
    Point point;
    point.x = uniform(0, x_size, STREAM_COORDINATE_GENERATION);
    point.y = uniform(0, y_size, STREAM_COORDINATE_GENERATION);
    return point;
}


Point generate_coordinate_near_hotspot(int id) 
{
    Point point;
    HotSpot *h;
    --id;
    h = &hotspots[id];

    float theta = uniform(0.0f, 2 * MATH_PI, STREAM_COORDINATE_GENERATION);
    float radius = uniform(0.0f, NEAR_RADIUS, STREAM_COORDINATE_GENERATION);
    point.x = h->x + radius * cos(theta);
    point.y = h->y + radius * sin(theta);

    if (point.x < 0) {
        point.x = 0;
    } else if (point.x >= x_size) {
        point.x = x_size - 0.1;
    }

    if (point.y < 0) {
        point.y = 0;
    } else if (point.y >= y_size) {
        point.y = y_size - 0.1;
    }

    return point;
}


void initial_setup()
{
    int h;
    Point p;

    successful_connections = 0;
    attempted_connections = 0;

    init_simlib();

    if (!allocate_connection_table(x_size, y_size)) {
        cleanup_on_error("Unable to allocate data table!");
    }

    /* Schedule the first user arrival for each hotspot */
    for (h = 0; h < num_hotspots; h++) {
        p = generate_coordinate_near_hotspot(hotspots[h].id);
        transfer[ARRIVAL_X] = p.x;
        transfer[ARRIVAL_Y] = p.y;
        transfer[HOTSPOT_ID] = hotspots[h].id;
        event_schedule(sim_time + expon(hotspots[h].interarrival_time, getStream(hotspots[h].id)), EVENT_HOTSPOT_ARRIVAL);   
    }

    /* Schedule a global arrival event at random (x, y) */
    p = generate_random_coordinate();
    transfer[ARRIVAL_X] = p.x;
    transfer[ARRIVAL_Y] = p.y;
    event_schedule(sim_time + expon(global_interarrival, STREAM_GLOBAL), EVENT_GLOBAL_ARRIVAL);    

    /* Schedule the end_simulation event */
    event_schedule(TOTAL_SIMULATION_TIME, EVENT_END_SIMULATION);
    
}


double connection_strength(HotSpot *hotspot, double x, double y)
{
    double dx, dy, distance;
    dx = x - hotspot->x;
    dy = y - hotspot->y;
    distance = dx * dx + dy * dy;
    if (distance < 1.0) {
        /* Prevent the strength approaching infinity as distance approaches 0 */
        return STRENGTH_COEFFICIENT * hotspot->base_strength;
    } else {
        return STRENGTH_COEFFICIENT * hotspot->base_strength / distance;
    }
}


int strength_id_comparator(const void *lhs, const void *rhs)
{
    const StrengthIdPair *lPair = lhs, *rPair = rhs;
    if (lPair->strength < rPair->strength) {
        return -1;
    } else if (lPair->strength > rPair->strength) {
        return 1;
    } else {
        return 0;
    }
}


int user_connection(double x, double y)
{
    StrengthIdPair strengths[MAX_NUM_HOTSPOTS];
    int h;
    
    /* Calculate the strengths at (x, y) */
    for (h = 0; h < num_hotspots; h++) {
        StrengthIdPair *pair = &strengths[h];
        pair->id = h + 1;
        pair->strength = connection_strength(&hotspots[h], x, y);
    }

    /* Sort the StrengthIdPairs in ascending order */
    qsort(strengths, num_hotspots, sizeof(*strengths), &strength_id_comparator);

    /* Iterate over the hotspots in order of descending strength */
    for (h = num_hotspots - 1; h >= 0; h--) {
        int id = strengths[h].id - 1;

        if (strengths[h].strength < min_connection_strength) {
            return -1; /* Signifying a failure to connect to the network */
        } else if (hotspots[id].current_users < hotspots[id].capacity) {
            HotSpot *h = &hotspots[id];
            ++h->current_users;

            transfer[HOTSPOT_ID] = id + 1;
            event_schedule(sim_time + expon(hotspots[id].stay_time, STREAM_DEPARTURE), EVENT_DEPARTURE); 

            return id + 1;
        }
    }

    return -1;
}


void arrival_event_handler(double x, double y)
{
    int connected_id;
    connected_id = user_connection(x, y);

    ++attempted_connections;
    set_user_connection_status(x, y, connected_id > 0);
    if (connected_id > 0) {
        ++successful_connections;
    }
}


void global_user_arrival_handler(double x, double y)
{
    Point p;
    float f;

    arrival_event_handler(x, y);

    // Schedule another user arrival event globally
    p = generate_random_coordinate();
    transfer[ARRIVAL_X] = p.x;
    transfer[ARRIVAL_Y] = p.y;
    f = expon(global_interarrival, STREAM_GLOBAL);
    event_schedule(sim_time + f, EVENT_GLOBAL_ARRIVAL);    
}


void local_user_arrival_handler(int hotspot_id, double x, double y) 
{
    Point p;
    float f;

    arrival_event_handler(x, y);

    p = generate_coordinate_near_hotspot(hotspot_id);
    transfer[ARRIVAL_X] = p.x;
    transfer[ARRIVAL_Y] = p.y;
    transfer[HOTSPOT_ID] = hotspot_id;

    f = expon(global_interarrival, STREAM_GLOBAL);

    event_schedule(sim_time + f, EVENT_HOTSPOT_ARRIVAL);   
}


void user_leaving_handler(int hotspot_id)
{
    --hotspots[hotspot_id - 1].current_users;
}


void end_simulation()
{
    if (fprintf(outfile, "Connection Chance: %f\n", (float)successful_connections / attempted_connections) < 0) {
        cleanup_on_error("Error writing to output file!");
    }
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
    bool done = false;
    
    readFile();

    initial_setup();

    maxatr = 6; 

    while (!event_list_empty() && !done) {
        timing();
        // printf("%d\n", sim_time);

        switch(next_event_type) {
            case EVENT_GLOBAL_ARRIVAL:
                global_user_arrival_handler(transfer[ARRIVAL_X], transfer[ARRIVAL_Y]);
                break;

            case EVENT_HOTSPOT_ARRIVAL:
                local_user_arrival_handler(transfer[HOTSPOT_ID], transfer[ARRIVAL_X], transfer[ARRIVAL_Y]);
                break;

            case EVENT_DEPARTURE:
                user_leaving_handler(transfer[HOTSPOT_ID]);
                break;

            case EVENT_END_SIMULATION:
                end_simulation();
                done = true;
                break;
        }

    }

    cleanup();

    return 0;
}
