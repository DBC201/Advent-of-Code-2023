#include <file_utils.h>
#include <coordinate_utils.h>
#include <string_utils.h>
#include <queue>
#include <set>

using cpp_utils::coordinate_utils::two_dimensional::direction_t;
using cpp_utils::coordinate_utils::two_dimensional::EAST;
using cpp_utils::coordinate_utils::two_dimensional::SOUTH;
using cpp_utils::coordinate_utils::two_dimensional::flip_direction;
using cpp_utils::coordinate_utils::two_dimensional::move;
using cpp_utils::coordinate_utils::two_dimensional::in_bounds;
using cpp_utils::coordinate_utils::CoordinateCache;
using cpp_utils::string_utils::vector_to_string;

struct Node {
	int y;
	int x;
	int cost = INT_MAX;
	direction_t direction;
	CoordinateCache<int> path;

	Node() = default;

	Node(int y, int x, int cost, direction_t direction) {
		this->y = y;
		this->x = x;
		this->cost = cost;
		this->direction = direction;
		path.insert({y, x});
	}

	Node* get_copy() {
		Node* copy = new Node;
		copy->y = y;
		copy->x = x;
		copy->direction = direction;
		copy->path = path;
		copy->cost = cost;
		return copy;
	}
};

struct NodeSorter {
	bool operator()(const Node* a, const Node* b) const {
		return a->cost > b->cost;
	}
};

struct NodeComparator {
	bool operator()(const Node* a, const Node* b) const {
		if (a->y != b->y)
			return a->y < b->y;
		else if (a->x != b->x)
			return a->x < b->x;
		else
			return a->direction < b->direction;
	}
};

void step(
	std::vector<std::vector<int>>& grid, 
	Node* node, 
	std::set<Node*, NodeComparator>& calculated, 
	std::priority_queue<Node*, std::vector<Node*>, 
	NodeSorter>& q,
	bool queue_insert) 
{
	std::vector<int> next_pos = move<int>({node->y, node->x}, node->direction);
	if (in_bounds<int>(next_pos, grid.size(), grid[0].size())) {
		node->y = next_pos[0];
		node->x = next_pos[1];
		node->path.insert(next_pos);
		node->cost += grid[node->y][node->x];
		if (queue_insert && !calculated.contains(node)) {
			q.push(node->get_copy());
		}
	}
}

int dijkstra(std::vector<std::vector<int>>& grid, int min_movement, int max_movement) {
	std::priority_queue<Node*, std::vector<Node*>, NodeSorter> q;
	std::set<Node*, NodeComparator> calculated;

	Node *s1, *s2;
	s1 = new Node(0, 0, 0, EAST);
	s2 = new Node(0, 0, 0, SOUTH);

	q.push(s1);
	q.push(s2);

	while (!q.empty()) {
		Node* curr = q.top();
		q.pop();
		if (curr->y == grid.size() - 1 && curr->x == grid[0].size() - 1) {
			int cost = curr->cost;
			delete curr;
			while (!q.empty()) {
				curr = q.top();
				q.pop();
				delete curr;
			}
			for (auto it=calculated.begin(); it != calculated.end();) {
				delete *it;
				it = calculated.erase(it);
			}
			return cost;
		}
		if (calculated.contains(curr)) {
			continue;
		}
		calculated.insert(curr);
		Node *right, *left;

		right = curr->get_copy();
		left = curr->get_copy();

		right->direction = flip_direction(curr->direction, 90);
		left->direction = flip_direction(curr->direction, -90);

		for (int i=1; i<=max_movement; i++) {
			step(grid, right, calculated, q, i >= min_movement);
			step(grid, left, calculated, q, i >= min_movement);
		}
		delete right;
		delete left;
	}
}

int main() {
	std::vector<std::vector<int>> grid;
	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		std::vector<int> row;
		for (char c: line) {
			row.push_back(c - '0');
		}
		grid.push_back(row);
	});

	std::cout << dijkstra(grid, 1, 3) << std::endl;

	std::cout << dijkstra(grid, 4, 10) << std::endl;
	
	return 0;
}
