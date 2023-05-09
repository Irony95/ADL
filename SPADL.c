/////////////////////////////////////////////////////////////////////////
//                                                                     //
//	CoSpace Robot                                                      //
//	Version 2.0.0                                                      //
//	DEC 16 2022                                                        //
//	Copyright (C) 2022 CoSpace Robot. All Rights Reserved              //
//                                                                     //
//  ONLY C Code can be compiled.                                       //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#define CsBot_AI_H//DO NOT delete this line
#ifndef CSBOT_REAL
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define DLL_EXPORT extern __declspec(dllexport)
#define false 0
#define true 1
#endif
int Duration = 0;
int CurAction = -1;
int CurGame = 0;
int US_Front = 0;
int US_Left = 0;
int US_Right = 0;
int PositionX = 0;
int PositionY = 0;
int RotationZ = 0;
int DeltaAngle = 0;
int DeltaDist = 0;
int NavID = 0;
int NavDist = 0;
int CtrID = 0;
int CtrDist = 0;
int StnID = 0;
int StnDist = 0;
int Time = 0;
int MyState = 0;
int WheelLeft = 0;
int WheelRight = 0;
int LED = 0;
int AI_TeamID = 1;   //Robot Team ID.    1:Blue Ream;    2:Red Team.
int AI_SensorNum = 15;
int a[100],b[100],c[100],d[100];
char pointsOfAction[100][100];

char pathing[2500][50][50];
int distanceTo[2500];
int nextPointOfAction = 0; 
int loadedPoints = 0;

int angleToTurn = 0;
int needTurnAfterStation = 0;
int P = 0, I = 0, D = 0, previousError = 0, PID = 0;
//1,1.25,1
int highSpeed = 100;
//higher p = move to cnter faster, d higher is more dampening, higher i = faster reaction to steady state error
//0.5, 0.01, 4
double Kp = 0.95, Ki = 0.01, Kd = 9.5;

//changing dist also means changing turning Cofficient
int curvedTurnStartDist = 20;

//lower = less sharp, higher = more sharp
double turningCoefficient = 0.9;
double startDistance = 0;

#define CsBot_AI_C//DO NOT delete this line

////////////CoSpace System Functions, Please DON't modify./////////////////////
///////////////////////////////////////////////////////////////////////////////

char TaskString[256];//For Individual Challenge ONLY
struct Task
{
    int TaskID;
    int StationID;
    int ActionID;
} TaskList[10];//For Individual Challenge ONLY

DLL_EXPORT int  GetGameID()
{
    return CurGame;
}
DLL_EXPORT int GetCurAction()
{
    return CurAction;
}

DLL_EXPORT void SetTaskString(char* gameTask)
{
    strcpy(TaskString, gameTask);
    printf(gameTask);
    printf("\r\n");
}

DLL_EXPORT void AddTaskItem(int TaskID, int StationID, int ActionID)
{
    if (TaskID < 0 || TaskID >= 10) return;
    TaskList[TaskID].TaskID = TaskID;
    TaskList[TaskID].StationID = StationID;
    TaskList[TaskID].ActionID = ActionID;
    printf("Task:(TaskID:%d, StID:%d, ActID:%d)\r\n", TaskID, StationID, ActionID);
}
//May not work well in Swarmland. 
int TurnTo(int curRot, int targetRot)
{
    int angularErrorThreshold = 10;
    int angleDiff = (curRot - targetRot + 360) % 360;
    int turningSpeed;

    if (angleDiff <= angularErrorThreshold || angleDiff > 360 - angularErrorThreshold)
    {
        Duration = 0;
        turningSpeed = 0;
        return 1;
    }
    else if (angleDiff <= 180)
        turningSpeed = angleDiff / 2 + 3;
    else
        turningSpeed = (angleDiff - 360) / 2 - 3;
        
    WheelLeft = turningSpeed;
    WheelRight = -WheelLeft;
    Duration = 128;
    return 1;
}



//calculates the difference in angle, +\- with respect to start facing
int calculateAngleDiff(int start, int end)
{
    int angleDiff = (end - start);
    if (angleDiff > 180) { return angleDiff - 360; }
    else if (angleDiff < -180) { return angleDiff + 360; }
    return angleDiff;
}


