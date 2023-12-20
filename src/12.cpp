#include <file_utils.h>
#include <string_utils.h>
#include <numeric>
#include <unordered_set>

void brute_search (std::string arrangement, std::vector<int> pattern, int &count, int index) {
	int pattern_sum = std::accumulate(pattern.begin(), pattern.end(), 0);
	int head = 0, tail = 0;
	int pattern_index = 0;
	bool match = true;
	int total_ampersand = 0;
	while(head < arrangement.length()) {
		if (arrangement[head] != '#') {
			int length = head-tail;
			if (length && pattern_index >= pattern.size()) {
				match = false;
			}
			if (length && match) {
				if (pattern[pattern_index] != length) {
					match = false;
				}
				pattern_index++;
			}
			tail = head+1;
		}
		else if (arrangement[head] == '#') {
			total_ampersand++;
		}
		head++;
	}

	if (tail != head) {
		int length = head-tail;
		if (length && pattern_index >= pattern.size()) {
			match = false;
		}
		if (length && match) {
			if (pattern[pattern_index] != length) {
				match = false;
			}
			pattern_index++;
		}
		tail = head+1;
	}
	
	if (match) {
		match = pattern_index == pattern.size();
	}

	if (total_ampersand == pattern_sum && match) {
		count++;
		return;
	}
	else if (total_ampersand > pattern_sum || index >= arrangement.length()) {
		return;
	}

	for (int i=index; i<arrangement.length(); i++) {
		if (arrangement[i] == '?') {
			std::string n = arrangement;
			n[i] = '#';
			brute_search(n, pattern, count, i+1);
		}
	}
}

int main() {
	std::vector<std::string> lines;
	std::vector<std::vector<int>> patterns;

	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		auto t = cpp_utils::string_utils::split_by_string(line, " ");
		lines.push_back(t[0]);
		auto string_pattern = cpp_utils::string_utils::split_by_string(t[1], ",");
		patterns.push_back(cpp_utils::string_utils::convert_string_vector<int>(string_pattern));
	});

	int sum = 0;
	for (int i=0; i<lines.size(); i++) {
		brute_search(lines[i], patterns[i], sum, 0);
		std::cout << i+1 << "/" << lines.size() << std::endl;
	}

	std::cout << sum << std::endl;

	for (int i=0; i<lines.size(); i++) {
		std::vector<int> pattern = patterns[i];
		std::string line = lines[i];
		for (int j=0; j<4; j++) {
			lines[i] += "?";
			lines[i] += line;
			patterns[i].insert(patterns[i].end(), pattern.begin(), pattern.end()); 
		}
	}

	sum = 0;
	for (int i=0; i<lines.size(); i++) {
		brute_search(lines[i], patterns[i], sum, 0);
		std::cout << i+1 << "/" << lines.size() << std::endl;
	}

	std::cout << sum << std::endl;

	return 0;
}
