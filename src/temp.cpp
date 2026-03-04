#include "functions.h"

int main() {
	CalendarManager MyCalendar;

	int day, month, year;
	cout << "input 1st Event date [d/m/yyyy]: ";
	cin >> day >> month >> year;

	Event newEvent("Comprog Exam", DMYtoTime(day, month, year), DMYtoTime(day, month, year), "University", "Don't for get to read", "CMU");
	
	bool status; string ErrorMSG;

	tie(status, ErrorMSG) = MyCalendar.addEvent(newEvent);

	if (status) cout << "Added newEvent (1) !\n";
	else cout << "Can't Add new event TT [ErrorMSG]: " << ErrorMSG << "\n";

	cout << "input 2nd Event date [d/m/yyyy]: ";
	cin >> day >> month >> year;

	Event newEvent2("Eng Exam", DMYtoTime(day, month, year), DMYtoTime(day-1, month, year), "University", "Don't for get to read", "CMU");

	tie(status, ErrorMSG) = MyCalendar.addEvent(newEvent2);

	if (status) cout << "Added newEvent (2) !\n";
	else cout << "Can't Add new event TT [ErrorMSG]: " << ErrorMSG << "\n";

	for (auto i : MyCalendar.getAllEvents()) {
		cout << i.getTitle() << "\n";
	}

}

