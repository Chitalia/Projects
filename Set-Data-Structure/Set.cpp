// Rohan Chitalia
//  Set.cpp
//  CS 32 Project 2
//
//  Created by  on 4/21/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Set.h"

Set::Set()
{
    m_size = 0;
    head = NULL;    //creates new set of size 0 with head pointing to NULL
}


Set::~Set()
{
    if (m_size != 0) //make sure there are elements in the set
    {
        Node* current = head;
        while (current != NULL) //loop through the non-empty set
        {
            Node*newOne;    //create a dummy pointer
            newOne = current->m_next;   //point the dummy to the current's next
            delete current; 
            current = newOne;   //current is now the next one
        }
    }
    m_size = 0;
}



Set::Set(const Set& other) 
{
    m_size = other.m_size; //get the size of the other set
    Node* current;
    Node* lastNew = NULL;
    if (m_size != 0)
    {
        for (current = other.head; current != NULL; current = current->m_next) //loop through the other set
        {
        
            Node* temp = new Node;  //allocated a new node
            temp->type = current->type; 
            temp->m_prev = lastNew;
            if (lastNew == NULL)  //if this is the first node we are adding, make head point to it
            {
                head = temp;
            }
            else
            {
            
                lastNew->m_next = temp;
            }
            lastNew = temp;     //the current node is now the last new one
            if (current->m_next == NULL)
            {
                temp->m_next = NULL;
            }
        }
    }
    
}


Set&Set::operator=(const Set& other)
{
    if (this != &other)
    {
        Set temp = other;   //create a dummy set with the same values as other
        swap(temp);     //swap this with the current set, so that the current set now has the values of other
    }
    return (*this);
}



bool Set::empty() const
{
    return (m_size == 0);
}



int Set::size() const
{
    return m_size;
}


bool Set::insert(const ItemType& value)
{
    if (contains(value))
    {
        return false; //leave if the set already contains the value
    }
    Node* current;
    if (m_size == 0)    //if this is the first node we are adding
    {
        Node* newItem = new Node;   //create the node
        head = newItem;             //have head point to it
        newItem->m_next = NULL;    
        newItem->m_prev = NULL;
        newItem->type = value;      //give it the value in the parameter
        m_size++;                   
        return true;
    }
    else
    {
        for (current = head; current != NULL; current = current->m_next)
        {
            if (current->m_next == NULL)        //make sure we are at the end of the list
            {
                Node* newItem = new Node;
                current->m_next = newItem;
                newItem->m_prev = current;
                newItem->type = value;          //give the node the value needed
                newItem->m_next = NULL;
                m_size++;
                return true;
            }
        }
        
    }
    
}



bool Set::erase(const ItemType& value)
{
    if (!contains(value))
    {
        return false;
    }
    Node* current;
    for (current = head; current != NULL; current = current->m_next) //loop through every node
    {
        if ((current->type == value) && (m_size == 1)) //check if this is the one and only node
        {
                head = NULL;
                delete current;
                m_size--;
                return true;
        }
        else if ((current->type == value) && (current->m_prev == NULL) && (current->m_next != NULL)) //check if the node is the first of multiple nodes in the set
        {
            head = current->m_next;
            current->m_next->m_prev = NULL;
            delete current;
            m_size--;
            return true;
        }
        else if ((current->m_next != NULL) && (current->type == value) && (current->m_prev != NULL)) //check if this is a node in between 2 other nodes
        {
            current->m_prev->m_next = current->m_next;
            current->m_next->m_prev = current->m_prev;
            delete current;
            m_size--;
            return true;
        }
        else if (current->type == value && (current->m_next == NULL))  //check if this is the last node
        {
            current->m_prev->m_next = NULL;
            delete current;
            m_size--;
            return true;
        }
    }
    return false;
}



bool Set::contains(const ItemType& value) const
{
    if (m_size == 0)
    {
        return false;
    }
    Node* current;
    for (current = head; current != NULL; current = current->m_next) //loop through the set, looking for the value
    {
        if (current->type == value)
        {
            return true;
        }
    }
    return false;
}


bool Set::get(int pos, ItemType& value) const
{
   if (m_size == 0)
   {
       return false;
   }
    if (pos >= 0 && pos < m_size)
    {
        Node* current = head;
        for (int i = 0; i < pos; i++)  //move pos positions in the set
        {
            current = current->m_next;
        }
        value = current->type;      //get the value of the pos number node
        return true;
    }
    return false;
}


void Set::swap(Set& other)
{
    int temp = m_size;          //swap the sizes
    m_size = other.m_size;
    other.m_size = temp;
    
    Node* one = head;           //swap the heads
    head = other.head;
    other.head = one;
}

void Set::dump() const
{
    if (m_size != 0)
    {
        for(Node* q = head; q != NULL; q = q->m_next)
        {
            std::cerr << q->type << std::endl;   // outputs the Set's values
        }
    }
    std::cerr<<"The set has size "<<m_size<<std::endl;
}

void unite(const Set& s1, const Set& s2, Set& result)
{
    Set A = s1;
    Set B = s2;
    result = A;
         
    for (int a = 0; a < A.size(); a++)
    {
        ItemType value;
        A.get(a, value);
        if (B.contains(value))
        {
            
            B.erase(value);    //if set B contains a value in A, then erase it from B
        }
    }
    for (int c = 0; c < B.size(); c++)  
    {
        ItemType value;
        B.get(c, value);
        result.insert(value);       //now insert every value in B into result (these don't contain any DUPLICATES from A)
    }
    
}

void subtract(const Set& s1, const Set& s2, Set& result)
{
    Set A = s1;
    Set B = s2;
    Set C;
    result = C; //clear the contents of result by setting it equal to empty Set C
    
    for (int a = 0; a < B.size(); a++)
    {
        ItemType value;
        B.get(a, value);
        if (A.contains(value))
        {
            A.erase(value);     //if A contains a value that is also in B, delete it from A
        }
        
    }
    
    for (int d = 0; d < A.size(); d++)
    {
        ItemType value;
        A.get(d, value);
        result.insert(value);   //insert these into result, which now only contains values from A and not B
    }
}

