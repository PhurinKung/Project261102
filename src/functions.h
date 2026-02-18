#pragma once
#include<iostream>
#include<tuple>
#include<string>
#include<vector>
#include<ctime>
#include<algorithm>

using namespace std;

class Event{
private:
    string title;
    time_t E_start, E_end;
    string category, details, places;
    unsigned long long id;
    
    // note: can add more if want to
public:
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

    void setID(unsigned long long n) {
        id = n;
    }

    //return private variables
    string getTitle(){ return title; }
    time_t getStartTime(){ return E_start; }
    time_t getEndTime(){ return E_end; }
    string getCategory(){ return category; }
    string getDetails(){ return details; }
    string getPlaces(){ return places; }
    unsigned long long getID(){ return id; }

    // compare func for sort
    // sort by start date, end date, id
    bool operator<(const Event& a) const {
        if (E_start != a.E_start) return E_start < a.E_start;
        if (E_end != a.E_end) return E_end < a.E_end;
        return id < a.id;
    }
};

class CalendarManager {
private:
    vector<Event> allEvents; // contains all events
    
    //todo : what we want to do with category na? write functions noi krub
    //where we save all category d krub, maybe other file mai or in data file pai loi
    vector<string> categories; // contains all categories ?????


    string datafilename = "calendar_data.txt"; // save all events in this file na / can change name na krub

    void sortEvents(); //function to sort all events (by date)
    unsigned long long nextID = 1; // ID for each events, update every time we add new one 
public:
    CalendarManager(); // auto load
    ~CalendarManager(); // auto save

    //file I/O
    void saveToFile();
    void loadFromFile();

    //core logic
    bool addEvent(Event newEvent); // return true if add sucess
    bool deleteEvent(int eventId); // return true if delete sucsees
    bool editEvent(int eventId, Event updatedEvent); // return true if edit sucsess

    // Find & Search
    const vector<Event>& getAllEvents() const; // ใช้ const& ไม่ให้ copy ข้อมูล เปลืองแรม
    vector<Event> getEventsByDate(int day, int month, int year);
    vector<Event> searchEvents(string keyword);
    vector<Event> getUpcomingEvents(int N); // next N events 
};

// functions definitions

CalendarManager::CalendarManager() {loadFromFile();}
CalendarManager::~CalendarManager() {saveToFile();}

void CalendarManager::loadFromFile() {
    // todo : recieve all data from filename to vector<Event>
    // dont forget to run id -> set nextID to maxid + 1
    
    // ? : if we already have data should we delete first ?
}

void CalendarManager::saveToFile() {
    // todo : put all data from vector<Event>  to filename (aka. save functions)
}

void CalendarManager::sortEvents() {
    sort(allEvents.begin(), allEvents.end());
}


bool CalendarManager::addEvent(Event newEvent){
    //todo : add newEvent to allEvents
    //todo : check if the date is correct
    
    newEvent.setID(nextID++); //run id
    allEvents.push_back(newEvent);
    this->sortEvents();//added event then sort
    return true;

    
    //return true if sucsess
}
bool CalendarManager::deleteEvent(int eventId){
    //todo : delete event by look for this eventID
    
    //return true if sucsess
}
bool CalendarManager::editEvent(int eventId, Event updatedEvent) {
    //todo : look for this eventId and update
    //if the date is change then sort (or just sort every time)
    //return true if sucsess
}

const vector<Event>& CalendarManager::getAllEvents() const {
    return allEvents;
}

vector<Event> CalendarManager::getEventsByDate(int day, int month, int year) {
    //todo : getallEvent on Date
    //maybe using lowerbound
}

vector<Event> CalendarManager::searchEvents(string keyword) {
    //todo : search for events by keyword  
}

vector<Event> CalendarManager::getUpcomingEvents(int N) {
    vector<Event> upcoming;
    int n = min(N, (int)allEvents.size());
    for (int i = 0; i < n ; ++i) upcoming.push_back(allEvents[i]);
    return upcoming;
}