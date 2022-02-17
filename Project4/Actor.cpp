#include "Actor.h"
#include "StudentWorld.h"
using namespace std;

TunnelMan::TunnelMan(StudentWorld *a):GraphObject(TID_PLAYER, 30, 60, right, 1.0, 0), hitPoints(10), waterSquirt(5),sonarCharge(1), goldNuggets(0)
{
    m_studentWorld = a;
    setVisible(true);
}

SonarKit::SonarKit(int x, int y, int level):StudentObject(TID_SONAR,x,y,right,1.0,2)
{
    setVisible(true);
    setPickUpStatus(1);
    setFieldState(2);
    setItemName("SonarKit");
    addTimeOnField(max(100, 300 - 10 * level));
}

WaterPool::WaterPool(int x, int y, int level):StudentObject(TID_WATER_POOL,x,y,right,1.0,2)
{
    setVisible(true);
    setPickUpStatus(1);
    setFieldState(2);
    setItemName("WaterPool");
    addTimeOnField(max(100, 300 - 10 * level));
}

Boulders::Boulders(int x, int y):StudentObject(TID_BOULDER,x,y,down,1.0,1)
{
    setVisible(true);
    setPickUpStatus(0);
    setItemName("Boulders");
}

BarrelOfOil::BarrelOfOil(int x, int y):StudentObject(TID_BARREL,x,y,right,1.0,2)
{
    setVisible(false);
    setPickUpStatus(1);
    setFieldState(1);
    setItemName("BarrelOfOil");
}

GoldNuggets::GoldNuggets(int x, int y, bool visibleStatus, int pickUpStatus):StudentObject(TID_GOLD,x,y,right,1.0,2)
{
    setVisible(false);
    setPickUpStatus(pickUpStatus);
    if (visibleStatus) {
        setFieldState(2);
        addTimeOnField(100);
    }
    if(!visibleStatus)
        setFieldState(1);
    setItemName("GoldNuggets");
}

Squirt::Squirt(int x, int y, Direction dir):StudentObject(TID_WATER_SPURT,x,y,dir,1.0,1)
{
    setVisible(true);
    setItemName("Squirt");
}

RegularProtester::RegularProtester(TunnelMan *a,int imageID):StudentObject(imageID, 60, 60, left, 1.0, 0)
{
    for (int i = 0; i < 64; i++)
        for (int j = 0; j < 64; j++)
            maze1[i][j] = 0;
    setVisible(true);
    setTunnelMan(a);
    setFieldState(1); //for protesters, permanent state means not leave the field
    setItemName("RegularProtester");
    numSquaresToMoveInCurrentDirection = 60;
    int temp = getMan()->getWorld()->getLevel();
    ticksToWaitBetweenMoves = max(0, 3-temp/4);
    ticks = 0;
    lastTimeShout = 16; //so that first time can shout immediately
    lastTimePerpendicular = 0;
}

HardcoreProtester::HardcoreProtester(TunnelMan *a, int imageID):RegularProtester(a,imageID)
{
    for (int i = 0; i < 64; i++)
        for (int j = 0; j < 64; j++)
            maze2[i][j] = 0;
    setHitPoints(20);
    setItemName("HardcoreProtester");
    int temp = getMan()->getWorld()->getLevel();
    M = 16 + temp*2;
}

bool StudentObject::falling(Direction d)
{
    if (d == down)
        if ((getY()-1) < 0)
            return false;
    if (d == right)
        if ((getX()+1) > 60)
            return false;
    if (d == left)
        if ((getX()-1) < 0)
            return false;
    if (d == up)
        if ((getY()+1) > 60)
            return false;
    if (d == down) { //multi-uses for both boulders (potentially kill tunnelman) and squirt)
        for (int i=getX(); i<getX()+4; i++)
        if(getY()-1 < 60)
            if (getMan()->getWorld()->checkEarth(i, getY()-1) || getMan()->getWorld()->searchArea(i, getY()-1, 0, "Boulders", 0))
                return false;
        moveTo(getX(), getY()-1);
        int tempX = getMan()->getX();
        int tempY = getMan()->getY();
        for (int i=tempX; i<tempX+4; i++)
            for(int j=tempY; j<tempY+4; j++)
                for(int k=getX(); k<getX()+4; k++)
                    for(int l=getY();l<getY()+4; l++)
                        if (i == k && j == l && getItemName() != "Squirt")
                            getMan()->setDead();
    }
    if (d == right) {
        for (int i=getY(); i<getY()+4; i++)
        if (i < 60)
            if (getMan()->getWorld()->checkEarth(getX()+1, i) || getMan()->getWorld()->searchArea(getX()+1, i, 0, "Boulders", 0))
                return false;
        moveTo(getX()+1, getY());
    }
    if (d == left) {
        for (int i=getY(); i<getY()+4; i++)
        if (i < 60)
            if (getMan()->getWorld()->checkEarth(getX()-1, i) || getMan()->getWorld()->searchArea(getX(), i, 0, "Boulders", 0))
                return false;
        moveTo(getX()-1, getY());
    }
    if (d == up) {
        if(getY()+1 > 60)
            return false;
        for (int i=getX(); i<getX()+4; i++)
            if (getMan()->getWorld()->checkEarth(i, getY()+1) || getMan()->getWorld()->searchArea(i, getY()+1, 0, "Boulders", 0))
                return false;
        moveTo(getX(), getY()+1);
    }
    return true;
}

