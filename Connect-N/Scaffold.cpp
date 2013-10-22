// Scaffold.cpp

#include "provided.h"
#include <algorithm>
#include <iostream>
using namespace std;

class ScaffoldImpl
{
  public:
    ScaffoldImpl(int nColumns, int nRows); 
    int cols() const;
    int rows() const;
    int numberEmpty() const;
    int checker(int c, int r) const; 
    void display() const; 
    bool makeMove(int c, int color); 
    int undoMove(int c); 
  private:
    vector<vector<int> > grid;
    int Col;
    int Row;
    int emptyPositions;
};

ScaffoldImpl::ScaffoldImpl(int nColumns, int nRows):Col(nColumns), Row(nRows), emptyPositions(Col*Row)
{
    
    grid.resize(nRows);  // grid now has R empty rows
    for (int r = 0; r < nRows; r++)
    {
        grid[r].resize(nColumns);  // row r now has C columns
    }
    for (int r = 0; r < nRows; r++)
    {
        for (int c = 0; c < nColumns; c++)
        {
            grid[r][c] = VACANT;
        }
    }

}


//////////////////////////////////////////////////////
int ScaffoldImpl::cols() const
{
    return Col; 
}

int ScaffoldImpl::rows() const
{
    return Row;  
}

int ScaffoldImpl::numberEmpty() const
{
    return emptyPositions; 
}

///////////////////////////////////////////////////////

int ScaffoldImpl::checker(int c, int r) const
{
    if (grid[r-1][c-1] == RED)
        return RED;
    else if (grid[r-1][c-1] == BLACK)
        return BLACK;
    else
        return VACANT;
}



void ScaffoldImpl::display() const
{
    for (int i = 0; i < Row; i++)
    {
        cout <<"|";
        for (int j = 0; j < Col; j++)
        {
            if (grid[i][j] == RED)
                cout <<"R|";
            else if (grid[i][j] == BLACK)
                cout <<"B|";
            else
                cout <<" |";
        }
        cout <<endl;
    }
    for (int k = 0; k < Col; k++)
    {
        cout <<"+-";
    }
    cout <<"+"<<endl;
}


bool ScaffoldImpl::makeMove(int c, int color)
{
    if (c < 1 || c > Col)
        return false;
        if (color == RED || color == BLACK)
        {
            for (int i = Row - 1; i >= 0; i--)
            {    
                if (grid[i][c - 1] == VACANT)
                {
                    grid[i][c - 1]= color;
                    emptyPositions--;
                    return true;
                }
            }
        }
    return false;
}



int ScaffoldImpl::undoMove(int c)
{
    if (c < 1 || c > Col)
        return VACANT;
    for (int i = 0; i < Row; i++)
    {
        if (grid[i][c-1] == BLACK)
        {
            grid[i][c-1] = VACANT;
            emptyPositions++;
            return BLACK;
        }
        if (grid[i][c-1] == RED)
        {
            grid[i][c-1] = VACANT;
            emptyPositions++;
            return RED;
        }
    }
    return VACANT;
}

//******************** Scaffold functions *******************************

// These functions simply delegate to ScaffoldImpl's functions.
// You probably don't want to change any of this code.

Scaffold::Scaffold(int nColumns, int nRows)
{
    m_impl = new ScaffoldImpl(nColumns, nRows);
}
 
Scaffold::~Scaffold()
{
    delete m_impl;
}
 
Scaffold::Scaffold(const Scaffold& other)
{
    m_impl = new ScaffoldImpl(*other.m_impl);
}
 
Scaffold& Scaffold::operator=(const Scaffold& rhs)
{
    if (this != &rhs)
    {
        Scaffold temp(rhs);
        swap(m_impl, temp.m_impl);
    }
    return *this;
}
 
int Scaffold::cols() const
{
    return m_impl->cols();
}

int Scaffold::rows() const
{
    return m_impl->rows();
}

int Scaffold::numberEmpty() const
{
    return m_impl->numberEmpty();
}

int Scaffold::checker(int c, int r) const
{
    return m_impl->checker(c, r);
}
 
void Scaffold::display() const
{
    m_impl->display();
}
 
bool Scaffold::makeMove(int c, int color)
{
    return m_impl->makeMove(c, color);
}
 
int Scaffold::undoMove(int c)
{
    return m_impl->undoMove(c);
}
