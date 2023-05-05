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
#include <math.h>
#define DLL_EXPORT extern __declspec(dllexport)
#define false 0
#define true 1
#endif

//The robot ID : six chars unique CID.
//Find it from your CoSpace Robot label or CoSpace program download GUI.
//Don't write the below line into two or more lines.
char AI_MyID[6] = {'1','2','3','4','5','6'};

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
 
void GameStart()
{
//Add your code here

}

 
void AILoopStart()
{
//Add your code here

}


#define CsBot_AI_C//DO NOT delete this line

DLL_EXPORT void SetGameID(int GameID)
{
    if(CurGame != GameID) GameStart();
    CurGame = GameID;
}

DLL_EXPORT int GetGameID()
{
    return CurGame;
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
DLL_EXPORT void GetCommand(int *AI_OUT)
{
    AI_OUT[0] = WheelLeft;
    AI_OUT[1] = WheelRight;
    AI_OUT[2] = LED;
    AI_OUT[3] = MyState;
}
void TurnTo(int curRot, int targetRot)
{
    int turningSpeed;
    int angularErrorThreshold = 2;
    int angleDiff = (curRot - targetRot + 360) % 360;
    if (angleDiff <= angularErrorThreshold || angleDiff > 360 - angularErrorThreshold)
        turningSpeed = 0;
    else if (angleDiff <= 180)
        turningSpeed = angleDiff / 6 + 2;
    else
        turningSpeed = (angleDiff - 360) / 6 - 2;
    WheelLeft = turningSpeed;
    WheelRight = -WheelLeft;
    if (turningSpeed == 0)
        return 0;
    return 1;
}
void Game0()
{

    if(Duration>0)
    {
        Duration--;
    }
    else if(true)
    {
        Duration = 0;
        CurAction =1;
    }
    switch(CurAction)
    {
        case 1:
            WheelLeft=0;
            WheelRight=0;
            LED=0;
            break;
        default:
            break;
    }

}


DLL_EXPORT void AILoop()
{
    AILoopStart();

    switch (CurGame)
    {
        case 9:
            break;
        case 10:
            WheelLeft=0;
            WheelRight=0;
            LED=0;
            MyState=0;
            break;
        case 0:
            Game0();
            break;
        default:
            break;
    }
}

