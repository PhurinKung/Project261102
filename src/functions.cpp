#include "functions.h"
// complete all func in functions.h

std::tuple<int, int, int, int, int> timeToDMY(time_t tme) { // day month year hour minute
	tm local_time;
	localtime_s(&local_time, &tme);

	int day = local_time.tm_mday;       // (1 - 31)
	int month = local_time.tm_mon + 1;  // (0 - 11) -> +1 = 1 - 12 
	int year = local_time.tm_year + 1900; // start from 1900 -> +1900 get current year
	int hour = local_time.tm_hour;
	int minute = local_time.tm_min;

	return std::make_tuple(day, month, year, hour, minute);
}

time_t DMYtoTime(int day, int month, int year, int hour , int minute ) {
	tm time_info = { 0 };

	time_info.tm_mday = day;
	time_info.tm_mon = month - 1;
	time_info.tm_year = year - 1900;
	time_info.tm_hour = hour;
	time_info.tm_min = minute;

	time_info.tm_isdst = -1;

	return mktime(&time_info);
}


// functions definitions

CalendarManager::CalendarManager() { loadFromFile(); }
CalendarManager::~CalendarManager() { saveToFile(); }

void CalendarManager::loadFromFile() {
	//load from data
	std::ifstream src;
	std::string line;

	if (!src.is_open()) return;
	
	allEvents.clear();

	unsigned long long MAXID = 0;
	while (getline(src, line)) {
		if (line.empty()) continue;

		std::stringstream S(line);
		std::string temporary;
		std::vector<std::string> data;
		
		while (getline(S, temporary, '|')) {
			data.push_back(temporary);
		}

		if (data.size() != 7) return;

		std::string title = data[0];
		time_t StartTime = stoll(data[1])
			, EndTime = stoll(data[2]);
		std::string category = data[3]
			, detail = data[4]
			, place = data[5];
		unsigned long long ID = stoull(data[6]);

		Event LoadedEvent(title,StartTime,EndTime,category,detail,place,ID);

		MAXID = std::max(ID,MAXID);
	}

	src.close();
	
	nextID = MAXID + 1;
	this->sortEvents();
	return ;
}

void CalendarManager::saveToFile() {
	// start ------ save data
	time_t now = time(nullptr);
	std::ofstream dest(data_filename);

	for (const auto& i : allEvents) {
		if (i.getEndTime() < now) continue;

		dest << i.getTitle() << "|" << i.getStartTime() << "|" << i.getEndTime() << "|" << i.getCategory() << "|"
			<< i.getDetails() << "|" << i.getPlaces() << "|" << i.getID() << "\n";
	}

	dest.close();
	// end ------ save data

	//start ----- save category
	dest.open(categories_data_filename);
	float r, g, b, a;

	for (const auto& i : categories) {
		std::tie(r, g, b, a) = i.getRGB();
		dest << i.getname() << "|" << r << "|" << g << "|" << b << "|" << a << "\n";
	}

	dest.close();

	return;
}

void CalendarManager::sortEvents() {
	sort(allEvents.begin(), allEvents.end());
}


std::pair<bool, std::string> CalendarManager::addEvent(Event newEvent) {
	if (newEvent.getStartTime() > newEvent.getEndTime()) {
		return { false, "Start time must less than or equal to End time" };
	}
	newEvent.setID(nextID++); //run id
	allEvents.push_back(newEvent);
	this->sortEvents();//added event then sort
	return { true, "Success" };
}

std::pair<bool, std::string> CalendarManager::deleteEvent(unsigned long long eventId) {
	auto it = find_if(allEvents.begin(), allEvents.end(),
		[eventId](const Event& e) {
			return e.getID() == eventId;
		});

	if (it != allEvents.end()) {
		allEvents.erase(it); //ลบ event
		return { true, "Success" };
	}

	std::string ErrorMSG = "Can not find this [" + std::to_string(eventId) + "] id";

	return { false, ErrorMSG };
}

std::pair<bool, std::string> CalendarManager::editEvent(unsigned long long eventId, Event updatedEvent) {
	//todo : look for this eventId and update
	//if the date is change then sort (or just sort every time)
	//return true if sucsess

	if (updatedEvent.getStartTime() > updatedEvent.getEndTime())
		return { false , "Start time must less than or equal to End time" };

	for (auto& e : allEvents) {
		if (e.getID() == eventId) {

			updatedEvent.setID(eventId);
			e = updatedEvent;

			sortEvents();
			return { true, "Success" };
		}
	}
	std::string ErrorMSG = "Can not find this [" + std::to_string(eventId) + "] id";
	return { false, ErrorMSG };
}

const std::vector<Event>& CalendarManager::getAllEvents() const {
	return allEvents;
}

std::vector<Event> CalendarManager::getEventsByDate(int day, int month, int year) {
	//todo : getallEvent on Date
	std::vector<Event> eventsOnDate;

	time_t startofthisdate = DMYtoTime(day, month, year, 0, 0);
	time_t endofthisdate = DMYtoTime(day, month, year, 23, 59);

	for (const auto& i : allEvents) {
		if (i.getEndTime() >= startofthisdate && i.getStartTime() <= endofthisdate)
			eventsOnDate.push_back(i);
	}

	return eventsOnDate;
}

bool findthisword(std::string keyword, std::string source) {
	transform(source.begin(), source.end(), source.begin(), ::tolower);
	return (source.find(keyword) != std::string::npos);
}

std::vector<Event> CalendarManager::searchEvents(std::string keyword) {
	std::vector<Event> KeywordFoundEvents;
	transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);

	for (auto i : allEvents) {
		if (findthisword(keyword, i.getTitle())) {
			KeywordFoundEvents.push_back(i);
			continue;
		}
		if (findthisword(keyword, i.getCategory())) {
			KeywordFoundEvents.push_back(i);
			continue;
		}
		if (findthisword(keyword, i.getDetails())) {
			KeywordFoundEvents.push_back(i);
			continue;
		}
	}

	return KeywordFoundEvents;
}

std::vector<Event> CalendarManager::getUpcomingEvents(int N) {
	std::vector<Event> upcoming;
	int n = std::min(N, (int)allEvents.size());
	for (int i = 0; i < n; ++i) upcoming.push_back(allEvents[i]);
	return upcoming;
}

std::vector<std::string> CalendarManager::getCategories() {
	std::vector<std::string> vec;
	for (const auto& i : categories) {
		vec.push_back(i.getname());
	}
	return vec;
}

std::vector<std::pair<std::string, std::tuple<float, float, float, float >>> CalendarManager::getColorCategory() {
	std::vector<std::pair<std::string, std::tuple<float, float, float, float>>> vec;
	for (const auto& i : categories) {
		vec.push_back({ i.getname(),i.getRGB() });
	}
	return vec;
}