int CurvedTurnTo(int curRot, int targetRot)
{
    int angularErrorThreshold = 20;
    int angleDiff = calculateAngleDiff(curRot, targetRot);

    //if turning is done

    printf("%i\n", angleDiff);
    if (angleDiff <= angularErrorThreshold && angleDiff >= -angularErrorThreshold)
    {
        printf("Finished Turning\n");
        Duration = 0;
        return 1;
    }
    //turning right
    if (angleDiff < 0)
    {
        WheelLeft = highSpeed;
        WheelRight = highSpeed-(abs(angleDiff) * turningCoefficient);
    }
    //turning left  
    else
    {
        WheelLeft = highSpeed-(abs(angleDiff) * turningCoefficient);
        WheelRight = highSpeed;
    }
    Duration = 128;
    return 1;

}
struct DeliveryItem
{
    int ItemID;
    int CenterID;
    int CollectionPtID;
    int Deadline;
    int ItemScore;
    int CurStatus; //1:at Center; 2:on Car; 3:at Station;
} DeliveryItemList[100]; //All Items

DLL_EXPORT void AddDeliveryItem(int ItemID, int CenterID, int StationID, int ItemScore, int Deadline, int CurStatus)
{
    if (ItemID < 0 || ItemID>99) {
        printf("ItemID Error! ID = %d", ItemID);
        return;
    }
    DeliveryItemList[ItemID].ItemID = ItemID;
    DeliveryItemList[ItemID].CenterID = CenterID;
    DeliveryItemList[ItemID].CollectionPtID = StationID;
    DeliveryItemList[ItemID].ItemScore = ItemScore;
    DeliveryItemList[ItemID].Deadline = Deadline;
    DeliveryItemList[ItemID].CurStatus = CurStatus;
}
DLL_EXPORT void RequestItems(int* Items)
{
    for (int i = 0; i < 6; i++)
    {
        Items[i] = -1;
    }
}

static void listout()
{
    /**
     * Extracts the ItemID, CollectionPtID, ItemScore, and Deadline from the first 100 elements of a DeliveryItemList.
     *
     * @param DeliveryItemList A list of delivery items.
     *
     * @returns Four arrays containing the ItemID, CollectionPtID, ItemScore, and Deadline of the first 100 elements of the DeliveryItemList.
     */
    for(int i=0; i<100; i++)
    {
        a[i]=DeliveryItemList[i].ItemID;
        b[i]=DeliveryItemList[i].CollectionPtID;
        c[i]=DeliveryItemList[i].ItemScore;
        d[i]=DeliveryItemList[i].Deadline;
    }
    for(int i=0; i<100; i++)
    {
        if (i>0 && a[i]<=0) continue;
        printf("ItemID : %d | ",a[i]);
        printf("StnID : %d | ",b[i]);
        printf("Score : %d | ",c[i]);
        printf("Deadline : %d | ",d[i]);
        printf("\n");
    }
    printf("**************************\n\n\n");
}
 
void GameStart()
{
//Add your code here
}

void AILoopStart()
{
    printf("%i\n", loadedPoints);  
//Add your code here
    if (!loadedPoints)
    {
        printf("loaded\n");       
        printf("loaded\n");      
        printf("loaded\n");     
        printf("loaded\n");     
        printf("loaded\n");     
        printf("loaded\n");     
        printf("loaded\n");     
        loadedPoints = true;
        FILE *fp;
        fp  = fopen ("path.txt", "r");
        int i = 0;
        int j = 0;
        if (fp != NULL)
        {
            while (!feof(fp))
            {
                char c = fgetc(fp);
                 if (c == '\n')
                {
                    pointsOfAction[i][j+1] =  '\0';
                    i++;
                    j = 0;
                }
                else
                {
                    pointsOfAction[i][j] = c;
                    j++;
                }
            }
            fclose(fp);
        }         
    }
}

 
DLL_EXPORT char* GetTeamName()
{
     return "test3";
}

