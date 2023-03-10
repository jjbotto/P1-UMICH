// IDENTIFIER  = 950181F63D0A883F183EC0A5CC67B19928FE896A


#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <deque>
#include <getopt.h>
#include "Game.h"
using namespace std;

// default constructor
Game::Game()
	: tiles_discovered(0), dimension(0), num_of_rooms(0), routing_scheme('X'), input_mode('M'),
	output_mode('M'), solution(false)  {}


// parses command line options
void Game::get_cl_options(int argc, char** argv) {
	int option_index = 0, option = 0;

	int arg_count = 0;

	opterr = false;

	struct option longOpts[] = { { "stack", no_argument, nullptr, 's'},
								 { "queue", no_argument, nullptr, 'q'},
								 { "output", required_argument, nullptr, 'o'},
								 { "help", no_argument, nullptr, 'h'},
								 { nullptr, 0, nullptr, '\0' } };

	while ((option = getopt_long(argc, argv, "sqo:h", longOpts, &option_index)) != -1) {
		switch (option) {
		case 's': {
			++arg_count;
			set_routing_scheme('S');
			break;
		}
		case 'q': {
			set_routing_scheme('Q');
			++arg_count;
			break;
		}
		case 'o': {
			char op = *optarg;
			if (op == 'L') {
				set_output_mode(op);
			}
			else {
				set_output_mode('M');
			}
			break;
		}
		case 'h': {
			std::cout << "This program reads a file via file redirection for a ship escape \n"
				<< "game map and then uses a pathfinding algorithm to find a path \n"
				<< "from the start position to the exit. It will output the path using file \n"
				<< "redirection.\n";
			exit(0);
		}
		}
	
		if (arg_count > 1) {
			cerr << "Multiple routing modes specified";
			exit(1);
		}		
	}
}

void Game::set_routing_scheme(char x) {
	routing_scheme = x;
}

char Game::get_routing_scheme() {
	return routing_scheme;
}

// fills map reading from spec based on input mode
void Game::fill_map() {
	// map mode input
	char iMode;
	int rooms;
	int dim;
	string junk1;
	cin >> iMode >> rooms >> dim;
	set_input_mode(iMode);
	set_num_of_rooms(rooms);
	set_dimension(dim);

	// sets map size
	map.resize(rooms, vector<vector<Tile>>(dim, vector<Tile>(dim)));


	if (input_mode == 'M') {

		for (int room = 0; room < get_num_rooms(); ++room) {
			// for comments in .txt files
			char slash;
			cin >> slash;
			while (slash == '/') {
				getline(cin, junk1);
				cin >> slash;
			}
			for (int row = 0; row < get_dimension(); ++row) {
				for (int col = 0; col < get_dimension(); ++col) {
					Tile a;
					char sym;
					if (row == 0 && slash != '/') {
						sym = slash;
						slash = '/';
					}
					else {
						cin >> sym;
					}
					check_symbol_error(sym);
					a.symbol = sym;

					map[room][row][col] = a;

					// sets start tile
					if (a.symbol == 'S') {
						start.room = room;
						start.row = row;
						start.col = col;
					}
					if (a.symbol == 'H') {
						countess.room = room;
						countess.row = row;
						countess.col = col;
					}

				}
			}
		}
	}
	// list mode input
	if (input_mode == 'L') {
		//junk chars
		char p1 = 'x', c1 = 'x', c2 = 'x', c3 = 'x', p2 = 'x';
		// coordinates
		int room = 0, row = 0, col = 0;
		char sym = 'x';

		// fills map 
		while (cin >> p1) {
			while (p1 != '(') {
				getline(cin, junk1);

				cin >> p1;
			}
			cin >> room >> c1 >> row >> c2 >> col >> c3 >> sym >> p2;
			check_rrc_errors(room, row, col);
			Tile a;
			a.symbol = sym;
			// checks for symbol errors
			check_symbol_error(a.symbol);
			if (a.symbol == 'S') {
				start.room = room;
				start.row = row;
				start.col = col;
			}
			if (a.symbol == 'H') {
				countess.room = room;
				countess.row = row;
				countess.col = col;
			}
			map[room][row][col] = a;
		}
		// fills unlisted coordinates with .
		for (int room = 0; room < get_num_rooms(); ++room) {
			for (int row = 0; row < get_dimension(); ++row) {
				for (int col = 0; col < get_dimension(); ++col) {
					if (map[room][row][col].symbol == 'X') {
						map[room][row][col].symbol = '.';
					}
				}
			}
		}
	}
	
	return;
}


