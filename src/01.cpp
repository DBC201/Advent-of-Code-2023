#include <iostream>
#include <file_utils.h>
#include <string_utils.h>
#include <unordered_map>
#include <unordered_set>

int main() {
	uint64_t sum = 0;
	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		bool first_found = false;
		bool last_found = false;
		uint16_t first;
		uint16_t last;
		for (int i=0; i<line.length(); i++) {
			if (std::isdigit(line[i]) && !first_found) {
				first = line[i] - '0';
				first_found = true;
			}

			if (std::isdigit(line[line.length() - i - 1]) && !last_found) {
				last = line[line.length() - i - 1] - '0';
				last_found = true;
			}

			if (first_found && last_found) {
				break;
			}
		}
		
		sum += first*10 + last;
	});

	std::cout << sum << std::endl;

	sum = 0;
	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		bool first_found = false;
		bool second_found = false;
		uint16_t first;
		uint16_t last;

		std::unordered_map<std::string, uint16_t> digits = {
			{"one", 1},
			{"two", 2},
			{"three", 3},
			{"four", 4},
			{"five", 5},
			{"six", 6},
			{"seven", 7},
			{"eight", 8},
			{"nine", 9}
		};

		std::unordered_map<std::string, uint16_t> reversed_digits;

		for (const auto &p: digits) {
			std::string reversed = p.first;
			std::reverse(reversed.begin(), reversed.end());
			reversed_digits.insert({reversed, p.second});
		}

		std::vector<std::unordered_set<std::string>> letter_cache(5, std::unordered_set<std::string>());
		std::vector<std::unordered_set<std::string>> end_letter_cache(5, std::unordered_set<std::string>());

		for (const auto &p: digits) {
			for (int i=1; i<=5; i++) {
				if (p.first.length() >= i) {
					letter_cache[i-1].insert(p.first.substr(0, i));
					std::string end = p.first.substr(p.first.length()-i, i);
					std::reverse(end.begin(), end.end());
					end_letter_cache[i-1].insert(end);
				}
			}
		}

		std::string forward_window;
		std::string backward_window;

		for (int i=0; i<line.length(); i++) {
			forward_window += line[i];

			if (!first_found && std::isdigit(line[i])) {
				first_found = true;
				first = line[i] - '0';
			}
			
			if (!first_found && !letter_cache[forward_window.length() - 1].contains(forward_window)) {
				do {
					forward_window.erase(0, 1);
				} while (forward_window.length() 
							&& !letter_cache[forward_window.length()-1].contains(forward_window));
			}

			if (!first_found && digits.contains(forward_window)) {
				first_found = true;
				first = digits.at(forward_window);
			}
			
			backward_window += line[line.length() - i - 1];

			if (!second_found && std::isdigit(line[line.length() - i - 1])) {
				second_found = true;
				last = line[line.length() - i - 1] - '0';
			}
			
			if (!second_found && !end_letter_cache[backward_window.length() - 1].contains(backward_window)) {
				do {
					backward_window.erase(0, 1);
				} while (backward_window.length() 
							&& !end_letter_cache[backward_window.length()-1].contains(backward_window));
			}

			if (!second_found && reversed_digits.contains(backward_window)) {
				second_found = true;
				last = reversed_digits.at(backward_window);
			}

			if (first_found && second_found) {
				break;
			}
		}

		sum += first*10 + last;
	});

	std::cout << sum << std::endl;
	
	return 0;
}
