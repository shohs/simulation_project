## Simulating Traffic on a Multi-Hotspot WiFi Network
### CS 480: Computer Simulation Final Project
### Steven Hohs, Tarif Haque, Lauren Wood

In this project, we simulate traffic on a WiFi network consisting of several hotspots using the C programming language. 

#### Our model for a WiFi Network deployed over a broad region
When a user arrives in the system, the user will attempt to connect to one of these hotspots. Whether or not the user will be able to connect to a hotspot depends on their distance from routers and the number of users already connected to the hotspot. In this system, the signal strength of the hotspot decreases as the user’s distance from it increases. Accordingly, the farther away a user is from the hotspot router, the less likely the user is to establish a connection with that hotspot. Another way that connection can fail is if the hotspot already has the number of users connected equal to its capacity.

Each hotspot will have its own mean user interarrival times and mean time that the user stays connected to the network. These values are independent from the other hotspots. There will also be a global interarrival time. This value is the average interval in which a user can arrive at any point in the system, not exclusively near a hotspot. Additionally, each hotspot has a base signal strength, which determines the range that users will be able to connect to the hotspot.

