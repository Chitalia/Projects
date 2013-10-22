// Player.cpp

#include "provided.h"
#include <string>
#include <iostream>
#include <stack>
#include <climits>
#include <vector>
using namespace std;


class HumanPlayerImpl
{
  public:
    virtual int chooseMove(const Scaffold& s, int N, int color);
};






class BadPlayerImpl
{
  public:
    virtual int chooseMove(const Scaffold& s, int N, int color);
};


struct Relate
{
    int depth;
    int column;
    
};


struct twoValues
{
    int rating;
    Relate r;
};



class SmartPlayerImpl
{
  public:
    virtual int chooseMove(const Scaffold& s, int N, int color);
    twoValues determineBestComputerMove(Scaffold &s, int N, int color, bool isHuman, int count);
    int ratingFunction(Scaffold &temp, int N, bool isHuman, int nColor);
private:
    vector<Relate> one;
    vector<Relate> zero;
    vector<Relate> negativOne;
};







int HumanPlayerImpl::chooseMove(const Scaffold& s, int N, int color)
{
    int i;
    cout <<"Please enter a column number: ";
    cin >>i;
    
    while (i < 1 || i > s.cols() || s.checker(i, 1) != VACANT)   //if the column entered is invalid, keep asking
    {
        cout <<"Please enter a VALID column number: ";
        cin >>i;
    }
    
    return i;
}



int BadPlayerImpl::chooseMove(const Scaffold& s, int N, int color)
{
    for (int i = 1; i < s.cols() + 1; i++)  //traverse through all of the columns
    {
        if(s.checker(i,1) == VACANT)   //if it is empty, return that position
        {
            return i;
        }
    }
    return -1;
}


