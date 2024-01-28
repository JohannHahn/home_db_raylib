#include "raylib-gui-layout/gui.h"
#include "raylib-gui-layout/layout.h"
#include "data_base.h"
#include "raylib-gui-layout/raylib/src/raylib.h"

float window_width = 900;
float window_height = 600;
Rectangle main_window_rec = Rectangle{0, 0, window_width, window_height};
Layout main_window_layout = Layout(main_window_rec, SLICE_HOR, 0.5f);
Layout category_view_layout = Layout(main_window_layout.get_slot(1), VERTICAL, 10);
DataBase db = DataBase("datenbank.db");
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

void category_tree(const char* root_id = NULL) {
    if(root_id == NULL) root_id = "0";
    const char* root_name = db.sql_query("select name from categories where c_id = %s", root_id).entries.c_str();
    Result children_id_result = db.sql_query("Select c_id from categories where subcatof = %s", root_id);

    if(Gui::tree_node(root_name)) {
	for(int i = 0; i < children_id_result.num_rows; ++i) {
	    category_tree(Gui::read_word(children_id_result.entries.c_str(), i));
	    Gui::tree_pop();
	}
    }
}

int main() {
    InitWindow(window_width, window_height, "Home DataBase");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
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
	Gui::begin_tree(main_window_layout.get_slot(1));
	category_tree();
	Gui::end_tree();

	EndDrawing();
    }
}
