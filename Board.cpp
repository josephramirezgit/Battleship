#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <list>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

  private:
    const Game& m_game;
    char m_board[MAXROWS][MAXCOLS];
    struct ships
    {
    public:
        int m_id;
        char m_symbol;
        int m_health;
        Point m_topOrLeft;
    };
    list<ships> m_boardShips;
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    this->clear();
}

void BoardImpl::clear()
{
    // make everything into a '.' meaning that the board is clear
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            m_board[r][c] = '.';
        }
    }
    
    // Delete all the ships in the list
    m_boardShips.clear();
}

void BoardImpl::block()
{
    // for floor (RC/2) tiles, block them
    for (int i = 0; i < (m_game.rows()) * (m_game.cols()) / 2; i++)
    {
        // make random points to block off
        int temp_r = randInt(m_game.rows());
        int temp_c = randInt(m_game.cols());
        
        // if it is not already blocked off, then block it off
        if (m_board[temp_r][temp_c] != '#')
        {
            m_board[temp_r][temp_c] = '#';
        }
        
        // if the above statement fails, then you need to retry it
        else
        {
            i--;
            continue;
        }
    }
}

void BoardImpl::unblock()
{
    // For every single point on the board, make it avalible if it is currently being blocked
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (m_board[r][c] == '#')
            {
                m_board[r][c] = '.'; // doing this will result in an unblocked position
            };
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    // check if the shipId is ok
    if (shipId >= 0 && shipId <= m_game.nShips() - 1)
    {
        // check if you place it within the board
        if (dir == HORIZONTAL)
        {
            if (m_game.shipLength(shipId) + topOrLeft.c <= m_game.cols())
            {
                // if the ship fits, make sure no position is blocked
                for (int c = topOrLeft.c; c < m_game.shipLength(shipId) + topOrLeft.c; c++)
                {
                    if (m_board[topOrLeft.r][c] == '.')
                    {
                        continue;
                    }
                    
                    // only return false if there is anything other than a period
                    else
                    {
                        return false;
                    }
                }
                // Now that you made sure that you can put the ship down, only put it down if you did not already put it down
                for (int r = 0; r < m_game.rows(); r++)
                {
                    for (int c = 0; c < m_game.cols(); c++)
                    {
                        // if the ship symbol is found, then you cannot place it down
                        if (m_board[r][c] == m_game.shipSymbol(shipId))
                        {
                            return false;
                        }
                    }
                }
                // now that we know we can put the ship down, put it down!
                for (int c = topOrLeft.c; c < m_game.shipLength(shipId) + topOrLeft.c; c++)
                {
                    m_board[topOrLeft.r][c] = m_game.shipSymbol(shipId);
                }
                
                // update the list to have this new ship on the board
                ships newShip;
                newShip.m_id = shipId;
                newShip.m_symbol = m_game.shipSymbol(shipId);
                newShip.m_health = m_game.shipLength(shipId);
                newShip.m_topOrLeft = topOrLeft;
                m_boardShips.push_front(newShip);
                // only return true if you nothing is blocked and you can fit it on the board and it was not already put down
                return true;
            }
            // if the ship does not fit, the return false
            else
            {
                return false;
            }
        }
        else if (dir == VERTICAL)
        {
            // Check if you can even place the ship in that position
            if (m_game.shipLength(shipId) + topOrLeft.r <= m_game.rows())
            {
                // if the ship fits, make sure no position is blocked
                for (int r = topOrLeft.r; r < m_game.shipLength(shipId) + topOrLeft.r; r++)
                {
                    if (m_board[r][topOrLeft.c] == '.')
                    {
                        continue;
                    }
                    //only return false if there is some position already in the way
                    else
                    {
                        return false;
                    }
                }
                // Now that you made sure that you can put the ship down, only put it down if you did not already put it down
                for (int r = 0; r < m_game.rows(); r++)
                {
                    for (int c = 0; c < m_game.cols(); c++)
                    {
                        if (m_board[r][c] == m_game.shipSymbol(shipId))
                        {
                            return false;
                        }
                    }
                }
                // now that we know we can put the ship down, put it down!
                for (int r = topOrLeft.r; r < m_game.shipLength(shipId) + topOrLeft.r; r++)
                {
                    m_board[r][topOrLeft.c] = m_game.shipSymbol(shipId);
                }
                
                // update the list of ships
                ships newShip;
                newShip.m_id = shipId;
                newShip.m_symbol = m_game.shipSymbol(shipId);
                newShip.m_health = m_game.shipLength(shipId);
                newShip.m_topOrLeft = topOrLeft;
                m_boardShips.push_front(newShip);
                // only return true if you nothing is blocked and you can fit it on the board and it was not already put down
                return true;
            }
            // if the ship does not fit, the return false
            else
            {
                return false;
            }
        }
    }
    
    // return false if you were never able to put the ship down
    return false;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    // Will return false if the shipId is invalid
    if (shipId < 0 || shipId > m_game.nShips() - 1)
    {
        return false;
    }
    // Will return false if the board does not contain the entire ship at the indicated locations.
    if (dir == HORIZONTAL)
    {
        // Check to make sure the whole section is ok
        for (int c = topOrLeft.c; c < m_game.shipLength(shipId) + topOrLeft.c; c++)
        {
            if (c >= m_game.cols())
            {
                return false;
            }
            if (m_board[topOrLeft.r][c] != m_game.shipSymbol(shipId))
            {
                return false;
            }
        }
        // If the for loop above terminates, this means that you can unplace the ship
        for (int c = topOrLeft.c; c < m_game.shipLength(shipId) + topOrLeft.c; c++)
        {
            if (c >= m_game.cols())
            {
                return false;
            }
            m_board[topOrLeft.r][c] = '.';
        }
    }
    // repeat the same process for vertically placed ships
    if (dir == VERTICAL)
    {
        for (int r = topOrLeft.r; r < m_game.shipLength(shipId) + topOrLeft.r; r++)
        {
            if (r >= m_game.cols())
            {
                return false;
            }
              
            if (m_board[r][topOrLeft.c] != m_game.shipSymbol(shipId))
            {
                return false;
            }
        }
        // If the for loop above terminates, this means that you can unplace the ship
        for (int r = topOrLeft.r; r < m_game.shipLength(shipId) + topOrLeft.r; r++)
        {
            if (r >= m_game.cols())
            {
                return false;
            }
            m_board[r][topOrLeft.c] = '.';
        }
    }
    // update the number of ships if you made sure you did not return false from earlier
    // find the ship you want to erase -> pass in that iterator
    for(list<ships>::const_iterator destoryThis = m_boardShips.begin(); destoryThis != m_boardShips.end(); destoryThis++)
    {
        if (destoryThis->m_id == shipId)
        {
            m_boardShips.erase(destoryThis);
            break;
        }
    }
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{
//    First line: The function must print two spaces followed by the digits for each
//    column, starting at 0, followed by a newline. You may assume there will be no
//    more than 10 columns.
    std::cout << "  ";
    for (int i = 0; i < m_game.cols(); i++)
    {
        std::cout << i;
    }
    std::cout << std::endl;
    
    
    for (int r = 0; r < m_game.rows(); r++)
    {
        std::cout << r << " ";
        for (int c = 0; c < m_game.cols(); c++)
        {
            // If you only want to see the shots, then only print out the characters X, o, and .
            if (shotsOnly == true)
            {
                if (m_board[r][c] != 'X' && m_board[r][c] != 'o' && m_board[r][c] != '.')
                {
                    std::cout << '.';
                }
                else
                {
                    std::cout << m_board[r][c];
                }
                
            }
            
            // otherwise, show the board regularly
            else
            {
                std::cout << m_board[r][c];
            }
        }
        std::cout << std::endl;
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{

    // if an attack is out of bounds
    if (p.r > m_game.rows() - 1 || p.r < 0 || p.c > m_game.cols() - 1 || p.c < 0)
    {
        return false;
    }

    
    // Invalid if an attack is made on a previously attacked location
    if (m_board[p.r][p.c] == 'X' || m_board[p.r][p.c] == 'o')
    {
        return false;
    }
    
    // If you hit something undamaged, make it damaged
    if (m_board[p.r][p.c] != '#' && m_board[p.r][p.c] != '.')
    {
        /*
         Find out what boat you hit
         Decrease that boat's healt
         if the health is now 0 then you destroyed the boat
         */
        
        // you made it to this point so you know you were able to hit the board
        shotHit = true;
        
        // Save the point you hit
        char symbol = m_board[p.r][p.c];
        
        // Update it so the board knows you hit it
        m_board[p.r][p.c] = 'X';
    
        // Find the shipID of that point you hit
        for(list<ships>::iterator destoryThis = m_boardShips.begin(); destoryThis != m_boardShips.end(); destoryThis++)
        {
            // if you find the ship symbol of the thing you hit, reduce the health of that ship
            if (destoryThis->m_symbol == symbol)
            {
                destoryThis->m_health--;
                if (destoryThis->m_health <= 0)
                {
                    // Make sure you remember that you blew up the ship
                    shipDestroyed = true;
                    shipId = destoryThis->m_id;
                }
                else
                {
                    // otherwise, you did not destroy the ship
                    shipDestroyed = false;
                    shipId = destoryThis->m_id;
                }
            }
        }
        
        
    }
    
    // otherwise a missed shot is transformed into an o characater
    else if (m_board[p.r][p.c] == '.')
    {
        m_board[p.r][p.c] = 'o';
        shotHit = false;
        shipDestroyed = false;
    }
    else
    {
        shotHit = false;
    }
    
    // You will always end up returning true since you catch false above
    return true;
    
}

bool BoardImpl::allShipsDestroyed() const
{
    for (list<ships>::const_iterator m = m_boardShips.cbegin(); m != m_boardShips.cend(); m++)
    {
        if (m->m_health != 0)
        {
            return false;
        }
    }
    return true;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
