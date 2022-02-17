#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <random>
#include <list>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

bool StudentWorld::isLevelFinished()
{
    int temp = getLevel();
    int L = min(2+temp, 21);
    if (barrelOfOil == L) {
        emptyOil();
        return true;
    }
    else
        return false;
}

void StudentWorld::createObjects(int itemNum, char itemType)
{
    if (itemType == 'W'){
        int x = rand()% 61;
        int y = rand()% 61;
        int pass = false;
        while (pass != true){
            x = rand()% 61;
            y = rand()% 61;
            if (y > 59)
                pass = true;
            else if (y > 56) {
                for (int i = x; i <x+4; i++)
                    for (int j = y; j < 60; j++)
                        if (m_earth[i][j]->isVisible())
                            continue;
            }
            else {
                int count = 0;
                for (int i = x; i < x+4; i++)
                    for (int j=y; j < y+4; j++)
                        if (m_earth[i][j]->isVisible())
                            count += 1;
                if (count == 0)
                    pass = true;
                }
            }
        StudentObject* newObject = new WaterPool(x,y,getLevel());
        newObject->setTunnelMan(m_tunnelman);
        m_objects.push_back(newObject);
        return;
    }

    while (itemNum != 0){
        int x = rand()% 61;
        int y = rand()% 57;
        int status = false;
        std::vector<StudentObject*>::iterator it;
        while ((x >= 27 && x <= 33 && y != 0 ) || status != true) {
            status = true;
            x = rand()% 61; //random 0-60
            y = rand()% 57; //ramdom 0-56
            if (itemType == 'B') {
                if (y < 20) {
                    status = false;
                    continue;
                }
            }
            it = m_objects.begin();
            while (it != m_objects.end()) {
                if (checkOverlap((*it)->getX(), (*it)->getY(), x, y, 6)) //if two boulders not overlap
                    status = false; // leave loop
                ++it;
            }
        }
        if (itemType == 'B') {
            StudentObject* newObject = new Boulders(x,y);
            m_objects.push_back(newObject);
        }
        else if (itemType == 'G'){
            StudentObject* newObject = new GoldNuggets(x,y,true,1);
            m_objects.push_back(newObject);
        }
        else {
            StudentObject* newObject = new BarrelOfOil(x,y);
            m_objects.push_back(newObject);
        }
        --itemNum;
    }
}

bool StudentWorld::removeEarth()
{
    //remove earth occupied by boulders
    std::vector<StudentObject*>::iterator it = m_objects.begin();
    while (it != m_objects.end()) {
        if ((*it)->getItemName() == "Boulders" && (*it)->isVisible()){
            int tempX = (*it)->getX();
            int tempY = (*it)->getY();
            for (int i=tempX; i<tempX+4; i++)
                for (int j=tempY; j<tempY+4; j++)
                    m_earth[i][j]->setVisible(false);
        }
        ++it;
    }
    
    //everytime player moves, remove earth accordingly
    int count = 0;
    int tempX = m_tunnelman->getX();
    int tempY = m_tunnelman->getY();
    if (tempY >= 60)
        return false;
    else if (tempY >= 56 && tempX < 60) {
        for (int i=tempX; i< tempX+4; i++)
            for (int j=tempY; j<60; j++)
                if (m_earth[i][j]->isVisible()){
                    m_earth[i][j]->setVisible(false);
                    ++count;
                }
    }
    else if (tempY >=56 && tempX >= 60) {
        for (int i=tempX; i<64; i++)
            for (int j=tempY; j<60; j++)
                if (m_earth[i][j]->isVisible()){
                    m_earth[i][j]->setVisible(false);
                    ++count;
                }
    }
    else if (tempX >= 60) {
        for (int i=tempX; i<64; i++)
            for (int j=tempY; j<tempY+4; j++)
                if (m_earth[i][j]->isVisible()){
                    m_earth[i][j]->setVisible(false);
                    ++count;
                }
    }
    else
        for (int i=tempX; i<tempX+4; i++)
            for (int j=tempY; j<tempY+4; j++)
                if (m_earth[i][j]->isVisible()){
                    m_earth[i][j]->setVisible(false);
                    ++count;
                }
    return count;
}

