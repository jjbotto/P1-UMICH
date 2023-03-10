#pragma once
// IDENTIFIER = 950181F63D0A883F183EC0A5CC67B19928FE896A


#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <deque>
#include <getopt.h>
using namespace std;

class Game {


public:
	struct Tile {
		char symbol = 'X';
		bool discovered = false;
		char direction_came_from = 'X';
	};

	struct Location {
		int room = 0;
		int row = 0;
		int col = 0;
	};
	vector<vector<vector<Tile>>> map;

	Game();

	void get_cl_options(int argc, char** argv);

	void set_routing_scheme(char x);

	char get_routing_scheme();

	void fill_map();

	void output_path();

	bool valid_tile(Tile x);

	void search();

	void backtracing();

	void check_symbol_error(char x);

	int char_to_int(char x);

	char int_to_char(int x);

	void check_rrc_errors(int room, int row, int col);

	char get_tile_symbol(int room, int row, int col);

	void set_dimension(int dim);

	int get_dimension();

	void set_num_of_rooms(int num);

	int get_num_rooms();

	void set_input_mode(char iMode);

	char get_input_mode();

	void set_output_mode(char oMode);

	char get_output_mode();

	int get_tiles_discovered();

	bool check_start_location(Location x);


private:
	Location start;
	Location countess;
	int tiles_discovered;
	int dimension;
	int num_of_rooms;
	char routing_scheme = 'Q';
	char input_mode;
	char output_mode;
	bool solution = false;	
	vector<Location> list_output;

};