bool checkD1(const Scaffold& s, int n, int nColor)
{
    if (s.rows() == 1 || s. cols() == 1)     //if there are only 1 row or 1 column, no need to check for diagonal
        return false;
    int count = 0;
    for (int i = 1; i < s.cols() + 1; i++)
    {
        for (int j = 1; j < s.rows() + 1; j++)    //traverse through the rows and colums
        {
            if (s.checker(i, j) == nColor)
            {
                int color = s.checker(i, j);   //save the color of the current piece, and set count to 1
                count = 1;
                if (n == count)
                {
                    return true;
                }
                for (int k = 1; k < n; k++)   //look for N in a row of the type color
                {
                    if ( (k + i) < s.cols()+1 && (k + j) < s.rows()+1)   //make sure that the location your are checking is actually in the scaffold
                    {
                        if (s.checker(k + i, k + j) == color)
                        {
                            count++;
                            if (count == n)   //if at any point you have N in a row, save the color of the type and return true
                            {
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




bool checkD2(const Scaffold& s, int n, int nColor) 
{
    if (s.rows() == 1 || s. cols() == 1)  //if there are only 1 row or 1 column, no need to check for diagonal
        return false;
    int count = 0;
    for (int i = 1; i < s.cols()+1; i++)
    {
        for (int j = 1; j < s.rows() + 1; j++)   //traverse through the rows and colums
        {
            if (s.checker(i, j) == nColor)
            {
                int color = s.checker(i, j);    //save the color of the current piece, and set count to 1
                count = 1;
                if (n == count)
                {
                    return true;
                }
                for (int k = 1; k < n; k++)    //look for N in a row of the type color
                {
                    if ( (i - k) > 0 && (j + k) < s.rows()+1)    //make sure that the location your are checking is actually in the scaffold
                    {
                        if (s.checker(i - k, j + k) == color)
                        {
                            count++;
                            if (count == n)     //if at any point you have N in a row, save the color of the type and return true
                            {
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




bool checkH(const Scaffold& s, int n, int nColor) 
{
    if (s. cols() == 1)   //if there is only 1 column, no need to check for horizontal
        return false;
    int count = 0;
    for (int i = 1; i < s.cols() + 1; i++)
    {
        for (int j = 1; j < s.rows() + 1; j++)   //traverse through the rows and colums
        {
            if (s.checker(i, j) == nColor)
            {
                int color = s.checker(i, j);   //save the color of the current piece, and set count to 1
                count = 1;
                if (n == count)
                {
                    return true;
                }
                for (int k = 1; k < n; k++)  
                {
                    if ((k + i) < s.cols() + 1)    //make sure that the location your are checking is actually in the scaffold
                    {
                        if (s.checker(k + i, j) == color)
                        {
                            count ++;
                            if (count == n)   //if at any point you have N in a row, save the color of the type and return true
                            {
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



bool checkV(const Scaffold& s, int n, int nColor)
{
    if (s.rows() == 1)   //if there is only 1 row, no need to check for any vertical
        return false;
    int count = 0;
    for (int i = 1; i < s.cols() + 1; i++)
    {
        for (int j = 1; j < s.rows() + 1; j++)   //traverse through the rows and colums
        {
            if (s.checker(i, j) == nColor)
            {
                int color = s.checker(i, j);   //save the color of the current piece, and set count to 1
                count = 1;
                if (n == count)
                {
                    return true;
                }
                for (int k = 1; k < n; k++)
                {
                    if ((k + j) < s.rows() + 1)  //make sure that the location your are checking is actually in the scaffold
                    {
                        if (s.checker(i, k + j) == color)
                        {
                            count ++;
                            if (count == n)   //if at any point you have N in a row, save the color of the type and return true
                            {
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



int SmartPlayerImpl::ratingFunction(Scaffold& temp, int N, bool isHuman, int nColor)
{
    if (isHuman && (checkV(temp, N, nColor) || checkH(temp, N, nColor) || checkD1(temp, N, nColor) || checkD2(temp, N, nColor))) //check if the computer won and then return 1
    {
        return 1; 
    }
     else if (!isHuman && (checkV(temp, N, nColor) || checkH(temp, N, nColor) || checkD1(temp, N, nColor) || checkD2(temp, N, nColor)))
    {
        return -1;
    }
    else if (!(checkV(temp, N, nColor) || checkH(temp, N, nColor) || checkD1(temp, N, nColor) || checkD2(temp, N, nColor)) && temp.numberEmpty() == 0) //check if it is a tie game and then return 0
    {
        return 0;
    }
    else
        return 2;  //if the game is still going on, then return some arbitrary integer
}




twoValues SmartPlayerImpl::determineBestComputerMove(Scaffold& s, int N, int color, bool isHuman, int count)
{
    twoValues f;
    for (int i = 1; i <= s.cols(); i++)    //iterate through all possible columns
    {
        if (!s.makeMove(i, color))   //make a move
            continue;
        int a = ratingFunction(s, N, isHuman, color);  //rate the move based on the rating function
        if (( a==1) || (a == 0) )   //if the move results in a win or a tie, save the move in an appropriate data structure
        {                Relate l;
            l.column = i;
            l.depth = count;
            if (a == 1)
            {
                one.push_back(l);
            }
            else if (a == 0)
            {

                zero.push_back(l);
            }
        }
        else   //if the game is still going on 
        {   twoValues t; 
            {
                if (color == BLACK)
                {    
                    t = determineBestComputerMove(s, N, RED, !isHuman, count + 1);   //recursively simulate the opponents move
                }
                else if (color == RED)
                {
                    t = determineBestComputerMove(s, N, BLACK, !isHuman, count + 1);
                }
            }
            if (t.rating == 0)   //if the oponents move evauate to a tie, save the move an an appropriate data structure
            {
                zero.push_back(t.r);
            }
            else if (t.rating == -1)  //if the oponents move evauate to a loss, save the move an an appropriate data structure
            {
                negativOne.push_back(t.r);
            }
        }
        s.undoMove(i);  //undo the initial move made
    }
if (isHuman)   
{
    if (!negativOne.empty())
    {
        int mindepth = INT_MAX;
        int minIndex = 0;
        for (int i = 0; i < negativOne.size(); i++)
        {
            if (negativOne[i].depth < mindepth)
            {
                mindepth = negativOne[i].depth;
                minIndex = i;
            }
        }
        f.r = negativOne[minIndex];
        
        negativOne[minIndex] = negativOne[negativOne.size() -1];
        negativOne.pop_back();
        f.rating = -1;  
    }
    else if (!zero.empty())  
    {
        int mindepth = INT_MAX;
        int minIndex = 0;
        for (int i = 0; i < zero.size(); i++)
        {
            if (zero[i].depth < mindepth)
            {
                mindepth = zero[i].depth;
                minIndex = i;
            }
        }
        f.r = zero[minIndex];
        zero[minIndex] = zero[zero.size() - 1];
        zero.pop_back();
        f.rating = 0;
    }
}
else   //if the user is a computer here
{
    if (!one.empty())
    {
        int mindepth = INT_MAX;
        int minIndex = 0;
        for (int i = 0; i < one.size(); i++)
        {
            if (one[i].depth < mindepth)
            {
                mindepth = one[i].depth;
                minIndex = i;
            }
        }
        f.r = one[minIndex];
        one[minIndex] = one[one.size() -1];
        one.pop_back();
        f.rating = 1;  
    }
    else if (!zero.empty()) //if no move results in a win for the computer, find one that results in a tie for it
    {
        int mindepth = INT_MAX;
        int minIndex = 0;
        for (int i = 0; i < zero.size(); i++)
        {
            if (zero[i].depth < mindepth)
            {
                mindepth = zero[i].depth;
                minIndex = i;
            }
        }
        f.r = zero[minIndex];
        zero[minIndex] = zero[zero.size() -1];
        zero.pop_back();
        f.rating = 0;
    }
}
return f;  //return a struct, with 1 value as the column number and the other as the result of the rating function
}




int SmartPlayerImpl::chooseMove(const Scaffold& s, int N, int color)
{
    Scaffold temp = s;
    return determineBestComputerMove(temp, N, color, false, 0).r.column;
        //Mekk moov.
        //Call cm on NEW BOARD
        //Undo move
    
}

//******************** Player derived class functions *************************

// These functions simply delegate to the Impl classes' functions.
// You probably don't want to change any of this code.

HumanPlayer::HumanPlayer(string nm)
 : Player(nm)
{
    m_impl = new HumanPlayerImpl;
}
 
HumanPlayer::~HumanPlayer()
{
    delete m_impl;
}
 
int HumanPlayer::chooseMove(const Scaffold& s, int N, int color)
{
    return m_impl->chooseMove(s, N, color);
}

BadPlayer::BadPlayer(string nm)
 : Player(nm)
{
    m_impl = new BadPlayerImpl;
}
 
BadPlayer::~BadPlayer()
{
    delete m_impl;
}
 
int BadPlayer::chooseMove(const Scaffold& s, int N, int color)
{
    return m_impl->chooseMove(s, N, color);
}

SmartPlayer::SmartPlayer(string nm)
 : Player(nm)
{
    m_impl = new SmartPlayerImpl;
}
 
SmartPlayer::~SmartPlayer()
{
    delete m_impl;
}
 
int SmartPlayer::chooseMove(const Scaffold& s, int N, int color)
{
    return m_impl->chooseMove(s, N, color);
}
