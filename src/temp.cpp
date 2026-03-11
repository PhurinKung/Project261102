#include "functions.h"

int main() {
	CalendarManager MyCalendar;

	MyCalendar.loadFromFile();

	std::cout << "Loaded File! there are " << MyCalendar.getAllEvents().size() << " data\n";
	for (auto i : MyCalendar.getAllEvents()) {
		std::cout << i.getTitle() << "\n";
	}

	int i = 1; char op;
	while (1) {
		std::cout << "Do you want to add new Event? [Y/N]: ";
		std::cin >> op;
		
		if (op == 'N') break;
		
		std::string title, category, place, detail;
		int day, month, year;

		std::cout << "Title ? ";
		std::cin >> title;
		
		std::cout << "Start time [dd mm yyyy] ? ";
		std::cin >> day >> month >> year;

		time_t startTime = DMYtoTime(day, month, year);

		std::cout << "End time [dd mm yyyy] ? ";
		std::cin >> day >> month >> year;

		time_t EndTime = DMYtoTime(day, month, year);

		std::cout << "category ? ";
		std::cin >> category;

		std::cout << "place ? ";
		std::cin >> place;

		std::cout << "detail ? ";
		getline(std::cin, detail);

		Event newEvent(title, startTime, EndTime, category, detail, place);
		
		bool status;
		std::string MSG;

		std::tie(status,MSG) = MyCalendar.addEvent(newEvent);
		
		std::cout << "Event[" << i << "] status : " << MSG;

		++i;
	}

	MyCalendar.saveToFile();
	std::cout << "Saved data!";
	return 0;
}

