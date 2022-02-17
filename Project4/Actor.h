#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"
#include "GameConstants.h"
#include <iostream>
#include <algorithm>
#include <queue>

class StudentWorld;
class Earth: public GraphObject  //inherit directly from graph object
{
public:
    Earth(int x, int y):GraphObject(TID_EARTH,x,y,right,0.25,3){setVisible(true);}
    virtual void doSomething(){}
    virtual ~Earth(){}
};

class TunnelMan: public GraphObject //inherit directly from grapho object
{
public:
    TunnelMan(StudentWorld *a);
    //mutators
    void adjustSonarCharge(int dx) {sonarCharge += dx;}
    void adjustGoldNuggets(int dx) {goldNuggets += dx;}
    void decreaseHitPoints() {hitPoints -= 1;}
    void adjustWaterSquirt() { waterSquirt += 5;}
    void decreaseWaterSquirt() {waterSquirt -= 1;}
    void setDead() {hitPoints = 0;}
    StudentWorld* getWorld() {return m_studentWorld;}
    
    //acessors
    int getWaterSquirt() {return waterSquirt;}
    int getSonarCharge() {return sonarCharge;}
    int getGoldNuggets() {return goldNuggets;}
    int getHitPoints() {return hitPoints;}
    bool isDead() {if (hitPoints > 0) return false; else return true;}
    
    //virtual functions
    virtual void doSomething();
    virtual ~TunnelMan(){}
private:
    StudentWorld *m_studentWorld;
    int hitPoints;
    int waterSquirt;
    int sonarCharge;
    int goldNuggets;
};

class StudentObject: public GraphObject //all m_obects
{
public:
    //default constructor
    StudentObject(int imageID, int x, int y, Direction pos, double size, unsigned int depth):
    GraphObject(imageID, x, y, pos, size, depth){}
    
    //Mutators
    void setItemName(std::string name) {itemName = name;}
    void setPickUpStatus(int status) {pickUpStatus = status;}
    void setFieldState(int state) {fieldState = state;}
    void addTimeOnField(int dx) {timeOnField += dx;}
    void setDeathStatus (int status) {deathStatus = status;}
    void setTunnelMan(TunnelMan *a) {m_tunnelman = a;}
    
    //Acessors
    int getPickUpStatus() {return pickUpStatus;}
    int getFieldState() {return fieldState;}
    bool isDead() {return deathStatus;}
    bool isTimeUp() {if (timeOnField > 0) return false; else return true;}
    bool falling(Direction d);
    std::string getItemName() {return itemName;}
    TunnelMan* getMan() {return m_tunnelman;}
    
    //virtual functions
    virtual void doSomething(){}
    virtual void annoyed(int decreaseAmount){}
    virtual ~StudentObject(){}
    
private:
    TunnelMan *m_tunnelman;
    int fieldState; //1 for permanent, 2 for temporary
    int pickUpStatus; //0 for none, 1 for Tunnel Man, 2 for Protestors
    int timeOnField = 0; //-1 for regular objects, store actual time for temp state items
    bool deathStatus = false;  //true for dead, false for not dead;
    std::string itemName; //name of object};
};

class SonarKit: public StudentObject //part 3: if invisible, dead
{
public:
    SonarKit(int x, int y, int level);
    virtual void doSomething();
    virtual ~SonarKit(){}
};

class WaterPool: public StudentObject //part 3: if invisible, dead
{
public:
    WaterPool(int x, int y, int level);
    virtual void doSomething();
    virtual ~WaterPool(){}
};

class Boulders: public StudentObject //part 3: if invisible dead
{
public:
    Boulders(int x, int y);
    bool isMoving() {return movingStatus;}
    bool hasEarthUnderneath();
    void setSoundStatus(bool status){soundStatus = status;}
    bool isSoundPlayed() {return soundStatus;}
    void setMovingStatus(bool status) {movingStatus = status;}
    virtual void doSomething();
    virtual ~Boulders(){}
private:
    bool soundStatus = false;
    bool movingStatus = false;
};

class BarrelOfOil: public StudentObject
{
public:
    BarrelOfOil(int x, int y);
    virtual void doSomething();
    virtual ~BarrelOfOil(){}
};

class GoldNuggets: public StudentObject
{
public:
    GoldNuggets(int x, int y, bool visibleStatus, int pickUpStatus);
    virtual void doSomething();
    virtual ~GoldNuggets(){}
};

class Squirt: public StudentObject
{
public:
    Squirt(int x, int y, Direction dir);
    void decreaseDistance() {travelDistance -= 1;}
    bool finishedDistance() {if (travelDistance > 0) return false; else return true;}
    virtual void doSomething();
    virtual ~Squirt(){}
private:
    int travelDistance = 4;
};

struct coordinate
{
    coordinate(int a, int b, BarrelOfOil::Direction c,int d){x = a; y = b; dir = c; level = d;}
    ~coordinate(){}
    int x;
    int y;
    int level;
    BarrelOfOil::Direction dir;
};

struct Node
{
    coordinate pt;
    int length;
};

class RegularProtester: public StudentObject  //game npc and tunnel man have similar features
{
public:
    RegularProtester(TunnelMan *a, int imageID);
    virtual ~RegularProtester(){}
    void setHitPoints(int HitPoints) {hitpoints = HitPoints;}
    int getHitPoints() {return hitpoints;}
    bool leavingOilField(int maze[][64], int xpos, int ypos, int xend = 60, int yend = 60); //queue based maze search
    void numSquaresGenerator(); //set numSquares equal to num of possible steps in current direction
    bool detectTunnelMan(); //if tunnelman is in horizontal or vertical line of signt;
    bool perpendicularMove(){return false;} //random generate if two directions are both good
    bool ifFaceTunnelMan();
    bool moveInCurrentDirection();
    bool pickNewDirection(int locator);//0 for general, 1 for up/down, 2 for left/right
    void takeOneStep(Direction d);
    int getTicks() {return ticks;}
    void adjustTicks(int dx){ticks += dx;}
    int getTicksToWait() {return ticksToWaitBetweenMoves;}
    void clearTicks() {ticks = 0;}
    void clearLastTimeShout() {lastTimeShout = 0;}
    void clearLastTimePerpendicular() {lastTimePerpendicular = 0;}
    void clearNumSquares() {numSquaresToMoveInCurrentDirection = 0;}
    void increaseLastTimeShout() {lastTimeShout += 1;}
    void increaseLastTimePerpendicular() {lastTimePerpendicular += 1;}
    void decreaseNumSquares() {numSquaresToMoveInCurrentDirection -= 1;}
    int getLastTimeShout() {return lastTimeShout;}
    int getLastTimePerpendicular() {return lastTimePerpendicular;}
    int BFC(int maze[][64], coordinate str, coordinate end);
    virtual void doSomething();
    bool scopy(int x, int y, char dir);
    bool setUpMazy();
    void annoyed(int decreaseAmount);
private:
    int maze1[64][64];
    int ticks;
    int numSquaresToMoveInCurrentDirection;
    int ticksToWaitBetweenMoves;
    int hitpoints = 5;
    int lastTimeShout; //number of ticks since last time shouted
    int lastTimePerpendicular;
};


class HardcoreProtester: public RegularProtester
{
public:
    HardcoreProtester(TunnelMan *a,int imageID);
    int BFS(int maze[][64], coordinate str, coordinate end);
    bool scope(int x, int y, char dir);
    bool setUpMaze();
    virtual ~HardcoreProtester(){}
    virtual void doSomething();
private:
    int maze2[64][64];
    int M;
    
};
#endif // ACTOR_H_
