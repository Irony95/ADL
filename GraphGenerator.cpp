#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class Node 
{
    string identifier;
    int x,y;
    Node* neighbours;
};

//returns the length of the node
int createNodes(Node* nodes, string textfile)
{
    ifstream MyReadFile(textfile);
    string text;
    int amountOfNodes = 0;
    while (getline(MyReadFile, text))
    {
        Node node = 
        amountOfNodes++;

    }
    return amountOfNodes;
}

Node* nodes;
int main()
{
    createNodes(nodes, "pointsPosition.txt");
}

