# notes

there are currently 2 main branches. main, which is the original code where the car drove as fast as possible making dangerous turns, and safe, which contains a very safe version of the program that we ran on the day of competition. Only the main branch is built for the learn map. 

# How to use

## step 1
Drive to, and record down the x and y value of every single location on the map, and what nodes they are connected to.
For example, Station 14 is located at x - 1250 and y - 900, and connected to Point 22, and Point 20. To denote this, we record it down as 

```S14 1250 900 | P22,P20```

The first letter dictate if the node is a Station(S), point(P), or Distrobution center(C). The number following is the node's ID. The next two number is the x and y location respectively. A vertical line follows, then all the points that are connected to the point defined. The full list of every point is written down in the file, pointsPosition.txt

As an additional notes,
1. you need to make sure there are 3 total Distrobution centers written down, and then followed by all the stations. 

2. Distrobution center and collection stations need to be in order from lowest to highest. An example is shown in pointsPosition.txt

3. the last line needs to have an empty new line, due to a quirk with python

If the map has less than 3 total Distrobution centers, you can just make one up that is really far away, and only connected to C01, so it will never be visited. (Make sure that C01 also knows it's connected to the fake C03) I.e.

```C03 99999 99999 | C01```

## step 2
edit the following:

1. edit the route coefficient array to include all the points that you want to slow down at. This could be at a roundabout, or an overhead bridge

2. edit the roundaboutPoints array, to include all the points of the roundabout, as there are special code handling those points

3. At the function RoundaboutAngleTo located at line 78, edit all the points to be those of the roundabouts, according to lower, upper, left and right.

Run the code and the output should be located at pathings.txt

## step 3
copy and paste the 3 variables generated in pathings.txt into the C file, lines 68 to 108. Then, compile and run the code.

If the code spends too long trying to deliver all the packages, increase the value of the double bruteForceTimeCoefficient. If the code doesn't go to enough stations before collecting again, decrease the double.


# The technique
By recording all the locations within the map, we can generate a graph of nodes and edges. Then by doing dijkstra's Algorithmn, we run through every single possible node in the graph, and generate the shortest path to every other location and the distance taken to reach that location. We store this large amount of information in a 2D array of strings, named pathings. pathings store all the nodes that we need to turn at, for every nodes available on the map.

The nodes are written in a way that shows the ID of the node(first 3 characters), and the final headings(last 3 characters). I.e. "S01090" means that at "S01", station 1, we should end up facing 90 degrees. "P18270" means that at P18, point 18, we should face angle 270.

Below is an example of pathings

```
{
    {"C01180"}, {"C01180", "S01180"},
    {"S01000", "C01180"}, {"S01180"}
}
```
The first array indicates the shortest path from C01 to C01. The second indicates the shortest path from C01 to S01. The Third is S01 to C01 and so on. 


Once we have all of the shortest path from one point to another, We just have to figure out what locations to visit when at the distrobution center. To do this, we brute force through the entire array of packages on board, including going back to C01.
Then, we calculate the approximation of the amount of points we can gain for following the path and the total time taken. 
Then, we calculate a formula for the worth of this route, where ```worth = points - k * time```
and k is the variable bruteForceTimeCoefficient in the C code. 

The highest value of worth is chosen, and the car will follow along the route specified.

e.g.

There are 2 stations available to visit, S01 and S02, given that\
S01 gives 90 points and S02 gives 10 points.\
S01 takes 3 seconds to reach, and S02 takes 17 seconds to reach.\
the current value of k is 2.

The possible permutations are:

* S01 S02 C01 ------ worth = 100 - 2 * 20 = 60
* S01 C01 ------ worth = 90 - 2 * 3 = 84
* S02 S01 C01 ------ worth = 100 - 2 * 20 = 60
* S02 C01 ------ worth = 10 - 2 * 17 = -24

since Route 2 has the highest worth of 84, we follow along route 2. This method repeats everytime we reach the collection center to collect new parcels. It runs until the end of the game, or crashes.