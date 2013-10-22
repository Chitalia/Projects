// Game.cpp

#include "provided.h"
#include <iostream>
using namespace std;

class GameImpl
{
  public:
    GameImpl(const Scaffold& s, int N, Player* red, Player* black); 
    bool completed(int& winner) const; 
    bool takeTurn(); 
    void play(); 
    int checker(int c, int r) const;
    bool checkVertical(const Scaffold& s, int n, int& winColor) const;   //name implies function
    bool checkHorizontal(const Scaffold& s, int n, int& winColor) const;
    bool checkDiagonal1(const Scaffold& s, int n, int& winColor) const;
    bool checkDiagonal2(const Scaffold& s, int n, int& winColor) const;
  private:
    int GOAL;
    int turn;
    int m_winner;
    Scaffold gameBoard;
    Player* redPlayer;
    Player* blackPlayer;
};

GameImpl::GameImpl(const Scaffold& s, int N, Player* red, Player* black):GOAL(N), gameBoard(s), redPlayer(red), blackPlayer(black), turn(RED), m_winner(VACANT){}






bool GameImpl::checkDiagonal1(const Scaffold& s, int n, int& winColor) const
{
    if (s.rows() == 1 || s. cols() == 1)     //if there are only 1 row or 1 column, no need to check for diagonal
        return false;
    int count = 0;
    for (int i = 1; i < s.cols() + 1; i++)
    {
        for (int j = 1; j < s.rows() + 1; j++)    //traverse through the rows and colums
        {
            if (checker(i, j) == BLACK || checker(i, j) == RED) 
            {
                int color = checker(i, j);   //save the color of the current piece, and set count to 1
                count = 1;
                if (n == count)
                {
                    winColor = color;
                    return true;
                }
                for (int k = 1; k < n; k++)   //look for N in a row of the type color
                {
                    if ( (k + i) < s.cols()+1 && (k + j) < s.rows()+1)   //make sure that the location your are checking is actually in the scaffold
                    {
                        if (checker(k + i, k + j) == color)
                        {
                            count++;
                            if (count == n)   //if at any point you have N in a row, save the color of the type and return true
                            {
                                winColor = color;
                                return true;
                            }
                        }
                        else
                            break;
                    }
                }
            }
        }
    }
    return false;   //if it reaches here, there are NOT N in a row
}




bool GameImpl::checkDiagonal2(const Scaffold& s, int n, int& winColor) const
{
    if (s.rows() == 1 || s. cols() == 1)  //if there are only 1 row or 1 column, no need to check for diagonal
        return false;
    int count = 0;
    for (int i = 1; i < s.cols()+1; i++)
    {
        for (int j = 1; j < s.rows() + 1; j++)   //traverse through the rows and colums
        {
            if (checker(i, j) == BLACK || checker(i, j) == RED)
            {
                int color = checker(i, j);    //save the color of the current piece, and set count to 1
                count = 1;
                if (n == count)
                {
                    winColor = color;
                    return true;
                }
                for (int k = 1; k < n; k++)    //look for N in a row of the type color
                {
                    if ( (i - k) > 0 && (j + k) < s.rows()+1)    //make sure that the location your are checking is actually in the scaffold
                    {
                        if (checker(i - k, j + k) == color)
                        {
                            count++;
                            if (count == n)     //if at any point you have N in a row, save the color of the type and return true
                            {
                                winColor = color;
                                return true;
                            }
                        }
                        else
                            break;
                    }
                }
            }
        }
    }
    return false;   //if it reaches here, there are NOT N in a row
}




bool GameImpl::checkHorizontal(const Scaffold& s, int n, int& winColor) const
{
    if (s. cols() == 1)   //if there is only 1 column, no need to check for horizontal
        return false;
    int count = 0;
    for (int i = 1; i < s.cols() + 1; i++)
    {
        for (int j = 1; j < s.rows() + 1; j++)   //traverse through the rows and colums
        {
            if (checker(i, j) == BLACK || checker(i, j) == RED)
            {
                int color = checker(i, j);   //save the color of the current piece, and set count to 1
                count = 1;
                if (n == count)
                {
                    winColor = color;
                    return true;
                }
                for (int k = 1; k < n; k++)  
                {
                    if ((k + i) < s.cols() + 1)    //make sure that the location your are checking is actually in the scaffold
                    {
                        if (checker(k + i, j) == color)
                        {
                            count ++;
                            if (count == n)   //if at any point you have N in a row, save the color of the type and return true
                            {
                                winColor = color;
                                return true;
                            }
                        }
                        else
                            break;
                    }
                }
            }
        }
    }
    return false;   //if it reaches here, there are NOT N in a row
}



