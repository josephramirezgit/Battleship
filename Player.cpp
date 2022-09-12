#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <stack>
#include <chrono>
#include <vector>
#include <queue>

using namespace std;

    
bool putShipsDown(Board& b, int shipId, int numShips, int rows, int cols)
{
    /*
     0 1 2
     bool:
     if you reach the end of the boats return true
     
     for i in rows
        for j in cols
         try to place vertical
             place it
             if (recursively place the ship + 1 == true)
                return true
             else
                 unplace the current ship
         try to place horizontally
            place it
            if (recursively place the ship + 1 == true);
                return true
            else
                unplace the current ship
    return false
     */
    // If you reach the end of the boats, return true
    if (shipId > numShips - 1)
    {
        return true;
    }
    
    // for all the rows and columns
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            // Attempt to first place vertically
            Point place (r, c);
            if (b.placeShip(place, shipId, VERTICAL))
            {
                // if you can place all the other ships, then return true
                if (putShipsDown(b, shipId + 1, numShips, rows, cols))
                {
                    return true;
                }
                // if you cannot do the above statement, unplace it and try again
                b.unplaceShip(place, shipId, VERTICAL);
            }
            // now attempt to place the ship horizontally
            if (b.placeShip(place, shipId, HORIZONTAL))
            {
                // if you can place all the other ships, then return true
                if (putShipsDown(b, shipId + 1, numShips, rows, cols))
                {
                    return true;
                }
                // unplace the ships if you cannot do so
                b.unplaceShip(place, shipId, HORIZONTAL);
            }
        }
    }
    // if none of these work, then return false
    return false;
}

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class HumanPlayer : public Player
{
  public:
    HumanPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    virtual bool isHuman() const;
};

HumanPlayer::HumanPlayer(string nm, const Game& g)
: Player(nm, g) {}

bool HumanPlayer::placeShips(Board& b)
{

    // save how many ships have already been put down
    int numShipsPlaced = 0;
    bool leaveNow = false;
    
    // for all of the ships,
    for (int i = 0; i < game().nShips(); i++)
    {
        // leave now if you reach this many ships
        if (i == game().nShips() - 1)
        {
            leaveNow = true;
        }
        
        //save the direction in a char
        char direction;
        // keep asking for directions until they are okay
        // do while loop to make sure at least one thing was put in
        do
        {
            cout << "Enter h or v for direction of " << game().shipName(i) << " (length: "
            << game().shipLength(i) << "): ";
            
            // make sure the durection put in is allowed
            cin >> direction;
            if (direction != 'h' && direction != 'v')
            {
                cout << "Direction must be h or v." << endl;
            }
            cin.ignore(10000, '\n');
        }
        while (direction != 'h' && direction != 'v');
        
        // Collect the point and direction
        Point p;
        Direction dir;
        bool exitHere = false;
        if (direction == 'h')
        {
            do
            {
                // Error checking to make sure that the input is correct
                dir = HORIZONTAL;
                cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
                bool success = getLineWithTwoIntegers(p.r, p.c);
                
                // if you cannot place something down, reprompt
                if (success == false)
                {
                    cout << "You must enter two integers." << endl;
                }
                else if (! game().isValid(p))
                {
                    cout << "The ship can not be placed there." << endl;
                }
                else if (! b.placeShip(p, i, dir))
                {
                    cout << "The ship can not be placed there." << endl;
                }
                
                // if you were able to place the ship down, make sure you take that note
                // dispaly as needed
                else
                {
                    numShipsPlaced++;
                    if (leaveNow == true && numShipsPlaced == game().nShips())
                    {
                        return true;
                    }
                    b.display(false);
                    exitHere = true;
                }
            }
            while (! exitHere);
        }
        
        // repeat the same process for vertical
        else
        {
            do
            {
                dir = VERTICAL;
                cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
                bool success = getLineWithTwoIntegers(p.r, p.c);
                
                // do the same thing where you make sure the input is valid
                if (success == false)
                {
                    cout << "You must enter two integers." << endl;
                }
                
                // do not go out of bounds
                else if (! game().isValid(p))
                {
                    cout << "The ship can not be placed there." << endl;
                }
                // make sure you can place the ship there to begin with
                else if (! b.placeShip(p, i, dir))
                {
                    cout << "The ship can not be placed there." << endl;
                }
                // otherwise take note
                else
                {
                    numShipsPlaced++;
                    if (leaveNow == true && numShipsPlaced == game().nShips())
                    {
                        return true;
                    }
                    b.display(false);
                    exitHere = true;
                }
            }
            while (! exitHere);
        }
        
    }
    
    // if you are able to place all the ships down, return true
    if (numShipsPlaced == game().nShips())
    {
        return true;
    }
    
    // if you could not, return false
    else
    {
        return false;
    }
}

