//  Rohan Chitalia
//  Set.h
//  CS 32 Project 2
//
//  Created by  on 4/21/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CS_32_Project_2_Set_h
#define CS_32_Project_2_Set_h
#include <string>


typedef std::string ItemType;

class Set
{
public:
    Set();
    
    ~Set();
    
    Set(const Set& other);
    
    Set &operator=(const Set& other);
    
    bool empty() const;
    
    int size() const;
    
    bool insert(const ItemType& value);
    
    bool erase(const ItemType& value);
    
    bool contains(const ItemType& value) const;
    
    bool get(int pos, ItemType& value) const;
    
    void swap(Set& other);
    
    void dump() const;
private:
    struct Node
    {
        ItemType type;
        Node* m_next;
        Node* m_prev;
    };
    
    int m_size;
    Node* head;
    
};

void unite(const Set& s1, const Set& s2, Set& result);
void subtract(const Set& s1, const Set& s2, Set& result);

#endif
