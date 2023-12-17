#include <file_utils.h>
#include <string_utils.h>
#include <numeric>
#include <unordered_set>
#include <threading_utils.h>
#include <timer_utils.h>

using cpp_utils::threading_utils::ThreadPool;
using cpp_utils::timer_utils::Timer;

void brute_search (std::string arrangement, std::vector<int> pattern, std::vector<std::string>& result, std::unordered_set<std::string>& cache) {
	if (cache.contains(arrangement)) {
		return;
	}
	else {
		cache.insert(arrangement);
	}
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
		result.push_back(arrangement);
		return;
	}
	else if (total_ampersand > pattern_sum) {
		return;
	}

	int count = 0;

	for (int i=0; i<arrangement.length(); i++) {
		if (arrangement[i] == '?') {
			std::string n = arrangement;
			n[i] = '#';
			brute_search(n, pattern, result, cache);
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

	Timer timer;
	timer.start();
	int sum = 0;
	ThreadPool threadPool;
	int done_count = 0;
	for (int i=0; i<lines.size(); i++) {
		std::unordered_set<std::string> cache;
		std::vector<std::string> results;
		brute_search(lines[i], patterns[i], results, cache);
		std::cout << ++done_count << "/" << lines.size() << std::endl;
		sum += results.size();
		// threadPool.enqueue([&lines, &patterns, i, &threadPool, &done_count, &sum]() {
		// 	std::unordered_set<std::string> cache;
		// 	std::vector<std::string> results;
		// 	brute_search(lines[i], patterns[i], results, cache);
		// 	threadPool.m.lock();
		// 	std::cout << ++done_count << "/" << lines.size() << std::endl;
		// 	sum += results.size();
		// 	threadPool.m.unlock();
		// });
	}

	threadPool.wait();

	std::cout << sum << std::endl;
	timer.stop();

	std::cout << timer.get_miliseconds() << " ms" << std::endl;

	for (int i=0; i<lines.size(); i++) {
		std::vector<int> pattern = patterns[i];
		std::string line = lines[i];
		for (int j=0; j<4; j++) {
			lines[i] += line;
			if (j != 3) {
				lines[i] += "?";
			}
			patterns[i].insert(patterns[i].end(), pattern.begin(), pattern.end()); 
		}
	}

	sum = 0;
	for (int i=0; i<lines.size(); i++) {
		std::unordered_set<std::string> cache;
		std::vector<std::string> results;
		brute_search(lines[i], patterns[i], results, cache);
		std::cout << i+1 << "/" << lines.size() << ": " << results.size() << std::endl;
		sum += results.size();
	}

	std::cout << sum << std::endl;

	return 0;
}
