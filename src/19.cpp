#include <file_utils.h>
#include <string_utils.h>
#include <unordered_map>
#include <queue>

bool valid(int rating, std::string raw_expected) {
	char comparator = raw_expected[1];
	int expected_rating = cpp_utils::string_utils::convert_string<int>(raw_expected.substr(2));
	switch (comparator)
	{
		case '>':
			return rating > expected_rating;
		case '<':
			return rating < expected_rating;
		default:
			throw std::runtime_error("unrecognized comparator");
	};
}

std::string get_next(std::vector<std::pair<std::string, std::string>> workflow,
					 std::vector<std::pair<char, int>> part) {
	for (const auto& subworkflow: workflow) {
		if (subworkflow.first.empty()) {
			break;
		}
		for (const auto& subpart: part) {
			if (subworkflow.first[0] == subpart.first && valid(subpart.second, subworkflow.first)) {
				return subworkflow.second;
			}
		}
	}
	return workflow.back().second;
}

int main() {
	std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> workflows;
	std::vector<std::vector<std::pair<char, int>>> parts;
	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		if (line.empty()) {
			return;
		}
		else if (line[0] == '{') {
			auto raw_parts = cpp_utils::string_utils::split_by_string(line.substr(1, line.length()-2), ",");
			std::vector<std::pair<char, int>> curr_parts;
			for (auto& raw_part: raw_parts) {
				auto tokens = cpp_utils::string_utils::split_by_string(raw_part, "=");
				char part_name = tokens[0][0];
				int part_rating = cpp_utils::string_utils::convert_string<int>(tokens[1]);
				curr_parts.push_back({part_name, part_rating});
			}
			parts.push_back(curr_parts);
		}
		else {
			auto tokens = cpp_utils::string_utils::split_by_string(line, "{");
			std::string workflow_name = tokens[0];
			auto raw_rules = cpp_utils::string_utils::split_by_string(tokens[1].substr(0, tokens[1].length()-1), ",");
			std::vector<std::pair<std::string, std::string>> workflow;
			for (int i=0; i<raw_rules.size()-1; i++) {
				auto raw_rule = cpp_utils::string_utils::split_by_string(raw_rules[i], ":");
				std::string ret_val = raw_rule[1];
				workflow.push_back({raw_rule[0], raw_rule[1]});
			}
			workflow.push_back({"", raw_rules.back()});
			workflows.insert({workflow_name, workflow});
		}
	});

	uint64_t sum = 0;

	for (auto& part: parts) {
		std::string next = get_next(workflows.at("in"), part);
		while (next != "A" && next != "R") {
			next = get_next(workflows.at(next), part);
		}
		if (next == "A") {
			for (const auto& subpart: part) {
				sum += subpart.second;
			}
		}
	}

	std::cout << sum << std::endl;

	struct Node {
		std::vector<std::pair<char, std::pair<uint64_t, uint64_t>>> rating_intervals;
		std::string curr_workflow;

		Node() {
			rating_intervals.push_back({'x', {1, 4000}});
			rating_intervals.push_back({'m', {1, 4000}});
			rating_intervals.push_back({'a', {1, 4000}});
			rating_intervals.push_back({'s', {1, 4000}});
		}
	};

	auto get_combination = [](Node node) {
		auto rating_intervals = node.rating_intervals;
		uint64_t result = 1;
		for (const auto& interval: rating_intervals) {
			result *= interval.second.second-interval.second.first + 1;
		}
		return result;
	};

	std::queue<Node> q;
	Node start;

	start.curr_workflow = "in";

	q.push(start);

	sum = 0;

	while (!q.empty()) {
		Node curr = q.front();
		q.pop();
		std::vector<std::pair<std::string, std::string>> workflow = workflows.at(curr.curr_workflow);

		for (const auto& subworkflow: workflow) {
			if (subworkflow.first.empty() && subworkflow.second[0] == 'A') {
				sum += get_combination(curr);
			}
			else if (subworkflow.first.empty() && subworkflow.second[0] != 'R') {
				Node next = curr;
				next.curr_workflow = subworkflow.second;
				q.push(next);
			}
			for (int i=0; i<curr.rating_intervals.size(); i++) {
				std::pair<char, std::pair<uint64_t, uint64_t>> subinterval = curr.rating_intervals[i];
				if (subworkflow.first[0] == subinterval.first) {
					uint64_t lower_bound = subinterval.second.first;
					uint64_t upper_bound = subinterval.second.second;
					if (subworkflow.first[1] == '<') {
						uint64_t value = cpp_utils::string_utils::convert_string<uint64_t>(subworkflow.first.substr(2));
						upper_bound = std::min(upper_bound, value-1);
						curr.rating_intervals[i].second.first = std::max(value, subinterval.second.first);
						curr.rating_intervals[i].second.second = std::min(subinterval.second.second, (uint64_t)4000);
					}
					else if (subworkflow.first[1] == '>') {
						uint64_t value = cpp_utils::string_utils::convert_string<uint64_t>(subworkflow.first.substr(2));
						lower_bound = std::max(lower_bound, value+1);
						curr.rating_intervals[i].second.first = std::max((uint64_t)1, subinterval.second.first);
						curr.rating_intervals[i].second.second = std::min(subinterval.second.second, value);
					}
					else {
						throw std::runtime_error("unrecognized comparator");
					}
					Node next = curr;
					next.rating_intervals[i] = {subinterval.first, {lower_bound, upper_bound}};
					next.curr_workflow = subworkflow.second;
					if (subworkflow.second == "A") {
						sum += get_combination(next);
					}
					else if (subworkflow.second != "R") {
						q.push(next);
					}
				}
			}
		}
	}
	
	std::cout << sum << std::endl;

	return 0;
}