bool Boulders::hasEarthUnderneath()
{
    if(getY() < 1)//stone it at the bottom of the Game, not possible to drop
        return false;
    for (int i = getX();i < getX()-4; i++)
        cout << "";
    return true;
}

bool RegularProtester::ifFaceTunnelMan()
{
    if (getDirection() == right)
        if (getMan()->getY() == getY() && getMan()->getX() >= getX())
            return true;
    if (getDirection() == left)
        if (getMan()->getY() == getY() && getMan()->getX() <= getX())
            return true;
    if (getDirection() == up)
        if (getMan()->getX() == getX() && getMan()->getY() >= getY())
            return true;
    if (getDirection() == down)
        if (getMan()->getX() == getX() && getMan()->getY() <= getY())
            return true;
    return false;
}

bool RegularProtester::detectTunnelMan() //check if TunnelMan is in direct line of sight
{
    if (getX() == getMan()->getX()) { //vertical line of sight
        int tempY1 = getMan()->getY();
        int tempY2 = getY();
        if (getMan()->getY() >= getY()){
            tempY1 = getY();
            tempY2 = getMan()->getY();
        }
        
        for (int i = getX(); i < getX() + 4; i++)
            for (int j = tempY1; j < tempY2 && j < 60
                 ; j++)
                if (getMan()->getWorld()->checkEarth(i, j))
                    return false;
        return true;
    }
    else if (getY() == getMan()->getY()) { //horizontal line of sight
        int tempX1 = getMan()->getX();
        int tempX2 = getX();
        if (getMan()->getX() >= getX()){
            tempX1 = getX();
            tempX2 = getMan()->getX();
        }
        
        for (int i = getY(); i < getY() + 4 && i < 60; i++)
            for (int j = tempX1; j < tempX2; j++)
                if (getMan()->getWorld()->checkEarth(j, i))
                    return false;
        return true;
    }
    else
        return false;
}

void RegularProtester::takeOneStep(Direction d)
{
    if (d == right)
        moveTo(getX()+1, getY());
    if (d == left)
        moveTo(getX()-1, getY());
    if (d == down)
        moveTo(getX(), getY()-1);
    if (d == up)
        moveTo(getX(), getY()+1);
}

bool RegularProtester::moveInCurrentDirection()
{
    if (numSquaresToMoveInCurrentDirection <= 0)
        return false;
    takeOneStep(getDirection());
    numSquaresToMoveInCurrentDirection -= 1;
    return true;
}

bool RegularProtester::pickNewDirection(int locator) //randomly generate directions
{
    int numberOfPossibleDirections = 0; //this is used to determine the probility of new generated direction 1 out of this number
    Direction possibleDirection[4]; //store possible direction to an array
    
    //check left
    if (getY() > 59 && getX()-1 >= 0 && locator != 1){
        possibleDirection[numberOfPossibleDirections] = left;
        numberOfPossibleDirections += 1;
    }
    else if (getX()-1 >= 0 && locator != 1) {
        bool status = true;
        if (getY()+4 > 59) {
            for (int i = getY(); i < 60; i++)
                if (getMan()->getWorld()->checkEarth(getX()-1, i) || getMan()->getWorld()->searchArea(getX()-1, i, 0, "Boulders", 0))
                    status = false;
        }
        else {
            for (int i = getY(); i < getY()+4; i++)
                if (getMan()->getWorld()->checkEarth(getX()-1, i)|| getMan()->getWorld()->searchArea(getX()-1, i, 0, "Boulders", 0))
                    status = false;
                }
        if (status == true){
        possibleDirection[numberOfPossibleDirections] = left;
        numberOfPossibleDirections += 1;
        }
    }
    
    //check right
    if (getY() > 59 && getX()+1 <=60 && locator != 1){
        possibleDirection[numberOfPossibleDirections] = right;
        numberOfPossibleDirections += 1;
    }
    else if (getX()+1 <= 60 && locator != 1){
        bool status = true;
        if (getY()+4 > 59) {
            for (int i = getY(); i < 60; i++)
                if (getMan()->getWorld()->checkEarth(getX()+4, i) || getMan()->getWorld()->searchArea(getX()+4, i, 0, "Boulders", 0))
                    status = false;
        }
        else {
            for (int i = getY(); i < getY()+4; i++)
                if (getMan()->getWorld()->checkEarth(getX()+4, i) || getMan()->getWorld()->searchArea(getX()+4, i, 0, "Boulders", 0))
                    status = false;
        }
        if (status == true){
            possibleDirection[numberOfPossibleDirections] = right;
            numberOfPossibleDirections += 1;
        }
    }

    //check down
    if (getY()-1 >= 0 && locator !=2){
        bool status = true;
        for (int i= getX(); i <getX()+4; i++)
            if (getMan()->getWorld()->checkEarth(i, getY()-1) || getMan()->getWorld()->searchArea(i, getY()-1, 0, "Boulders", 0))
                status = false;
        if (status == true && locator !=2){
        possibleDirection[numberOfPossibleDirections] = down;
        numberOfPossibleDirections += 1;
        }
    }
    
    //check up
    if (getY()+1 > 56 && getY()+1 <=60 && locator != 2){
        possibleDirection[numberOfPossibleDirections] = up;
        numberOfPossibleDirections += 1;
    }
    else if (getY()+1 <=56 && locator != 2){
        bool status = true;
        for (int i = getX(); i < getX()+4; i++)
            if (getMan()->getWorld()->checkEarth(i, getY()+4) ||getMan()->getWorld()->searchArea(i, getY()+4, 0, "Boulders", 0))
                status = false;
        if (status == true){
            possibleDirection[numberOfPossibleDirections] = up;
            numberOfPossibleDirections += 1;
        }
    }
    if (numberOfPossibleDirections == 0)
        return false;
    int x = rand()% numberOfPossibleDirections;
    setDirection(possibleDirection[x]);
    numSquaresGenerator();
    return true;
}

