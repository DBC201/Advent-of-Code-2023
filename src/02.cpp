#include <file_utils.h>
#include <string_utils.h>
#include <unordered_map>

int main() {
	std::unordered_map<std::string, uint32_t> cube_counts = {
		{"red", 12},
		{"green", 13},
		{"blue", 14}
	};
	uint32_t sum = 0;
	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		auto t = cpp_utils::string_utils::split_by_string(line, ": ");
		uint32_t game_num = cpp_utils::string_utils::convert_string<uint32_t>(t[0].substr(5));
		auto sets = cpp_utils::string_utils::split_by_string(t[1], "; ");

		for (const auto &set: sets) {
			auto reveals = cpp_utils::string_utils::split_by_string(set, ", ");
			for (const auto &reveal: reveals) {
				auto tokens = cpp_utils::string_utils::split_by_string(reveal, " ");
				uint32_t current_cube_count = cpp_utils::string_utils::convert_string<uint32_t>(tokens[0]);
				if (current_cube_count > cube_counts.at(tokens[1])) {
					return;
				}
			}
		}
		sum += game_num;
	});

	std::cout << sum << std::endl;

	uint64_t power_sum = 0;

	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		auto t = cpp_utils::string_utils::split_by_string(line, ": ");
		uint32_t game_num = cpp_utils::string_utils::convert_string<uint32_t>(t[0].substr(5));
		auto sets = cpp_utils::string_utils::split_by_string(t[1], "; ");

		std::unordered_map<std::string, uint64_t> max_cube_counts = {
			{"red", 0},
			{"green", 0},
			{"blue", 0}
		};

		for (const auto &set: sets) {
			auto reveals = cpp_utils::string_utils::split_by_string(set, ", ");
			for (const auto &reveal: reveals) {
				auto tokens = cpp_utils::string_utils::split_by_string(reveal, " ");
				uint64_t current_cube_count = cpp_utils::string_utils::convert_string<uint64_t>(tokens[0]);
				max_cube_counts.at(tokens[1]) = std::max(max_cube_counts.at(tokens[1]), current_cube_count);
			}
		}

		uint64_t power = 1;
		
		for (const auto &p: max_cube_counts) {
			power *= p.second;
		}

		power_sum += power;
	});

	std::cout << power_sum << std::endl;

	return 0;
}

