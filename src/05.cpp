#include <file_utils.h>
#include <string_utils.h>
#include <unordered_map>

int main() {
	std::vector<std::vector<std::vector<uint64_t>>> mappings(7, std::vector<std::vector<uint64_t>>());
	std::vector<uint64_t> seeds;
	std::string source, destination;
	int mapping_index = -1;

	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		if (line.empty()) {
			return;
		}
		else if (line.substr(0, 7) == "seeds: ") {
			auto seed_strs = cpp_utils::string_utils::split_by_string(line.substr(7), " ");
			seeds = cpp_utils::string_utils::convert_string_vector<uint64_t>(seed_strs);
		} else if (line.substr(line.length()-5, 5) == " map:") {
			if (mapping_index >= 0) {
				std::sort(mappings[mapping_index].begin(), mappings[mapping_index].end(), [](auto a, auto b) {
					return a[0] < b[0];
				});
			}
 			mapping_index++;
			auto t = cpp_utils::string_utils::split_by_string(line.substr(0, line.length()-5), "-");
			source = t[0];
			destination = t[2];
		}
		else if (std::isdigit(line[0])) {
			auto num_strs = cpp_utils::string_utils::split_by_string(line, " ");
			auto nums = cpp_utils::string_utils::convert_string_vector<uint64_t>(num_strs);
			uint64_t destination_start = nums[0];
			uint64_t source_start = nums[1];
			uint64_t range = nums[2];
			mappings[mapping_index].push_back({source_start, destination_start, range});
		}
	});

	std::sort(mappings[mapping_index].begin(), mappings[mapping_index].end(),
		[](auto a, auto b) {
			return a[0] < b[0];
		}
	);

	std::vector<std::vector<uint64_t>> correspondances(8, std::vector<uint64_t>(seeds.size(), 0));

	correspondances[0] = seeds;	
	
	for (int i=0; i<correspondances.size()-1; i++) {
		for (int j=0; j<correspondances[i].size(); j++) {
			int k;
			for (k=0; k<mappings[i].size(); k++) {
				if (mappings[i][k][0] > correspondances[i][j]) {
					break;
				}
			}

			if (k == 0) {
				correspondances[i+1][j] = correspondances[i][j]; 
			}
			else {
				uint64_t diff = correspondances[i][j] - mappings[i][k-1][0];
				if (diff >= mappings[i][k-1][2]) {
					correspondances[i+1][j] = correspondances[i][j]; 
				}
				else {
					correspondances[i+1][j] = mappings[i][k-1][1] + diff;
				}
			}
		}
	}

	std::sort(correspondances[correspondances.size()-1].begin(), correspondances[correspondances.size()-1].end(), [](auto a, auto b) {
		return a < b;
	});

	std::cout << correspondances[correspondances.size() - 1][0] << std::endl;

	std::vector<std::vector<std::vector<uint64_t>>> correspondance_ranges(8);

	for (int i=0; i<seeds.size(); i+=2) {
		correspondance_ranges[0].push_back({seeds[i], seeds[i] + seeds[i+1] - 1});
	}

	std::sort(correspondance_ranges[0].begin(), correspondance_ranges[0].end(), [](auto a, auto b) {
		return a[0] < b[0];
	});

	for (int i=0; i<correspondance_ranges.size()-1; i++) {
		int correspondance_index = 0;
		int mapping_index = 0;

		while (correspondance_index < correspondance_ranges[i].size() && mapping_index < mappings[i].size()) {
			uint64_t mapping_start = mappings[i][mapping_index][0];
			uint64_t mapping_end = mapping_start + mappings[i][mapping_index][2] - 1;

			uint64_t curr_start = correspondance_ranges[i][correspondance_index][0];
			uint64_t curr_end = correspondance_ranges[i][correspondance_index][1];

			if (mapping_end < curr_start) {
				mapping_index++;
				continue;
			}

			// final_start = max(mapping_start, curr_start)
			// final_end = min(mapping_end, curr_end)
			// if mapping_end > curr_end => correspondance_index++
			// if curr_end > mapping_end => mapping_index++
			// if mapping_end == curr_end => mapping_index++, correspondance_index++

			uint64_t final_start = std::max(mapping_start, curr_start);
			uint64_t final_end = std::min(mapping_end, curr_end);

			uint64_t range_start = final_start - mapping_start + 1;
			uint64_t range_end = final_end - mapping_start + 1;

			uint64_t correspondance_start = mappings[i][mapping_index][1] + range_start - 1;
			uint64_t correspondance_end = mappings[i][mapping_index][1] + range_end - 1;

			correspondance_ranges[i+1].push_back({correspondance_start, correspondance_end});

			if (curr_start < mapping_start) {
				correspondance_ranges[i+1].push_back({curr_start, mapping_start-1});
			}

			if (curr_end > mapping_end) {
				correspondance_ranges[i][correspondance_index][0] = mapping_end+1;
				mapping_index++;
			}
			else if (curr_end < mapping_end) {
				correspondance_index++;
			}
			else {
				correspondance_index++;
				mapping_index++;
			}
		}
		
		for (correspondance_index; correspondance_index < correspondance_ranges[i].size(); correspondance_index++) {
			correspondance_ranges[i+1].push_back(correspondance_ranges[i][correspondance_index]);
		}

		std::sort(correspondance_ranges[i+1].begin(), correspondance_ranges[i+1].end(), [](auto a, auto b) {
			return a[0] < b[0];
		});
	}

	int last_index = correspondance_ranges.size() - 1;

	std::cout << correspondance_ranges[last_index][0][0] << std::endl;

	return 0;
}
