// Dictionary.cpp
//Rohan Chitalia
// This is a correct but horribly inefficient implementation of Dictionary
// functionality.

#include "Dictionary.h"
#include <string>
#include <list>
#include <cctype>
#include <vector>
#include <algorithm>  // for swap
using namespace std;

void removeNonLetters(string& s);
string sortHim(string permutation);

struct Mapper
{
    string sortedWord;
    vector<string> anagramWords;
    
};

  // This class does the real work of the implementation.  

class DictionaryImpl
{
  public:
    DictionaryImpl():Hash_dict(50000) {}
    ~DictionaryImpl() {}
    void insert(string word);
    void lookup(string letters, void callback(string)) const;
    int Key(const string& a) const;
  private:
    vector <list<Mapper> > Hash_dict;
    
};


void DictionaryImpl::insert(string word)
{
    removeNonLetters(word);   //remove all non letters from word
    if (!word.empty())
    {
        string temp = sortHim(word);   //sort the given word
        int b = Key(word);   //call the hash function
    
        for (list<Mapper>::iterator a = Hash_dict[b].begin(); a != Hash_dict[b].end(); a++)   //loop through the hash table to see if anagrams of the word are already in the dictionary
        {
            if (a->sortedWord == temp)
            {
                a->anagramWords.push_back(word);  //if so, push back the word
                return;
            }
        } 
    Mapper A;  //if not, create a new struct with the value and push that back
    A.sortedWord = temp;
    A.anagramWords.push_back(word);
    Hash_dict[b].push_back(A);
    }
}


void removeNonLetters(string& s)
{
    string::iterator to = s.begin();
    for (string::const_iterator from = s.begin(); from != s.end(); from++)
    {
        if (isalpha(*from))
        {
            *to = tolower(*from);
            to++;
        }
    }
    s.erase(to, s.end());  // chop off everything from "to" to end.
} 



int DictionaryImpl::Key(const string& a) const  //HASH FUNCTION
{
    int total = 0;
    for (int i = 0; i < a.size(); i++) 
    {
        total+=(a[i]*a[i]*a[i]);   //sum up the cube of the askii values of each character
    }
    return total % 50000;  //mod it by 50000 (for number of buckets)
}




void DictionaryImpl::lookup(string letters, void callback(string)) const
{
    
if (callback == NULL)
    return;
removeNonLetters(letters);
if (letters.empty()) 
    return;
int b = Key(letters);  //call the hash function
string sorted = sortHim(letters);
for (list<Mapper>:: const_iterator a = Hash_dict[b].begin(); a != Hash_dict[b].end(); a++)  //loop through the elements of the linked list
    {
        if (a->sortedWord == sorted)  //if anagrams exist
        {
            for (vector<string>::const_iterator q = a->anagramWords.begin(); q!= a->anagramWords.end(); q++)
            {
                callback(*q);  //callback each anagram
            }
        }
    }
    
        
}
    

string sortHim(string permutation) 
{
    sort(permutation.begin(), permutation.end());
    return permutation;
}

//******************** Dictionary functions **********************************

// These functions simply delegate to DictionaryImpl's functions.
// You probably don't want to change any of this code.

Dictionary::Dictionary()
{
    m_impl = new DictionaryImpl;
}

Dictionary::~Dictionary()
{
    delete m_impl;
}

void Dictionary::insert(string word)
{
    m_impl->insert(word);
}

void Dictionary::lookup(string letters, void callback(string)) const
{
    m_impl->lookup(letters, callback);
}