char info[3000];
DLL_EXPORT char* GetDebugInfo()
{
    sprintf(info, "Duration=%d;CurAction=%d;CurGame=%d;US_Front=%d;US_Left=%d;US_Right=%d;PositionX=%d;PositionY=%d;RotationZ=%d;DeltaAngle=%d;DeltaDist=%d;NavID=%d;NavDist=%d;CtrID=%d;CtrDist=%d;StnID=%d;StnDist=%d;Time=%d;MyState=%d;WheelLeft=%d;WheelRight=%d;LED=%d;",Duration,CurAction,CurGame,US_Front,US_Left,US_Right,PositionX,PositionY,RotationZ,DeltaAngle,DeltaDist,NavID,NavDist,CtrID,CtrDist,StnID,StnDist,Time,MyState,WheelLeft,WheelRight,LED);
    return info;
}
DLL_EXPORT void SetGameID(int GameID)
{
    int i = 0;
    if (CurGame != GameID)
    {
        GameStart();
        for (i = 0; i < 100; i++)
        {
            DeliveryItemList[i].ItemID = i;
            DeliveryItemList[i].CenterID = -1;
            DeliveryItemList[i].CollectionPtID = 0;
            DeliveryItemList[i].Deadline = 0;
            DeliveryItemList[i].ItemScore = 0;
            DeliveryItemList[i].CurStatus = 0;
        }
        for (i = 0; i < 10; i++)
        {
            TaskList[i].TaskID = i;
            TaskList[i].StationID = 0;
            TaskList[i].ActionID = 0;
        }
    }
    CurGame = GameID;
}


DLL_EXPORT void SetDataAI(volatile int* packet, volatile int *AI_IN)
{

    int sum = 0;

    US_Front = AI_IN[0]; packet[0] = US_Front; sum += US_Front;
    US_Left = AI_IN[1]; packet[1] = US_Left; sum += US_Left;
    US_Right = AI_IN[2]; packet[2] = US_Right; sum += US_Right;
    PositionX = AI_IN[3]; packet[3] = PositionX; sum += PositionX;
    PositionY = AI_IN[4]; packet[4] = PositionY; sum += PositionY;
    RotationZ = AI_IN[5]; packet[5] = RotationZ; sum += RotationZ;
    DeltaAngle = AI_IN[6]; packet[6] = DeltaAngle; sum += DeltaAngle;
    DeltaDist = AI_IN[7]; packet[7] = DeltaDist; sum += DeltaDist;
    NavID = AI_IN[8]; packet[8] = NavID; sum += NavID;
    NavDist = AI_IN[9]; packet[9] = NavDist; sum += NavDist;
    CtrID = AI_IN[10]; packet[10] = CtrID; sum += CtrID;
    CtrDist = AI_IN[11]; packet[11] = CtrDist; sum += CtrDist;
    StnID = AI_IN[12]; packet[12] = StnID; sum += StnID;
    StnDist = AI_IN[13]; packet[13] = StnDist; sum += StnDist;
    Time = AI_IN[14]; packet[14] = Time; sum += Time;
    MyState = AI_IN[15]; packet[15] = MyState; sum += MyState;
    packet[16] = sum;

}

static void run()
{
    WheelLeft = 0;
    WheelRight = 0;
    LED = 0;
    P = DeltaDist;
    I = I + DeltaDist;
    D = DeltaDist-previousError;
    PID = (Kp*P) + (Ki*I) + (Kd*D);
    previousError = DeltaDist;
    WheelLeft = highSpeed+PID;
    WheelRight = highSpeed-PID; 
}


