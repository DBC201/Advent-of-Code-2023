#include <file_utils.h>
#include <unordered_set>
#include <unordered_map>

using cpp_utils::file_utils::FileUtils;

uint64_t calculate_distance (std::vector<uint64_t> g1, 
						std::vector<uint64_t> g2, 
						std::unordered_set<uint64_t>& non_empty_rows, 
						std::unordered_set<uint64_t>& non_empty_columns,
						uint64_t expansion_distance) {
	uint64_t dist = 0;
	uint64_t min_y = std::min(g1[0], g2[0]);
	uint64_t max_y = std::max(g1[0], g2[0]);
	uint64_t min_x = std::min(g1[1], g2[1]);
	uint64_t max_x = std::max(g1[1], g2[1]);
	

	for (uint64_t x=min_x; x<max_x; x++) {
		if (!non_empty_columns.contains(x)) {
			dist += expansion_distance;
		}
		else {
			dist++;
		}
	}

	for (uint64_t y=min_y; y<max_y; y++) {
		if (!non_empty_rows.contains(y)) {
			dist += expansion_distance;
		}
		else {
			dist++;
		}
	}

	return dist;
}

int main() {
	FileUtils fileUtils(INPUT_PATH);

	std::vector<std::string> grid;
	fileUtils.process_lines([&](std::string line) {
		grid.push_back(line);
	});

	fileUtils.close();

	std::vector<std::vector<uint64_t>> galaxies;
	std::unordered_set<uint64_t> non_empty_rows;
	std::unordered_set<uint64_t> non_empty_columns;

	for (uint64_t y=0; y<grid.size(); y++) {
		for (uint64_t x=0; x<grid[y].size(); x++) {
			if (grid[y][x] == '#') {
				non_empty_columns.insert(x);
				non_empty_rows.insert(y);
				galaxies.push_back({y, x});
			}
		}
	}

	std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>> distances;

	uint64_t sum = 0;

	for (uint64_t g1=0; g1<galaxies.size(); g1++) {
		for (uint64_t g2=g1+1; g2<galaxies.size(); g2++) {
			if (distances.contains(g1) && distances.at(g1).contains(g2)) {
				continue;
			}
			
			if (!distances.contains(g1)) {
				distances.insert({g1, {}});
			}

			if (!distances.contains(g2)) {
				distances.insert({g2, {}});
			}

			uint64_t dist = calculate_distance(galaxies[g1], galaxies[g2], non_empty_rows, non_empty_columns, 2);
			
			distances.at(g1).insert({g2, dist});
			distances.at(g2).insert({g1, dist});

			sum += dist;
		}
	}

	std::cout << sum << std::endl;

	distances.clear();

	sum = 0;

	for (uint64_t g1=0; g1<galaxies.size(); g1++) {
		for (uint64_t g2=g1+1; g2<galaxies.size(); g2++) {
			if (distances.contains(g1) && distances.at(g1).contains(g2)) {
				continue;
			}
			
			if (!distances.contains(g1)) {
				distances.insert({g1, {}});
			}

			if (!distances.contains(g2)) {
				distances.insert({g2, {}});
			}

			uint64_t dist = calculate_distance(galaxies[g1], galaxies[g2], non_empty_rows, non_empty_columns, 1000000);
			
			distances.at(g1).insert({g2, dist});
			distances.at(g2).insert({g1, dist});

			sum += dist;
		}
	}

	std::cout << sum << std::endl;

	return 0;
}
