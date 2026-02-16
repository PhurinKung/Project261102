#pragma once
#include<bits/stdc++.h>

using namespace std;

class Event{
private:
    string title;

    // ? : how to store an event time ?? using tuple or else (does it have anything easier mai krub)
    tuple<int,int,int,int> E_start, E_end; // Year month date time
    
    string category, details, places;    
    // note: can add more if want to

public:
//todo : write all func 

    void create(); // to do : write  input
    
    // ? : use operator to know what edit or just write all func
    void editString(char, string); 
    void editTime(char, tuple<int,int,int,int>);

    //return private variables
    string getTitle(){ return title; }
    tuple<int,int,int,int> getStartTime(){ return E_start; }
    tuple<int,int,int,int> getStartTime(){ return E_end; }
    string getCategory(){ return category; }
    string getDetails(){ return details; }
    string getPlaces(){ return places; }
};

void Event::create(){
    
}

void Event::editString(char op , string temp){
    // ? : do like this mai krub
    
    if(op == 't'){ // edit title
        // ? : recheck word 
        title = temp;
    }
    else if(op){
        // bla bla bla
    }

    return;
}

void Event::editTime(char, tuple<int,int,int,int>){
    // edit what is not strings
}

// ------------------------------------------------
// all others func.

vector<Event> showWhatsNext(int); 
// show first n Events that upcoming

vector<Event> showthisdate(); 
// show Events on this date 
// ? : what should I input na

vector<Event> showthisRange();
// show Events on these range 
// ? : what should I input na

// ?? : can func 2 and 3 group together

Event searchFor(string);
// show event from serch
// ? : return with pointer mai?