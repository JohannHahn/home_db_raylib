#include "data_base.h"
#include "gui.h"

float window_width = 900;
float window_height = 600;
Rectangle main_window_rec = Rectangle{0, 0, window_width, window_height};
Layout main_window_layout = Layout(main_window_rec, SLICE_HOR, 0.5f);
DataBase db = DataBase("datenbank.db");
int bg_col_int = 0xff181818;
Color background_col = *(Color*)&bg_col_int; 

const char* selected_category;

void resize() {
    window_width = GetScreenWidth();
    window_height = GetScreenHeight();
    main_window_rec = Rectangle{0, 0, window_width, window_height};
    main_window_layout = Layout(main_window_rec, SLICE_HOR, 0.5f);
}

void category_tree(const char* root_id = NULL, bool default_open = false) {
    if(root_id == NULL) root_id = "0";
    const char* root_name = db.sql_query("select name from categories where c_id = %s", root_id).entries.c_str();
    Result children_id_result = db.sql_query("Select c_id from categories where subcatof = %s", root_id);
    bool button_click = false;
    if(Gui::tree_node(root_name, default_open, &button_click)) {
	if(button_click) {
	    selected_category = root_id;
	}
	for(int i = 0; i < children_id_result.num_rows; ++i) {
	    category_tree(Gui::read_word(children_id_result.entries.c_str(), i));
	    Gui::tree_pop();
	}
    }
}

void table(Rectangle boundary) {

    Result col_names_result = db.sql_query("select ger_name from col_names");
    Result rows = db.sql_query("select name, description, producer, prod_date, age from things where category_id = %s", selected_category); 
    rows.print("rows = ");
    Gui::table(boundary, col_names_result.num_rows - 1, rows.num_rows, col_names_result.entries.c_str(), rows.entries.c_str());
}

int main() {
    InitWindow(window_width, window_height, "Home DataBase");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    resize();
    SetTargetFPS(60);
    bool open[3] = {0};
    while(!WindowShouldClose()) {
	if(IsWindowResized()) {
	    resize();
	}
	BeginDrawing();
	ClearBackground(background_col);
	
	table(main_window_layout.get_slot(0));

	Gui::begin_tree(main_window_layout.get_slot(1));
	category_tree(NULL, true);
	Gui::end_tree();

	EndDrawing();
    }
}
