#pragma once
#include<iostream>
#include<tuple>
#include<string>
#include<vector>
#include<ctime>

using namespace std;

class Event{
private:
    string title;
    time_t E_start, E_end;
    string category, details, places;
    unsigned long long id;
    
    // note: can add more if want to

    // compare func for sort
    // sort by start date, end date, id
    // bool operator<(const Event &a) const {
    //     if(E_start != a.E_start) return E_start < a.E_start;
    //     if(E_end != a.E_end) return E_end < a.E_end;
    //     return id < a.id;
    // }

public:
//todo : write all func 

    Event(string t = "", time_t s = 0, time_t e = 0, string c = "Default", 
            string d = "", string p = "", unsigned long long i = 0){
        title = t;
        E_start = s;
        E_end = e;
        category = c;
        details = d;
        places = p;
        id = i;
    }
   
    // ? : use operator to know what edit or just write all func
    void editString(char, string); 
    void editTime(char, time_t);

    //return private variables
    string getTitle(){ return title; }
    time_t getStartTime(){ return E_start; }
    time_t getEndTime(){ return E_end; }
    string getCategory(){ return category; }
    string getDetails(){ return details; }
    string getPlaces(){ return places; }
};


void Event::editString(char op , string temp){
    // ? : do like this mai krub
    
    if(op == 't'){ // edit title
        // ? : recheck word before edit, like-- does it have '|' 
        title = temp;
    }
    else if(op){
        // bla bla bla
    }

    return;
}

void Event::editTime(char op, time_t newTime){
    // edit what is not strings
}

// ------------------------------------------------
// all others func.

vector<Event> showWhatsNext(int); 
// show first n Events that upcoming

vector<Event> showthisRange(time_t, time_t); 
// in case u want 2 find a date, use time start and time end on that date to find e_start of every event

Event searchFor(string);
// show event from search
// ? : return with pointer mai?