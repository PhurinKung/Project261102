#include "calendarGUI.h"
#include "imgui.h"
#include <ctime>
#include <string>

namespace cgui
{
	int FirstDayOfMonth(int, int);
	int HowManyDaysInThisMonth(int, int);
	void DrawCalendar();
	

	void ThewholecalendarGUI() 
	{
		DrawCalendar();
	}

	void DrawCalendar()
	{
		::ImGui::Begin("Calendar", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
		
		::ImGui::BeginTable("CalendarTable", 7);
		int ThisYear = 2026; //recive from input kub
		int ThisMonth = 1; //also this na

		//header of calendar
		const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
		//::ImGui::TableHeadersRow();
		::ImGui::TableNextRow();
		for (int i = 0; i < 7; i++) {
			::ImGui::TableSetColumnIndex(i);

			ImVec2 text_size = ::ImGui::CalcTextSize(days[i]);
			float available_width = ::ImGui::GetContentRegionAvail().x;
			float offset = (available_width - text_size.x) * 0.5f;
			::ImGui::SetCursorPosX(::ImGui::GetCursorPosX() + offset);
			::ImGui::Text("%s", days[i]);
		}

		//days of month
		int DayOne = FirstDayOfMonth(ThisYear, ThisMonth); //position of the first day of the month
		int AllDay = HowManyDaysInThisMonth(ThisYear, ThisMonth);

		int nday = 1;
		for (int i = 0; i < 6; i++) {
			::ImGui::TableNextRow();
			for (int j = 0; j < 7; j++) {
				::ImGui::TableSetColumnIndex(j);
				if (i == 0 && j < DayOne) ::ImGui::Text("");
				else if (nday > AllDay) ::ImGui::Text("");
				else {
					std::string day_str = std::to_string(nday);
					ImVec2 text_size = ::ImGui::CalcTextSize(day_str.c_str());
					float available_width = ::ImGui::GetContentRegionAvail().x;
					float offset = (available_width - text_size.x) * 0.5f;
					::ImGui::SetCursorPosX(::ImGui::GetCursorPosX() + offset);
					::ImGui::Text("%s", day_str.c_str());
					nday++;
				}
			}
		}


		::ImGui::EndTable();
		::ImGui::End();
	}

	int FirstDayOfMonth(int y, int m)
	{
		using namespace std;
		tm timestamp = {};
		timestamp.tm_year = y - 1900;
		timestamp.tm_mon = m - 1;
		timestamp.tm_mday = 1;

		mktime(&timestamp);
		return  timestamp.tm_wday;
	}

	int HowManyDaysInThisMonth(int y, int m)
	{
		tm timestamp = {};
		timestamp.tm_year = y - 1900;
		timestamp.tm_mon = m;
		//วันที่ 0 ของเดือนถัดไป = วันสุดท้ายของเดือนนี้
		timestamp.tm_mday = 0;
		mktime(&timestamp);
		return timestamp.tm_mday;
	}
}