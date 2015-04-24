## Simulating Traffic on a Multi-Hotspot WiFi Network
### CS 480: Computer Simulation Final Project
### Steven Hohs, Tarif Haque, Lauren Wood

In this project, we simulate traffic on a WiFi network consisting of several hotspots using the C programming language. 

#### Our model for a WiFi Network deployed over a broad region
When a user arrives in the system, the user will attempt to connect to an available hotspot. Whether or not the user will be able to connect to a hotspot depends on her distance from routers and the number of users already connected to the hotspot. In this system, the signal strength of the hotspot decreases as the user’s distance from it increases. Accordingly, the farther away a user is from the hotspot router, the less likely the user is to establish a connection with that hotspot. Another way that connection can fail is if the hotspot already has the number of users connected equal to its capacity.

Each hotspot will have its own mean user interarrival times and mean time that the user stays connected to the network. These values are independent from the other hotspots. There will also be a global interarrival time. This value is the average interval in which a user can arrive at any point in the system, not exclusively near a hotspot. Additionally, each hotspot has a base signal strength, which determines the range that users will be able to connect to the hotspot.

#### Implementation
The main method is found in *simproject.c* and the project may be compiled in a standard Linux terminal with the following command:

    gcc -Wall -g simproject.c simlib.c -o simproject -lm

To run the simulation, simply run the executable:

    ./simproject

Several functions from the simulation library *simlib.c* were used to abstract the simulation process. We primarily use simlib to schedule events in the event queue and determine the next event via a timing routine. Because we use a discrete-event simulation to model the traffic on the network, we define three events: a global user arrival, a hotspot specific user arrival, and a user departure event. Each of these events is associated with a random number stream following an exponential distribution with a given mean interarrival time. 

