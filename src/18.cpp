#include <file_utils.h>
#include <string_utils.h>
#include <coordinate_utils.h>
#include <queue>

using cpp_utils::coordinate_utils::CoordinateCache;
using cpp_utils::coordinate_utils::two_dimensional::NORTH;
using cpp_utils::coordinate_utils::two_dimensional::SOUTH;
using cpp_utils::coordinate_utils::two_dimensional::EAST;
using cpp_utils::coordinate_utils::two_dimensional::WEST;
using cpp_utils::coordinate_utils::two_dimensional::direction_t;
using cpp_utils::coordinate_utils::two_dimensional::move;
using cpp_utils::coordinate_utils::two_dimensional::in_bounds;

/*
The digger starts in a 1 meter cube hole in the ground. 
They then dig the specified number of meters up (U), down (D), left (L), or right (R), clearing full 1 meter cubes as they go.
The directions are given as seen from above, so if "up" were north, then "right" would be east, and so on.
Each trench is also listed with the color that the edge of the trench should be painted as an RGB hexadecimal color code.
*/

void fill(CoordinateCache<int>& edgeCoordinates, CoordinateCache<int>& outside, std::vector<int> start, int y_max, int x_max, int y_min, int x_min) {
	if (outside.contains(start) || edgeCoordinates.contains(start)) {
		return;
	}
	std::queue<std::vector<int>> q;
	q.push(start);

	while (!q.empty()) {
		std::vector<int> curr = q.front();
		q.pop();
		
		std::vector<int> next = move<int>(curr, NORTH);
		if (in_bounds(next, y_max, x_max, y_min, x_min) && !outside.contains(next) && !edgeCoordinates.contains(next)) {
			q.push(next);
			outside.insert(next);
		}

		next = move<int>(curr, SOUTH);
		if (in_bounds(next, y_max, x_max, y_min, x_min) && !outside.contains(next) && !edgeCoordinates.contains(next)) {
			q.push(next);
			outside.insert(next);
		}

		next = move<int>(curr, EAST);
		if (in_bounds(next, y_max, x_max, y_min, x_min) && !outside.contains(next) && !edgeCoordinates.contains(next)) {
			q.push(next);
			outside.insert(next);
		}

		next = move<int>(curr, WEST);
		if (in_bounds(next, y_max, x_max, y_min, x_min) && !outside.contains(next) && !edgeCoordinates.contains(next)) {
			q.push(next);
			outside.insert(next);
		}
	}
}

direction_t get_direction(std::string dir) {
	if (dir == "U") {
		return NORTH;
	}
	else if (dir == "D") {
		return SOUTH;
	}
	else if (dir == "L") {
		return WEST;
	}
	else if (dir == "R") {
		return EAST;
	}
	else {
		throw std::runtime_error("wrong direction");
	}
}

direction_t get_direction2(char dir) {
	switch(dir) {
		case '0':
			return EAST;
		case '1':
			return SOUTH;
		case '2':
			return WEST;
		case '3':
			return NORTH;
		default:
			throw std::runtime_error("wrong direction");
	}
}

int64_t calculate_area(std::vector<std::vector<int64_t>> edges) {
	int64_t x_sum = 0, y_sum = 0;
	for (int i=0; i<edges.size(); i++) {
		x_sum += edges[i][1] * edges[(i+1)%edges.size()][0];
		y_sum += edges[i][0] * edges[(i+1)%edges.size()][1];	
	}

	return abs(x_sum - y_sum)/2;
}

int main() {
	std::vector<std::vector<std::string>> tokens;
	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		tokens.push_back(cpp_utils::string_utils::split_by_string(line, " "));
	});

	CoordinateCache<int> edgeCoordinates;
	std::vector<std::vector<int64_t>> v;
	std::vector<int> current_coordinate = {0, 0};
	int min_y = INT_MAX;
	int min_x = INT_MAX;
	int max_y = INT_MIN;
	int max_x = INT_MIN;
	for (const auto& token: tokens) {
		std::string direction = token[0];
		int disposition = cpp_utils::string_utils::convert_string<int>(token[1]);
		for (int i=1; i<=disposition; i++) {
			current_coordinate = cpp_utils::coordinate_utils::two_dimensional::move<int>(current_coordinate, get_direction(direction));
			edgeCoordinates.insert(current_coordinate);
			min_y = std::min(min_y, current_coordinate[0]);
			max_y = std::max(max_y, current_coordinate[0]);
			min_x = std::min(min_x, current_coordinate[1]);
			max_x = std::max(max_x, current_coordinate[1]);
		}
	}

	CoordinateCache<int> outside;

	for (int y=min_y; y<=max_y; y++) {
		fill(edgeCoordinates, outside, {y, min_x}, max_y+1, max_x+1, min_y, min_x);
		fill(edgeCoordinates, outside, {y, max_x}, max_y+1, max_x+1, min_y, min_x);
	}

	for (int x=min_x; x<=max_x; x++) {
		fill(edgeCoordinates, outside, {max_y, x}, max_y+1, max_x+1, min_y, min_x);
		fill(edgeCoordinates, outside, {min_y, x}, max_y+1, max_x+1, min_y, min_x);
	}

	int area = (max_y+1-min_y) * (max_x+1-min_x);

	std::cout << area - outside.size() << std::endl;

	std::vector<std::vector<int64_t>> edge_points;
	std::vector<int64_t> current_coordinate2 = {0, 0};
	int64_t perimeter = 0;
	for (const auto& token: tokens) {
		std::string raw = token[2].substr(2, token[2].length()-3);
		int64_t disposition = cpp_utils::string_utils::convert_hexadecimal<int64_t>(raw.substr(0, 5));
		current_coordinate2 = move<int64_t>(current_coordinate2, get_direction2(raw.back()), disposition);
		edge_points.push_back(current_coordinate2);
		perimeter += disposition;
	}

	std::cout << calculate_area(edge_points) + perimeter/2 + 1 << std::endl;

	return 0;
}
