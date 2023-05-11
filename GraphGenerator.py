import math
from tqdm import tqdm

nodes = {}
maxDist = 9999999
exportDelimiter = ","
#the two points indicating the route, and the coefficient. 
routeCoefficient = [
    ("P13", "P12", 1.3),
    ("P13", "P10", 1.3),
    ("P11", "P12", 1.3),
    ("P11", "P10", 1.3),
    ("P25", "P24", 1.3),
    ("P25", "P26", 1.3),
    ("P27", "P26", 1.3),
    ("P27", "P24", 1.3),

    ("P21", "S12", 1.4),
    ("P02", "P09", 1.4),
    ("P08", "P28", 1.4),
]
#if we turn more than this angle, we add the coefficient
minTurnCoefficient = 30
turnCoefficient = 1.2

class Node:
    def __init__(self, x, y, identifier) -> None:
        self.x = x
        self.y = y
        self.identifier = identifier
        self.neighbours = []

        self.direction = {}
        self.distanceTo = {}


    def addNeighbour(self, neighbour):
        self.neighbours.append(neighbour)

    def EuclideanDist(self, neighbour):
        xDist = abs(self.x - nodes[neighbour].x)
        yDist = abs(self.y - nodes[neighbour].y)
        pureDist = math.sqrt(math.pow(xDist,2) + math.pow(yDist, 2))

        #multiply by any coefficients, as the route might take longer
        for coeff in routeCoefficient:
            if ((self.identifier == coeff[0] and neighbour == coeff[1]) or 
                (self.identifier == coeff[1] and neighbour == coeff[0])):                
                    pureDist = pureDist*coeff[2]
        return pureDist
    
    def AngleTo(self, point):
        xDist = abs(self.x - nodes[point].x)
        yDist = abs(self.y - nodes[point].y)
        acuteAngle = round(math.atan2(yDist, xDist) * 180/math.pi)
        #quadrant 1 
        if (nodes[point].x >= self.x and nodes[point].y >= self.y):
            return 90+acuteAngle
        #quadrant 2
        elif (nodes[point].x <= self.x and nodes[point].y >= self.y):
            return 270-acuteAngle
        #quadrant 3
        elif (nodes[point].x <= self.x and nodes[point].y <= self.y):
            if (270+acuteAngle >= 360):
                return 0
            return 270+acuteAngle
        #quadrant 4
        return 90-acuteAngle
        




def main():   
    global nodes 
    populateNodes()

    for startingLocation in tqdm(nodes):
        #dikstras algorithmn
        locations = nodes.copy()
        dist = {}
        prev = {}
        for node in locations:
            dist[node] = maxDist
            prev[node] = ""
        dist[startingLocation]  = 0

        while(len(locations) != 0):
            #set u as the identifier of the shortest dist still in locations
            u = None        
            for loc in locations.values():
                if (u == None):
                    u = loc.identifier
                elif(dist[loc.identifier] < dist[u]):
                    u = loc.identifier

            locations.pop(u)

            for neighbour in nodes[u].neighbours:
                alt = dist[u] + nodes[u].EuclideanDist(neighbour)

                #multiply with a turn coefficient if the car needs to turn(it takes longer to turn)
                if (u in prev and prev[u] in prev):
                    prevAngle = nodes[prev[u]].AngleTo(u)
                    currentAngle = nodes[u].AngleTo(neighbour)
                    if (abs(calculateAngleDiff(prevAngle, currentAngle)) > minTurnCoefficient):                    
                        alt *= turnCoefficient
                        
                if (neighbour in locations and alt < dist[neighbour]):
                    dist[neighbour] = alt
                    prev[neighbour] = u
        
        for node in prev:
            #only add the pathing if the path is not already discovered
            if (startingLocation not in nodes[node].direction and startingLocation != node):
                pathing = []
                u = node
                while (u != startingLocation):
                    pathing.insert(0, u)
                    u = prev[u]
                pathing.insert(0, u)
                nodes[startingLocation].direction[node] = pathing.copy()
                pathing.reverse()
                nodes[node].direction[startingLocation] = pathing.copy()

                nodes[node].distanceTo[startingLocation] = round(dist[node])
                nodes[startingLocation].distanceTo[node] = round(dist[node])
            elif (startingLocation == node):
                nodes[node].direction[startingLocation] = [node]
                nodes[node].distanceTo[startingLocation] = 0

    exportNodes();
            
            
def populateNodes():
    global nodes
    with open("pointsPosition.txt") as f:
        lines = f.readlines()

        #create the node
        for line in lines:
            line = line[:-1]
            if (len(line) == 0 or line[0] == "#"):
                continue

            locations = (line.split(" | ")[0]).split(" ")
            identifier = locations[0]
            x = int(locations[1])
            y = int(locations[2])
            node = Node(x,y,identifier)
            nodes[identifier] = node


        #add the neighbours
        for line in lines:
            line = line[:-1]
            if (len(line) == 0 or line[0] == "#"):
                continue

            identifier = line[:3]
            locations = (line.split(" | ")[1]).split(",")
            for neighbour in locations:
                nodes[identifier].addNeighbour(neighbour)

def exportNodes():
    file = open("pathing.txt", "w")
    file.write("const char *pathings[][50] = {\n")
    for k, start in enumerate(nodes):
        if (start[0] == "P"):
            continue
        for j, end in enumerate(nodes[start].direction):
            truePath = "{"
            previousAngle = -360
            for i, point in enumerate(nodes[start].direction[end]):


                if (i == len(nodes[start].direction[end])-1):
                    truePath += '"' + point + "180" + '"'
                else:
                    angle = nodes[point].AngleTo(nodes[start].direction[end][i+1])

                    #we do not bother to add the point, as we do not need to turn at this angle
                    if (abs(angle - previousAngle) < minTurnCoefficient):
                        previousAngle = angle
                        continue
                    previousAngle = angle

                    angle = str(angle)
                    #make sure the angle is 3 digit long
                    while (len(angle) < 3):
                        angle = "0" + angle

                    truePath +=  '"' + point + angle + '",' 

            file.write(truePath + "},")
        file.write("\n")

    file.write("};")
    file.close()


    file = open("distances.txt", "w")
    file.write("const int dist[] = {")
    for k, start in enumerate(nodes):
        if start[0] == "P":
            continue
        for end, dist in nodes[start].distanceTo.items():
            if (end[0] == "P'"):
                continue
            file.write(str(dist) + ",")
        file.write("\n")
    file.write("};")
    file.close()
                               

def calculateAngleDiff( start, end):

    angleDiff = (end - start)
    if (angleDiff > 180):
        return angleDiff - 360
    elif (angleDiff < -180):
        return angleDiff + 360
    return angleDiff

if (__name__ == "__main__"):
    main()