Point HumanPlayer::recommendAttack()
{
    // Prompt the user for somewhere to attack
    Point p;
    
    // user can only get an attack recommended based on what was asked
    cout << "Enter the row and column to attack (e.g., 3 5): ";
    getLineWithTwoIntegers(p.r, p.c);
    
    return p;
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{

}

void HumanPlayer::recordAttackByOpponent(Point p)
{

}

bool HumanPlayer::isHuman() const
{
    return true;
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer : public Player
{
  public:
    MediocrePlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    int state;
    vector <Point> spotsHit;
    vector <Point> spotsHitOnCross;
    bool hit;
    bool destroyed;
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g)
: Player(nm, g), state(1), hit(false), destroyed(false) {}

bool MediocrePlayer::placeShips(Board& b)
{
    bool worked = false;
    for (int i = 0; i < 50; i++)
    {
        // First, it must call the block function on the specified board. This will randomly
        // block off half the positions on the board. No segment of any ship will occupy one
        // of these squares.
        b.block();
        
        Point p (0, 0);
        
        // Recursive function to fulfill step 2
        // make sure you know if the function worked
        if (putShipsDown(b, 0, game().nShips(), game().rows(), game().cols()))
        {
            worked = true;
        }
        else
        {
            worked = false;
        }
        
        // step 3 requires you to unblock it
        b.unblock();
        
        // check to see if the boats were placed down
        if (worked == true)
        {
            return true;
        }
    }
    
    b.display(false);
    // if it has been 50 times, then give up and return false;
    return false;
}

Point MediocrePlayer::recommendAttack()
{
    
    /*
     Know the state
     If in state 1
        Look for a attack that can be made
            If this attack has not been added to a vector of points that i have previously attacked
     If in state 2
        Look for attacks that can be made on the point
            recommend an attack based on a random point such that I have not hit it before
                You know what you ahve not hit before based on adding points on to a vector
     */
    
    // same a point to be returned
    Point p (game().randomPoint());
    
    // initalize the state
    if (state == 2 && !spotsHitOnCross.empty() && destroyed == true)
    {
        state = 1;
    }
    else if (!spotsHitOnCross.empty())
    {
        state = 2;
    }
    // maybe change here
    else if (hit == false || (destroyed == true && hit == true))
    {
        state = 1;
    }
    else
    {
        state = 2;
    }
    
    // act depending on the state
    // state 1
    if (state == 1)
    {
        // If you are in state 1, make sure to get rid of your vector of points on a cross you can hit
        // Execute this in case you accidentally destroy a ship which forces you back into state 1
        spotsHitOnCross.clear();

        /*
         pick a rand point
         see if its hit (iterating)
         outer loop whose job is to do the innder loop that matches
            then break out of the inner
         
         inner shoudl see if you hit
         either it was on the hit list or it wasnt
         */
        

        bool found = false;
        bool restart = true;
        while (restart == true)
        {
            found = false;;
            for (vector<Point>::iterator i = spotsHit.begin(); i != spotsHit.end(); i++)
            {
                // if you find the same point, restart
                if (i->r == p.r && i->c == p.c)
                {
                    p.r = randInt(game().rows());
                    p.c = randInt(game().cols());
                    found = true;
                }
            }
            if (found == false)
            {
                restart = false;
            }
        }
        // make sure you have not previously attacked this point already
        
        // if you reach the end then
        return p;
    }
    
    // state 2
    else
    {
        // use the back function here
        p = spotsHit.back();
        
        // You only want to make the vector of points if your current vector of points on teh cross is not empty
        if (spotsHitOnCross.empty())
        {
            // make the cross
            // add points going up
                // while should exit if the row exceeds 0
            int pointsAdded = 0;
            // make a temp point that starts at p
            Point i (p);
            // only add points if you are above 0 and if you have not added 4 points yet
            // ORIGINAL while (i.r >= 0 && pointsAdded < 4)
            while (game().isValid(i) && pointsAdded < 4)
            {
                // keep adding upwards
                i.r--;
                if (game().isValid(i) == false)
                {
                    break;
                }
                Point addPoint (i.r, p.c);
                spotsHitOnCross.push_back(addPoint);
                pointsAdded++;
            }
            
            // Now add points downwards with the same logic
            pointsAdded = 0;
            i.r = p.r;
            i.c = p.c;
            // only add a point downwards if you have not yet added 4 and you do not exceed the bottommost row
            // while (i.r <= game().rows() - 1 && pointsAdded < 4)
            while (game().isValid(i) && pointsAdded < 4)
            {
                // keep adding upwards downwards
                i.r++;
                if (game().isValid(i) == false)
                {
                    break;
                }
                Point addPoint (i.r, p.c);
                spotsHitOnCross.push_back(addPoint);
                pointsAdded++;
            }
            
            // Now add points rightwards with the same logic
            pointsAdded = 0;
            i.r = p.r;
            i.c = p.c;
            // only add a point rightwards if you have not yet added 4 and you do not exceed the rightmost row
            //  while (i.c <= game().cols() - 1 && pointsAdded < 4)
            while (game().isValid(i) && pointsAdded < 4)
            {
                // keep adding upwards rightwards
                i.c++;
                if (game().isValid(i) == false)
                {
                    break;
                }
                Point addPoint (p.r, i.c);
                spotsHitOnCross.push_back(addPoint);
                pointsAdded++;
            }
            
            // Now add points leftwards with the same logic
            pointsAdded = 0;
            i.r = p.r;
            i.c = p.c;
            // only add a point downwards if you have not yet added 4 and you do not exceed the leftmost row
            // while (i.c >= 0 && pointsAdded < 4)
            while (game().isValid(i) && pointsAdded < 4)
            {
                // keep adding upwards rightwards
                i.c--;
                if (game().isValid(i) == false)
                {
                    break;
                }
                Point addPoint (p.r, i.c);
                spotsHitOnCross.push_back(addPoint);
                pointsAdded++;
            }
            
            // Make sure that there are no points in the vector that have already been hit
            // This makes sure that there are never any repeats
            for (vector<Point>::iterator all = spotsHit.begin(); all != spotsHit.end(); all++)
            {
                // go thru both the big and small vectors that have both the spots on the cross and the spots that have all been hit
                for (vector<Point>::iterator onCross = spotsHitOnCross.begin(); onCross != spotsHitOnCross.end(); onCross++)
                {
                    if (all->r == onCross->r && all->c == onCross->c)
                    {
                        onCross = spotsHitOnCross.erase(onCross);
                        break;
                    }
                }
            }
        }
        
        // randomly pick a spot from the cross of points thats allowed
        // make sure you get rid of any thing you are attacking so the vector is as updated as possible
        int subscript = randInt( static_cast<int> (spotsHitOnCross.size()));
        Point pointToAttack = spotsHitOnCross[subscript];
        spotsHitOnCross.erase(spotsHitOnCross.begin() + subscript);
        
        return pointToAttack;
    }
    
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    // simply save all the parameters
    if (! validShot)
    {
        hit = false;
        destroyed = false;
        validShot = false;
    }
    else
    {
        spotsHit.push_back(p);
        hit = shotHit;
        destroyed = shipDestroyed;
        validShot = true;
    }
    
}

void MediocrePlayer::recordAttackByOpponent(Point p)
{
    // do nothing
}
// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer : public Player
{
  public:
    GoodPlayer (string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    int state;
    vector <Point> spotsHit;
    bool hit;
    bool destroyed;
    // hold all of the queues in one vector
    vector <queue<Point>> spotsHitOnCross;
    // Make queues for each of the directions for the points you hit
    queue <Point> spotsUp;
    queue <Point> spotsDown;
    queue <Point> spotsRight;
    queue <Point> spotsLeft;
    // Make an int for which direction you are
    int dirOfAttack;
    // Need an integer to determine the direction
    // If the direction is -1 then you have not found it
    // if the direction is 0 then you found it once
    // if the durection is 1 then yoou flipped it
    // if the direction is more than 1, then you need to flip the state back to 1
    int directionFound;
    // Need to find a way to make sure that you just got into state 1
    bool newToState2;
    //  Make state 1 into a function because this would otherwise require a lot of copy and pasting
    Point returnPointFromState1();
    
    // Determine whether or not you have to remake the possible points to attack
    bool hasMadePointsToAttack;
    vector <Point> checkeredAttacks;
};

// Initialize the constructor accordingly
GoodPlayer::GoodPlayer (string nm, const Game& g): Player(nm, g), state(1), hit(false), destroyed(false),  dirOfAttack(-1), directionFound(-1), newToState2(true), hasMadePointsToAttack(false)
{
    // Cross of points moves around in a clockwise fashion
    spotsHitOnCross.push_back(spotsUp);
    spotsHitOnCross.push_back(spotsRight);
    spotsHitOnCross.push_back(spotsDown);
    spotsHitOnCross.push_back(spotsLeft);
}

Point GoodPlayer::returnPointFromState1()
{
    /*
     first make a bunch of points that you are free to attack
        This list of points should only consist of every other square on the board
    Until this vector is not empty
        Randomly pick points from it
        Ensure no repeats
     If that vector is empty
        start randomly pocking points
     */
    
    // if you are in state 1 your direction shoudl be up
    dirOfAttack = 0;
    directionFound = -1;

    // first make all of the points you want to attack
    if (hasMadePointsToAttack == false)
    {
        for (int r = 0; r < game().rows(); r++)
        {
            // for all of the even rows or the very first/top row, add only the even columns to the possible attack columns
            if (r == 0 || r % 2 == 0)
            {
                for (int c = 0; c < game().cols(); c += 2)
                {
                    Point p (r, c);
                    checkeredAttacks.push_back(p);
                }
            }
            // for all of the odd columns, add only the odd columns to the possible attack columns
            else
            {
                for (int c = 1; c < game().cols(); c += 2)
                {
                    Point p (r, c);
                    checkeredAttacks.push_back(p);
                }
            }
        }
        // if you made it to the end of the for loop, then you have a vector of points that
        // is checkered. This prevents redundancy in shots when guessing
        hasMadePointsToAttack = true;
    }
    
    // FOR DEBUGGING
//    cerr << "SIZE OF CHECKEREDATTACK VECTOR: " << checkeredAttacks.size() << endl;
    
    // while you can still pick points to attack, then start picking from them randomly
    if (! checkeredAttacks.empty())
    {
        // First randomly pick a point
        // randomly pick a spot from the checkered points that are allowed
        // make sure you get rid of any thing you are attacking so the vector is as updated as possible
        int subscript = randInt( static_cast<int> (checkeredAttacks.size()));
        
        Point pointToAttack = checkeredAttacks[subscript];
        
        checkeredAttacks.erase(checkeredAttacks.begin() + subscript);
        
        return pointToAttack;
        
    }
    // if you ran out of points from the cross, just start randomly picking
    // note: the game should end before you get to this part
    else
    {
        
        Point p;
        p.r = randInt(game().rows());
        p.c = randInt(game().cols());
        
        bool found = false;
        bool restart = true;
        while (restart == true)
        {
            found = false;;
            for (vector<Point>::iterator i = spotsHit.begin(); i != spotsHit.end(); i++)
            {
                // if you find the same point, restart
                if (i->r == p.r && i->c == p.c)
                {
                    p.r = randInt(game().rows());
                    p.c = randInt(game().cols());
                    found = true;
                }
            }
            if (found == false)
            {
                restart = false;
            }
        }
        return p;
    }
    
    
}

bool GoodPlayer::placeShips(Board& b)
{
    // after doing some testing by hand, I determined that its best to placer ships in this confuguration so that the cross of points
    // I will only use this configuration if the ships are being tested with a mediocre player on a 10x10 board
    if (game().rows() == 10 && game().cols() == 10 && game().nShips() == 5)
    {
//        cerr << "Made it into the function " << endl;
        // make sure all of these evaluate to true
        int countOfAddedShips = 0;
        
        // Find the ship length for the one that is 5 long
        for (int i = 0; i < 5; i++)
        {
            if (game().shipLength(i) == 5)
            {
                if (b.placeShip(Point (9, 1), i, HORIZONTAL) == true)
                {
                    countOfAddedShips++;
//                    cerr << "Able to put length 5 down" << endl;
                }
            }
        }
        
        
        
        // Find the ship length for the one that is 4 long
        for (int i = 0; i < 5; i++)
        {
            if (game().shipLength(i) == 4)
            {
                if (b.placeShip(Point (0, 0), i, VERTICAL) == true)
                {
                    countOfAddedShips++;
//                    cerr << "Able to put length 4 down" << endl;
                }
                
            }
        }
        
        
        
        // Find the ship length for the one that is 2 long
        for (int i = 0; i < 5; i++)
        {
            if (game().shipLength(i) == 2)
            {
                if (b.placeShip(Point (4, 2), i, HORIZONTAL) == true)
                {
                    countOfAddedShips++;
//                    cerr << "Able to put length 2 down" << endl;
                }
            }
        }
        
        
        
        // place the last two ships that are of length 3
        // find the first one and then find the second one
        for (int i = 0; i < 5; i++)
        {
            if (game().shipLength(i) == 3)
            {
                if (b.placeShip(Point (7, 9), i, VERTICAL) == true)
                {
                    countOfAddedShips++;
                    break;
//                    cerr << "Able to put length 3 down once" << endl;
                }
            }
        }
        
        // 5 3 4 3 2
        
//        b.display(false);
        for (int i = 4; i >= 0; i--)
        {
            if (game().shipLength(i) == 3)
            {
                if (b.placeShip(Point (0, 9), i, VERTICAL) == true)
                {
                    countOfAddedShips++;
                    break;
//                    cerr << "Able to put length 3 down twice" << endl;
                }
            }
        }
        
        
        
//        cerr << "NUMBER OF SHIPS ADDED " << countOfAddedShips << endl;
        if (countOfAddedShips == 5)
        {
            return true;
        }
        
        // if I cannot place my standard ships accordingly, I am just going to try random spots
        else
        {
            // make sure you clear the ships before you move on in case you accidently placed a ship from the function above
            b.clear();
            
            // Try this 50 times and then if it doesn't work, just put them all next to each other
            for (int i = 0; i < 50; i++)
            {
                // now keep the same code as before for mediocre players
                bool worked = false;
                while (worked == false)
                {
                    // just keep picking points randomly
                    // clear board first
                    b.clear();
                    
                    // save how many ships you have placed
                    int numShipsPlaced = 0;
                    
                    // for all ships
                    for (int i = 0; i < game().nShips(); i++)
                    {
                        // randomly pick a point
                        Point p (randInt(game().rows()), randInt(game().cols()));
                        Direction dir;
                        // randomly pick a direction
                        int d = randInt(2);
                        if (d == 0)
                        {
                            dir = HORIZONTAL;
                        }
                        else
                        {
                            dir = VERTICAL;
                        }
                        
                        // keep going if you can place it
                        if (b.placeShip(p, i, dir))
                        {
                            numShipsPlaced++;
                        }
                        
                        // if you cannot, then restart
                        else
                        {
                            break;
                        }
                    }
                    if (numShipsPlaced == game().nShips())
                    {
                        worked = true;
                        return true;
                    }
                }
            }
            
            // If you somehow are unable to place the ships, the just do what awful player does
            for (int k = 0; k < game().nShips(); k++)
                if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
                    return false;
            return true;
        
        }

    }
    
    // if we are not testing the standard ships, then just place the ships the same way mediocre player would
    // this time there is no need to
    else
    {
        // make sure you clear the ships before you move on in case you accidently placed a ship from the function above
        b.clear();
        
        // Try this 50 times and then if it doesn't work, just put them all next to each other
        for (int i = 0; i < 50; i++)
        {
            // now keep the same code as before for mediocre players
            bool worked = false;
            while (worked == false)
            {
                // just keep picking points randomly
                // clear board first
                b.clear();
                
                // save how many ships you have placed
                int numShipsPlaced = 0;
                
                // for all ships
                for (int i = 0; i < game().nShips(); i++)
                {
                    // randomly pick a point
                    Point p (randInt(game().rows()), randInt(game().cols()));
                    Direction dir;
                    // randomly pick a direction
                    int d = randInt(2);
                    if (d == 0)
                    {
                        dir = HORIZONTAL;
                    }
                    else
                    {
                        dir = VERTICAL;
                    }
                    
                    // keep going if you can place it
                    if (b.placeShip(p, i, dir))
                    {
                        numShipsPlaced++;
                    }
                    
                    // if you cannot, then restart
                    else
                    {
                        break;
                    }
                }
                if (numShipsPlaced == game().nShips())
                {
                    worked = true;
                    return true;
                }
            }
        }
        
        // If you somehow are unable to place the ships, the just do what awful player does
        for (int k = 0; k < game().nShips(); k++)
            if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
                return false;
        return true;
    
    }
    return true;
}


Point GoodPlayer::recommendAttack()
{
        
    // save a point to be returned
    Point p (game().randomPoint());
    
    // Initialize the state
    // At the very start of the game, you must be in state 1
    if (hit == false && destroyed == false && state == 1)
    {
        state = 1;
    }
    // if i missed and i was in state 1 already, then stay here
    else if (hit == false && state == 1)
    {
        state = 1;
    }
    
    else if (hit == false && directionFound > 1)
    {
        state = 1;
    }
    
    // If you ever destroy a ship, you must go to state 1
    else if (destroyed == true)
    {
        state = 1;
    }
    
    // if there are absoluteky no more things to hit, then the state should be 1
    else if (state == 2 && spotsHitOnCross[0].size() == 0 && spotsHitOnCross[1].size() == 0 && spotsHitOnCross[2].size() == 0 && spotsHitOnCross[3].empty() == 0)
    {
        state = 1;
    }
    
    // If you miss but are still in state 2, do not leave state 2 yet
    else if (hit == false && state == 2)
    {
        directionFound = -1;
        state = 2;
    }
    
    else
    {
        // You would only make this cross if you go from state 1 to state 2
        if (state == 1)
        {
            directionFound = -1;
            // Here, we want to look at the last shot that we hit
            // use the back function here
            p = spotsHit.back();
            
            // If you are in state 1, make sure to get rid of your vector of points on a cross you can hit
            // Execute this in case you accidentally destroy a ship which forces you back into state 1
            while (spotsUp.size() != 0)
            {
                spotsUp.pop();
            }
            while (spotsRight.size() != 0)
            {
                spotsRight.pop();
            }
            while (spotsDown.size() != 0)
            {
                spotsDown.pop();
            }
            while (spotsLeft.size() != 0)
            {
                spotsLeft.pop();
            }
            
            // restart the vector of points so that you can save the direction
            spotsHitOnCross.clear();
            spotsHitOnCross.push_back(spotsUp);
            spotsHitOnCross.push_back(spotsRight);
            spotsHitOnCross.push_back(spotsDown);
            spotsHitOnCross.push_back(spotsLeft);

            
            // This is the same from mediocre player
            // You only want to make the vector of points if your current vector of points on the cross is not empty
            if (spotsUp.empty() == true && spotsDown.empty() == true && spotsLeft.empty() == true && spotsRight.empty() == true)
            {
                // make the cross
                // add points going up
                    // while should exit if the row exceeds 0
                int pointsAdded = 0;
                // make a temp point that starts at p
                Point i (p);
                // only add points if you are above 0 and if you have not added 4 points yet
                // ORIGINAL while (i.r >= 0 && pointsAdded < 4)
                while (game().isValid(i) && pointsAdded < 4)
                {
                    // keep adding upwards
                    i.r--;
                    if (game().isValid(i) == false)
                    {
                        break;
                    }
                    // Check if this shot was already hit
                    for (vector<Point>::iterator it = checkeredAttacks.begin(); it != checkeredAttacks.end(); it++)
                    {
                        if (it->r == i.r && it->c == i.c)
                        {
                            continue;
                        }
                    }
                    Point addPoint (i.r, p.c);
                    spotsUp.push(addPoint);
                    pointsAdded++;
                }
                
                // restart the point
                i = p;
                // Now add points downwards with the same logic
                pointsAdded = 0;
                i.r = p.r;
                i.c = p.c;
                // only add a point downwards if you have not yet added 4 and you do not exceed the bottommost row
                // while (i.r <= game().rows() - 1 && pointsAdded < 4)
                while (game().isValid(i) && pointsAdded < 4)
                {
                    // keep adding downwards
                    i.r++;
                    if (game().isValid(i) == false)
                    {
                        break;
                    }
                    // Check if this shot was already hit
                    for (vector<Point>::iterator it = checkeredAttacks.begin(); it != checkeredAttacks.end(); it++)
                    {
                        if (it->r == i.r && it->c == i.c)
                        {
                            continue;
                        }
                    }
                    Point addPoint (i.r, p.c);
                    spotsDown.push(addPoint);
                    pointsAdded++;
                }
                
                // restart the point
                i = p;
                // Now add points rightwards with the same logic
                pointsAdded = 0;
                i.r = p.r;
                i.c = p.c;
                // only add a point rightwards if you have not yet added 4 and you do not exceed the rightmost row
                //  while (i.c <= game().cols() - 1 && pointsAdded < 4)
                while (game().isValid(i) && pointsAdded < 4)
                {
                    // keep adding rightwards
                    i.c++;
                    if (game().isValid(i) == false)
                    {
                        break;
                    }
                    // Check if this shot was already hit
                    for (vector<Point>::iterator it = checkeredAttacks.begin(); it != checkeredAttacks.end(); it++)
                    {
                        if (it->r == i.r && it->c == i.c)
                        {
                            continue;
                        }
                    }
                    Point addPoint (p.r, i.c);
                    spotsRight.push(addPoint);
                    pointsAdded++;
                }
                
                // restart the point
                i = p;
                // Now add points leftwards with the same logic
                pointsAdded = 0;
                i.r = p.r;
                i.c = p.c;
                // only add a point downwards if you have not yet added 4 and you do not exceed the leftmost row
                // while (i.c >= 0 && pointsAdded < 4)
                while (game().isValid(i) && pointsAdded < 4)
                {
                    // keep adding leftwards
                    i.c--;
                    if (game().isValid(i) == false)
                    {
                        break;
                    }
                    // Check if this shot was already hit
                    for (vector<Point>::iterator it = checkeredAttacks.begin(); it != checkeredAttacks.end(); it++)
                    {
                        if (it->r == i.r && it->c == i.c)
                        {
                            continue;
                        }
                    }
                    Point addPoint (p.r, i.c);
                    spotsLeft.push(addPoint);
                    pointsAdded++;
                }

            }

        }
        
        // if all of the if and else if options have been not used, then you go to state 2
        state = 2;
    }
    
    // act depending on the state
    // state 1
    if (state == 1)
    {
        return returnPointFromState1();
    }
    
    // state 2
    else
    {
        // If you are able to hit the shot but are not new to the shot, then you found the direction once perhaps
        if (newToState2 == false && hit == true && directionFound == -1)
        {
            directionFound = 0;
        }
        
        // if just arrived in state 2 and hit was successful
        // then go until you find a valid direction to hit
        if (newToState2 == true && hit == true)
        {
            dirOfAttack = 0;
            if (! spotsUp.empty() && dirOfAttack == 0)
            {
                Point pointToAttack = spotsUp.front();
                spotsUp.pop();
                newToState2 = false;
                return pointToAttack;
            }
            else
            {
                dirOfAttack++;
            }
            
            // attack right next
            if (! spotsRight.empty() && dirOfAttack == 1)
            {
                Point pointToAttack = spotsRight.front();
                spotsRight.pop();
                newToState2 = false;
                return pointToAttack;
            }
            else
            {
                dirOfAttack++;
            }
            
            // attack down next
            if (! spotsDown.empty() && dirOfAttack == 2)
            {
                Point pointToAttack = spotsDown.front();
                spotsDown.pop();
                newToState2 = false;
                return pointToAttack;
            }
            else
            {
                dirOfAttack++;
            }
            
            // attack the bottom one finally
            if (! spotsLeft.empty() && dirOfAttack == 3)
            {
                Point pointToAttack = spotsLeft.front();
                spotsLeft.pop();
                newToState2 = false;
                return pointToAttack;
            }
            else
            {
                // if you never hiot any direction, then you must try and return a point from state 1
                state = 1;
                directionFound = -1;
                return returnPointFromState1();
            }
            
        }
        
        // if you are new to state 2 but then fail to hit
        // make the durection -1 because you should check clockwise first
        if (newToState2 == true && hit == false)
        {
            directionFound = -1;
        }
        
        
        // restart the direction if you think you hit it more than once in the supposed direction
        if (newToState2 == false && hit == false && directionFound == 1)
        {
            directionFound = -1;
        }
        
        // If you did not hit anything, then you would just go around clockwise
        if (hit == false && directionFound == -1)
        {
            dirOfAttack++;
        }
        
        // If you do not hit the ship, but already you think you know the direction, then you need to hit the other direction
        if (hit == false && directionFound == 0)
        {
            directionFound = 1;
            // If you are up, then go down
            if (dirOfAttack == 0)
            {
                dirOfAttack = 2;
            }
            // if you are down, then you go up
            else if (dirOfAttack == 2)
            {
                dirOfAttack = 0;
            }
            // if you are right, then you go left
            else if (dirOfAttack == 1)
            {
                dirOfAttack = 3;
            }
            // if you are left, then you go right
            else // dirOfAttack == 3
            {
                dirOfAttack = 1;
            }
        }
        
        
        // if you are going up, then make sure your stack to attack is not empty
        if (dirOfAttack == 0)
        {
            // if you can pull something from the stack, attack in that direction
            if (! spotsUp.empty())
            {
                Point pointToAttack = spotsUp.front();
                spotsUp.pop();
                newToState2 = false;
                return pointToAttack;
            }
            else
            {
                // if you cannot, then change direction again
                dirOfAttack++;
            }
        }
        // if you are going right, then make sure your stack to attack is not empty
        if (dirOfAttack == 1)
        {
            if (! spotsRight.empty())
            {
                // if you can pull something from the stack, attack in that direction
                Point pointToAttack = spotsRight.front();
                spotsRight.pop();
                newToState2 = false;
                return pointToAttack;
            }
            else
            {
                // if you cannot, then change direction again
                dirOfAttack++;
            }
        }

        // if you are going down, then make sure your stack to attack is not empty
        if (dirOfAttack == 2)
        {
            if (! spotsDown.empty())
            {
                // if you can pull something from the stack, attack in that direction
                Point pointToAttack = spotsDown.front();
                spotsDown.pop();
                newToState2 = false;
                return pointToAttack;
            }
            else
            {
                // if you cannot, then change direction again
                dirOfAttack++;
            }
        }
        // if you are going left, then make sure your stack to attack is not empty
        if (dirOfAttack == 3)
        {
            if (! spotsLeft.empty())
            {
                // if you can pull something from the stack, attack in that direction
                Point pointToAttack = spotsLeft.front();
                spotsLeft.pop();
                newToState2 = false;
                return pointToAttack;
            }
            else
            {
                // if you cannot, then change direction again
                dirOfAttack++;
            }
        }
 
        // if you make it to the end then increment make sure you go back to state 1
        if (dirOfAttack > 3)
        {
            state = 1;
        }
        
        // If this somehow gets all the way down here, then you ran out of good points to hit
        return returnPointFromState1();
    }
    
    return p;
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    // just update the parameters accordingly
    if (! validShot)
    {
        hit = false;
        destroyed = false;
        validShot = false;
    }
    else
    {
        spotsHit.push_back(p);
        hit = shotHit;
        destroyed = shipDestroyed;
        validShot = true;
    }
}

void GoodPlayer::recordAttackByOpponent(Point p)
{
    return;
}

//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}
