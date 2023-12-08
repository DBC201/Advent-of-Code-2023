#include <file_utils.h>
#include <unordered_map>
#include <string_utils.h>

using cpp_utils::file_utils::FileUtils;

struct Node {
	Node* left;
	Node* right;
	std::string name;
};

Node *getNode(std::unordered_map<std::string, Node*>& nodes, std::string& name) {
	if (nodes.contains(name)) {
		return nodes.at(name);
	}
	else {
		Node *n = new Node;
		n->name = name;
		nodes.insert({name, n});
		return n;
	}
}

// not a day goes by without using Ancient Greek discoveries...
// over 2000 years, and they still live on
uint64_t Euclid_GCD(uint64_t n1, uint64_t n2) {
	// a = b * c + r

	uint64_t a = std::max(n1, n2);
	uint64_t b = std::min(n1, n2);
	uint64_t r;


	while (b != 0) {
		r = a % b;
		a = b;
		b = r;
	}


	return a;
} 

int main() {
	std::unordered_map<std::string, Node*> nodes;

	FileUtils fileUtils(INPUT_PATH);

	std::string steps = fileUtils.get_line();

	fileUtils.process_lines([&](std::string line) {
		if (line.empty()) {
			return;
		}
		auto t = cpp_utils::string_utils::split_by_string(line, " = ");
		std::string name = t[0];
		auto children = cpp_utils::string_utils::split_by_string(t[1].substr(1, t[1].length() - 2), ", ");

		Node *parent = getNode(nodes, name);
		Node *left = getNode(nodes, children[0]);
		Node *right = getNode(nodes, children[1]);

		parent->left = left;
		parent->right = right;
	});

	fileUtils.close();

	Node *curr = nodes.at("AAA");

	uint32_t step_count = 0;

	while (curr->name != "ZZZ") {
		char next_step = steps[step_count % steps.length()];

		if (next_step == 'R') {
			curr = curr->right;
		}
		else {
			curr = curr->left;
		}

		step_count++;
	}

	std::cout << step_count << std::endl;

	std::vector<Node*> starts;

	for (const auto& p: nodes) {
		if (p.first[p.first.length()-1] == 'A') {
			starts.push_back(p.second);
		}
	}

	std::vector<uint64_t> step_counts(starts.size(), 0);

	uint32_t found_count = 0;
	step_count = 0;

	while (found_count < step_counts.size()) {
		char next_step = steps[step_count % steps.length()];
		for (int i=0; i<starts.size(); i++) {
			Node *curr = starts[i];
			if (curr->name[curr->name.length() - 1] == 'Z' && step_counts[i] == 0) {
				found_count++;
				step_counts[i] = step_count;
			}

			if (next_step == 'R') {
				starts[i] = curr->right;
			}
			else {
				starts[i] = curr->left;
			}
		}
		
		step_count++;
	}
	
	uint64_t gcm = (step_counts[0] * step_counts[1])/Euclid_GCD(step_counts[0], step_counts[1]);

	for (int i=2; i<step_counts.size(); i++) {
		gcm = (gcm * step_counts[i])/Euclid_GCD(gcm, step_counts[i]);
	}

	std::cout << gcm << std::endl;
	return 0;
}
