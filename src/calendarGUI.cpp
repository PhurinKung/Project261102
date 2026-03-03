#include "calendarGUI.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <ctime>
#include <string>

namespace cgui
{

	int FirstDayOfMonth(int, int);
	int HowManyDaysInThisMonth(int, int);
	void DrawCalendar();
	void DrawCalendarV2();
	void thisistest();
	void UpcomingEvent();
	void NewEvent();
	

	void ThewholecalendarGUI() 
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		static bool first_time = true;

		if (first_time)
		{
			first_time = false;

			// Reset everything
			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

			ImGuiID dock_main_id = dockspace_id; 
			ImGuiID dock_right_id, dock_left_bottom_id;

			dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.4f, NULL, &dock_main_id);
			dock_left_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.40f, NULL, &dock_main_id);


			// --- ASSIGN WINDOWS ---
			ImGui::DockBuilderDockWindow("Calendar", dock_main_id);
			ImGui::DockBuilderDockWindow("test", dock_left_bottom_id);
			ImGui::DockBuilderDockWindow("Upcoming Event", dock_right_id);

			ImGui::DockBuilderFinish(dockspace_id);
		}

		ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport());
		//ImGui::DockSpaceOverViewport();
		DrawCalendarV2();
		thisistest();
		//DrawCustomWindow();
		UpcomingEvent();
		NewEvent();
	}

	void DrawCustomWindow() {
		//i just wanna see how to layout and style in imgui so this is just example window codes
		// --- WINDOW CUSTOMIZATION ---
		// 1. Change the background color of the window (R, G, B, Alpha)
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.11f, 0.16f, 1.00f));
		// 2. Make the window corners rounded
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);

		// Create the window
		ImGui::Begin("Custom Window Name");

		// --- FONT CUSTOMIZATION ---

		ImGui::Text("Here is my custom text!");
		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Adds vertical spacing

		// --- BUTTON CUSTOMIZATION ---
		// 4. Set the three states of button colors
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.31f, 0.47f, 1.0f));        // Normal
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.41f, 0.57f, 1.0f)); // Hovering
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.21f, 0.37f, 1.0f));  // Clicked

		// 5. Round the button corners
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

		// Create the button (Width, Height)
		if (ImGui::Button("Click Me", ImVec2(120, 40))) {
			// Code to run when clicked goes here
		}

		// --- CLEANUP (CRITICAL) ---
		// 6. You must Pop exactly the number of times you Pushed inside this section!
		ImGui::PopStyleVar(1);   // Removes Button Rounding
		ImGui::PopStyleColor(3); // Removes the 3 Button Colors

		ImGui::End(); // Ends the Window

		// Clean up the Window customizations pushed before ImGui::Begin
		ImGui::PopStyleVar(1);   // Removes Window Rounding
		ImGui::PopStyleColor(1); // Removes Window Background Color
	}


	void thisistest() {
		ImGui::Begin("test", NULL);
		ImGui::Text("u i a");

		static char event_name[100] = "hehe";
		ImGui::InputText("Event Name", event_name, sizeof(event_name));
		// วางคำสั่งนี้ลอยๆ ได้เลย ข้อความจะถูกเซฟลง event_name ตลอดเวลาที่พิมพ์

		if (ImGui::Button("Save Event")) {
			// This is where you trigger your logic!

			// Clear the text box after saving (optional)
		}
		static int selectedItem = 0;
		const char* items[] = { "Apple", "Banana", "Orange", "Strawberry", "Kiwi" };
		ImGui::Combo("###Select Fruit", &selectedItem, items, IM_ARRAYSIZE(items));
		ImGui::Separator();
		ImGui::Text("tada : %s", items[selectedItem]);
		
		ImGui::End();
	}

	void DrawCalendar()
	{
		ImGui::Begin("Calendar", NULL, ImGuiWindowFlags_NoCollapse);
		
		if (ImGui::BeginTable("CalendarTable", 7)) {
			int ThisYear = 2026; //recive from input kub
			int ThisMonth = 1; //also this na

			//header of calendar
			const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
			//ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			for (int i = 0; i < 7; i++) {
				ImGui::TableSetColumnIndex(i);

				ImVec2 text_size = ::ImGui::CalcTextSize(days[i]);
				float available_width = ::ImGui::GetContentRegionAvail().x;
				float offset = (available_width - text_size.x) * 0.5f;
				ImGui::SetCursorPosX(::ImGui::GetCursorPosX() + offset);
				ImGui::Text("%s", days[i]);
			}

			//days of month
			static int DayOne = FirstDayOfMonth(ThisYear, ThisMonth); //position of the first day of the month
			static int AllDay = HowManyDaysInThisMonth(ThisYear, ThisMonth);

			int nday = 1;
			for (int i = 0; i < 6; i++) {
				ImGui::TableNextRow();
				for (int j = 0; j < 7; j++) {
					ImGui::TableSetColumnIndex(j);
					if (i == 0 && j < DayOne) ::ImGui::Text("");
					else if (nday > AllDay) ::ImGui::Text("");
					else {
						std::string day_str = std::to_string(nday);
						ImVec2 text_size = ::ImGui::CalcTextSize(day_str.c_str());
						float available_width = ::ImGui::GetContentRegionAvail().x;
						float offset = (available_width - text_size.x) * 0.5f;
						ImGui::SetCursorPosX(::ImGui::GetCursorPosX() + offset);
						ImGui::Text("%s", day_str.c_str());
						nday++;
					}
				}
			}
			ImGui::EndTable();
		}
		ImGui::End();
	}

	void DrawCalendarV2() 
	{
		ImGui::Begin("Calendar", NULL, ImGuiWindowFlags_NoCollapse);
		static int ThisYear = 2026; //recive from input kub
		static int ThisMonth = 1; //also this na

		const char* month_names[] = { "January", "February", "March", "April", "May", "June",
										 "July", "August", "September", "October", "November", "December" };

		std::string header_text = std::string(month_names[ThisMonth - 1]) + " " + std::to_string(ThisYear);
		ImGui::SetWindowFontScale(2.0f);
		ImGui::Text( "%s", header_text.c_str());
		ImGui::SetWindowFontScale(1.0f); // คืนค่าขนาดตัวอักษรกลับเป็นปกติ


		// --- เริ่มส่วนที่แก้ไขให้ชิดขอบขวา ---
		float box_w = ImGui::GetWindowWidth() * 0.05f; // ขนาดความกว้างของปุ่ม
		if (box_w < 15.0f) box_w = 15.0f;
		float spacing = ImGui::GetStyle().ItemSpacing.x; // ระยะห่างระหว่างปุ่ม
		float total_w = (box_w * 2.0f) + spacing; // ความกว้างรวมของปุ่มทั้งสอง

		// คำนวณหาตำแหน่งแกน X ที่จะทำให้ปุ่มชิดขวาพอดี (ความกว้างหน้าต่าง - ความกว้างปุ่ม - Padding)
		float right_align_x = ImGui::GetWindowWidth() - total_w - ImGui::GetStyle().WindowPadding.x - 15.0f;

		// ใช้ SameLine แล้วใส่ค่า offset เข้าไป เพื่อดันปุ่มไปขวาสุด
		ImGui::SameLine(right_align_x);

		// ดันการตั้งค่าสีใหม่เข้าไป: ทำให้กล่องโปร่งใส และตัวหนังสือสีขาว
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // พื้นหลังปกติโปร่งใส (No box)
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.1f)); // ตอนเมาส์ชี้ (ไฮไลท์จางๆ ถ้าไม่ชอบให้เปลี่ยนตัวท้ายเป็น 0.0f)
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.2f)); // ตอนกดคลิก
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // ตัวอักษรสีขาว
		//ย้อนกลับ <
		if (ImGui::Button("<", ImVec2(box_w, 0))) {
			ThisMonth--;
			if (ThisMonth < 1) {
				ThisMonth = 12;
				ThisYear--;
			}
		}

		//ถัดไป >
		ImGui::SameLine();
		if (ImGui::Button(">", ImVec2(box_w, 0))) {
			ThisMonth++;
			if (ThisMonth > 12) {
				ThisMonth = 1;
				ThisYear++;
			}
		}
		ImGui::PopStyleColor(4);
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Separator();
		
		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // เพิ่มช่องว่าง
		if (ImGui::BeginTable("CalendarTable", 7)) {

			//header of calendar
			const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
			//ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			for (int i = 0; i < 7; i++) {
				ImGui::TableSetColumnIndex(i);

				ImVec2 text_size = ImGui::CalcTextSize(days[i]);
				float available_width = ImGui::GetContentRegionAvail().x;
				float offset = (available_width - text_size.x) * 0.5f;
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
				ImGui::Text("%s", days[i]);
			}

			//days of month
			int DayOne = FirstDayOfMonth(ThisYear, ThisMonth); //position of the first day of the month
			int AllDay = HowManyDaysInThisMonth(ThisYear, ThisMonth);

			static int selected_day = -1;
			int nday = 1;
			for (int i = 0; i < 6; i++) {
				ImGui::TableNextRow();
				for (int j = 0; j < 7; j++) {
					ImGui::TableSetColumnIndex(j);
					if (i == 0 && j < DayOne) ImGui::Text(""); //ช่องเปล่าก่อนวันแรก
					else if (nday > AllDay) ImGui::Text(""); //ช่องเปล่าหลังวันท้ายของเดือน
					else {
						std::string day_str = std::to_string(nday);

						ImGui::PushID(nday);
						float cell_w = ImGui::GetContentRegionAvail().x;
						float cell_h = 80.0f;
						ImVec2 cursor_pos = ImGui::GetCursorScreenPos(); // top right coordinate of the box
						
						//selectable boxes
						bool is_selected = (selected_day == nday);
						if (ImGui::Selectable("##day", is_selected, 0, ImVec2(cell_w, cell_h))) {
							selected_day = nday;
						}

						//fill numbers
						ImDrawList* draw_list = ImGui::GetWindowDrawList();
						ImVec2 text_size = ImGui::CalcTextSize(day_str.c_str());
						float offset_x = (cell_w - text_size.x) * 0.5f;
						float offset_y = (cell_h - text_size.y) * 0.5f;
						ImVec2 text_pos = ImVec2(cursor_pos.x + offset_x, cursor_pos.y + offset_y);

						draw_list->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), day_str.c_str());
						
						// 5. วาดจุด Event (ตัวอย่าง: ให้มีจุดแดงในวันที่ 5, 10, 15...)
						/*
						bool has_event = (nday % 5 == 0); // ตรงนี้คุณเปลี่ยนไปเช็คจากข้อมูล Calendar ของคุณได้
						if (has_event) {
							ImVec2 dot_pos = ImVec2(cursor_pos.x + (cell_w * 0.5f), cursor_pos.y + cell_h - 8.0f);
							draw_list->AddCircleFilled(dot_pos, 3.0f, IM_COL32(255, 100, 100, 255));
						}
						*/
						

						ImGui::PopID(); // คืนค่า ID
						nday++;
					}
				}
			}
			ImGui::EndTable();
		}
		ImGui::End();
	}

	bool isNewEventOpen = false;
	void NewEvent() {
		if (isNewEventOpen) {

			ImVec2 screenSize = ImGui::GetIO().DisplaySize;
			ImVec2 targetSize(screenSize.x * 0.5f, screenSize.y * 0.6f);
			ImGui::SetNextWindowSize(targetSize, ImGuiCond_Always);

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			ImGui::Begin("New Event", &isNewEventOpen, ImGuiWindowFlags_NoResize);

			ImFont* Title = ImGui::GetIO().Fonts->Fonts[1];
			ImGui::PushFont(Title);
			ImGui::Text("New Event");
			ImGui::PopFont();

			ImGui::SetNextItemWidth(-20.0f);
			static char event_name[100] = "Event's name"; //event's name
			ImGui::InputText("##Event_Name", event_name, sizeof(event_name));
			// วางคำสั่งนี้ลอยๆ ได้เลย ข้อความจะถูกเซฟลง event_name ตลอดเวลาที่พิมพ์

			ImGui::SetNextItemWidth(-20.0f);
			static char location[250] = "Location"; //the location
			ImGui::InputText("##Location", location, sizeof(location));

			//categories
			ImGui::SetNextItemWidth(150.0f);
			static int selectedItem = 0;
			const char* items[] = { "Work", "Personal", "Business", "Others" };
			ImGui::Combo("###Select Fruit", &selectedItem, items, IM_ARRAYSIZE(items));



			//details
			static char detail[1024] = "";
			ImVec2 boxSize(-20.0f, 150.0f);
			ImGui::InputTextMultiline("##Detail", detail, sizeof(detail), boxSize);

			ImVec2 buttonSize(50, 35);

			float WindowHeight = ImGui::GetWindowHeight();
			float WindowWidth = ImGui::GetWindowWidth();

			float targetY = WindowHeight - buttonSize.y - 20.0f;
			float targetX = WindowWidth - buttonSize.x - 20.0f;

			ImGui::SetCursorPos(ImVec2(targetX, targetY));

			if (ImGui::Button("save", buttonSize)) {
				// This is where you trigger your logic!

				// Clear the text box after saving (optional)
			}

			ImGui::End();
			ImGui::PopStyleColor();
		}
	}

	void UpcomingEvent()
	{
		ImGui::Begin("Upcoming Event", NULL, ImGuiWindowFlags_NoCollapse);
		int n_events = 5;
		ImFont* Title = ImGui::GetIO().Fonts->Fonts[1];
		ImGui::PushFont(Title);
		ImGui::Text("Upcoming Events");
		ImGui::PopFont();
		for (int i = 0; i < n_events; i++) {
			ImGui::BulletText("  waha ur date");
		}

		ImVec2 buttonSize(60, 60);

		float WindowHeight = ImGui::GetWindowHeight();
		float WindowWidth = ImGui::GetWindowWidth();

		float targetY = WindowHeight - buttonSize.y - 30.0f; // 45 = 30 + 15 ---ระยะห่างขอบ + ระยะห่างของแต่ละปุ่ม
		float targetX = WindowWidth - (2 * buttonSize.x) - 45.0f; // *2 since we have 2 buttons

		ImGui::SetCursorPos(ImVec2(targetX, targetY));

		ImGui::Button("search", buttonSize);
		ImGui::SameLine(0.0f, 15.0f);
		ImGui::PushFont(Title);
		if (ImGui::Button("+", buttonSize)) {
			isNewEventOpen = true;
		}
		ImGui::PopFont();


		ImGui::End();
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