void RegularProtester::numSquaresGenerator()
{
    int tempNum = 0;
    if (getDirection() == right)
    {
        if (getY() > 59)
            tempNum = 60 - getX();
        else if (getY() > 56){
            for (int i = getX() +4 ; i <= 63; i++){
                for (int j = getY(); j < 60; j++)
                    if (getMan()->getWorld()->checkEarth(i, j)){
                        int x = rand()% tempNum+1;
                        numSquaresToMoveInCurrentDirection = x;
                        return;
                    }
                tempNum += 1;
            }
        }

        else {
            for (int i = getX()+4; i <= 63; i++) {
                for (int j = getY(); j < getY()+4; j++)
                    if (getMan()->getWorld()->checkEarth(i, j)){
                        int x = rand()% tempNum+1;
                        numSquaresToMoveInCurrentDirection = x;
                        return;
                    }
                        tempNum += 1;
            }
        }
        int x = rand()% tempNum+1;
        numSquaresToMoveInCurrentDirection = x;
    }
    
    else if (getDirection() == left)
    {
        if (getY() > 59)
            tempNum = getX();
        else if (getY() > 56){
            for (int i = getX() -1 ; i >= 0; i--){
                for (int j = getY(); j < 60; j++)
                    if (getMan()->getWorld()->checkEarth(i, j)){
                        int x = rand()% tempNum+1;
                        numSquaresToMoveInCurrentDirection = x;
                        return;
                    }
                tempNum += 1;
            }
        }
        
        else {
            for (int i = getX()-1; i >= 0; i--) {
                for (int j = getY(); j < getY()+4; j++)
                    if (getMan()->getWorld()->checkEarth(i, j)){
                        int x = rand()% tempNum+1;
                        numSquaresToMoveInCurrentDirection = x;
                        return;
                    }
                tempNum += 1;
            }
        }
        int x = rand()% tempNum+1;
        numSquaresToMoveInCurrentDirection = x;
    }
    
    else if (getDirection() == up){
        if (getY() > 58){
            numSquaresToMoveInCurrentDirection = 1;
            return;
        }
        else {
            for (int j = getY()+4; j < 60; j++){
                for (int i = getX(); i < getX()+4; i++)
                    if (getMan()->getWorld()->checkEarth(i, j)){
                        int x = rand()% tempNum+1;
                        numSquaresToMoveInCurrentDirection = x;
                        return;
                    }
                tempNum += 1;
            }
        }
        if (getY() <= 56)
            tempNum += 4;
        else
            tempNum += 60 - getY();
        int x = rand()% tempNum+1;
        numSquaresToMoveInCurrentDirection = x;
        return;
    }
    
    else if (getDirection() == down){
        for (int j = getY()-1; j >=0; j--){
            for (int i = getX(); i < getX()+4; i++)
                if (getMan()->getWorld()->checkEarth(i, j)){
                    int x = rand()% tempNum+1;
                    numSquaresToMoveInCurrentDirection = x;
                    return;
                }
            tempNum += 1;
        }
        int x = rand()% tempNum+1;
        numSquaresToMoveInCurrentDirection = x;
        return;
    }
}

void RegularProtester::annoyed(int decreaseAmount)
{
    if (decreaseAmount == 1000){
        getMan()->getWorld()->increaseScore(50);
        getMan()->getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        int x = getMan()->getWorld()->getLevel();
        int N = max(50,100-x*10);
        ticks -= N;
        return;
    }
    
    hitpoints -= decreaseAmount;
    if (hitpoints > 0){
        getMan()->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        int x = getMan()->getWorld()->getLevel();
        int N = max(50,100-x*10);
        ticks -= N;
    }
    
    if (hitpoints <= 0 && getFieldState() != 2){
        if (decreaseAmount == 200){
            getMan()->getWorld()->increaseScore(25);
            getMan()->getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        }
        setFieldState(2);
        getMan()->getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        ticks += 1000;
        if (decreaseAmount == 2 && getItemName() == "RegularProtester")
            getMan()->getWorld()->increaseScore(100);
        if (decreaseAmount == 2 && getItemName() == "HardcoreProtester")
            getMan()->getWorld()->increaseScore(250);
        if (decreaseAmount == 100)
            getMan()->getWorld()->increaseScore(500);
    }
}

