#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

// This is for holding all of our ships
#include <list>
#include <iterator>

using namespace std;

class GameImpl
{
public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
    

private:
    int m_rows;
    int m_cols;
    struct ship
    {
        // symbol
        char m_shipSymbol;
        
        // shipID
        int m_shipID;
        
        // length
        int m_length;
        
        // hp for keeping track of destroying ships
        int m_hp;
        
        // ship name
        string m_shipName;
    };
    list <ship> m_ships;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
{
    // The game has rows and columns that are the same for both players
    m_rows = nRows;
    m_cols = nCols;
}

int GameImpl::rows() const
{
    // return the game's rows
    return m_rows;
}

int GameImpl::cols() const
{
    // return the game's columns
    return m_cols;
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    // Prof Smallberg graciously already did the error checking
    // Now, you are only allowed to add a ship if the placeShip() function returns true
   
    // Make a temp struct to hold all the new data to add to the linked list
    ship newShip;
    newShip.m_shipSymbol = symbol;
    newShip.m_length = length;
    newShip.m_shipName = name;
    newShip.m_shipID = static_cast<int>(m_ships.size());
    newShip.m_hp = length;
    
    // Now store this data into the linked list
    m_ships.push_front(newShip);
    
    // The implementations for catching the false statements were already written
    return true;
}

int GameImpl::nShips() const
{
    
    return static_cast<int>(m_ships.size());
}

int GameImpl::shipLength(int shipId) const
{
    // Traverse the linked list and find the ship ID
    for (list<ship>::const_iterator p = m_ships.begin(); p != m_ships.end(); p++)
    {
        if ((*p).m_shipID == shipId)
        {
            return p->m_length;
        }
    }
    
    return 0;

}

char GameImpl::shipSymbol(int shipId) const
{
    // Traverse the linked list and find the ship ID
    for (list<ship>::const_iterator p = m_ships.begin(); p != m_ships.end(); p++)
    {
        if ((*p).m_shipID == shipId)
        {
            return p->m_shipSymbol;
        }
    }
    return '?';
}

string GameImpl::shipName(int shipId) const
{
    // Traverse the linked list and find the ship ID
    for (list<ship>::const_iterator p = m_ships.begin(); p != m_ships.end(); p++)
    {
        if ((*p).m_shipID == shipId)
        {
            return p->m_shipName;
        }
    }
    return "";  // This compiles but may not be correct
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    // prompt if its human beforehand
    if (p1->isHuman())
    {
        // humans get special prompts
        cout << p1->name() << " must place " << nShips() << " ships." << endl;
        b1.display(true);
    }
    if (p2->isHuman())
    {
        // humans get special prompts
        cout << p2->name() << " must place " << nShips() << " ships." << endl;
        b2.display(true);
    }
    
    // if you cannot place the ships for either, return the nullptr
    // do not continue if either of these are false
    if (p1->placeShips(b1) == false)
    {
        return nullptr;
    }
    if (p2->placeShips(b2) == false)
    {
        return nullptr;
    }
    
    // Humans have special prompts
    // start the game
    // the game keeps going until one of the boards has no more ships
    while (!b1.allShipsDestroyed() || !b2.allShipsDestroyed())
    {
        // Print out relevant statements
        cout << p1->name() << "'s turn.  Board for " <<  p2->name() << ":" << endl;
        
        // If the first player is human, do not show undamaged segments on the opponent's board, since that would be cheating
        if (p1->isHuman())
        {
            b2.display(true);
        }
        else
        {
            b2.display(false);
        }
        
        // Now, make the first player's attack on p2's board and save what you did
        bool shotHit = false;
        bool shipDestroyed = false;
        int shipId = 0;
        Point p = p1->recommendAttack();
        bool validShot = false;
        validShot = b2.attack(p, shotHit, shipDestroyed, shipId);
        p1->recordAttackResult(p, validShot, shotHit, shipDestroyed, shipId);
        
        // before the showing the attack announce the results
        if (shotHit == true && shipDestroyed == true)
        {
            // announce if attacked and destroyed
            cout << p1->name() << " attacked (" << p.r << "," << p.c << ") and destroyed the " <<
            shipName(shipId) << ", resulting in:" << endl;
        }
        // announce if wasted shot
        else if (validShot == false)
        {
            cout << p1->name() << " wasted a shot at (" << p.r << "," << p.c << ")." << endl;
        }
        // announce if only attacked
        else if (shotHit == true)
        {
            cout << p1->name() << " attacked (" << p.r << "," << p.c << ") and hit something, resulting in:" << endl;
        }
        // announce if missed
        else
        {
            cout << p1->name() << " attacked (" << p.r << "," << p.c << ") and missed, resulting in:" << endl;
        }
        
        // display the results of the attack
        if (p1->isHuman())
        {
            b2.display(true);
        }
        else
        {
            b2.display(false);
        }
        
        // if either board got all ships destroyed, exit loop
        if (b1.allShipsDestroyed() || b2.allShipsDestroyed())
        {
            break;
        }
        
        // Display the resulst after the attack
        if (shouldPause == true)
        {
            cout << "Press enter to continue: ";
            cin.ignore();
        }
        
        // Repeat these steps with the roles of the first and second player reversed
        cout << p2->name() << "'s turn.  Board for " <<  p1->name() << ":" << endl;
        if (p2->isHuman())
        {
            b1.display(true);
        }
        else
        {
            b1.display(false);
        }
        
        // Now, make the first player's attack on p2's board and save what you did
        p = p2->recommendAttack();
        validShot = b1.attack(p, shotHit, shipDestroyed, shipId);
        p2->recordAttackResult(p, validShot, shotHit, shipDestroyed, shipId);
        
        // announce destroyed ships
        if (shotHit == true && shipDestroyed == true)
        {
            cout << p2->name() << " attacked (" << p.r << "," << p.c << ") and destroyed the " <<
            shipName(shipId) << ", resulting in:" << endl;
        }
        // announce if shots are wasted
        else if (validShot == false)
        {
            cout << p2->name() << " wasted a shot at (" << p.r << "," << p.c << ")." << endl;
        }
        // accounce if hit something
        else if (shotHit == true)
        {
            cout << p2->name() << " attacked (" << p.r << "," << p.c << ") and hit something, resulting in:" << endl;
        }
        // accounce if missed
        else
        {
            cout << p2->name() << " attacked (" << p.r << "," << p.c << ") and missed, resulting in:" << endl;
        }
        
        // display the results of the attack
        if (p2->isHuman())
        {
            b1.display(true);
        }
        else
        {
            b1.display(false);
        }
        
        // if either got all ships destroyed, then break
        if (b1.allShipsDestroyed() || b2.allShipsDestroyed())
        {
            break;
        }
        
        // only ask this if yoyu want the user to pause
        if (shouldPause == true)
        {
            cout << "Press enter to continue: ";
            cin.ignore();
        }
    }
    // accounce winners
    if (b1.allShipsDestroyed() == true)
    {
        // Bluto wins!
        if (p2->isHuman())
        {
            b2.display(false);
        }
        cout << p2->name() << " wins!" << endl;
        return p2;
    }
    // announce winners
    else
    {
        if (p1->isHuman())
        {
            b1.display(false);
        }
        cout << p1->name() << " wins!" << endl;
        return p1;
    }
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

