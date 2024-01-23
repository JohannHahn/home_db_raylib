#include "raylib-gui-layout/gui.h"
#include "raylib-gui-layout/layout.h"
#include "data_base.h"
#include "raylib-gui-layout/raylib/src/raylib.h"

float window_width = 900;
float window_height = 600;
Rectangle main_window_rec = Rectangle{0, 0, window_width, window_height};
Layout main_window_layout = Layout(main_window_rec, SLICE_HOR, 0.5f);

void on_click(void* label) {
    std::cout << (char*)label << "\n";
}

void resize() {
    window_width = 900;
    window_height = 600;
    main_window_rec = Rectangle{0, 0, window_width, window_height};
    main_window_layout = Layout(main_window_rec, SLICE_HOR, 0.5f);
}

int main() {
    InitWindow(window_width, window_height, "Home DataBase");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    DataBase db = DataBase("datenbank.db");
    Result r = db.sql_query("Select name, category_id from things");
    r.print("after query: \n");
    resize();
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
	if(IsWindowResized()) {
	    resize();
	}
	BeginDrawing();
	ClearBackground(RED);
	Gui::table(main_window_layout.get_slot(0), r.num_cols, r.num_rows, r.col_names.c_str(), r.entries.c_str(), on_click);
	EndDrawing();
    }
}
