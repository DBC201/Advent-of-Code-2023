#include <file_utils.h>
#include <string_utils.h>
#include <coordinate_utils.h>
#include <unordered_set>

int main() {
	std::vector<std::string> lines;
	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		lines.push_back(line);
	});

	auto has_adjacent_symbol = [&](int y, int x) {
		for (int dy=-1; dy<=1; dy++) {
			for (int dx=-1; dx<=1; dx++) {
				if (dx == 0 && dy == 0) {
					continue;
				}
				int new_x = x + dx;
				if (new_x < 0 || new_x >= lines[y].length()) {
					continue;
				}

				int new_y = y + dy;
				if (new_y < 0 || new_y >= lines.size()) {
					continue;
				}

				if (!std::isdigit(lines[new_y][new_x]) && lines[new_y][new_x] != '.') {
					return true;
				}

			}
		}
		return false;
	};

	auto num_valid = [&] (int y, int start, int finish){
		for (start; start<=finish; start++) {
			if (has_adjacent_symbol(y, start)) {
				return true;
			}
		}

		return false;
	};

	int result = 0;

	for (int y=0; y<lines.size(); y++) {
		int head = 0;
		int tail = 0;
		for (head; head<=lines[y].size(); head++) {
			if (!std::isdigit(lines[y][head])) {
				int length = head-tail;
				if (length > 0 && num_valid(y, tail, head-1)) {
					result += cpp_utils::string_utils::convert_string<int>(lines[y].substr(tail, length));
				}
				tail = head+1;
			}
		}
	}

	std::cout << result << std::endl;

	auto get_gear_ratio = [&](int y, int x) {

		std::unordered_set<std::string> considered;
		std::vector<int> gears;

		for (int dy=-1; dy<=1; dy++) {
			for (int dx=-1; dx<=1; dx++) {
				if (dx == 0 && dy == 0) {
					continue;
				}
				int new_x = x + dx;
				if (new_x < 0 || new_x >= lines[y].length()) {
					continue;
				}

				int new_y = y + dy;
				if (new_y < 0 || new_y >= lines.size()) {
					continue;
				}

				std::string coord = cpp_utils::coordinate_utils::coordinate_vector_to_string<int>({new_y, new_x});

				if (considered.contains(coord)) {
					continue;
				}

				if (std::isdigit(lines[new_y][new_x])) {
					int tail = new_x;
					int head = new_x;
					bool head_done = false;
					bool tail_done = false;
					while (!tail_done || !head_done) {
						if (tail < 0) {
							tail_done = true;
						}

						if (head >= lines[new_y].length()) {
							head_done = true;
						}
						if (!tail_done) {
							if (std::isdigit(lines[new_y][tail])) {
								considered.insert(cpp_utils::coordinate_utils::coordinate_vector_to_string<int>({new_y, tail}));
								tail--;
							}
							else {
								tail_done = true;
							}
						}

						if (!head_done) {
							if (std::isdigit(lines[new_y][head])) {
								considered.insert(cpp_utils::coordinate_utils::coordinate_vector_to_string<int>({new_y, head}));
								head++;
							}
							else {
								head_done = true;
							}
						}
					}
					int val = cpp_utils::string_utils::convert_string<int>(lines[new_y].substr(tail+1, head-tail-1));
					gears.push_back(val);
					if (gears.size() > 2) {
						return 0;
					}
				}

			}
		}
		if (gears.size() == 2) {
			return gears[0] * gears[1];
		} else {
			return 0;
		}
	};

	uint64_t sum = 0;

	for (int y=0; y<lines.size(); y++) {
		for (int x=0; x<lines[y].length(); x++) {
			if (lines[y][x] != '.' && !std::isdigit(lines[y][x])) {
				uint64_t gear_ratio = get_gear_ratio(y, x);
				sum += gear_ratio;
			}
		}
	}

	std::cout << sum << std::endl;

	return 0;
}