bool RegularProtester::leavingOilField(int maze[][64], int xpos, int ypos,int xend, int yend)
{
    if (xpos == xend && ypos == yend)
        return true;
    maze[xpos][ypos] = 1;
    //check up
    if (ypos < 63 && maze[xpos][ypos+1] != 1){
        int status = true;
        for (int i = xpos; i < xpos + 4; i++)
            if (getMan()->getWorld()->checkEarth(i, ypos+1))
                status = false;
        if (status == true && leavingOilField(maze, xpos, ypos+1)){
            if (xpos == getX() && ypos == getY()){
                setDirection(up);
                moveTo(getX(), getY()+1);
            }
            return true;
        }
    }
    
    //check right
    if (xpos < 63 && maze[xpos+1][ypos] != 1){
        int status = true;
        for (int j = ypos; j < ypos + 4; j++)
            if (getMan()->getWorld()->checkEarth(xpos+1, j))
                status = false;
        if (status == true && leavingOilField(maze, xpos+1, ypos)){
            if (xpos == getX() && ypos == getY()){
                setDirection(right);
                moveTo(getX()+1, getY());
            }
            return true;
        }
    }
    
    //check left
    if (xpos > 0 && maze[xpos-1][ypos] != 1){
        int status = true;
        for (int j = ypos; j < ypos + 4; j++)
            if (getMan()->getWorld()->checkEarth(xpos-1, j))
                status = false;
        if (status == true && leavingOilField(maze, xpos-1, ypos)){
            if (xpos == getX() && ypos == getY()){
                setDirection(left);
                moveTo(getX()-1, getY());
            }
            return true;
        }
    }
    
    //check down
    if (ypos > 0 && maze[xpos][ypos-1] != 1){
        int status = true;
        for (int i = xpos; i < xpos + 4; i++)
            if (getMan()->getWorld()->checkEarth(i, ypos-1))
                status = false;
        if (status == true && leavingOilField(maze, xpos, ypos-1)){
            if (xpos == getX() && ypos == getY()){
                setDirection(down);
                moveTo(getX(), getY()-1);
            }
            return true;
        }
    }
    return false;
}

bool HardcoreProtester::scope(int x, int y, char dir) //give x, y  return if can move in indicate direction
{
    if (x <=0 || y <= 0 || y >= 61 || x >= 61)
        return false;
    
    if (dir == 'u' && y < 60){
        for (int i=x; i<x+4; i++)
            for(int j=y+1; j<y+5;j++)
                if (getMan()->getWorld()->checkEarth(i, j) || getMan()->getWorld()->checkBoulder(x, y))
                    return false;
        if (maze2[x][y+1] == 1)
            return false;
    }
    else if (dir == 'd' && y > 0){
        for (int i=x; i<x+4; i++)
            for(int j=y-1; j<y+3;j++)
                if (getMan()->getWorld()->checkEarth(i, j) || getMan()->getWorld()->checkBoulder(i, j))
                    return false;
        if (maze2[x][y-1] == 1)
            return false;
    }
    else if (dir == 'r' && x < 60){
        for (int i=x+1; i<x+5; i++)
            for (int j=y; j<y+4; j++)
                if (getMan()->getWorld()->checkEarth(i, j) || getMan()->getWorld()->checkBoulder(i, j))
                    return false;
        if (maze2[x+1][y] == 1)
            return false;
    }
    else if (dir == 'l' && x > 0){
        for (int i=x-1; i<x+3;i++)
            for(int j=y; j<y+4; j++)
                if (getMan()->getWorld()->checkEarth(i, j) || getMan()->getWorld()->checkBoulder(i, j))
                    return false;
        if (maze2[x-1][y] == 1)
            return false;
    }
    
    if (dir == 'u')
        maze2[x][y+1] = 1;
    else if (dir == 'd')
        maze2[x][y-1] = 1;
    else if (dir == 'r')
        maze2[x+1][y] =1;
    else if (dir == 'l')
        maze2[x-1][y] = 1;
    return true;
}

bool RegularProtester::scopy(int x, int y, char dir) //give x, y  return if can move in indicate direction
{
    if (x <=0 || y <= 0 || y >= 61 || x >= 61)
        return false;
    
    if (dir == 'u' && y < 60){
        for (int i=x; i<x+4; i++)
            for(int j=y+1; j<y+5;j++)
                if (getMan()->getWorld()->checkEarth(i, j) || getMan()->getWorld()->checkBoulder(x, y))
                    return false;
        if (maze1[x][y+1] == 1)
            return false;
    }
    else if (dir == 'd' && y > 0){
        for (int i=x; i<x+4; i++)
            for(int j=y-1; j<y+3;j++)
                if (getMan()->getWorld()->checkEarth(i, j) || getMan()->getWorld()->checkBoulder(i, j))
                    return false;
        if (maze1[x][y-1] == 1)
            return false;
    }
    else if (dir == 'r' && x < 60){
        for (int i=x+1; i<x+5; i++)
            for (int j=y; j<y+4; j++)
                if (getMan()->getWorld()->checkEarth(i, j) || getMan()->getWorld()->checkBoulder(i, j))
                    return false;
        if (maze1[x+1][y] == 1)
            return false;
    }
    else if (dir == 'l' && x > 0){
        for (int i=x-1; i<x+3;i++)
            for(int j=y; j<y+4; j++)
                if (getMan()->getWorld()->checkEarth(i, j) || getMan()->getWorld()->checkBoulder(i, j))
                    return false;
        if (maze1[x-1][y] == 1)
            return false;
    }
    
    if (dir == 'u')
            maze1[x][y+1] = 1;
    else if (dir == 'd')
            maze1[x][y-1] = 1;
    else if (dir == 'r')
            maze1[x+1][y] =1;
    else if (dir == 'l')
            maze1[x-1][y] = 1;
    return true;
}

bool HardcoreProtester::setUpMaze()
{
    for (int i=0; i<64; i++)
        for(int j=0; j<64; j++)
            maze2[i][j] = 0;
    
    for (int k=0; k<64; k++)
        for(int l=0; l<64; l++)
            if (getMan()->getWorld()->checkEarth(k, l) || getMan()->getWorld()->checkBoulder(k, l))
                maze2[k][l] = 1;
    
    for (int i=0; i<64; i++){
        for(int j=60; j<64; j++)
            maze2[i][j] = 0;
    }
    maze2[getX()][getY()]= 1;
    return true;
}