void Game::search() {
	deque<Location> search_container;
	Location current = start;
	map[current.room][current.row][current.col].discovered = true;
	search_container.push_back(current);
	++tiles_discovered;

	while (!search_container.empty()) {
		// queue
		if (routing_scheme == 'Q') {
			current = search_container.front();
			search_container.pop_front();
		}
		// stack
		else if (routing_scheme == 'S') {
			current = search_container.back();
			search_container.pop_back();
		}
		int elevator_floor = current.room;
		char symbol = map[current.room][current.row][current.col].symbol;

		

		// for North
		if (current.row - 1 >= 0) {
			Location next;
			next.room = current.room;
			next.row = current.row - 1;
			next.col = current.col;
			if (valid_tile(map[next.room][next.row][next.col])) {
				search_container.push_back(next);
				++tiles_discovered;
				map[next.room][next.row][next.col].discovered = true;
				map[next.room][next.row][next.col].direction_came_from = 'n';
			}
			if (map[next.room][next.row][next.col].symbol == 'H') {
				solution = true;
			}
		}
		if (solution == true) {
			break;
		}
		// for East
		if (current.col + 1 < get_dimension()) {
			Location next;
			next.room = current.room;
			next.row = current.row;
			next.col = current.col + 1;
			if (valid_tile(map[next.room][next.row][next.col])) {
				search_container.push_back(next);
				++tiles_discovered;
				map[next.room][next.row][next.col].discovered = true;
				map[next.room][next.row][next.col].direction_came_from = 'e';
			}
			if (map[next.room][next.row][next.col].symbol == 'H') {
				solution = true;
			}
		}
		if (solution == true) {
			break;
		}
		// for South
		if (current.row + 1 < get_dimension()) {
			Location next;
			next.room = current.room;
			next.row = current.row + 1;
			next.col = current.col;
			if (valid_tile(map[next.room][next.row][next.col])) {
				search_container.push_back(next);
				++tiles_discovered;
				map[next.room][next.row][next.col].discovered = true;
				map[next.room][next.row][next.col].direction_came_from = 's';
			}
			if (map[next.room][next.row][next.col].symbol == 'H') {
				solution = true;
			}
		}
		if (solution == true) {
			break;
		}
		// for West
		if (current.col - 1 >= 0) {
			Location next;
			next.room = current.room;
			next.row = current.row;
			next.col = current.col - 1;
			if (valid_tile(map[next.room][next.row][next.col])) {
				search_container.push_back(next);
				++tiles_discovered;
				map[next.room][next.row][next.col].discovered = true;
				map[next.room][next.row][next.col].direction_came_from = 'w';
			}
			if (map[next.room][next.row][next.col].symbol == 'H') {
				solution = true;
			}
		}
		if (solution == true) {
			break;
		}

		// for elevators
		if (symbol == 'E') {
			Location next;
			for (int i = 0; i < num_of_rooms; ++i) {
				next.room = i;
				next.row = current.row;
				next.col = current.col;

				if (valid_tile(map[next.room][next.row][next.col]) && map[next.room][next.row][next.col].symbol == 'E') {
					search_container.push_back(next);
					++tiles_discovered;
					map[next.room][next.row][next.col].discovered = true;
					map[next.room][next.row][next.col].direction_came_from = int_to_char(elevator_floor);

					if (map[next.room][next.row][next.col].symbol == 'H') {
						solution = true;
					}
				}
			}
		}
		if (solution == true) {
			break;
		}
	}
	return;
}


bool Game::valid_tile(Tile x) {
	if (x.symbol != '#' && x.discovered == false) {
		return true;
	}
	return false;
}

void Game::backtracing() {
	Location current = countess;

	Tile current_tile = map[current.room][current.row][current.col];
	if (solution == false) {
		return;
	}

	while (map[start.room][start.row][start.col].symbol == 'S') {
		// elevators
		if (current_tile.direction_came_from == '0' || current_tile.direction_came_from == '1' ||
			current_tile.direction_came_from == '2' || current_tile.direction_came_from == '3' ||
			current_tile.direction_came_from == '4' || current_tile.direction_came_from == '5' ||
			current_tile.direction_came_from == '6' || current_tile.direction_came_from == '7' ||
			current_tile.direction_came_from == '8' || current_tile.direction_came_from == '9') {

			map[char_to_int(current_tile.direction_came_from)][current.row][current.col].symbol = int_to_char(current.room);
			current.room = char_to_int(current_tile.direction_came_from);
			current_tile = map[char_to_int(current_tile.direction_came_from)][current.row][current.col];

		}
		else if (current_tile.direction_came_from == 'n') {
			map[current.room][current.row + 1][current.col].symbol = 'n';
			current_tile = map[current.room][current.row + 1][current.col];
			current.row = current.row + 1;

		}
		else if (current_tile.direction_came_from == 'e') {
			map[current.room][current.row][current.col - 1].symbol = 'e';
			current_tile = map[current.room][current.row][current.col - 1];
			current.col = current.col - 1;

		}
		else if (current_tile.direction_came_from == 's') {
			map[current.room][current.row - 1][current.col].symbol = 's';
			current_tile = map[current.room][current.row - 1][current.col];
			current.row = current.row - 1;

		}
		else if (current_tile.direction_came_from == 'w') {
			map[current.room][current.row][current.col + 1].symbol = 'w';
			current_tile = map[current.room][current.row][current.col + 1];
			current.col = current.col + 1;

		}
		list_output.push_back(current);
	}

}

