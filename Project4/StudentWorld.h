#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "GameWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <string>
#include <vector>

class TunnelMan;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir): GameWorld(assetDir){}
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void createObjects(int itemNum,char itemType); //create objects for oil,gold,and,boulders
    void addObjects(StudentObject *a) {m_objects.push_back(a);}
    void increaseOil() {barrelOfOil += 1;}
    int getBarrelOfOil() {return barrelOfOil;}
    bool checkOverlap(int tempX, int tempY, int x, int y, int radius);
    bool removeEarth(); //remove Earth Object, return true when player stepped on earth
    bool checkLaunchStatus(int x, int y); //for squirt
    bool checkEarth(int x, int y);
    bool checkBoulder(int x, int y);
    bool searchArea(int x, int y, int radius,std::string item, int status);//status: 0 for pure search, 1 for visible search
    bool isLevelFinished();
    void emptyOil() {barrelOfOil = 0;}
    void removeDeadGameObjects();
    void randomGenerator();
    int countProtesters();
    bool stunProtester(int x, int y, std::string target, int decreaseAmount);
    std::string reportStatus();
    
private:
    Earth* m_earth[64][64];
    TunnelMan* m_tunnelman;
    std::vector<StudentObject*> m_objects;
    int barrelOfOil = 0;
    int ticks = 10000;
};

#endif // STUDENTWORLD_H_
