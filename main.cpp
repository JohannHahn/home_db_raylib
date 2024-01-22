#include "raylib-gui-layout/gui.h"
#include "raylib-gui-layout/layout.h"
#include "data_base.h"

float window_width = 900;
float window_height = 600;

void on_click(void* label) {
    std::cout << (char*)label << "\n";
}

int main() {
    InitWindow(window_width, window_height, "Home DataBase");
    DataBase db = DataBase("datenbank.db");
    Result r = db.sql_query("Select name, category_id from things");
    std::cout << r.entries << ", " << r.col_names << "\n"; 
    while(!WindowShouldClose()) {
	BeginDrawing();
	ClearBackground(RED);
	Gui::table(Rectangle{100, 100, 200, 200}, 3, 2, r.col_names.c_str(), r.entries.c_str(), 0, on_click);
	EndDrawing();
    }
}
