#include <file_utils.h>
#include <string_utils.h>
#include <unordered_map>

struct LensNode
{
	LensNode *previous = nullptr;
	LensNode *next = nullptr;
	int focal_length;
	std::string label;
};

int get_hash(std::string s)
{
	int hash = 0;
	for (char c : s)
	{
		hash += c;
		hash *= 17;
		hash %= 256;
	}

	return hash;
}

void append(std::vector<std::pair<LensNode*, LensNode*>> &boxes, int box_index, LensNode* curr) {
	LensNode *boxTail = boxes[box_index].second;
	if (boxTail == nullptr) {
		boxes[box_index].first = curr;
		boxes[box_index].second = curr;
	}
	else {
		boxTail->next = curr;
		curr->previous = boxTail;
		boxes[box_index].second = curr;
	}
}

void set_lens(std::vector<std::pair<LensNode*, LensNode*>> &boxes,
			  std::vector<std::unordered_map<std::string, LensNode *>> &box_mappings, std::string lens, int focal_length)
{
	int box_index = get_hash(lens);
	LensNode *curr;

	if (!box_mappings[box_index].contains(lens)) {
		curr = new LensNode;
		curr->focal_length = focal_length;
		curr->label = lens;
		append(boxes, box_index, curr);
		box_mappings[box_index].insert({lens, curr});
	}
	else {
		curr = box_mappings[box_index].at(lens);
		curr->focal_length = focal_length;
	}
}

void delete_lens(std::vector<std::pair<LensNode*, LensNode*>> &boxes,
				 std::vector<std::unordered_map<std::string, LensNode *>> &box_mappings, std::string lens)
{
	int box_index = get_hash(lens);
	if (box_mappings[box_index].contains(lens))
	{
		LensNode *curr = box_mappings[box_index].at(lens);
		if (curr->previous == nullptr && curr->next == nullptr) {
			boxes[box_index] = {nullptr, nullptr};
		}
		else if (curr->previous == nullptr) {
			boxes[box_index].first = curr->next;
			curr->next->previous = nullptr;
		}
		else if (curr->next == nullptr) {
			boxes[box_index].second = curr->previous;
			curr->previous->next = nullptr;
		}
		else {
			curr->previous->next = curr->next;
			curr->next->previous = curr->previous;
		}
		box_mappings[box_index].erase(lens);
		delete curr;
	}
}

int main()
{
	cpp_utils::file_utils::FileUtils fileUtils(INPUT_PATH);
	std::vector<std::string> input;
	fileUtils.process_lines([&](std::string line)
							{
		std::vector<std::string> curr_line 
			= cpp_utils::string_utils::convert_string_vector<std::string>(
					cpp_utils::string_utils::split_by_string(line, ","));
		input.insert(input.end(), curr_line.begin(), curr_line.end()); });

	int sum = 0;
	for (std::string s : input)
	{
		sum += get_hash(s);
	}

	std::cout << sum << std::endl;

	std::vector<std::pair<LensNode*, LensNode*>> boxes(256, {nullptr, nullptr});
	std::vector<std::unordered_map<std::string, LensNode *>> box_mappings(256);

	for (std::string operation : input)
	{
		if (operation.back() == '-')
		{
			delete_lens(boxes, box_mappings, operation.substr(0, operation.length() - 1));
		}
		else
		{
			auto tokens = cpp_utils::string_utils::split_by_string(operation, "=");
			int focal_length = cpp_utils::string_utils::convert_string<int>(tokens[1]);
			set_lens(boxes, box_mappings, tokens[0], focal_length);
		}
	}

	int result = 0;

	for (int i=0; i<boxes.size(); i++) {
		LensNode *curr = boxes[i].first;
		int j=1;

		while (curr != nullptr) {
			result += (i+1) * curr->focal_length * j;
			curr = curr->next;
			j++;
		}
	}

	std::cout << result << std::endl;

	return 0;
}
