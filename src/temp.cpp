#include "functions.h"

int main() {
	CalendarManager MyCalendar;

	Event newEvent("Comprog Exam", 1, 1, "University", "Don't for get to read", "CMU");

	if (MyCalendar.addEvent(newEvent)) cout << "Added newEvent (1) !\n";
	else cout << "Can't Add new event TT\n";

	Event newEvent2("Eng Exam", 0, 0, "University", "Don't for get to read", "CMU");

	if (MyCalendar.addEvent(newEvent2)) cout << "Added newEvent (2) !\n";
	else cout << "Can't Add new event TT\n";
	
	vector<Event> temporary;
	temporary = MyCalendar.getAllEvents();
	
	for (auto i : temporary) cout << i.getTitle() << "\n";

}