bool RegularProtester::setUpMazy()
{
    for (int i=0; i<64; i++)
        for(int j=0; j<64; j++)
            maze1[i][j] = 0;
    
    for (int k=0; k<64; k++)
        for(int l=0; l<64; l++)
            if (getMan()->getWorld()->checkEarth(k, l) || getMan()->getWorld()->checkBoulder(k, l))
                maze1[k][l] = 1;
    
    for (int i=0; i<64; i++){
        for(int j=60; j<64; j++)
            maze1[i][j] = 0;
    }
    maze1[getX()][getY()]= 1;
    return true;
}

int HardcoreProtester::BFS(int maze[][64], coordinate std, coordinate end)
{
    int temp = getMan()->getWorld()->getLevel();
    int T = 16 + temp*2;
    queue<coordinate>BFS;
    if (scope(std.x, std.y, 'u')){
        coordinate up(std.x,std.y+1,GraphObject::up,1);
        BFS.push(up);
    }
    if (scope(std.x, std.y, 'd')){
        coordinate down(std.x,std.y-1,GraphObject::down,1);
        BFS.push(down);
    }
    if (scope(std.x, std.y, 'r')){
        coordinate right(std.x+1,std.y,GraphObject::right,1);
        BFS.push(right);
    }
    if (scope(std.x, std.y, 'l')){
        coordinate left(std.x-1,std.y,GraphObject::left,1);
        BFS.push(left);
    }
    
    while (BFS.size() != 0){
        coordinate curr = BFS.front();
        BFS.pop();
        if (curr.x == end.x && curr.y == end.y)
            if (curr.level <= T || getFieldState() == 2){
                setDirection(curr.dir);
                takeOneStep(curr.dir);
                return curr.level;
            }
        
        if (scope(curr.x,curr.y, 'u')){ //up
            coordinate nw(curr.x,curr.y+1,curr.dir, curr.level+1);
            if (nw.x == end.x && nw.y == end.y){
                if (nw.level <= T || getFieldState() == 2){
                    setDirection(nw.dir);
                    takeOneStep(nw.dir);
                }
                return nw.level;
            }
            BFS.push(nw);
        }
        if (scope(curr.x,curr.y,'d')){ //down
            coordinate nw(curr.x,curr.y-1,curr.dir, curr.level+1);
            if (nw.x == end.x && nw.y == end.y){
                if (nw.level <= T || getFieldState() == 2){
                    setDirection(nw.dir);
                    takeOneStep(nw.dir);
                }
                return nw.level;
            }
            BFS.push(nw);
        }
        if (scope(curr.x,curr.y, 'r')){ //right
            coordinate nw(curr.x+1,curr.y,curr.dir, curr.level+1);
            if (nw.x == end.x && nw.y == end.y){
                if (nw.level <= T || getFieldState() == 2){
                    setDirection(nw.dir);
                    takeOneStep(nw.dir);
                }
                return nw.level;
            }
            BFS.push(nw);
        }
        if (scope(curr.x,curr.y,'l')){ //left
            coordinate nw(curr.x-1,curr.y,curr.dir, curr.level+1);
            if (nw.x == end.x && nw.y == end.y){
                if (nw.level <= T || getFieldState() == 2){
                    setDirection(nw.dir);
                    takeOneStep(nw.dir);
                }
                return nw.level;
            }
            BFS.push(nw);
        }
    }
    return 40000;
}

int RegularProtester::BFC(int maze[][64], coordinate std, coordinate end)
{
    int temp = getMan()->getWorld()->getLevel();
    int T = 16 + temp*2;
    queue<coordinate>BFC;
    if (scopy(std.x, std.y, 'u')){
        coordinate up(std.x,std.y+1,GraphObject::up,1);
        BFC.push(up);
    }
    if (scopy(std.x, std.y, 'd')){
        coordinate down(std.x,std.y-1,GraphObject::down,1);
        BFC.push(down);
    }
    if (scopy(std.x, std.y, 'r')){
        coordinate right(std.x+1,std.y,GraphObject::right,1);
        BFC.push(right);
    }
    if (scopy(std.x, std.y, 'l')){
        coordinate left(std.x-1,std.y,GraphObject::left,1);
        BFC.push(left);
    }
    
    while (BFC.size() != 0){
        coordinate curr = BFC.front();
        BFC.pop();
        if (curr.x == end.x && curr.y == end.y)
            if (curr.level <= T || getFieldState() == 2){
                setDirection(curr.dir);
                takeOneStep(curr.dir);
                return curr.level;
            }
        
        if (scopy(curr.x,curr.y, 'u')){ //up
            coordinate nw(curr.x,curr.y+1,curr.dir, curr.level+1);
            if (nw.x == end.x && nw.y == end.y){
                if (nw.level <= T || getFieldState() == 2){
                    setDirection(nw.dir);
                    takeOneStep(nw.dir);
                }
                return nw.level;
            }
            BFC.push(nw);
        }
        if (scopy(curr.x,curr.y,'d')){ //down
            coordinate nw(curr.x,curr.y-1,curr.dir, curr.level+1);
            if (nw.x == end.x && nw.y == end.y){
                if (nw.level <= T || getFieldState() == 2){
                    setDirection(nw.dir);
                    takeOneStep(nw.dir);
                }
                return nw.level;
            }
            BFC.push(nw);
        }
        if (scopy(curr.x,curr.y, 'r')){ //right
            coordinate nw(curr.x+1,curr.y,curr.dir, curr.level+1);
            if (nw.x == end.x && nw.y == end.y){
                if (nw.level <= T || getFieldState() == 2){
                    setDirection(nw.dir);
                    takeOneStep(nw.dir);
                }
                return nw.level;
            }
            BFC.push(nw);
        }
        if (scopy(curr.x,curr.y,'l')){ //left
            coordinate nw(curr.x-1,curr.y,curr.dir, curr.level+1);
            if (nw.x == end.x && nw.y == end.y){
                if (nw.level <= T || getFieldState() == 2){
                    setDirection(nw.dir);
                    takeOneStep(nw.dir);
                }
                return nw.level;
            }
            BFC.push(nw);
        }
    }
    return 40000;
}


