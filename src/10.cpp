#include <file_utils.h>
#include <coordinate_utils.h>
#include <unordered_set>
#include <queue>
#include <set_utils.h>

using cpp_utils::file_utils::FileUtils;
using cpp_utils::coordinate_utils::CoordinateCache;
using cpp_utils::set_utils::set_equal;

/*
| is a vertical pipe connecting north and south.
- is a horizontal pipe connecting east and west.
L is a 90-degree bend connecting north and east.
J is a 90-degree bend connecting north and west.
7 is a 90-degree bend connecting south and west.
F is a 90-degree bend connecting south and east.
. is ground; there is no pipe in this tile.
S is the starting position of the animal; there is a pipe on this tile, but your sketch doesn't show what shape the pipe has.
*/

enum Direction {
	NORTH,
	SOUTH,
	EAST,
	WEST
};

bool moveable(char destination, Direction direction) {
	std::unordered_map<char, std::unordered_set<Direction>> valid_incoming_directions = {
		{'|', {NORTH, SOUTH}},
		{'-', {EAST, WEST}},
		{'L', {SOUTH, WEST}},
		{'J', {SOUTH, EAST}},
		{'7', {EAST, NORTH}},
		{'F', {NORTH, WEST}},
	};

	return valid_incoming_directions.contains(destination) && valid_incoming_directions.at(destination).contains(direction);
}

std::vector<std::vector<int>> get_neighbors(std::vector<std::string>& grid, std::vector<int>& coordinate, CoordinateCache<int>& cache) {
	std::vector<std::vector<int>> neighbors;
	std::unordered_map<char, std::vector<Direction>> valid_outgoing_directions = {
		{'|', {NORTH, SOUTH}},
		{'-', {EAST, WEST}},
		{'L', {NORTH, EAST}},
		{'J', {NORTH, WEST}},
		{'7', {WEST, SOUTH}},
		{'F', {SOUTH, EAST}},
		{'S', {NORTH, SOUTH, EAST, WEST}}
	};
	std::vector<Direction> possible_directions = valid_outgoing_directions.at(grid[coordinate[0]][coordinate[1]]);
	for (int i=0; i<possible_directions.size(); i++) {
		int y = coordinate[0];
		int x = coordinate[1];
		Direction direction = possible_directions[i];
		switch(direction) {
			case NORTH:
				y--;
				break;
			case SOUTH:
				y++;
				break;
			case EAST:
				x++;
				break;
			case WEST:
				x--;
				break;
		}

		if (x < 0 || x >= grid[0].size() || y < 0 || y >= grid.size() || cache.contains({y, x}) || !moveable(grid[y][x], direction)) {
			continue;
		}

		neighbors.push_back({y, x, coordinate[2] + 1});
	}
	return neighbors;
}

void replace_s(std::vector<std::string>& grid, int s_y, int s_x) {
	if (grid[s_y][s_x] != 'S') {
		return;
	}
	
	std::unordered_set<Direction> moveable_directions;

	for (int i=0; i<4; i++) {
		Direction direction = static_cast<Direction>(i);
		int x = s_x;
		int y = s_y;
		switch(direction) {
			case NORTH:
				y--;
				break;
			case SOUTH:
				y++;
				break;
			case EAST:
				x++;
				break;
			case WEST:
				x--;
				break;
		}

		if (x < 0 || x >= grid[0].size() || y < 0 || y >= grid.size() || !moveable(grid[y][x], direction)) {
			continue;
		}
		else {
			moveable_directions.insert(direction);
		}
	}

	std::unordered_map<char, std::unordered_set<Direction>> valid_outgoing_directions = {
		{'|', {NORTH, SOUTH}},
		{'-', {EAST, WEST}},
		{'L', {NORTH, EAST}},
		{'J', {NORTH, WEST}},
		{'7', {WEST, SOUTH}},
		{'F', {SOUTH, EAST}},
	};

	for (auto p: valid_outgoing_directions) {
		if (set_equal<std::unordered_set<Direction>>(p.second, moveable_directions)) {
			grid[s_y][s_x] = p.first;
			return;
		}
	}
}

int main() {
	std::vector<std::string> grid;
	FileUtils fileUtils(INPUT_PATH);
	int s_y = 0;
	int s_x = -1;
	fileUtils.process_lines([&](std::string line) {
		grid.push_back(line);
		int s = line.find("S");
		if (s != -1) {
			s_x = s;
		}
		else if (s_x == -1) {
			s_y++;
		}
	});

	fileUtils.close();

	CoordinateCache<int> edgeCache;

	std::queue<std::vector<int>> q;

	q.push({s_y, s_x, 0});

	int max_dist = INT_MIN;

	while (!q.empty()) {
		std::vector<int> coordinate = q.front();
		q.pop();
		
		edgeCache.insert({coordinate[0], coordinate[1]});
		max_dist = std::max(max_dist, coordinate[2]);

		std::vector<std::vector<int>> neighbors = get_neighbors(grid, coordinate, edgeCache);

		for (auto& next: neighbors) {
			q.push(next);
		}
	}

	std::cout << max_dist << std::endl;

	replace_s(grid, s_y, s_x);

	// for (int y=0; y<grid.size(); y++) {
	// 	for (int x=0; x<grid[y].size(); x++) {
	// 		std::cout << grid[y][x];
	// 	}
	// 	std::cout << std::endl;
	// }
	
	// std::cout << std::endl;

	// for (int y=0; y<grid.size(); y++) {
	// 	for (int x=0; x<grid[y].size(); x++) {
	// 		if (edgeCache.contains({y, x})) {
	// 			std::cout << "x";
	// 		}
	// 		else {
	// 			std::cout << grid[y][x];
	// 		}
	// 	}
	// 	std::cout << std::endl;
	// }
	
	// std::cout << std::endl;

	int enclosed_count = 0;

	for (int y=0; y<grid.size(); y++) {
		for (int x=0; x<grid[y].size(); x++) {
			if (!edgeCache.contains({y, x})) {
				int edge_count = 0;
				/*
					|, FJ, L7
					LJ and F7 can be thought as vertexes, they dip and go back, instead of surrounding.
					
					tail should start at F or L, be reset when J,7 is reached 						
				*/

				int last_curve_point = 0;

				for (int i=x+1; i<grid[y].size(); i++) {
					if (edgeCache.contains({y, i}) && grid[y][i] == '|') {
						edge_count++;
					}

					if (edgeCache.contains({y, i}) && (grid[y][i] == 'F' || grid[y][i] == 'L')) {
						last_curve_point = i;
					}

					if (edgeCache.contains({y, i}) && (grid[y][i] == 'J' || grid[y][i] == '7')) {
						int length = i-last_curve_point+1;
						std::string edge = grid[y].substr(last_curve_point, length);
						if ((edge[0] == 'F' && edge.back() == 'J') || (edge[0] == 'L' && edge.back() == '7')) {
							edge_count++;
						}
					}
				}

				if (edge_count % 2) {
					enclosed_count++;
				}
			}
		}
	}

	std::cout << enclosed_count << std::endl;
	
	return 0;
}
