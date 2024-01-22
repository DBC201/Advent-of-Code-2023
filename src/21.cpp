#include <file_utils.h>
#include <coordinate_utils.h>
#include <queue>

using cpp_utils::coordinate_utils::CoordinateCache;
using cpp_utils::coordinate_utils::two_dimensional::NORTH;
using cpp_utils::coordinate_utils::two_dimensional::SOUTH;
using cpp_utils::coordinate_utils::two_dimensional::EAST;
using cpp_utils::coordinate_utils::two_dimensional::WEST;


struct Tile {
	int64_t y;
	int64_t x;
	uint64_t step_count;

	Tile(int64_t y, int64_t x, uint64_t step_count) {
		this->y = y;
		this->x = x;
		this->step_count = step_count;
	}
};

uint64_t get_plot_count(std::vector<std::string>& grid, int64_t start_y, int64_t start_x, uint64_t max_step_count) {
	std::queue<Tile> q;
	CoordinateCache<int64_t> visited;

	uint64_t result = 0;

	Tile start(start_y, start_x, 0);
	q.push(start);

	while (!q.empty()) {
		Tile curr = q.front();
		q.pop();
		if (curr.step_count % 2 == 0 && curr.step_count > 0) {
			result++;
		}
		std::vector<int64_t> up = cpp_utils::coordinate_utils::two_dimensional::move<int64_t>({curr.y, curr.x}, NORTH);
		std::vector<int64_t> down = cpp_utils::coordinate_utils::two_dimensional::move<int64_t>({curr.y, curr.x}, SOUTH);
		std::vector<int64_t> left = cpp_utils::coordinate_utils::two_dimensional::move<int64_t>({curr.y, curr.x}, EAST);
		std::vector<int64_t> right = cpp_utils::coordinate_utils::two_dimensional::move<int64_t>({curr.y, curr.x}, WEST);

		std::vector<std::vector<int64_t>> next_possible_coordinates = {up, down, left, right};

		for (const auto& next_possible_coordinate: next_possible_coordinates) {
			if (cpp_utils::coordinate_utils::two_dimensional::in_bounds<int64_t>(next_possible_coordinate, grid.size(), grid[0].size()) 
					&& grid[next_possible_coordinate[0]][next_possible_coordinate[1]] != '#'
					&& !visited.contains(next_possible_coordinate)
					&& curr.step_count < max_step_count) {
				Tile next(next_possible_coordinate[0], next_possible_coordinate[1], curr.step_count+1);
				visited.insert(next_possible_coordinate);
				q.push(next);
			}
		}
	}

	return result;
}

int main() {
	std::vector<std::string> grid;

	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		grid.push_back(line);
	});
	
	int64_t start_y;
	int64_t start_x;

	for (int y=0; y<grid.size(); y++) {
		for (int x=0; x<grid[y].size(); x++) {
			if (grid[y][x] == 'S') {
				start_y = y;
				start_x = x;
				break;
			}
		}
	}

	std::cout << get_plot_count(grid, start_y, start_x, 64) << std::endl;

	return 0;
}