void TunnelMan::doSomething()
{
    if (isDead())
        return;
    int ch;
    getWorld()->getKey(ch);
    switch(ch)
    {
        case KEY_PRESS_LEFT: {
            if (getDirection() == left && getX()>0
                && !getWorld()->searchArea(getX()-1, getY(), 3, "Boulders", 0)){
                moveTo(getX()-1, getY());
                if(getWorld()->removeEarth())
                {
                    getWorld()->playSound(SOUND_DIG);
                }
            }
            else {
                moveTo(getX(), getY());
                setDirection(left);
            }
            break;
        }
        case KEY_PRESS_RIGHT: {
            if (getDirection() == right && getX()<60 && !getWorld()->searchArea(getX()+1, getY(), 3, "Boulders", 0)){
                moveTo(getX()+1, getY());
                if(getWorld()->removeEarth())
                    getWorld()->playSound(SOUND_DIG);
            }
            else {
                moveTo(getX(), getY());
                setDirection(right);
            }
            break;
        }
        case KEY_PRESS_UP: {
            if (getDirection() == up && getY()<60 && !getWorld()->searchArea(getX(), getY()+1, 3, "Boulders", 0)){
                moveTo(getX(), getY()+1);
                if(getWorld()->removeEarth())
                    getWorld()->playSound(SOUND_DIG);
            }
            else {
                moveTo(getX(), getY());
                setDirection(up);
            }
            break;
        }
        case KEY_PRESS_DOWN: {
            if (getDirection() == down && getY()>0 && !getWorld()->searchArea(getX(), getY()-1, 3, "Boulders", 0)){
                moveTo(getX(), getY()-1);
                if(getWorld()->removeEarth())
                    getWorld()->playSound(SOUND_DIG);
            }
            else {
                moveTo(getX(), getY());
                setDirection(down);
            }
            break;
        }
        case KEY_PRESS_ESCAPE: {
            setDead();
            return;
        }
        case KEY_PRESS_SPACE: {
            if (getWaterSquirt() == 0)
                return;
            int x = getX();
            int y = getY();
            /*
            if (getDirection() == down)
                y -= 4;
            if (getDirection() == up)
                y += 4;
            if (getDirection() == right)
                x += 4;
            if (getDirection() == left)
                x -= 4;
             */
            if(getWorld()->checkLaunchStatus(x, y) && getWaterSquirt() > 0){
                StudentObject* newObject = new Squirt (x,y,getDirection());
                newObject->setTunnelMan(this);
                getWorld()->addObjects(newObject);
            }
            decreaseWaterSquirt();
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
            break;
        }
        case 'z':
        case 'Z': {
            if(getSonarCharge()==0)
                break;
            getWorld()->playSound(SOUND_SONAR);
            adjustSonarCharge(-1);
            getWorld()->searchArea(getX(), getY(), 12, "GoldNuggets", 1);
            getWorld()->searchArea(getX(), getY(), 12, "BarrelOfOil", 1);
            break;
        }
        case KEY_PRESS_TAB: {
            if (getGoldNuggets() == 0)
                break;
            StudentObject* newObject = new GoldNuggets(getX(),getY(),true,2);
            newObject->setTunnelMan(this);
            getWorld()->addObjects(newObject);
            adjustGoldNuggets(-1);
            break;
        }
    }
}

void Boulders::doSomething()
{
    if (isDead())
        return;
    if (!isMoving()) {
        for (int i=getX(); i<getX()+4; i++)
            if (getMan()->getWorld()->checkEarth(i, getY()-1))
                return;
        setMovingStatus(true);
        addTimeOnField(30);
        return;
    }
    if (isMoving() && !isTimeUp()) {
        addTimeOnField(-1);
        return;
    }
    if (isMoving() && isTimeUp()){
        if (!isSoundPlayed()) {
            getMan()->getWorld()->playSound(SOUND_FALLING_ROCK);
            setSoundStatus(true);
        }
        if (!falling(down))
            setDeathStatus(true);
        getMan()->getWorld()->stunProtester(getX(), getY(), "RegularProtester", 100);
        getMan()->getWorld()->stunProtester(getX(), getY(), "HardcoreProtester", 100);
    }
}

void Squirt::doSomething()
{
    if (isDead())
        return;
    if (getMan()->getWorld()->searchArea(getX(), getY(), 3, "RegularProtester", 0) ||getMan()->getWorld()->searchArea(getX(), getY(), 3, "HardcoreProtester", 0)){
        if (getMan()->getWorld()->searchArea(getX(), getY(), 3, "RegularProtester", 0))
            getMan()->getWorld()->stunProtester(getX(), getY(),"RegularProtester", 2);
        if (getMan()->getWorld()->searchArea(getX(), getY(), 3, "HardcoreProtester", 0))
            getMan()->getWorld()->stunProtester(getX(), getY(),"HardcoreProtester", 2);
        setDeathStatus(true);
        return;
    }
    if (finishedDistance())
        setDeathStatus(true);
    if (falling(getDirection()))
        decreaseDistance();
    if (!falling(getDirection()))
        setDeathStatus(true);
}