DLL_EXPORT void GetCommand(int *AI_OUT)
{
    AI_OUT[0] = WheelLeft;
    AI_OUT[1] = WheelRight;
    AI_OUT[2] = LED;
    AI_OUT[3] = MyState;
}
void Game0()
{
    printf("asdfasfasdfasfd");
    char value[] = {pointsOfAction[nextPointOfAction][1], pointsOfAction[nextPointOfAction][2], '\0'};    
    int pointID = atoi(value);

    char angleStr[] = {pointsOfAction[nextPointOfAction][3], pointsOfAction[nextPointOfAction][4], pointsOfAction[nextPointOfAction][5], '\0'};
    int nextAngle = atoi(angleStr);    
    
    //printf("Currently going to next point at line %i\n", nextPointOfAction+1);  
    if(Duration>0)
    {
        Duration--;
    }
    else if (needTurnAfterStation)
    {
        Duration = 10;
        CurAction = 1;
        needTurnAfterStation = 0;
        
    }
    else if (pointsOfAction[nextPointOfAction][0] == 'D')
    {
        char value1[] = {pointsOfAction[nextPointOfAction][1], pointsOfAction[nextPointOfAction][2], '\0'};
        int id = atoi(value1);
        char value2[] = {pointsOfAction[nextPointOfAction][3], pointsOfAction[nextPointOfAction][4], pointsOfAction[nextPointOfAction][5], '\0'};
        int distanceToId = atoi(value2);
        char value3[] = {pointsOfAction[nextPointOfAction][6], pointsOfAction[nextPointOfAction][7], pointsOfAction[nextPointOfAction][8], '\0'};
        int heading = atoi(value3);

        int headDiff = calculateAngleDiff(RotationZ, heading);
        if (NavID == id && (NavDist-distanceToId) <= 2)
        {
            needTurnAfterStation = 1;
            angleToTurn = heading;
            nextPointOfAction++;
            
        }
        else if (NavID == id && (NavDist-distanceToId) <=curvedTurnStartDist && abs(headDiff) < 90+10)
        {
            angleToTurn = heading;
            Duration = 10;
            CurAction = 2;            

            nextPointOfAction++;
            
        }
        else
        {
            CurAction = 4;
        }
    }

    
    else if(pointsOfAction[nextPointOfAction][0] == 'P' && NavID==pointID && NavDist<=6)
    {
        angleToTurn = nextAngle;
        Duration = 10;
        CurAction = 1;
        

        nextPointOfAction++;
        
    }
    else if(pointsOfAction[nextPointOfAction][0] == 'P' && NavID==pointID && NavDist<=curvedTurnStartDist && NavDist>6 && abs(calculateAngleDiff(RotationZ, nextAngle)) < 90+10)
    {
        angleToTurn = nextAngle;
        Duration = 10;
        CurAction = 2;
        
        nextPointOfAction++;
        
    }
    else if(pointsOfAction[nextPointOfAction][0] == 'S' && StnID == pointID && StnDist>=-7 && StnDist<=7)
    {
        angleToTurn = nextAngle;
        
        Duration = 80;
        CurAction =3;
        nextPointOfAction++;
        
    }
    else if(pointsOfAction[nextPointOfAction][0] == 'C' && CtrID == pointID && CtrDist>=-7 && CtrDist<=7)
    {
        angleToTurn = nextAngle;
        
        Duration = 80;
        CurAction =3;
        nextPointOfAction++;
        
    }
    else if (pointsOfAction[nextPointOfAction][0] == 'B')
    {
        char backDurStr[] = {pointsOfAction[nextPointOfAction][1], pointsOfAction[nextPointOfAction][2], pointsOfAction[nextPointOfAction][3], '\0'};
        Duration = atoi(backDurStr);
        char newAngle[] = {pointsOfAction[nextPointOfAction][4], pointsOfAction[nextPointOfAction][5], pointsOfAction[nextPointOfAction][6], '\0'};
        angleToTurn = atoi(newAngle);
        CurAction = 5;
        nextPointOfAction++;
        
    }
    else if(true)
    {
        Duration = 0;
        CurAction = 4;
    }
    switch(CurAction)
    {
        case 1:
            LED=0;
            TurnTo(RotationZ, angleToTurn);
            break;
        case 2:
            LED=0;
            CurvedTurnTo(RotationZ, angleToTurn);
            break;
        case 3:
            WheelLeft=0;
            WheelRight=0;
            LED=1;
            if (Duration == 1)
            {
                listout();
            }
            if (angleToTurn < RotationZ-30 || angleToTurn > RotationZ+30)
            {
                needTurnAfterStation = 1;
            }
            break;
        case 4:
            run();
            break;
        case 5:
            WheelLeft = -highSpeed;
            WheelRight = -highSpeed;
            
            if (angleToTurn < RotationZ-30 || angleToTurn > RotationZ+30)
            {
                needTurnAfterStation = 1;
            }
        default:
            break;
    }

}


DLL_EXPORT void AILoop()
{
    AILoopStart();
    switch (CurGame)
    {
    case 0:
        Game0();
        break;
    default:
        WheelLeft=0;
        WheelRight=0;
        LED=0;
        MyState=0;
        break;
    }
}


