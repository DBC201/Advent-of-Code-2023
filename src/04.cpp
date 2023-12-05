#include <file_utils.h>
#include <string_utils.h>
#include <unordered_set>
#include <vector_utils.h>
#include <unordered_map>

int main() {
	std::vector<std::vector<int>> numbers;
	std::vector<std::unordered_set<int>> winning_numbers;

	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		auto t = cpp_utils::string_utils::split_by_string(line, ": ");
		auto t2 = cpp_utils::string_utils::split_by_string(t[1], " | ");

		auto num_strs = cpp_utils::string_utils::split_by_string(t2[0], " ");
		auto winning_num_strs = cpp_utils::string_utils::split_by_string(t2[1], " ");

		cpp_utils::vector_utils::remove_element(num_strs, "");
		cpp_utils::vector_utils::remove_element(winning_num_strs, "");

		auto nums = cpp_utils::string_utils::convert_string_vector<int>(num_strs);
		auto winning_nums = cpp_utils::string_utils::convert_string_vector<int>(winning_num_strs);

		numbers.push_back(nums);
		winning_numbers.push_back(std::unordered_set<int>(winning_nums.begin(), winning_nums.end()));
	}); 

	int sum = 0;

	for (int i=0; i<numbers.size(); i++) {
		int curr_sum = 0;
		for (const auto &num: numbers[i]) {
			if (winning_numbers[i].contains(num) && curr_sum == 0) {
				curr_sum = 1;
			}
			else if (winning_numbers[i].contains(num)) {
				curr_sum *= 2;
			}
		}
		sum += curr_sum;
	}

	std::cout << sum << std::endl;
	
	std::vector<uint64_t> instance_counts(numbers.size(), 1);

	for (int i=0; i<numbers.size(); i++) {
		int winning_count = 0;
		for (const auto &num: numbers[i]) {
			if (winning_numbers[i].contains(num)) {
				winning_count++;
			}
		}

		for (int j=1; j<=winning_count; j++) {
			instance_counts[i+j] += instance_counts[i];
		}
	}

	uint64_t sum2 = 0;

	for (const auto &val: instance_counts) {
		sum2 += val;
	}

	std::cout << sum2 << std::endl;

	return 0;
}