void BarrelOfOil::doSomething()
{
    if (isDead())
        return;
    if (!isVisible())
        if (getMan()->getWorld()->checkOverlap(getX(), getY(), getMan()->getX(), getMan()->getY(),4)){
        setVisible(true);
        return;
        }
    if (getMan()->getWorld()->checkOverlap(getX(), getY(), getMan()->getX(), getMan()->getY(),3)){
        setDeathStatus(true);
        getMan()->getWorld()->playSound(SOUND_FOUND_OIL);
        getMan()->getWorld()->increaseScore(1000);
        getMan()->getWorld()->increaseOil();
    }
}

void GoldNuggets::doSomething()
{
    if (isDead())
        return;
    if (!isVisible())
        if (getMan()->getWorld()->checkOverlap(getX(), getY(), getMan()->getX(), getMan()->getY(),4)){
            setVisible(true);
            return;
        }
    if (getPickUpStatus() == 1)
        if (getMan()->getWorld()->checkOverlap(getX(), getY(), getMan()->getX(), getMan()->getY(),3)){
        setDeathStatus(true);
        getMan()->getWorld()->playSound(SOUND_GOT_GOODIE);
        getMan()->getWorld()->increaseScore(10);
        getMan()->adjustGoldNuggets(1);
        return;
    }
    if (getPickUpStatus() == 2 && isTimeUp()) {
        setVisible(false);
        setDeathStatus(true);
    }
    
    if (getPickUpStatus() == 2 && !isTimeUp()) {
        if(getMan()->getWorld()->stunProtester(getX(), getY(), "RegularProtester", 200))
            setDeathStatus(true);
        if(getMan()->getWorld()->stunProtester(getX(), getY(), "HardcoreProtester", 1000))
            setDeathStatus(true);
        addTimeOnField(-1);
    }
}

void SonarKit::doSomething()
{
    if (isDead())
        return;
    if (getMan()->getWorld()->checkOverlap(getX(), getY(), getMan()->getX(), getMan()->getY(),3)){
        setDeathStatus(true);
        getMan()->getWorld()->playSound(SOUND_GOT_GOODIE);
        getMan()->getWorld()->increaseScore(75);
        getMan()->adjustSonarCharge(1);
        return;
    }
    addTimeOnField(-1);
    if (isTimeUp())
        setDeathStatus(true);
}

void WaterPool::doSomething()
{
    if (isDead())
        return;
    if (getMan()->getWorld()->checkOverlap(getX(), getY(), getMan()->getX(), getMan()->getY(),3)){
        setDeathStatus(true);
        getMan()->getWorld()->playSound(SOUND_GOT_GOODIE);
        getMan()->getWorld()->increaseScore(100);
        getMan()->adjustWaterSquirt();
        return;
    }
    addTimeOnField(-1);
    if (isTimeUp())
        setDeathStatus(true);
}

void RegularProtester::doSomething()
{
    coordinate str(getX(),getY(),right,0);
    coordinate useful(60,60,right,0);
    for(int i=0; i<64; i++)
        for (int j=0; j<64; j++)
            maze1[i][j] = 0;
    if (isDead() && getFieldState() != 2)
        return;
    else if (ticks <= ticksToWaitBetweenMoves) {
        ticks += 1;
        return;
    }
    
    else if (getFieldState() == 2){
        if (getX() == 60 && getY() == 60){
            cout << getX();
            cout << getY();
            setFieldState(-1);
            setDeathStatus(true);
            return;
        }

        else if (setUpMazy() && BFC(maze1, str, useful) <= 1000){
            clearNumSquares();
            clearTicks();
            increaseLastTimeShout();
            increaseLastTimePerpendicular();
        }
        else {
            for(int i=0; i<64; i++)
                for (int j=0; j<64; j++)
                    maze1[i][j] = 0;
            leavingOilField(maze1,getX(),getY());
            increaseLastTimeShout();
            increaseLastTimePerpendicular();
            clearTicks();
            return;
        }
    }
    
    else if (getMan()->getWorld()->checkOverlap(getX(), getY(), getMan()->getX(), getMan()->getY(), 4) && ifFaceTunnelMan() && lastTimeShout > 15){
        getMan()->getWorld()->playSound(SOUND_PROTESTER_YELL);
        getMan()->decreaseHitPoints();
        getMan()->decreaseHitPoints();
        lastTimeShout = 0; //reset ticks to prevent another shout within 15 non-resting ticks
        lastTimePerpendicular += 1;
        ticks = 0; //reset the normal ticks so that protester doesn't do anything until reaches wait limit
        return;
        }
    
    else if (detectTunnelMan() && !getMan()->getWorld()->checkOverlap(getX(), getY(), getMan()->getX(), getMan()->getY(), 4)){
        if (getX() == getMan()->getX()) {
            if (getY() > getMan()->getY()) {
                setDirection(down);
                moveTo(getX(), getY()-1);
            }
            else if (getY() < getMan()->getY()) {
                setDirection(up);
                moveTo(getX(), getY()+1);
            }
        }
        else if (getY() == getMan()->getY()){
            if (getX() > getMan()->getX()){
                setDirection(left);
                moveTo(getX()-1, getY());
            }
            else if (getX() < getMan()->getX()){
                moveTo(getX()+1, getY());
                setDirection(right);
            }
        }
        numSquaresToMoveInCurrentDirection = 0;
        ticks = 0;
        lastTimeShout += 1;
        lastTimePerpendicular += 1;
        return;
        }
    
    else if (!detectTunnelMan()){
        if (lastTimePerpendicular > 200){
            if (getDirection() == up || getDirection() == down){
                if (pickNewDirection(2))
                {
                    takeOneStep(getDirection());
                    numSquaresToMoveInCurrentDirection -= 1;
                    lastTimePerpendicular = 0;
                    ticks = 0;
                    lastTimeShout += 1;
                    return;
                }
            }
            else if (getDirection() == left || getDirection() == right){
                if (pickNewDirection(1))
                {
                    takeOneStep(getDirection());
                    numSquaresToMoveInCurrentDirection -= 1;
                    lastTimePerpendicular = 0;
                    ticks = 0;
                    lastTimeShout += 1;
                    return;
                }
            }
        }
        
        if (!moveInCurrentDirection()){
            pickNewDirection(0);
            takeOneStep(getDirection());
            numSquaresToMoveInCurrentDirection -= 1;
        }
        ticks = 0;
        lastTimeShout += 1;
        lastTimePerpendicular += 1;
        return;
    }
    else {
      ticks = 0;
      lastTimeShout += 1;
      lastTimePerpendicular += 1;
      }
}