bool StudentWorld::checkEarth(int x, int y)
{
    if (m_earth[x][y]->isVisible())
        return true;
    else
        return false;
}

bool StudentWorld::checkOverlap(int tempX, int tempY, int x, int y, int radius)
{
    int dx = tempX - x;
    int dy = tempY - y;
    int dxdy = dx*dx + dy*dy;
    if (dxdy > radius * radius)
        return false;
    else
        return true;
}

bool StudentWorld::checkLaunchStatus(int x, int y)
{
    if (x < 0 || x > 60 || y > 60 || y < 0)
        return false;
    for (int i = x; i < x+4; i++)
        for (int j = y; j < y+4; j++){
            if (i > 63 || j > 59)
                continue;
            if (m_earth[i][j]->isVisible())
                return false;
        }
    for (vector<StudentObject*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
        if ((*it)->getItemName() == "Boulders")
            if (checkOverlap(x, y, (*it)->getX(), (*it)->getY(),3))
                return false;
    return true;
}

bool StudentWorld::searchArea(int x, int y, int radius, string item, int status)
{
    int tempX;
    int tempY;
    
    if (radius == 0) //not radius check but pure check if the image of an item is in the designated coordinator
    {
        for(vector<StudentObject*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
            if ((*it)->getItemName() == item){
                for (int i=(*it)->getX(); i<(*it)->getX()+4; i++)
                    for (int j=(*it)->getY(); j<(*it)->getY()+4; j++)
                        if (i == x && j == y)
                            return true;
            }
        return false;
    }
    
    x += 2; //now start radius check, always check two objects' center (so x and y increase by two)
    y += 2;
    for(vector<StudentObject*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
    {
        if ((*it)->getItemName() == item) {
            tempX = (*it)->getX()+2;
            tempY = (*it)->getY()+2;
            if (((tempX-x)*(tempX-x)+(tempY-y)*(tempY-y)) <= (radius*radius) && status == 0)
                return true;
            if (((tempX-x)*(tempX-x)+(tempY-y)*(tempY-y)) <= (radius*radius))
                (*it)->setVisible(true);
        }
    }
    if(status == 1)  //if set searchArea visible, return true regardless
        return true;
    else             //if search specific item, return true
        return false;
}

string StudentWorld::reportStatus()
{
    string s = "";
    s += "Lvl: ";
    int level = getLevel();
    if (level < 10)
        s+= " ";
        s+= to_string(level);
    s += "  Lives: ";
    int lives = getLives();
    s += to_string(lives);
    s += "  Hlth: ";
    int health = m_tunnelman->getHitPoints()*10;
    if (health < 10)
        s += "  ";
    else if (health < 100)
        s += "";
    s += to_string(health);
    s += "%  Wtr: ";
    int squirts = m_tunnelman->getWaterSquirt();
    if (squirts < 10)
        s += " ";
    s += to_string(squirts);
    s += "  Gld: ";
    int gold = m_tunnelman->getGoldNuggets();
    if (gold < 10)
        s += " ";
    s += to_string(gold);
    s += "  Oil Left: ";
    int temp = getLevel();
    int barrelsLeft = min(2 + temp, 21) - barrelOfOil;
    if (barrelsLeft < 10)
        s += " ";
    s += to_string(barrelsLeft);
    s += "  Sonar: ";
    int sonar = m_tunnelman->getSonarCharge();
    if (sonar < 10)
        s += " ";
    s += to_string(sonar);
    s += "  Scr: ";
    int score = getScore();
    if (score < 10)
        s += "00000";
    else if (score < 100)
        s += "0000";
    else if (score < 1000)
        s += "000";
    else if (score < 10000)
        s += "00";
    else if (score < 100000)
        s += "0";
    s += to_string(score);
    return s;
}

void StudentWorld::randomGenerator()
{
    int G = getLevel() * 25 + 300;
    int v1 = rand()% G; //randomly generate G numbers
    if (v1 == 0) { //only 1/G times v1 can equal 0
        int v2 = rand()% 5; //we want to know the probability of 1/5 and 4/5
        if (v2 == 0){  //add Sonar Kit
            StudentObject* newObject = new SonarKit(0,60,getLevel());
            newObject->setTunnelMan(m_tunnelman);
            addObjects(newObject);
        }
        else
            createObjects(1, 'W');
    }
}

void StudentWorld::removeDeadGameObjects()
{
    vector<StudentObject*>::iterator it = m_objects.begin();
    while (it != m_objects.end()) {
        if ((*it)->isDead()) {
            delete *it;
            it = m_objects.erase(it);
        }
        else
            ++it;
    }
}

bool StudentWorld::stunProtester(int x, int y, string target, int decreaseAmount)
{
    for (vector<StudentObject*>::iterator it = m_objects.begin(); it != m_objects.end(); it++){
        if ((*it)->getItemName() == target && checkOverlap(x, y, (*it)->getX(), (*it)->getY(), 3) && (*it)->getFieldState() != 2) {
            (*it)->annoyed(decreaseAmount);
            if (decreaseAmount == 200 || decreaseAmount == 1000)
                return true;
        }
    }
    return false;
}

int StudentWorld::countProtesters()
{
    int count = 0;
    for (vector<StudentObject*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
    {
        if ((*it)->getItemName() == "RegularProtester" || (*it)->getItemName() == "HardcoreProtester")
            ++count;
    }
    return count;
}

int StudentWorld::init()
{
    //initialize tunnel man
    m_tunnelman = new TunnelMan(this);
    
    //initialize Earth objects using 2-D array
    for (int i=0; i<64; i++)
        for (int j=0; j<64; j++)
                m_earth[i][j] = new Earth(i,j);
    for (int i = 30; i < 34; i++)
        for (int j = 4; j < 60; j++)
            m_earth[i][j]->setVisible(false);
    for (int i=0; i <64; i++)
        for(int j = 60; j <64; j++ )
            m_earth[i][j]->setVisible(false);
    //initialize barrel of oils, gold nuggets, and boulders
    int temp = getLevel();
    int B = min(temp/2+2,9);
    int G = max(5-temp/2,2);
    int L = min(2+temp, 21);
    createObjects(B, 'B');
    createObjects(G, 'G');
    createObjects(L, 'L');
    for (vector<StudentObject*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
        (*it)->setTunnelMan(m_tunnelman);
    removeEarth();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    ticks += 1;
    setGameStatText(reportStatus());
    m_tunnelman->doSomething();
    randomGenerator();
    int K = 2 + getLevel() * 1.5;
    if (K > 15)
        K = 15;
    int T = 200 - getLevel();
    if (T < 25)
        T = 25;
    if (ticks >= T && countProtesters() < K)
    {
        int probability = fmin(90, getLevel()*10+30);
        probability = probability / 10;
        int x = rand()% 10;
        if (x < probability)
            m_objects.push_back(new HardcoreProtester(m_tunnelman,TID_HARD_CORE_PROTESTER));
        else
            m_objects.push_back(new RegularProtester(m_tunnelman,TID_PROTESTER));
        ticks = 0;
    }
    
    for(vector<StudentObject*>::iterator it = m_objects.begin(); it != m_objects.end(); it++) {
        (*it)->doSomething();
        if (m_tunnelman->isDead()) {
            playSound(SOUND_PLAYER_GIVE_UP);
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
    }
    removeDeadGameObjects();
    if (isLevelFinished()){
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    vector<StudentObject*>::iterator it = m_objects.begin();
    while (it != m_objects.end()) {
        delete *it;
        it = m_objects.erase(it);
    }
    
    for (int i=0; i<64; i++)
        for(int j=0; j<64; j++)
            delete m_earth[i][j];
    delete m_tunnelman;
}

bool StudentWorld::checkBoulder(int x, int y)
{
    for (vector<StudentObject*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
    {
        if ((*it)->getItemName() == "Boulders" && !(*it)->isDead() && (*it)->getFieldState() != 0)
        {
            for (int i=(*it)->getX(); i < (*it)->getX()+4; i++)
                for (int j=(*it)->getY(); j < (*it)->getY()+4; j++)
                    if (i == x && j == y)
                        return true;
        }
    }
    return false;
}
