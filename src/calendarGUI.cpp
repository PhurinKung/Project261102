#include "calendarGUI.h"
#include "imgui.h"
#include <chrono>
#include <string>

namespace cgui
{
	void ThewholecalendarGUI() 
	{
		DrawCalendar();
	}


	void DrawCalendar()
	{
		::ImGui::Begin("Calendar", NULL, ImGuiWindowFlags_NoResize);
		
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
		unsigned int DayOne = FirstDayOfMonth(ThisYear, ThisMonth); //position of first day of the month
		unsigned int AllDay = HowManyDaysInThisMonth(ThisYear, ThisMonth);

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

	unsigned int FirstDayOfMonth(int y, unsigned int m)
	{
		using namespace ::std::chrono;
		year_month_day ymd{year{y}/month{m}/day{1u} };
		weekday wd{ sys_days{ymd} };

		return wd.c_encoding();
	}

	unsigned int HowManyDaysInThisMonth(int y, unsigned int m)
	{
		using namespace ::std::chrono;
		year_month_day ymdl{ year{y} / month{m} / last };

		return (unsigned int)ymdl.day();
	}
}