#include "calendarGUI.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "functions.h"
#include <ctime>
#include "functions.h"

namespace cgui
{

	static int selected_year = -1;
	static int selected_month = -1;
	static int selected_day = -1; // -1 means no day is selected yet

	static int ThisYear = -1;
	static int ThisMonth = -1;

	static bool editing = false;
	bool confirmDelete = false;
	static bool createnewcategory = false;
	static bool confirmdelete = false;
	static CalendarManager myCalendar;
	static Event current_editing_event, current_deleting_event;

	int FirstDayOfMonth(int, int);
	int HowManyDaysInThisMonth(int, int);
	void DrawMainCalendar();
	void thisistest();
	void UpcomingEvent();
	void NewEvent();
	void SearchEvent();
	void CreateNewCategory();
	void ShowEvent();
	void EditEvent();
	void ConfirmDelete();
	void MiniCalendar(const char* , int& , int& , int& );

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
		DrawMainCalendar();
		ShowEvent();
		UpcomingEvent();
		NewEvent();
		SearchEvent();
		CreateNewCategory();
		EditEvent();
		ConfirmDelete();
	}
	void ShowEvent() {
		// 1. Create a window class to override the docking behavior
		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window_class);

		// 2. Begin your window (keep NoTitleBar so it stays clean if you ever undock it)
		ImGui::Begin("showevent", nullptr, ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowFontScale(1.25f);
		if (selected_day != -1)
		{
			// Print the date they clicked
			std::vector<Event> events_today = myCalendar.getEventsByDate(selected_day, selected_month, selected_year);
			if (!events_today.empty())
			{
				if (ImGui::BeginTabBar("EventTabs")) {
					for (auto& ev : events_today) {
						if (ImGui::BeginTabItem(ev.getTitle().c_str())) {
							ImGui::SetWindowFontScale(1.75f);
							ImGui::Text("%s", ev.getTitle().c_str());
							ImGui::Separator();
							ImGui::SetWindowFontScale(1.25f);

							//ImGui::Text("%02d/%02d/%d", selected_day, selected_month, selected_year);
							auto [s_day, s_mon, s_year, s_hour, s_min] = Utils::timeToDMY(ev.getStartTime());
							auto [e_day, e_mon, e_year, e_hour, e_min] = Utils::timeToDMY(ev.getEndTime());

							// 5. Print the exact details
							ImGui::Text("Start Time : %02d/%02d/%d,  %02d:%02d", s_day, s_mon, s_year, s_hour, s_min);
							ImGui::Text("End Time   : %02d/%02d/%d,  %02d:%02d", e_day, e_mon, e_year, e_hour, e_min);
							ImGui::Text("Category   : %s", ev.getCategory().c_str());
							ImGui::Text("Location   : %s", ev.getPlaces().c_str());

							ImGui::SeparatorText("Details");
							ImGui::TextWrapped("%s", ev.getDetails().c_str());

							float WindowWidth = ImGui::GetWindowWidth();
							float WindowHeight = ImGui::GetWindowHeight();

							ImVec2 buttonSize(60, 35);
							ImVec2 buttonSize_2(80, 35);

							float targetX = WindowWidth - buttonSize.x - buttonSize_2.x - 20.0f - 10.0f;
							float targetY = WindowHeight - buttonSize.y - 20.0f;

							ImGui::SetCursorPos(ImVec2(targetX, targetY));

							if (ImGui::Button("edit", buttonSize)) {
								editing = true;
								current_editing_event = ev;
							}
							ImGui::SameLine(0.0f, 20.0f);
							if (ImGui::Button("delete", buttonSize_2)) {
								confirmDelete = true;
								current_deleting_event = ev;
							}
							ImGui::EndTabItem();
						}
					}
					ImGui::EndTabBar();
				}

			}
			else {
				ImGui::TextDisabled("No event for this day.");
			}
		}
		else
		{
			ImGui::TextDisabled("Select a day on the calendar to view events.");
		}

		if (confirmDelete) {
			ImGui::OpenPopup("ConfirmDelete");
		}

		ConfirmDelete();

		ImGui::End();
	}

	void EditEvent() {
		// 1. Static variables to hold the editing state
		static char edit_title[256] = "";
		static char edit_location[256] = "";
		static char edit_details[1024] = "";
		static int s_day, s_mon, s_year, s_hour, s_min;
		static int e_day, e_mon, e_year, e_hour, e_min;
		static int selected_cat_idx = 0;

		// 2. TRIGGER BLOCK: This runs EXACTLY ONCE when you click "edit"
		if (editing) {
			// Copy the strings
			snprintf(edit_title, sizeof(edit_title), "%s", current_editing_event.getTitle().c_str());
			snprintf(edit_location, sizeof(edit_location), "%s", current_editing_event.getPlaces().c_str());
			snprintf(edit_details, sizeof(edit_details), "%s", current_editing_event.getDetails().c_str());

			// --- INJECT THE OLD START TIME ---
			auto [sd, sm, sy, sh, smin] = Utils::timeToDMY(current_editing_event.getStartTime());
			s_day = sd; s_mon = sm; s_year = sy; s_hour = sh; s_min = smin;

			// --- INJECT THE OLD END TIME ---
			auto [ed, em, ey, eh, emin] = Utils::timeToDMY(current_editing_event.getEndTime());
			e_day = ed; e_mon = em; e_year = ey; e_hour = eh; e_min = emin;

			// Match the old category to the dropdown
			std::vector<std::string> cats = myCalendar.getCategories();
			selected_cat_idx = 0;
			for (int i = 0; i < cats.size(); i++) {
				if (cats[i] == current_editing_event.getCategory()) {
					selected_cat_idx = i;
					break;
				}
			}

			ImGui::OpenPopup("Edit Window");
			editing = false;
		}

		// 3. DRAW THE POPUP
		if (ImGui::BeginPopupModal("Edit Window", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			// --- TITLE ---
			ImGui::SetWindowFontScale(1.25f);
			ImGui::PushItemWidth(-1.0f);
			ImGui::InputText("##EditTitle", edit_title, sizeof(edit_title));
			ImGui::PopItemWidth();
			ImGui::Separator();

			float label_align = 140.0f;

			// --- START TIME (Your exact layout code) ---
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Start Time"); ImGui::SameLine(label_align);
			ImGui::PushItemWidth(35.0f);
			char edit_start_btn[32];
			snprintf(edit_start_btn, sizeof(edit_start_btn), "%02d/%02d/%d##editstart", s_day, s_mon, s_year);
			if (ImGui::Button(edit_start_btn, ImVec2(150, 0))) {
				ImGui::OpenPopup("Start Date");
			}
			MiniCalendar("Start Date", s_day, s_mon, s_year);
			ImGui::SameLine(); ImGui::Text(", "); ImGui::SameLine();
			ImGui::DragInt("##sh", &s_hour, 0.5f, 0, 23, "%02d"); 
			ImGui::SameLine(); ImGui::Text(":"); ImGui::SameLine();
			ImGui::DragInt("##smin", &s_min, 0.5f, 0, 59, "%02d");
			ImGui::PopItemWidth();

			// --- END TIME (Your exact layout code) ---
			ImGui::AlignTextToFramePadding();
			ImGui::Text("End Time"); ImGui::SameLine(label_align);
			ImGui::PushItemWidth(35.0f);
			char edit_end_btn[32];
			snprintf(edit_end_btn, sizeof(edit_end_btn), "%02d/%02d/%d##editend", e_day, e_mon, e_year);
			if (ImGui::Button(edit_end_btn, ImVec2(150, 0))) {
				ImGui::OpenPopup("End Date");
			}
			MiniCalendar("End Date", e_day, e_mon, e_year);
			ImGui::SameLine(); ImGui::Text(", "); ImGui::SameLine();
			ImGui::DragInt("##eh", &e_hour, 0.5f, 0, 23, "%02d"); 
			ImGui::SameLine(); ImGui::Text(":"); ImGui::SameLine();
			ImGui::DragInt("##emin", &e_min, 0.5f, 0, 59, "%02d");
			ImGui::PopItemWidth();

			// --- CATEGORY ---
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Category   :"); ImGui::SameLine(label_align);
			ImGui::PushItemWidth(180.0f);
			std::vector<std::string> cats = myCalendar.getCategories();
			if (!cats.empty()) {
				if (ImGui::BeginCombo("##EditCat", cats[selected_cat_idx].c_str())) {
					for (int i = 0; i < cats.size(); i++) {
						bool is_selected = (selected_cat_idx == i);
						if (ImGui::Selectable(cats[i].c_str(), is_selected)) selected_cat_idx = i;
						if (is_selected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
			ImGui::PopItemWidth();

			// --- LOCATION ---
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Location   :"); ImGui::SameLine(label_align);
			ImGui::PushItemWidth(250.0f);
			ImGui::InputText("##EditLoc", edit_location, sizeof(edit_location));
			ImGui::PopItemWidth();

			// --- DETAILS ---
			ImGui::SeparatorText("Details");
			ImGui::InputTextMultiline("##EditDet", edit_details, sizeof(edit_details), ImVec2(-1.0f, 140.0f), ImGuiInputTextFlags_WordWrap);

			ImGui::Dummy(ImVec2(0.0f, 10.0f));

			// --- SAVE & CANCEL BUTTONS ---
			ImVec2 buttonSize(80, 30);
			float targetX = ImGui::GetWindowWidth() - (buttonSize.x * 2.0f) - ImGui::GetStyle().ItemSpacing.x - 10.0f;
			ImGui::SetCursorPosX(targetX);

			if (ImGui::Button("Cancel##canceledit", buttonSize)) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Save##saveedit", buttonSize)) {
				// Convert the numbers from the DragInt boxes straight into time_t
				time_t new_start = Utils::DMYtoTime(s_day, s_mon, s_year, s_hour, s_min);
				time_t new_end = Utils::DMYtoTime(e_day, e_mon, e_year, e_hour, e_min);
				std::string selected_cat_name = cats.empty() ? "Default" : cats[selected_cat_idx];

				Event updated_event(
					edit_title, new_start, new_end, selected_cat_name, edit_details, edit_location, current_editing_event.getID()
				);

				myCalendar.editEvent(current_editing_event.getID(), updated_event);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void ConfirmDelete() {
		ImVec2 screenSize = ImGui::GetIO().DisplaySize;
		ImVec2 targetSize(screenSize.x * 0.3f, screenSize.y * 0.2f);

		ImGui::SetNextWindowSize(targetSize, ImGuiCond_Always);
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

		if (ImGui::BeginPopupModal("ConfirmDelete", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)){
			
			ImFont* Title = ImGui::GetIO().Fonts->Fonts[2];
			ImGui::PushFont(Title);
			ImGui::Text("Confirm Delete");
			ImGui::PopFont();

			ImGui::Separator();

			ImGui::Text("Are you sure you want to delete this event?");
			
			ImGui::Separator();
			
			ImVec2 buttonSize(80.0f, 25.0f);

			float WindowWidth = ImGui::GetWindowWidth();
			float WindowHeight = ImGui::GetWindowHeight();

			float targetX = WindowWidth - (2 * buttonSize.x) - 20.0f - 10.0f;
			float targetY = WindowHeight - buttonSize.y - 20.0f;

			ImGui::SetCursorPos(ImVec2(targetX, targetY));
			if (ImGui::Button("Cancle", buttonSize)) {
				confirmDelete = false;
				ImGui::CloseCurrentPopup();
			}
			

			ImGui::SameLine(0.0f, 10.0f);

			if (ImGui::Button("Confirm", buttonSize)) {
				myCalendar.deleteEvent(current_deleting_event.getID());
				confirmDelete = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleColor();
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

	void DrawMainCalendar() 
	{
		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window_class);

		ImGui::Begin("Calendar", NULL, ImGuiWindowFlags_NoTitleBar);

		//get current date for month and year as a default opening kub
		if (selected_year == -1 || selected_month == -1) {
			std::time_t currentTime = std::time(nullptr);
			auto [current_day, current_month, current_year, current_hour, current_minute] = Utils::timeToDMY(currentTime);
			selected_year = current_year;
			selected_month = current_month;
		}

		const char* month_names[] = { "January", "February", "March", "April", "May", "June",
										 "July", "August", "September", "October", "November", "December" };

		std::string header_text = std::string(month_names[selected_month - 1]) + " " + std::to_string(selected_year);
		ImGui::SetWindowFontScale(2.0f);
		ImGui::Text( "%s", header_text.c_str());
		ImGui::SetWindowFontScale(1.0f);


		// --- เริ่มส่วนที่แก้ไขให้ชิดขอบขวา ---
		float box_w = ImGui::GetWindowWidth() * 0.05f; // ขนาดความกว้างของปุ่ม
		if (box_w < 15.0f) box_w = 15.0f;
		float spacing = ImGui::GetStyle().ItemSpacing.x; // ระยะห่างระหว่างปุ่ม
		float total_w = (box_w * 2.0f) + spacing; // ความกว้างรวมของปุ่มทั้งสอง

		// คำนวณหาตำแหน่งแกน X ที่จะทำให้ปุ่มชิดขวาพอดี (ความกว้างหน้าต่าง - ความกว้างปุ่ม - Padding)
		float right_align_x = ImGui::GetWindowWidth() - total_w - ImGui::GetStyle().WindowPadding.x - 15.0f;

		// ใช้ SameLine แล้วใส่ค่า offset เข้าไป เพื่อดันปุ่มไปขวาสุด
		ImGui::SameLine(right_align_x);

		//ทำให้กล่องโปร่งใส และตัวหนังสือสีขาว
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // พื้นหลังปกติโปร่งใส (No box)
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.1f)); // ตอนเมาส์ชี้ (ไฮไลท์จางๆ ถ้าไม่ชอบให้เปลี่ยนตัวท้ายเป็น 0.0f)
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.2f)); // ตอนกดคลิก
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // ตัวอักษรสีขาว
		
		// > <
		if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { 
			selected_month--;
			if (selected_month < 1) {
				selected_month = 12;
				selected_year--;
			}
		}
		ImGui::SameLine(0.0f, spacing);
		if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { 
			selected_month++;
			if (selected_month > 12) {
				selected_month = 1;
				selected_year++;
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
			int DayOne = FirstDayOfMonth(selected_year, selected_month); //position of the first day of the month
			int AllDay = HowManyDaysInThisMonth(selected_year, selected_month);

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
						
						bool is_selected = (selected_day == nday && selected_month == selected_month && selected_year == selected_year);

						if (ImGui::Selectable("##day", is_selected, 0, ImVec2(cell_w, cell_h))) {
							if (is_selected) {
								// if selected then deselect
								selected_day = -1;
							}
							else {
								// if didnt select this then select this
								selected_day = nday;
							}
						}

						//fill numbers
						ImDrawList* draw_list = ImGui::GetWindowDrawList();
						ImVec2 text_size = ImGui::CalcTextSize(day_str.c_str());
						float offset_x = (cell_w - text_size.x) * 0.5f;
						float offset_y = 8.0f;
						ImVec2 text_pos = ImVec2(cursor_pos.x + offset_x, cursor_pos.y + offset_y);

						draw_list->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), day_str.c_str());
						
						// fill event dots if there are any
						std::vector<Event> events_today = myCalendar.getEventsByDate(nday, selected_month, selected_year);
						int num_events = events_today.size();

						auto category_color = myCalendar.getColorCategory();

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
									ImU32 dot_color = IM_COL32(150, 150, 150, 200);

									// --- 2. เช็ค Category และดึงสีมาใช้ ---
									if (event_index < events_today.size()) {
										std::string cat_name = events_today[event_index].getCategory();

										for (const auto& cat : category_color) {
											if (cat.first == cat_name) {
												// Unpack Tuple กลับมาเป็น r, g, b, a
												auto [r, g, b, a] = cat.second;

												// แปลงทศนิยมเป็นรหัสสี ImU32
												dot_color = ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
												break;
											}
										}
									}
									draw_list->AddCircleFilled(ImVec2(current_x, current_y), dot_radius, dot_color);

									current_x += dot_size + dot_spacing;
								}
							}
						}
						ImGui::PopID(); // คืนค่า ID
						nday++;
					}
				}
			}
			ImGui::EndTable();
		}
		ImGui::End();
	}

	void MiniCalendar(const char* popup_id, int& return_day, int& return_month, int& return_year) {
		static int this_year;
		static int this_month;
		static int this_day;

		const char* month_names[] = { "January", "February", "March", "April", "May", "June",
										 "July", "August", "September", "October", "November", "December" };

		bool is_open = true;
		if (ImGui::BeginPopupModal(popup_id, &is_open, ImGuiWindowFlags_AlwaysAutoResize)) {

			if (ImGui::IsWindowAppearing()) //ในวินาทีแรกที่popupแสดงขึ้นมาจะเข้าเงื่อนไขนี้
			{
				this_day = return_day;
				this_month = return_month;
				this_year = return_year;
			}
			if (!is_open) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetWindowFontScale(1.25f);

			float arrow_width = ImGui::GetFrameHeight(); // ความกว้างปุ่มลูกศร (ทรงจัตุรัส)
			const char* current_month_text = month_names[this_month - 1];
			float text_width = ImGui::CalcTextSize(current_month_text).x;
			float window_width = ImGui::GetWindowWidth();
			float padding = ImGui::GetStyle().WindowPadding.x;

			// 1. วาดปุ่มซ้าย (ชิดขอบซ้ายโดยอัตโนมัติอยู่แล้ว)
			if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
				this_month--;
				if (this_month < 1) { this_month = 12; this_year--; }
			}

			// 2. วาดชื่อเดือนให้อยู่ตรงกลางหน้าต่างเป๊ะๆ
			ImGui::SameLine();
			ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
			ImGui::Text("%s", current_month_text);

			// 3. วาดปุ่มขวา ดันไปชิดขอบขวา (ความกว้างหน้าต่าง - ขนาดปุ่ม - ระยะขอบ)
			ImGui::SameLine();
			ImGui::SetCursorPosX(window_width - arrow_width - padding);
			if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
				this_month++;
				if (this_month > 12) { this_month = 1; this_year++; }
			}

			// --- CENTER THE YEAR ---
			std::string year_str = std::to_string(this_year);
			float year_width = ImGui::CalcTextSize(year_str.c_str()).x;
			float year_start_x = (ImGui::GetWindowSize().x - year_width) * 0.5f;
			if (year_start_x > 0.0f) ImGui::SetCursorPosX(year_start_x);

			ImGui::SetWindowFontScale(1.0f);
			ImGui::Text("%s", year_str.c_str());

			if (ImGui::BeginTable("CalendarTable", 7)) {
				const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
				ImGui::TableNextRow();
				ImGui::SetWindowFontScale(1.0f);
				for (int i = 0; i < 7; i++) {
					ImGui::TableSetColumnIndex(i);

					ImVec2 text_size = ImGui::CalcTextSize(days[i]);
					float available_width = ImGui::GetContentRegionAvail().x;
					float offset = (available_width - text_size.x) * 0.5f;
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
					ImGui::Text("%s", days[i]);
				}
				ImGui::SetWindowFontScale(1.25f);
				//days of month
				int DayOne = FirstDayOfMonth(this_year, this_month); //position of the first day of the month
				int AllDay = HowManyDaysInThisMonth(this_year, this_month);

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
							float cell_h = cell_w;
							ImVec2 cursor_pos = ImGui::GetCursorScreenPos(); // top right coordinate of the box

							bool is_selected = (this_day == nday);

							if (ImGui::Selectable("##day", is_selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(cell_w, cell_h))) {
								this_day = nday;
							}

							//fill numbers
							ImDrawList* draw_list = ImGui::GetWindowDrawList();
							ImVec2 text_size = ImGui::CalcTextSize(day_str.c_str());
							float offset_x = (cell_w - text_size.x) * 0.5f;
							float offset_y = 8.0f;
							ImVec2 text_pos = ImVec2(cursor_pos.x + offset_x, cursor_pos.y + offset_y);

							draw_list->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), day_str.c_str());
							ImGui::PopID(); // คืนค่า ID
							nday++;
						}
					}
				}
				ImGui::EndTable();
			}
			//ImGui::Dummy(ImVec2(0.0f, 1.0f));
			ImVec2 buttonSize(80.0f, 25.0f);
			float targetX = ImGui::GetWindowWidth() - buttonSize.x - ImGui::GetStyle().WindowPadding.x;
			ImGui::SetCursorPosX(targetX);
			ImGui::SetWindowFontScale(1.0f);
			if (ImGui::Button("Confirm", buttonSize)) {
				return_day = this_day;
				return_month = this_month;
				return_year = this_year;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}


	//add new event
	void NewEvent() {

		ImVec2 screenSize = ImGui::GetIO().DisplaySize;
		ImVec2 targetSize(screenSize.x * 0.45f, screenSize.y * 0.45f);
		
		ImGui::SetNextWindowSize(targetSize, ImGuiCond_Always);
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

		if (ImGui::BeginPopupModal("NewEvent", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {

			ImFont* Title = ImGui::GetIO().Fonts->Fonts[2];
			ImGui::PushFont(Title);
			ImGui::Text("New Event");
			ImGui::PopFont();

			ImGui::SameLine();

			//close button
			ImVec2 closeButton(20.0f, 20.0f);

			float WindowWidth = ImGui::GetWindowWidth();
			float WindowHeight = ImGui::GetWindowHeight();

			float targetX = WindowWidth - closeButton.x - 10.0f;

			ImGui::SetCursorPosX(targetX);

			if (ImGui::Button("X", closeButton)) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetNextItemWidth(-20.0f);
			static char event_name[100] = ""; //event's name
			ImGui::InputTextWithHint("##Event's name", "Event's Name", event_name, IM_ARRAYSIZE(event_name));
			// วางคำสั่งนี้ลอยๆ ได้เลย ข้อความจะถูกเซฟลง event_name ตลอดเวลาที่พิมพ์

			ImGui::SetNextItemWidth(-20.0f);
			static char location[250] = ""; //the location
			ImGui::InputTextWithHint("##Location", "Location", location, IM_ARRAYSIZE(location));

			//categories
			ImGui::SetNextItemWidth(150.0f);
			static int selectedItem = 0;
			const char* items[] = { "Work", "Personal", "Business", "+ Add new" };
			if (ImGui::Combo("##Select categories", &selectedItem, items, IM_ARRAYSIZE(items))) {
				if (selectedItem == 3) {
					createnewcategory = true;
				}
			}

			/*-------------------------select date and time-----------------------------*/
			//start
			ImGui::Text("STARTS");
			ImGui::SameLine();

			static int startdate = 0;
			const char* Date[] = {
				"1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
				"11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
				"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"
			};
			ImGui::SetNextItemWidth(60.0f);
			ImGui::Combo("##Select startdate", &startdate, Date, IM_ARRAYSIZE(Date));
			ImGui::SameLine();

			static int startmonth = 0;
			const char* Month[] = {
				"JAN", "FEB", "MAR", "APR", "MAY", "JUN","JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
			};
			ImGui::SetNextItemWidth(75.0f);
			ImGui::Combo("##Select Month", &startmonth, Month, IM_ARRAYSIZE(Month));
			

			ImGui::SameLine();

			//const char* Year[] = {
			//	"2026", "2027", "2028", "2029", "2030", "2031","2032", "2033", "2034", "2035", "2036"
			//};
			//ImGui::SetNextItemWidth(75.0f);
			//ImGui::Combo("##Select Year", &startyear, Year, IM_ARRAYSIZE(Year));
			static int startyear = 0;
			if (startyear == 0) {
				startyear = selected_year;
			}

			char yearPreview[8];
			snprintf(yearPreview, sizeof(yearPreview), "%d", startyear);

			ImGui::SetNextItemWidth(75.0f);
			if (ImGui::BeginCombo("##StartYear", yearPreview)) {
				for (int y = selected_year; y < selected_year + 10; y++) {
					char buf[8];
					snprintf(buf, sizeof(buf), "%d", y);

					bool isSelected = (startyear == y);
					if (ImGui::Selectable(buf, isSelected)) {
						startyear = y;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			
			ImGui::SameLine(0.0f, 30.0f);

			//start time
			ImGui::Text("Time :");
			ImGui::SameLine();

			static int startHour = 0;
			char hourPreview[8];
			snprintf(hourPreview, sizeof(hourPreview), "%02d", startHour);

			ImGui::SetNextItemWidth(60.0f);
			if (ImGui::BeginCombo("##StartHour", hourPreview)) {
				for (int h = 0; h < 24; h++) {
					char buf[8];
					snprintf(buf, sizeof(buf), "%02d", h);

					bool isSelected = (startHour == h);
					if (ImGui::Selectable(buf, isSelected)) {
						startHour = h;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::SameLine();
			ImGui::Text(" : ");
			ImGui::SameLine();

			static int startMin = 0;
			char minPreview[8];
			snprintf(minPreview, sizeof(minPreview), "%02d", startMin);

			ImGui::SetNextItemWidth(60.0f);
			if (ImGui::BeginCombo("##StartMin", minPreview)) {
				for (int m = 0; m <= 60; m++) {
					char buf[8];
					snprintf(buf, sizeof(buf), "%02d", m);

					bool isSelected = (startMin == m);
					if (ImGui::Selectable(buf, isSelected)) {
						startMin = m;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			//End
			ImGui::Text("ENDS");
			ImGui::SameLine(0.0f, 25.0f);

			ImGui::SetNextItemWidth(60.0f);
			static int enddate = 0;
			ImGui::Combo("##Select enddate", &enddate, Date, IM_ARRAYSIZE(Date));
			ImGui::SameLine();

			ImGui::SetNextItemWidth(75.0f);
			static int endMonth = 0;
			ImGui::Combo("##Select endMonth", &endMonth, Month, IM_ARRAYSIZE(Month));
			ImGui::SameLine();

			ImGui::SetNextItemWidth(75.0f);
			static int endyear = 0;
			if (endyear == 0) {
				endyear = selected_year;
			}

			char yearEndPreview[8];
			snprintf(yearEndPreview, sizeof(yearEndPreview), "%d", endyear);

			ImGui::SetNextItemWidth(75.0f);
			if (ImGui::BeginCombo("##EndYear", yearEndPreview)) {
				for (int y = selected_year; y < selected_year + 10; y++) {
					char buf[8];
					snprintf(buf, sizeof(buf), "%d", y);

					bool isSelected = (endyear == y);
					if (ImGui::Selectable(buf, isSelected)) {
						endyear = y;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::SameLine(0.0f, 30.0f);

			//end time
			ImGui::Text("Time :");
			ImGui::SameLine();

			static int endHour = 0;
			char hourEndPreview[8];
			snprintf(hourEndPreview, sizeof(hourEndPreview), "%02d", endHour);

			ImGui::SetNextItemWidth(60.0f);
			if (ImGui::BeginCombo("##EndHour", hourEndPreview)) {
				for (int h = 0; h < 24; h++) {
					char buf[8];
					snprintf(buf, sizeof(buf), "%02d", h);

					bool isSelected = (endHour == h);
					if (ImGui::Selectable(buf, isSelected)) {
						endHour = h;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::SameLine();
			ImGui::Text(" : ");
			ImGui::SameLine();

			static int endMin = 0;
			char minEndPreview[8];
			snprintf(minEndPreview, sizeof(minEndPreview), "%02d", endMin);

			ImGui::SetNextItemWidth(60.0f);
			if (ImGui::BeginCombo("##EndMin", minEndPreview)) {
				for (int m = 0; m < 60; m++) {
					char buf[8];
					snprintf(buf, sizeof(buf), "%02d", m);

					bool isSelected = (endMin == m);
					if (ImGui::Selectable(buf, isSelected)) {
						endMin = m;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			
			/*------------------------------------------------------------------*/

			/*------------------------details---------------------------*/
			static char detail[1024] = "";
			ImVec2 boxSize(-20.0f, 150.0f);
			ImGui::InputTextMultiline("##Detail", detail, sizeof(detail), boxSize, ImGuiInputTextFlags_WordWrap);
			/*----------------------------------------------------------*/

			//save button
			ImVec2 buttonSize(50, 35);

			targetX = WindowWidth - buttonSize.x - 20.0f;
			float targetY = WindowHeight - buttonSize.y - 20.0f;

			ImGui::SetCursorPos(ImVec2(targetX, targetY));

			if (ImGui::Button("save", buttonSize)) {
				// This is where you trigger your logic!

				time_t st_timeinfo = Utils::DMYtoTime(atoi(Date[startdate]), startmonth+1, startyear, startHour, startMin);
				time_t end_timeinfo = Utils::DMYtoTime(atoi(Date[enddate]), endMonth+1, endyear, endHour, endMin);

				Event newEvent(event_name, st_timeinfo, end_timeinfo, items[selectedItem], detail, location);
				myCalendar.addEvent(newEvent);
				
				//delete old info
				event_name[0] = '\0';
				location[0] = '\0';
				detail[0] = '\0';

				selectedItem = 0;

				startdate = 0; startmonth = 0; startyear = 0;
				startHour = 0; startMin = 0;

				enddate = 0; endMonth = 0; endyear = 0;
				endHour = 0; endMin = 0;

				// Clear the text box after saving (optional)
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		
		}
		
		ImGui::PopStyleColor();
	}

	//search
	void SearchEvent() {

		ImVec2 screenSize = ImGui::GetIO().DisplaySize;
		ImVec2 targetSize(screenSize.x * 0.45f, screenSize.y * 0.6f);

		ImGui::SetNextWindowSize(targetSize, ImGuiCond_Appearing);
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

		if (ImGui::BeginPopupModal("SearchEvent", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {

			ImFont* Title = ImGui::GetIO().Fonts->Fonts[2];
			ImGui::PushFont(Title);
			ImGui::Text("Search Events");
			ImGui::PopFont();

			ImGui::SameLine();

			ImVec2 closeButton(20.0f, 20.0f);
			float WindowWidth = ImGui::GetWindowWidth();
			float targetX = WindowWidth - closeButton.x - 10.0f;

			ImGui::SetCursorPosX(targetX);

			static char searchText[512] = "";
			static std::vector<Event> search_results;
			static bool has_searched = false;

			if (ImGui::Button("X", closeButton)) {
				searchText[0] = '\0';
				search_results.clear();
				has_searched = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetNextItemWidth(0.75 * ImGui::GetWindowWidth());
			ImGui::InputTextWithHint("##SearchInput", "Search here...", searchText, IM_ARRAYSIZE(searchText));

			ImGui::SameLine();

			if (ImGui::Button("search")) {
				// เรียกใช้ฟังก์ชันค้นหาจาก Backend
				search_results = myCalendar.searchEvents(std::string(searchText));
				has_searched = true;
			}

			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			// 2. สร้างกล่องเลื่อนได้ (Child Window) สำหรับแสดงผลลัพธ์
			// พารามิเตอร์ ImVec2(0, 0) แปลว่า ให้กินพื้นที่ที่เหลือด้านล่างทั้งหมด
			ImGui::BeginChild("SearchResultsRegion", ImVec2(0, 0), true);

			if (has_searched) {
				if (search_results.empty()) {
					ImGui::TextDisabled("No events found matching '%s'", searchText);
				}
				else {
					ImGui::TextDisabled("Found %d events:", (int)search_results.size());
					ImGui::Dummy(ImVec2(0.0f, 5.0f));

					// 3. วนลูปวาดผลลัพธ์ทีละอัน
					for (const auto& ev : search_results) {
						ImGui::PushID(ev.getID()); // ป้องกัน ID ซ้ำ

						std::string display_name = ev.getTitle() + " (" + ev.getCategory() + ")";

						if (ImGui::Selectable(display_name.c_str())) {
							auto [s_day, s_month, s_year, s_hour, s_min] = Utils::timeToDMY(ev.getStartTime());
							selected_day = s_day;
							selected_month = s_month;
							selected_year = s_year;

							searchText[0] = '\0';
							search_results.clear();
							has_searched = false;

							ImGui::CloseCurrentPopup();
						}

						// draw date and detail
						auto [d, m, y, h, min] = Utils::timeToDMY(ev.getStartTime());
						ImGui::TextDisabled("  Date: %02d/%02d/%d", d, m, y);
						ImGui::TextDisabled("  Detail : %s", ev.getDetails().c_str());
						ImGui::TextDisabled("  Location : %s", ev.getPlaces().c_str());

						ImGui::Separator();
						ImGui::PopID();
					}
				}
			}
			else {
				ImGui::TextDisabled("Type a keyword and press search.");
			}

			ImGui::EndChild(); // ปิดกล่องเลื่อนได้

			ImGui::EndPopup();
		}

		ImGui::PopStyleColor();
	}

	void CreateNewCategory() {
		static ImVec4 color = ImVec4(0.11f, 0.38f, 0.48f, 200.0 / 255.0f); // The currently selected color
		static ImVec4 backup_color;                               // Saves the color in case you cancel
		static ImVec4 saved_palette[32] = {};                     // Array to hold the 32 little palette squares
		static bool saved_palette_init = true;
		ImGuiColorEditFlags base_flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf |
				ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV |
				ImGuiColorEditFlags_DisplayHex;

		if (createnewcategory)
		{
			ImGui::OpenPopup("Category Color Picker");
			createnewcategory = false;
		}

		if (saved_palette_init) {
			for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++) {
				ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
					saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
				saved_palette[n].w = 1.0f;
			}
			saved_palette_init = false;
		}

		//Popup if trigger
		if (ImGui::BeginPopupModal("Category Color Picker", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static char new_category[256] = "";
			ImGui::Text("Pick a color for your new category :");
			ImGui::InputText("##", new_category, sizeof(new_category));
			ImGui::Separator();

			// The main picker
			ImGui::ColorPicker4("##picker", (float*)&color, base_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
			ImGui::SameLine();

			// The right side panel
			ImGui::BeginGroup();
			ImGui::Text("Current");
			ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));

			ImGui::Separator();
			ImGui::Text("Palette");
			for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
			{
				ImGui::PushID(n);
				if ((n % 8) != 0) ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

				ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
				if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
					color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w);

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
						memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
						memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
					ImGui::EndDragDropTarget();
				}
				ImGui::PopID();
			}
			ImGui::EndGroup();

			ImGui::Separator();

			// --- NEW RIGHT-ALIGNMENT MATH ---
			float button_width = 120.0f;
			float spacing = ImGui::GetStyle().ItemSpacing.x;
			float total_buttons_width = (button_width * 2.0f) + spacing;

			// Calculate where the cursor needs to start to fit exactly on the right edge
			float right_align_x = ImGui::GetWindowWidth() - total_buttons_width - ImGui::GetStyle().WindowPadding.x;

			// Move the cursor to that position before drawing the first button
			ImGui::SetCursorPosX(right_align_x);

			// 4. Buttons to confirm or cancel the popup
			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Save Color", ImVec2(120, 0))) {
				if (strlen(new_category) > 0) {

					// ImVec4 stores RGBA as x, y, z, w
					EventCategory new_cat(std::string(new_category),
						color.x, // R
						color.y, // G
						color.z, // B
						color.w  // A
					);
					// 2. Push it to your main CalendarManager instance
					myCalendar.addCategory(new_cat);

					// 3. Clear the text box so it's empty next time the popup opens
					memset(new_category, 0, sizeof(new_category));
					myCalendar.addCategory(new_cat);

					//Clear the text box so it's empty next time the popup opens
					memset(new_category, 0, sizeof(new_category));

				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void UpcomingEvent()
	{
		int n_events = 5;
		ImFont* Title = ImGui::GetIO().Fonts->Fonts[2];
		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window_class);

		ImGui::Begin("Upcoming Event", NULL, ImGuiDockNodeFlags_NoTabBar);
		ImGui::SetWindowFontScale(1.25f);
		std::vector<Event> upcoming_events = myCalendar.getUpcomingEvents(5);
		ImGui::PushFont(Title);
		ImGui::Text("Upcoming Events");
		ImGui::PopFont();

		if (upcoming_events.empty()) {
			ImGui::TextDisabled("No upcoming events.");
		}
		else {
			for (int i = 0; i < upcoming_events.size(); i++) {
				const Event& ev = upcoming_events[i];
				ImGui::PushID(ev.getID());

				time_t now = time(nullptr);
				std::string status_text = "";

				if (now >= ev.getStartTime() && now <= ev.getEndTime()) {
					status_text = "In progress";
				}
				else if (now < ev.getStartTime()) {
					int days = difftime(ev.getStartTime(), now) / 86400;

					if (days == 0) {
						int hours = difftime(ev.getStartTime(), now) / 3600;
						status_text = "in " + std::to_string(hours) + " hours";
					}
					else {
						status_text = "in " + std::to_string(days) + " days";
					}
				}

				bool is_open = ImGui::CollapsingHeader(ev.getTitle().c_str());

				//if the last drawing was clicked
				if (ImGui::IsItemClicked()) {
					auto [s_day, s_month, s_year, s_hour, s_min] = Utils::timeToDMY(ev.getStartTime());

					// Update the globally selected date to the calendar and showevent
					selected_day = s_day;
					selected_month = s_month;
					selected_year = s_year;

					ThisMonth = s_month;
					ThisYear = s_year;
				}

				float window_width = ImGui::GetWindowContentRegionMax().x;
				float text_width = ImGui::CalcTextSize(status_text.c_str()).x;

				ImGui::SameLine(window_width - text_width);
				ImGui::TextDisabled("%s", status_text.c_str());

				if (is_open) {
					auto [s_day, s_month, s_year, s_hour, s_min] = Utils::timeToDMY(ev.getStartTime());
					auto [e_day, e_month, e_year, e_hour, e_min] = Utils::timeToDMY(ev.getEndTime());


					ImGui::SeparatorText("Time");
					ImGui::Text("%02d/%02d/%d (%02d:%02d) - %02d/%02d/%d (%02d:%02d)",
						s_day, s_month, s_year, s_hour, s_min,
						e_day, e_month, e_year, e_hour, e_min);

					ImGui::SeparatorText("Category");
					ImGui::Text("%s", ev.getCategory().c_str());

					ImGui::SeparatorText("Location");
					ImGui::Text("%s", ev.getPlaces().c_str());

					ImGui::SeparatorText("Details");
					ImGui::TextWrapped("%s", ev.getDetails().c_str());
					ImGui::TextWrapped("");
				}

				ImGui::PopID();
			}
		}

		//----------------------------------------------
		ImVec2 buttonSize(60, 60);

		float WindowHeight = ImGui::GetWindowHeight();
		float WindowWidth = ImGui::GetWindowWidth();

		float targetY = WindowHeight - buttonSize.y - 30.0f; // 45 = 30 + 15 ---ระยะห่างขอบ + ระยะห่างของแต่ละปุ่ม
		float targetX = WindowWidth - (2 * buttonSize.x) - 45.0f; // *2 since we have 2 buttons

		ImGui::SetCursorPos(ImVec2(targetX, targetY));

		if (ImGui::Button("search", buttonSize)) {
			ImGui::OpenPopup("SearchEvent");
		}
		ImGui::SameLine(0.0f, 15.0f);
		ImGui::PushFont(Title);
		if (ImGui::Button("+", buttonSize)) {
			ImGui::OpenPopup("NewEvent");
		}
		ImGui::PopFont();

		SearchEvent();
		NewEvent();

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