void Game::output_path() {
	
	if (output_mode == 'L') {
		cout << "//path taken\n";
		while (!list_output.empty()) {
			Location back = list_output.back();
			cout << "(" << back.room << "," << back.row << "," << back.col << "," << map[back.room][back.row][back.col].symbol << ")\n";

			list_output.pop_back();
		}
	}
	else {
		cout << "Start in level " << start.room << ", row " << start.row << ", column " << start.col << "\n";
		for (int room = 0; room < get_num_rooms(); ++room) {
			cout << "//level " << room << "\n";
			for (int row = 0; row < get_dimension(); ++row) {
				for (int col = 0; col < get_dimension(); ++col) {
					char sym = get_tile_symbol(room, row, col);
					cout << sym;
				}
				cout << '\n';
			}
		}
	}
}

// error checking function for symbols
void Game::check_symbol_error(char x) {

	if (x != '.' && x != '#' && x != 'E' && x != 'S' && x != 'H'
		&& x != '0' && x != '1' && x != '2' && x != '3' && x != '4'
		&& x != '5' && x != '6' && x != '7' && x != '8' && x != '9') {
		cerr << "Invalid map character";
		return exit(1);
	}
	return;
}

int Game::char_to_int(char x) {
	if (x == '0') {
		return 0;
	}
	else if (x == '1') {
		return 1;
	}
	else if (x == '2') {
		return 2;
	}
	else if (x == '3') {
		return 3;
	}
	else if (x == '4') {
		return 4;
	}
	else if (x == '5') {
		return 5;
	}
	else if (x == '6') {
		return 6;
	}
	else if (x == '7') {
		return 7;
	}
	else if (x == '8') {
		return 8;
	}
	else if (x == '9') {
		return 9;
	}
	return -1;
}

char Game::int_to_char(int x) {
	if (x == 0) {
		return '0';
	}
	else if (x == 1) {
		return '1';
	}
	else if (x == 2) {
		return '2';
	}
	else if (x == 3) {
		return '3';
	}
	else if (x == 4) {
		return '4';
	}
	else if (x == 5) {
		return '5';
	}
	else if (x == 6) {
		return '6';
	}
	else if (x == 7) {
		return '7';
	}
	else if (x == 8) {
		return '8';
	}
	else if (x == 9) {
		return '9';
	}
	return 'a';
}

// error checking for rooms, rows, cols
void Game::check_rrc_errors(int room, int row, int col) {
	if (room > get_num_rooms() - 1 || room < 0) {
		cerr << "Invalid map level";
		return exit(1);
	}
	if (row > get_dimension() - 1 || row < 0) {
		cerr << "Invalid map row";
		return exit(1);
	}
	if (col > get_dimension() - 1 || col < 0) {
		cerr << "Invalid map column";
		return exit(1);
	}
}

char Game::get_tile_symbol(int room, int row, int col) {
	return map[room][row][col].symbol;
}

void Game::set_dimension(int dim) {
	dimension = dim;
}
int Game::get_dimension() {
	return dimension;
}
void Game::set_num_of_rooms(int num) {
	num_of_rooms = num;
}
int Game::get_num_rooms() {
	return num_of_rooms;
}
void Game::set_input_mode(char iMode) {
	input_mode = iMode;
}
char Game::get_input_mode() {
	return input_mode;
}
void Game::set_output_mode(char oMode) {
	output_mode = oMode;
}
char Game::get_output_mode() {
	return output_mode;
}
int Game::get_tiles_discovered() {
	return tiles_discovered;
}

bool Game::check_start_location(Location x) {
	if (x.room == start.room && x.row == start.row && x.col == start.col) {
		return true;
	}
	return false;
}































