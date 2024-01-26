#include "raylib-gui-layout/gui.h"
#include "raylib-gui-layout/layout.h"
#include "data_base.h"
#include "raylib-gui-layout/raylib/src/raylib.h"

float window_width = 900;
float window_height = 600;
Rectangle main_window_rec = Rectangle{0, 0, window_width, window_height};
Layout main_window_layout = Layout(main_window_rec, SLICE_HOR, 0.5f);
Layout category_view_layout = Layout(main_window_layout.get_slot(1), VERTICAL, 10);

void on_click(void* label) {
    std::cout << (char*)label << "\n";
}

void resize() {
    window_width = 900;
    window_height = 600;
    main_window_rec = Rectangle{0, 0, window_width, window_height};
    main_window_layout = Layout(main_window_rec, SLICE_HOR, 0.5f);
    category_view_layout = Layout(main_window_layout.get_slot(1), VERTICAL, 10);
}

int main() {
    InitWindow(window_width, window_height, "Home DataBase");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    DataBase db = DataBase("datenbank.db");
    Result r = db.sql_query("Select name, category_id from things");
    resize();
    SetTargetFPS(60);
    bool open[3] = {0};
    while(!WindowShouldClose()) {
	if(IsWindowResized()) {
	    resize();
	}
	BeginDrawing();
	ClearBackground(RED);
	Gui::table(main_window_layout.get_slot(0), r.num_cols, r.num_rows, r.col_names.c_str(), r.entries.c_str(), on_click);

	Gui::begin_tree();
	Gui::tree_node(category_view_layout.get_slot(0), "node", &open[0]);
	if(open[0]) { 
	    Gui::tree_node(category_view_layout.get_slot(1), "node2", &open[1]);
	    if(open[1]) Gui::tree_node(category_view_layout.get_slot(1), "node3", &open[2]);
	}
	Gui::end_tree();

	EndDrawing();
    }
}
