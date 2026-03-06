#include "functions.h"
// complete all func in functions.h

std::tuple<int, int, int, int, int> Utils::timeToDMY(time_t tme) { // day month year hour minute
	tm local_time;
	localtime_s(&local_time, &tme);

	int day = local_time.tm_mday;       // (1 - 31)
	int month = local_time.tm_mon + 1;  // (0 - 11) -> +1 = 1 - 12 
	int year = local_time.tm_year + 1900; // start from 1900 -> +1900 get current year
	int hour = local_time.tm_hour;
	int minute = local_time.tm_min;

	return std::make_tuple(day, month, year, hour, minute);
}

//std::tuple<int, int, int, int, int> Utils::timeToDMY(time_t tme) { // day month year hour minute
//	tm* local_time = localtime(&tme);
//
//	int day = local_time->tm_mday;       // (1 - 31)
//	int month = local_time->tm_mon + 1;  // (0 - 11) -> +1 = 1 - 12 
//	int year = local_time->tm_year + 1900; // start from 1900 -> +1900 get current year
//	int hour = local_time->tm_hour;
//	int minute = local_time->tm_min;
//
//	return std::make_tuple(day, month, year, hour, minute);
//}

time_t Utils::DMYtoTime(int day, int month, int year, int hour , int minute ) {
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
CalendarManager::~CalendarManager() { saveToFile();  }

void CalendarManager::loadFromFile() {
	//load from data
	std::ifstream datasrc(data_filename);
	
	std::string line;

	if (datasrc.is_open()) {
		allEvents.clear();

		unsigned long long MAXID = 0;
		while (getline(datasrc, line)) {
			if (line.empty()) continue;

			std::stringstream S(line);
			std::string temporary;
			std::vector<std::string> data;
		
			while (getline(S, temporary, '|')) {
				data.push_back(temporary);
			}

			if (data.size() != 7) continue;

			try {
				std::string title = data[0];
				time_t StartTime = stoll(data[1])
					, EndTime = stoll(data[2]);
				std::string category = data[3]
					, detail = data[4]
					, place = data[5];
				unsigned long long ID = stoull(data[6]);

				Event LoadedEvent(title, StartTime, EndTime, category, detail, place, ID);
				allEvents.push_back(LoadedEvent);

				MAXID = std::max(ID, MAXID);
			}
			catch(const std::exception& e){
				continue;
			}
		}

		datasrc.close();
	
		nextID = MAXID + 1;
		this->sortEvents();
	}

	std::ifstream catsrc(categories_data_filename);
	if (catsrc.is_open()) {
		while (getline(catsrc, line)) {
			if (line.empty()) continue;

			std::stringstream S(line);
			std::string temporary;
			std::vector<std::string> data;

			while (std::getline(S, temporary, '|')) {
				data.push_back(temporary);
			}

			if (data.size() != 5) continue;

			try {
				std::string category_name = data[0];
				float r = stof(data[1]),
					g = stof(data[2]),
					b = stof(data[3]),
					a = stof(data[4]);

				EventCategory LoadCategory(category_name, r, g, b, a);
				categories.push_back(LoadCategory);
			}
			catch (const std::exception& e) {
				continue;
			}
		}

		catsrc.close();
	}
	else {
		categories.push_back(EventCategory("Personal", 1.0f, 1.0f, 1.0f, 1.0f)); // สีขาว
		categories.push_back(EventCategory("Study", 0.2f, 0.6f, 1.0f, 1.0f)); // สีฟ้า
		categories.push_back(EventCategory("Emergency", 1.0f, 0.2f, 0.2f, 1.0f)); // สีแดง
	}

	return ;
}

void CalendarManager::saveToFile() {
	// start ------ save data
	time_t now = time(nullptr);
	std::ofstream dest(data_filename);

	for (const auto& i : allEvents) {
		std::cout << "Saving..." << i.getTitle() << "\n";
		if (i.getEndTime() < now) continue;


		dest << i.getTitle() << "|" << i.getStartTime() << "|" << i.getEndTime() << "|" << i.getCategory() << "|"
			<< i.getDetails() << "|" << i.getPlaces() << "|" << i.getID() << "\n";
		std::cout << "Saved " << i.getTitle() << "\n";

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
	// end ---- save category

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

std::pair<bool, std::string> CalendarManager::addCategory(EventCategory newCategory) {
	std::string name = newCategory.getname();

	for (const auto& i : categories) {
		if (name == i.getname()) {
			return { false, "already have this category !" };
		}
	}

	categories.push_back(newCategory);
	return { true, "Success" };
}

std::pair<bool, std::string> CalendarManager::deleteCategory(std::string sadCategory) {
	if (sadCategory == "Personal") {
		return { false, "Can't delete default category" };
	}

	auto it = find_if(categories.begin(), categories.end(),
		[sadCategory](const EventCategory& c) {
			return c.getname() == sadCategory;
		});

	if (it != categories.end()) {
		categories.erase(it);
		return { true, "success" };
	}

	// loop find all deleted category
	for (auto& e : allEvents) {
		if (e.getCategory() == sadCategory) {
			e.changeCats("Personal");
		}
	}

	return { false, "Can't find this category" };
}

std::pair<bool, std::string> CalendarManager::editCategory(std::string nameOldCategory, EventCategory newCategory) {
	for (auto& c : categories) {
		if (c.getname() == nameOldCategory) {
			c = newCategory;
			return { true, "Success" };
		}
	}
	return { false, "Can't find this cate" };
}

const std::vector<Event>& CalendarManager::getAllEvents() const {
	return allEvents;
}

std::vector<Event> CalendarManager::getEventsByDate(int day, int month, int year) {
	std::vector<Event> eventsOnDate;

	time_t startofthisdate = Utils::DMYtoTime(day, month, year, 0, 0);
	time_t endofthisdate = Utils::DMYtoTime(day, month, year, 23, 59);

	for (const auto& i : allEvents) {
		if (i.getEndTime() >= startofthisdate && i.getStartTime() <= endofthisdate)
			eventsOnDate.push_back(i);
	}

	return eventsOnDate;
}

std::string Utils::ToLowerSafe(std::string s) {
	for (auto& i : s) {
		i = tolower((unsigned char)i);
	}
	return s;
}

bool Utils::findthisword(const std::string & keyword, std::string source) {
	source = Utils::ToLowerSafe(source);

	return (source.find(keyword) != std::string::npos);
}

std::vector<Event> CalendarManager::searchEvents(std::string keyword) {
	std::vector<Event> KeywordFoundEvents;
	keyword = Utils::ToLowerSafe(keyword);

	for (const auto& i : allEvents) {
		if (Utils::findthisword(keyword, i.getTitle())
			|| Utils::findthisword(keyword, i.getCategory())
			|| Utils::findthisword(keyword, i.getDetails())
			|| Utils::findthisword(keyword, i.getPlaces())) {
			KeywordFoundEvents.push_back(i);
		}
	}

	return KeywordFoundEvents;
}

std::vector<Event> CalendarManager::getUpcomingEvents(int N) {
	std::vector<Event> upcoming;

	time_t now = time(nullptr);

	for (const auto& i : allEvents) {
		if (i.getEndTime() >= now)
			upcoming.push_back(i);

		if (upcoming.size() == N) break;
	}


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