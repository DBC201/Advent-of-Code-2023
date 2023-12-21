#include <file_utils.h>
#include <coordinate_utils.h>
#include <unordered_map>

using cpp_utils::file_utils::FileUtils;
using cpp_utils::coordinate_utils::CoordinateCache;
using cpp_utils::string_utils::vector_to_string;

enum direction_t {
	NORTH,
	WEST,
	SOUTH,
	EAST,
	NUM_DIRECTIONS
};

void sort(auto& v, direction_t direction) {
	switch(direction) {
	case NORTH:
		std::sort(v.begin(), v.end(), [](auto a, auto b) {
			return a[0] < b[0];
		});
		break;
	case WEST:
		std::sort(v.begin(), v.end(), [](auto a, auto b) {
			return a[1] < b[1];
		});
		break;
	case SOUTH:
		std::sort(v.begin(), v.end(), [](auto a, auto b) {
			return a[0] > b[0];
		});
		break;
	case EAST:
		std::sort(v.begin(), v.end(), [](auto a, auto b) {
			return a[1] > b[1];
		});
		break;
	}
}

std::vector<int> move(std::vector<std::string>& grid, std::vector<int> coordinate, direction_t direction) {
	while (1) {
		int new_y = coordinate[0];
		int new_x = coordinate[1];

		switch(direction) {
		case NORTH:
			new_y--;
			break;
		case WEST:
			new_x--;
			break;
		case SOUTH:
			new_y++;
			break;
		case EAST:
			new_x++;
			break;
		}

		if (new_y < 0 || new_y >= grid.size() || new_x < 0 || new_x >= grid[new_y].size() || grid[new_y][new_x] != '.') {
			return coordinate;
		}
		grid[coordinate[0]][coordinate[1]] = '.';
		grid[new_y][new_x] = 'O';
		coordinate[0] = new_y;
		coordinate[1] = new_x;
	}
}

void tilt(std::vector<std::string>& grid, CoordinateCache<int>& rockCoordinateCache, direction_t direction) {
	std::vector<std::vector<int>> coordinates = rockCoordinateCache.get_coordinate_vectors();
	sort(coordinates, direction);
	rockCoordinateCache.clear();
	for (int i=0; i<coordinates.size(); i++) {
		auto pos = move(grid, coordinates[i], direction);
		rockCoordinateCache.insert(pos);
	}
}

void cycle(std::vector<std::string>& grid, CoordinateCache<int>& rockCoordinateCache) {
	tilt(grid, rockCoordinateCache, NORTH);
	tilt(grid, rockCoordinateCache, WEST);
	tilt(grid, rockCoordinateCache, SOUTH);
	tilt(grid, rockCoordinateCache, EAST);
}

std::vector<int> move_north(std::vector<std::string>& grid, std::vector<int> coordinates) {
	while (coordinates[0] > 0) {
		int new_y = coordinates[0] - 1;
		if (grid[new_y][coordinates[1]] != '.') {
			break;
		}
		grid[coordinates[0]][coordinates[1]] = '.';
		coordinates[0] = new_y;
		grid[coordinates[0]][coordinates[1]] = 'O';
	}
	return coordinates;
}

int main() {
	std::vector<std::string> grid;
	FileUtils fileUtils(INPUT_PATH);

	fileUtils.process_lines([&](std::string line) {
		grid.push_back(line);
	});

	fileUtils.close();

	int sum = 0;
	

	CoordinateCache<int> rockCoordinateCache;
	std::vector<std::string> p1_grid = grid;

	for (int y=0; y<p1_grid.size(); y++) {
		for (int x=0; x<p1_grid.size(); x++) {
			if (p1_grid[y][x] == 'O') {
				rockCoordinateCache.insert({y, x});
				std::vector<int> coords = move_north(p1_grid, {y, x});
				sum += p1_grid.size() - coords[0];
			}
		}
	}

	std::cout << sum << std::endl;
	
	std::unordered_map<std::string, int> states;
	std::string state;
	uint64_t i;
	uint64_t cycle_count = 1000000000;

	for (i=1; i<=cycle_count; i++) {
		cycle(grid, rockCoordinateCache);
		state = vector_to_string(grid);
		if (states.contains(state)) {
			break;
		}
		states.insert({state, i});
	}

	uint64_t start = states.at(state);
	uint64_t remaining_loop_count = (cycle_count-i) % (i-start);

	for (i=0; i<remaining_loop_count; i++) {
		cycle(grid, rockCoordinateCache);
	}

	uint64_t sum2 = 0;
	for (const auto& coordinate: rockCoordinateCache.get_coordinate_vectors()) {
		sum2 += grid.size() - coordinate[0];
	}
	
	std::cout << sum2 << std::endl;

	return 0;
}
