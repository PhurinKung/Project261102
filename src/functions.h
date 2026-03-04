#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include<iostream>
#include<tuple>
#include<string>
#include<vector>
#include<ctime>
#include<algorithm>
#include<cctype>

using namespace std;

tuple<int, int, int, int, int> timeToDMY(time_t tme);

time_t DMYtoTime(int day, int month, int year, int hour = 0, int minute = 0);

class Event {
private:
	string title;
	time_t E_start, E_end;
	string category, details, places;
	unsigned long long id;

	// note: can add more if want to
public:
	Event(string t = "", time_t s = 0, time_t e = 0, string c = "Default",
		string d = "", string p = "", unsigned long long i = 0) { // title
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
	string getTitle() const { return title; }
	time_t getStartTime() const { return E_start; }
	time_t getEndTime() const { return E_end; }
	string getCategory() const { return category; }
	string getDetails() const { return details; }
	string getPlaces() const { return places; }
	unsigned long long getID() const { return id; }

	// compare func for sort
	// sort by start date, end date, id
	bool operator<(const Event& a) const {
		if (E_start != a.E_start) return E_start < a.E_start;
		if (E_end != a.E_end) return E_end < a.E_end;
		return id < a.id;
	}
};

class EventCategory {
private:
	string category;
	float r, g, b, a;
public:
	EventCategory(string name, float R, float G, float B, float A) {
		category = name;
		r = R;
		g = G;
		b = B;
		a = A;
	}

	string getname() const { return category; }
	tuple<float, float, float, float> getRGB() const { return { r,g,b,a }; }
};

class CalendarManager {
private:
	vector<Event> allEvents; // contains all events

	vector<EventCategory> categories;

	string data_filename = "calendar_data.txt"; // save all events in this file na / can change name na krub
	string categories_data_filename = "categories_data.txt";

	void sortEvents(); //function to sort all events (by date)
	unsigned long long nextID = 1; // ID for each events, update every time we add new one 
public:
	CalendarManager(); // auto load
	~CalendarManager(); // auto save

	//file I/O
	void saveToFile();
	void loadFromFile();

	//core logic
	pair<bool, string> addEvent(Event newEvent); // return status and error message
	pair<bool, string> deleteEvent(unsigned long long eventId); // return status and error message
	pair<bool, string> editEvent(unsigned long long eventId, Event updatedEvent); // return status and error message

	// Find & Search
	const vector<Event>& getAllEvents() const; // ใช้ const& ไม่ให้ copy ข้อมูล เปลืองแรม
	vector<Event> getEventsByDate(int day, int month, int year);
	vector<Event> searchEvents(string keyword);
	vector<Event> getUpcomingEvents(int N); // next N events 

	vector<string> getCategories();
	vector<pair<string, tuple<float, float, float, float >>> getColorCategory(); // return pair <string name,tuple RGBA>
};