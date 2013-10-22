//
//  eval.cpp
//  CS 32 Homework 2 Part 2
//
//  Created by  on 4/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <string>
#include <stack>
#include <cassert>
using namespace std;

int evaluate(const string& infix, string& postfix, bool& result);
bool precedence (char a, char b);
bool value(string postfix);
bool validateInfix(string infix);


bool precedence (char a, char b) //does a have higher or equal to precedence than b?
{
    if (a == b)
        return true;
    if (a == '&' && b =='|')
        return true;
    if (a == '&' && b == '!')
        return false;
    if (a == '!' && b =='|')
        return true;
    if (a == '!' && b =='&')
        return true;
    if (a == '|' && b =='&')
        return false;
    if (a == '|' && b =='!')
        return false;
}

bool validateInfix(string infix)
{
    if (infix[0] == '|' || infix[0] == '&' || infix[0] == ')')
    {
        return false;
    }
    if (infix[infix.size() - 1] == '|' || infix[infix.size() -1] == '&')
    {
        return false;
    }
    for (int i = 0; i < infix.size()-1; i++)
    {
        if ((infix[i] == 'T' || infix[i] == 'F') && (infix[i+1] == 'T' || infix[i+1] == 'F'))
        {
            return false;
        }
        if ((infix[i] == '|' || infix[i] == '&') && (infix[i+1] == '|' || infix[i+1] == '&'))
        {
            return  false;
        }
        if (infix[i] == '(')
        {
            if (infix[i+1] == '&' || infix[i+1] == '|' || infix[i+1] == ')')
            {
                return false;
            }
        }
        if (infix[i] == ')')
        {
            if (infix [i-1] == '&' || infix[i-1] == '|' || infix[i-1] =='!')
            {
                return false;
            }
            if (infix [i+1] == 'T' || infix[i+1] == 'F' || infix[i+1] == '!')
            {
                return false;
            }
        }
    }
    for (int m = 0; m < infix.size(); m++)
    {
        if (infix[m] == ')')
        {
            if (infix [m-1] == '&' || infix[m-1] == '|' || infix[m-1] =='!')
            {
                return false;
            }
        }
    }
    return true;
}

bool value(string postfix, bool& good)
{
    good = true;
    string temp = postfix;
    stack <bool> A;
    for (int i = 0; i < temp.size(); i++)
    {
        if (temp[i] == 'T')
        {
            A.push(true);
        }
        else if (temp[i] == 'F')
        {
            A.push(false);
        }
        else if(temp[i] == '&' || temp[i] == '|') //if it's an operator
        {
            bool m = A.top();
            A.pop();
            bool n = A.top();
            A.pop();
            if (temp[i] == '&')
            {
                A.push(m && n);
            }
            else if (temp[i] == '|')
            {
                A.push(m || n);
            }
        }
        else if (temp[i] == '!')
        {
            bool q = A.top();
            A.pop();
            A.push(!q);
        }
        else 
        {
            good = false;
        }
    }
    if (A.size() != 1)
    {
        good = false;
        return false;
    }
    return A.top();
}


int evaluate(const string& infix, string& postfix, bool& result)
{
    if (!validateInfix(infix))
    {
        return 1;
    }
    postfix = "";
    stack <char> A;
    for (int i = 0; i < infix.size(); i ++)
    { 
       if (infix[i] == 'T' || infix[i] == 'F')
           {
               postfix+=infix[i];
           }
       else if (infix[i] == '(' || infix[i] == '!' )
           {
               A.push(infix[i]);
           }
        else if ( (infix[i] == '&' || infix[i] == '|') && A.empty())
        {
            A.push(infix[i]);
        }
        else if ( (infix[i] == '&' || infix[i] == '|') && (!A.empty()))
        {
            int b = A.size();
            for (int j = 0; j < b; j++)
            {
               if (A.top() == '&' || A.top() == '|' || A.top() == '!')
               {
                   if (precedence(A.top(), infix[i]))
                    {
                        postfix+=A.top();
                        A.pop();
                    }
                    else
                       break;
               }
               else
                   break;
            }
            A.push(infix[i]);
        }
        else if (infix[i] == ')')
        {
            {
                while (!A.empty() && A.top() != '(')
                    {
                        postfix+=A.top();
                        A.pop();
                    }
                    if (A.empty())
                    {
                        return 1;
                    }
            }
            A.pop();
        }
        else if (infix[i] == ' ')
        {
            
        }
        else
            return 1;
    }
    
    while (A.size() != 0)
    {
        postfix+=A.top();
        A.pop();
    }
    bool temp;
    value(postfix, temp);
    if (temp == false)
        return 1;
    else
        result = value(postfix, temp);
    return 0;
     
}






 

            



    
