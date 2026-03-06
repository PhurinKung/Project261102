#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include<tuple>
#include<ctime>
#include<algorithm>
#include<cctype>
#include <exception>
#include <sstream>

class Utils {
public:
	static std::tuple<int, int, int, int, int> timeToDMY(time_t tme);
	static time_t DMYtoTime(int day, int month, int year, int hour = 0, int minute = 0);

	static std::string ToLowerSafe(std::string s);
	static bool findthisword(const std::string& keyword, std::string source);
};

class Event {
private:
	std::string title;
	time_t E_start, E_end;
	std::string category, details, places;
	unsigned long long id;

	// note: can add more if want to
public:
	Event(std::string t = "", time_t s = 0, time_t e = 0, std::string c = "Default",
		std::string d = "", std::string p = "", unsigned long long i = 0) { // title
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
	std::string getTitle() const { return title; }
	time_t getStartTime() const { return E_start; }
	time_t getEndTime() const { return E_end; }
	std::string getCategory() const { return category; }
	std::string getDetails() const { return details; }
	std::string getPlaces() const { return places; }
	unsigned long long getID() const { return id; }

	void changeCats(std::string newCat) { category = newCat; }

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
	std::string category;
	float r, g, b, a;
	
	std::string toPatternName(std::string s) {
		if (s.empty()) return s;

		s = Utils::ToLowerSafe(s);
		s[0] = toupper((unsigned char)s[0]);
		return s;
	}

public:
	EventCategory(std::string name, float R, float G, float B, float A) {
		category = toPatternName(name);
		r = R;
		g = G;
		b = B;
		a = A;
	}

	std::string getname() const { return category; }
	std::tuple<float, float, float, float> getRGB() const { return { r,g,b,a }; }
};

class CalendarManager {
private:
	std::vector<Event> allEvents; // contains all events

	std::vector<EventCategory> categories;

	std::string data_filename = "calendar_data.txt"; // save all events in this file na / can change name na krub
	std::string categories_data_filename = "categories_data.txt";

	void sortEvents(); //function to sort all events (by date)
	unsigned long long nextID = 1; // ID for each events, update every time we add new one 
public:
	CalendarManager(); // auto load
	~CalendarManager(); // auto save

	//file I/O
	void saveToFile();
	void loadFromFile();

	//core logic
	std::pair<bool, std::string> addEvent(Event newEvent); // return status and error message
	std::pair<bool, std::string> deleteEvent(unsigned long long eventId); // return status and error message
	std::pair<bool, std::string> editEvent(unsigned long long eventId, Event updatedEvent); // return status and error message

	std::pair<bool, std::string> addCategory(EventCategory newCategory);
	std::pair<bool, std::string> deleteCategory(std::string sadCategory);

	// Find & Search
	const std::vector<Event>& getAllEvents() const; // ใช้ const& ไม่ให้ copy ข้อมูล เปลืองแรม
	std::vector<Event> getEventsByDate(int day, int month, int year);
	std::vector<Event> searchEvents(std::string keyword);
	std::vector<Event> getUpcomingEvents(int N); // next N events 


	std::vector<std::string> getCategories();
	std::vector<std::pair<std::string, std::tuple<float, float, float, float >>> getColorCategory(); // return pair <string name,tuple RGBA>
};