bool GameImpl::checkVertical(const Scaffold& s, int n, int& winColor) const
{
    if (s.rows() == 1)   //if there is only 1 row, no need to check for any vertical
        return false;
    int count = 0;
    for (int i = 1; i < s.cols() + 1; i++)
    {
        for (int j = 1; j < s.rows() + 1; j++)   //traverse through the rows and colums
        {
            if (checker(i, j) == BLACK || checker(i, j) == RED)
            {
                int color = checker(i, j);   //save the color of the current piece, and set count to 1
                count = 1;
                if (n == count)
                {
                    winColor = color;
                    return true;
                }
                for (int k = 1; k < n; k++)
                {
                    if ((k + j) < s.rows() + 1)  //make sure that the location your are checking is actually in the scaffold
                    {
                        if (checker(i, k + j) == color)
                        {
                            count ++;
                            if (count == n)   //if at any point you have N in a row, save the color of the type and return true
                            {
                                winColor = color;
                                return true;
                            }
                        }
                        else
                            break;
                    }
                }
            }
        }
    }
    return false;   //if it reaches here, there are NOT N in a row
}





bool GameImpl::completed(int& winner) const
{
    //check if there are no empty spaces and if the game is not over. Then it is a tie
    if (gameBoard.numberEmpty() == 0 && !( checkHorizontal(gameBoard, GOAL, winner) || checkDiagonal1(gameBoard, GOAL, winner) || checkDiagonal2(gameBoard, GOAL, winner) || checkVertical(gameBoard, GOAL, winner)  ) )
    {
        winner = TIE_GAME;
        return true;
    }
    //check if the game is over. if it is, return true regardlessly
    else if ( checkHorizontal(gameBoard, GOAL, winner) || checkDiagonal1(gameBoard, GOAL, winner) || checkDiagonal2(gameBoard, GOAL, winner) || checkVertical(gameBoard, GOAL, winner) )
        {
            return true;
        }
    //
    else
        return false; //if the game is not over, nor is the board full, then return false
       
}



bool GameImpl::takeTurn()
{
    if (completed(m_winner))   //check to see that the game is still on
        return false;
    else
    {
        if (turn == RED)  //if turn is red
        {
            gameBoard.makeMove(redPlayer->chooseMove(gameBoard, GOAL, RED), RED);  //make a move for the red player by putting in a red coin at the respective chooseMove function for the type of player
            gameBoard.display();  
            turn = BLACK;  //set the turn to Black if the original turn was Red's
            if (!completed(m_winner))  
            {
                cout <<endl;
                cout <<"Black's turn"<<endl;
                cout <<endl;
            }
            return true;   //someone's turn was taken
        }
        else
        {
            gameBoard.makeMove(blackPlayer->chooseMove(gameBoard, GOAL, BLACK), BLACK);  //make a move for the black player by putting in a black coin at the respective chooseMove function for the type of player
            gameBoard.display();
            turn = RED;   //set the turn to black's if the original turn was red
            if (!completed(m_winner))
                {
                cout <<endl;
                cout <<"Red's turn"<<endl;
                cout <<endl;
                }
            return true; //someone's turn was taken
        }
    }
}

void GameImpl::play()
{
    cout <<"WELCOME TO CONNECT "<<GOAL<<" HERE WE HAVE: "<<redPlayer->name()<< " vs "<<blackPlayer->name()<< endl;
    cout <<endl;
    gameBoard.display();
    cout <<endl;
    cout <<"Red's turn"<<endl;
    cout <<endl;
    while (this->completed(m_winner) == false)   //while the game is NOT over
    {

        this->takeTurn();  //take someone's turn, red's for the first. the turns will change accordingly
        
        if (this->completed(m_winner) && m_winner == RED)  //if at any point red wins, display it 
        {
            cout <<endl;
            cout <<"RED, "<<redPlayer->name()<< ", WINS"<<endl;
        }
        else if (this->completed(m_winner) && m_winner == BLACK)  //if at any point black wins, display it
        {
            cout <<endl;
            cout <<"BLACK, "<<blackPlayer->name()<< ", WINS"<<endl;
        }
        else if (this->completed(m_winner) && m_winner == TIE_GAME) //if there ends up being a tie game, display it
        {
            cout <<endl;
            cout <<"TIE GAME"<<endl;
        }
    }
}

int GameImpl::checker(int c, int r) const
{
    return gameBoard.checker(c, r);
} 

//******************** Game functions *******************************

// These functions simply delegate to GameImpl's functions.
// You probably don't want to change any of this code.

Game::Game(const Scaffold& s, int N, Player* red, Player* black)
{
    m_impl = new GameImpl(s, N, red, black);
}
 
Game::~Game()
{
    delete m_impl;
}
 
bool Game::completed(int& winner) const
{
    return m_impl->completed(winner);
}

bool Game::takeTurn()
{
    return m_impl->takeTurn();
}

void Game::play()
{
    m_impl->play();
}
 
int Game::checker(int c, int r) const
{
    return m_impl->checker(c, r);
}
