import math
from tqdm import tqdm

nodes = {}
maxDist = 9999999
exportDelimiter = ","
#the two points indicating the route, and the coefficient. 
routeCoefficient = [
    ("P13", "P12", 1.6),
    ("P13", "P10", 1.6),
    ("P11", "P12", 1.6),
    ("P11", "P10", 1.6),
    ("P25", "P24", 1.6),
    ("P25", "P26", 1.6),
    ("P27", "P26", 1.6),
    ("P27", "P24", 1.6),

    ("P21", "S12", 1.4),
    ("P02", "P09", 1.4),
    ("P08", "P28", 1.4),
]
minTurnCoefficient = 50
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
            if (self.identifier == coeff[0] or self.identifier == coeff[1]):
                if (neighbour == coeff[0] or neighbour == coeff[1]):                    
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
    for start in tqdm(nodes):
        for end in nodes[start].direction:
            truePath = ""
            for i, point in enumerate(nodes[start].direction[end]):
                if (i == len(nodes[start].direction[end])-1):
                    truePath += exportDelimiter + point + "180"
                else:
                    #make sure the angle is 3 digit long
                    angle = str(nodes[point].AngleTo(nodes[start].direction[end][i+1]))
                    while (len(angle) < 3):
                        angle = "0" + angle

                    truePath += exportDelimiter + point + angle


            file.write(f"{start}:{end}: " + truePath)
            file.write("\n")
                
            
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