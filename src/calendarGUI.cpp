#include "calendarGUI.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <ctime>
#include <string>
#include <vector>
#include <string>

namespace cgui
{
	static int ThisYear = 2026;
	static int ThisMonth = 1;
	static int selected_day = -1; // -1 means no day is selected yet

	int FirstDayOfMonth(int, int);
	int HowManyDaysInThisMonth(int, int);
	void DrawCalendar();
	void DrawCalendarV2();
	void thisistest();
	void UpcomingEvent();
	void DrawCustomWindow();
	void showevent();

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
			dock_left_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.35f, NULL, &dock_main_id);


			// --- ASSIGN WINDOWS ---
			ImGui::DockBuilderDockWindow("Calendar", dock_main_id);
			ImGui::DockBuilderDockWindow("showevent", dock_left_bottom_id);
			ImGui::DockBuilderDockWindow("Upcoming Event", dock_right_id);

			ImGui::DockBuilderFinish(dockspace_id);
		}

		ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport());
		//ImGui::DockSpaceOverViewport();
		DrawCalendarV2();
		showevent();
		//DrawCustomWindow();
		UpcomingEvent();
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

	void showevent() {
		// 1. Create a window class to override the docking behavior
		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window_class);

		// 2. Begin your window (keep NoTitleBar so it stays clean if you ever undock it)
		ImGui::Begin("showevent", nullptr, ImGuiWindowFlags_NoTitleBar);
		std::vector<std::string>events = {"one day", "ow day"};
		if (selected_day != -1)
		{
			// Print the date they clicked
			//ImGui::Text("Events for: %02d/%02d/%d", selected_day, ThisMonth, ThisYear);
			//ImGui::Separator();
			if (!events.empty())
			{
				if (ImGui::BeginTabBar("EventTabs")) {
					for (auto& tab : events) {
						if (ImGui::BeginTabItem(tab.c_str())) {
							ImGui::SetWindowFontScale(1.5f);
							ImGui::Text("%s", tab.c_str());
							ImGui::SetWindowFontScale(1.0f);
							//ImGui::Dummy(ImVec2(0.0f, 0.5f));
							ImGui::Text("%02d/%02d/%d", selected_day, ThisMonth, ThisYear);

							ImGui::Text("Time : %d");
							ImGui::Text("location : %s");
							ImGui::SeparatorText("Details");

							ImGui::EndTabItem();
						}
					}
					ImGui::EndTabBar();
				}

			}
			else {
				ImGui::TextDisabled("Empty");
			}
		}
		else
		{
			// What it shows when they first open the app and haven't clicked a day
			ImGui::TextDisabled("Select a day on the calendar to view events.");
		}
		ImGui::End();
	}
	void thisistest(){

		// 1. Create a window class to override the docking behavior
		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window_class);

		// 2. Begin your window (keep NoTitleBar so it stays clean if you ever undock it)
		ImGui::Begin("test", nullptr, ImGuiWindowFlags_NoTitleBar);

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

		//details
		ImGui::SetNextItemWidth(-1.0f);
		ImGui::InputText("###Name", event_name, sizeof(event_name));
		// "MyTabBar" is an internal ID, it doesn't show up on screen
		if (ImGui::BeginTabBar("MyTabBar")) {

			// First Tab
			// Passing a boolean pointer (&is_open) automatically adds the little 'X' close button
			bool is_open = true;
			if (ImGui::BeginTabItem("Lettuce", &is_open)) {
				ImGui::Text("Document \"Lettuce\"");
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Lorem ipsum dolor sit amet...");

				// Always call EndTabItem if BeginTabItem returns true
				ImGui::EndTabItem();
			}

			// Second Tab (No 'X' button because we don't pass a boolean)
			if (ImGui::BeginTabItem("Eggplant")) {
				ImGui::Text("Document \"Eggplant\"");
				ImGui::EndTabItem();
			}

			// Third Tab
			if (ImGui::BeginTabItem("Carrot")) {
				ImGui::Text("Document \"Carrot\"");
				ImGui::EndTabItem();
			}

			// Always call EndTabBar if BeginTabBar returns true
			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	void DrawCalendar()
	{
		ImGui::Begin("Calendar", NULL, ImGuiWindowFlags_NoCollapse);
		
		if (ImGui::BeginTable("CalendarTable", 7)) {
			//int ThisYear = 2026; //recive from input kub
			//int ThisMonth = 1; //also this na

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

			//static int selected_day = -1;
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
						float cell_h = 100.0f;
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
						float offset_y = 8.0f;
						ImVec2 text_pos = ImVec2(cursor_pos.x + offset_x, cursor_pos.y + offset_y);

						draw_list->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), day_str.c_str());
						
						// fill event dots if there are any
						int num_events = 3; // Example: test with more than 8

						if (num_events > 0)
						{
							float dot_radius = 3.0f;
							float dot_spacing = 4.0f;
							float row_spacing = 4.0f; // Vertical space between rows of dots
							int max_per_row = 8;

							// Calculate how many rows are needed
							int num_rows = (num_events + max_per_row - 1) / max_per_row;
							float dot_size = dot_radius * 2.0f;

							// Center the entire block of dots vertically in the cell
							float total_height = (num_rows * dot_size) + ((num_rows - 1) * row_spacing);
							float start_y = cursor_pos.y + (cell_h - total_height) * 0.5f + dot_radius;

							ImU32 category_colors[] = {
								IM_COL32(255, 50, 50, 150),
								IM_COL32(50, 255, 50, 150),
								IM_COL32(50, 150, 255, 150),
								IM_COL32(255, 200, 50, 150)
							};

							for (int row = 0; row < num_rows; ++row)
							{
								// Calculate how many dots are in this specific row
								int dots_in_this_row = (row == num_rows - 1) ? (num_events - row * max_per_row) : max_per_row;

								// Center this specific row horizontally
								float row_width = (dots_in_this_row * dot_size) + ((dots_in_this_row - 1) * dot_spacing);
								float current_x = cursor_pos.x + (cell_w - row_width) * 0.5f + dot_radius;
								float current_y = start_y + row * (dot_size + row_spacing);

								for (int col = 0; col < dots_in_this_row; ++col)
								{
									int event_index = row * max_per_row + col;
									ImU32 dot_color = category_colors[event_index % 4];

									draw_list->AddCircleFilled(ImVec2(current_x, current_y), dot_radius, dot_color);

									current_x += dot_size + dot_spacing;
								}
							}
						}
						/*
						if (nday) // is there any event on this day?
						{
							float event_y = cursor_pos.y + offset_y + text_size.y + 6.0f;
							for (int i = 0; i < 4; i++) {
								const char* event_text = " Event Namesgfdgsdsgs";
								ImVec2 event_size = ImGui::CalcTextSize(event_text);

								float padding_x = 5.0f; // Margin on the left and right
								float event_x = cursor_pos.x + padding_x;
								float max_x = cursor_pos.x + cell_w - padding_x;
								float max_y = cursor_pos.y + cell_h;
								ImVec2 event_pos = ImVec2(event_x, event_y);

								ImU32 highlight_color = IM_COL32(0, 0, 200, 150);
								draw_list->AddRectFilled(
									event_pos,
									ImVec2(max_x, event_pos.y + event_size.y),
									highlight_color,
									2.0f //rounding
								);

								ImGui::PushClipRect(event_pos, ImVec2(max_x,max_y), true); //ตัดอักษรที่เกินhighlight ออกนะ
								draw_list->AddText(event_pos, IM_COL32(255, 255, 255, 255), event_text);
								ImGui::PopClipRect();
								event_y = event_y + event_size.y + 6.0f;
								if (event_y + event_size.y + 4.0f > max_y) break;
							}
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

	void UpcomingEvent()
	{
		ImGui::Begin("Upcoming Event", NULL, ImGuiWindowFlags_NoCollapse);
		int n_events = 5;
		for (int i = 0; i < n_events; i++) {
			ImGui::PushID(i);
			const char* event_name = "jdkas";
			if (ImGui::CollapsingHeader(event_name)) {
				ImGui::Text("%02d/%02d/%d", selected_day, ThisMonth, ThisYear);
				ImGui::SeparatorText("Time");
				ImGui::Text("10:00AM");
				ImGui::SeparatorText("Location");
				ImGui::SeparatorText("Details");
			}
			ImGui::PopID();
		}
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