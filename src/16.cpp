#include <file_utils.h>
#include <coordinate_utils.h>
#include <queue>

using namespace cpp_utils::coordinate_utils::two_dimensional;
using cpp_utils::coordinate_utils::CoordinateCache;

struct Beam {
	direction_t direction;
	std::vector<size_t> coordinate;
};

size_t energized_count(std::vector<std::string> grid, std::vector<size_t> start, direction_t direction) {
	CoordinateCache energizedCoordinates;

	std::queue<Beam*> beams;
	beams.push(new Beam);
	beams.front()->direction = direction;
	beams.front()->coordinate = start;
	
	std::vector<std::vector<std::vector<bool>>> visited(
		grid.size(), std::vector<std::vector<bool>>(
			grid[0].size(), std::vector<bool>(static_cast<size_t>(NUM_DIRECTIONS), false)));

	while (!beams.empty()) {
		Beam* beam = beams.front();
		beams.pop();
		size_t y = beam->coordinate[0];
		size_t x = beam->coordinate[1];
		char obstacle = grid[y][x];

		energizedCoordinates.insert(beam->coordinate);
		visited[y][x][static_cast<size_t>(beam->direction)] = true;

		if (obstacle == '.') {

		}
		else if (obstacle == '/' && (beam->direction == WEST || beam->direction == EAST)) {
			beam->direction = flip_direction(beam->direction, -90);
		}
		else if (obstacle == '/' && (beam->direction == NORTH || beam->direction == SOUTH)) {
			beam->direction = flip_direction(beam->direction, 90);
		}
		else if (obstacle == '\\' && (beam->direction == WEST || beam->direction == EAST)) {
			beam->direction = flip_direction(beam->direction, 90);
		}
		else if (obstacle == '\\' && (beam->direction == NORTH || beam->direction == SOUTH)) {
			beam->direction = flip_direction(beam->direction, -90);
		}
		else if (obstacle == '|' && (beam->direction == EAST || beam->direction == WEST)) {
			beam->direction = NORTH;
			Beam* beam2 = new Beam;
			beam2->coordinate = beam->coordinate;
			beam2->direction = SOUTH;
			beams.push(beam2);
		}
		else if (obstacle == '|' && (beam->direction == NORTH || beam->direction == SOUTH)) {

		}
		else if (obstacle == '-' && (beam->direction == EAST || beam->direction == WEST)) {

		}
		else if (obstacle == '-' && (beam->direction == NORTH || beam->direction == SOUTH)) {
			beam->direction = WEST;
			Beam* beam2 = new Beam;
			beam2->coordinate = beam->coordinate;
			beam2->direction = EAST;
			beams.push(beam2);
		}

		beam->coordinate = cpp_utils::coordinate_utils::two_dimensional::move(beam->coordinate, beam->direction);

		if (in_bounds(beam->coordinate, grid.size(), grid[0].size()) && !visited[beam->coordinate[0]][beam->coordinate[1]][static_cast<size_t>(beam->direction)]) {
			beams.push(beam);
		}
		else {
			delete beam;
		}
	}

	return energizedCoordinates.size();
}

int main() {
	std::vector<std::string> grid;
	cpp_utils::file_utils::FileUtils fileUtils(INPUT_PATH);
	fileUtils.process_lines([&] (std::string line) {
		grid.push_back(line);
	});
	fileUtils.close();

	std::cout << energized_count(grid, {0, 0}, EAST) << std::endl;

	size_t max_val = 0;

	for (size_t y=0; y<grid.size(); y++) {
		max_val = std::max(energized_count(grid, {y, 0}, EAST), max_val);
		max_val = std::max(energized_count(grid, {y, grid[0].size()-1}, WEST), max_val);
	}

	for (size_t x=0; x<grid[0].size(); x++) {
		max_val = std::max(energized_count(grid, {0, x}, SOUTH), max_val);
		max_val = std::max(energized_count(grid, {grid.size()-1, x}, NORTH), max_val);
	}

	std::cout << max_val << std::endl;

	return 0;
}
