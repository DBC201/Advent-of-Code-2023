#include <file_utils.h>

using cpp_utils::file_utils::FileUtils;

std::vector<std::string> transpose(std::vector<std::string> &v) {
	std::vector<std::string> result(v[0].size(), std::string(v.size(), '0'));

	for (int y=0; y<v.size(); y++) {
		for (int x=0; x<v[y].length(); x++) {
			result[x][y] = v[y][x];
		}
	}

	return result;
}

int get_horizontal_line(std::vector<std::string>& grid) {
	for (int i=1; i<grid.size(); i++) {
		if (grid[i] == grid[i-1]) {
			int j, k;
			for (j=i-2; j >= 0; j--) {
				k = i + i-j-1;
				if (k >= grid.size()) {
					return i;
				}
				if (grid[j] != grid[k]) {
					break;
				}
			}

			if (j == -1) {
				return i;
			}
		}
	}

	return 0;
}

bool fixable(std::string a, std::string b) {
	if (a.length() != b.length()) {
		return false;
	}
	int diff_count = 0;
	for (int i=0; i<a.length(); i++) {
		if (a[i] != b[i]) {
			diff_count++;
		}

		if (diff_count > 1) {
			return false;
		}
	}

	return diff_count == 1;
}

int get_horizontal_line2(std::vector<std::string>& grid) {
	for (int i=1; i<grid.size(); i++) {
		if (grid[i] == grid[i-1] || fixable(grid[i], grid[i-1])) {
			int j, k;
			int fix_count = fixable(grid[i], grid[i-1]) ? 1 : 0;
			for (j=i-2; j >= 0; j--) {
				k = i + i-j-1;
				if (k >= grid.size()) {
					break;
				}

				if (grid[j] == grid[k]) {
					continue;
				}

				if (grid[j] != grid[k] && fix_count > 1) {
					break;
				}

				bool is_fixable = fixable(grid[j], grid[k]);

				if (!is_fixable) {
					break;
				}

				fix_count++;
			}

			if ((j == -1 || k >= grid.size()) && fix_count == 1) {
				return i;
			}
		}
	}

	return 0;
}

int main() {
	FileUtils fileUtils(INPUT_PATH);

	std::vector<std::vector<std::string>> grids;

	grids.push_back({});

	fileUtils.process_lines([&](std::string line) {
		if (line.empty()) {
			grids.push_back({});
		}
		else {
			grids.back().push_back(line);
		}
	});
	fileUtils.close();
	int sum = 0;
	for (int i=0; i<grids.size(); i++) {
		int c = sum;
		auto t = transpose(grids[i]);
		int left_of_vertical_count = get_horizontal_line(t);
		int above_horizontal_count = get_horizontal_line(grids[i]);
		if (left_of_vertical_count == 0) {
			sum += above_horizontal_count * 100;
		}
		else {
			sum += left_of_vertical_count;
		}
	}

	std::cout << sum << std::endl;

	sum = 0;

	for (int i=0; i<grids.size(); i++) {
		int c = sum;
		auto t = transpose(grids[i]);
		int left_of_vertical_count = get_horizontal_line2(t);
		int above_horizontal_count = get_horizontal_line2(grids[i]);
		if (left_of_vertical_count == 0) {
			sum += above_horizontal_count * 100;
		}
		else {
			sum += left_of_vertical_count;
		}
	}

	std::cout << sum << std::endl;

	return 0;
}