void HardcoreProtester::doSomething()
{
    coordinate str(getX(),getY(),right,0);
    coordinate end(getMan()->getX(),getMan()->getY(),right,0);
    coordinate useful(60,60,right,0);
    int temp = getMan()->getWorld()->getLevel();
    int T = 16 + temp*2;
    for(int i=0; i<64; i++)
        for (int j=0; j<64; j++)
            maze2[i][j] = 0;
    if (isDead())
        return;
    
    else if (getTicks() <= getTicksToWait()) {
        adjustTicks(1);
        return;
    }
    
    else if (getFieldState() == 2){
        if (getX() == 60 && getY() == 60){
            setDeathStatus(true);
            return;
        }
        else if (setUpMaze() && BFS(maze2, str, useful) <= 1000){
            clearNumSquares();
            clearTicks();
            increaseLastTimeShout();
            increaseLastTimePerpendicular();
        }
        else {
            for(int i=0; i<64; i++)
                for (int j=0; j<64; j++)
                    maze2[i][j] = 0;
            leavingOilField(maze2,getX(),getY());
            increaseLastTimeShout();
            increaseLastTimePerpendicular();
            clearTicks();
            return;
        }
    }
    
    else if (getMan()->getWorld()->checkOverlap(getX(), getY(), getMan()->getX(), getMan()->getY(), 4) && ifFaceTunnelMan() && getLastTimeShout() > 15){
        getMan()->getWorld()->playSound(SOUND_PROTESTER_YELL);
        getMan()->decreaseHitPoints();
        getMan()->decreaseHitPoints();
        clearLastTimeShout(); //reset ticks to prevent another shout within 15 non-resting ticks
        increaseLastTimePerpendicular();
        clearTicks(); //reset the normal ticks so that protester doesn't do anything until reaches wait limit
        return;
    }
    
    else if (setUpMaze() && BFS(maze2, str, end) <= T){
        clearNumSquares();
        clearTicks();
        increaseLastTimeShout();
        increaseLastTimePerpendicular();
    }
    
    else if (detectTunnelMan() && !getMan()->getWorld()->checkOverlap(getX(), getY(), getMan()->getX(), getMan()->getY(), 4)){
        if (getX() == getMan()->getX()) {
            if (getY() > getMan()->getY()) {
                setDirection(down);
                moveTo(getX(), getY()-1);
            }
            else if (getY() < getMan()->getY()) {
                setDirection(up);
                moveTo(getX(), getY()+1);
            }
        }
        else if (getY() == getMan()->getY()){
            if (getX() > getMan()->getX()){
                setDirection(left);
                moveTo(getX()-1, getY());
            }
            else if (getX() < getMan()->getX()){
                moveTo(getX()+1, getY());
                setDirection(right);
            }
        }
        clearNumSquares();
        clearTicks();
        increaseLastTimeShout();
        increaseLastTimePerpendicular();
        return;
    }
    
    else if (!detectTunnelMan()){
        if (getLastTimePerpendicular() > 50){
            if (getDirection() == up || getDirection() == down){
                if (pickNewDirection(2))
                {
                    takeOneStep(getDirection());
                    decreaseNumSquares();
                    clearLastTimePerpendicular();
                    clearTicks();
                    increaseLastTimeShout();
                    return;
                }
            }
            else if (getDirection() == left || getDirection() == right){
                if (pickNewDirection(1))
                {
                    takeOneStep(getDirection());
                    decreaseNumSquares();
                    clearLastTimePerpendicular();
                    clearTicks();
                    increaseLastTimeShout();
                    return;
                }
            }
        }
        
        if (!moveInCurrentDirection()){
            pickNewDirection(0);
            takeOneStep(getDirection());
            decreaseNumSquares();
        }
        clearTicks();
        increaseLastTimeShout();
        increaseLastTimePerpendicular();
        return;
    }
    else {
        clearTicks();
        increaseLastTimeShout();
        increaseLastTimePerpendicular();
    }
}
