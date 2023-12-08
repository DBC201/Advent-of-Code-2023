#include <file_utils.h>
#include <string_utils.h>
#include <unordered_map>

enum hand_type {
	HIGH_CARD,
	ONE_PAIR,
	TWO_PAIR,
	THREE_OF_A_KIND,
	FULL_HOUSE,
	FOUR_OF_A_KIND,
	FIVE_OF_A_KIND
};

struct hand {
	hand_type type;
	std::string cards;
	uint64_t bet;
};

void set_type_with_joker(hand* h) {
	std::unordered_map<char, uint16_t> occurences;

	for (int i=0; i<h->cards.length(); i++) {
		char c = h->cards[i];

		if (occurences.contains(c)) {
			occurences.at(c)++;
		}
		else {
			occurences.insert({c, 1});
		}
	}

	if (!occurences.contains('J')) {
		return;
	}
	
	std::pair<char, uint16_t> max_occurance = {'x', 0};

	for (auto p: occurences) {
		if (p.first != 'J' && p.second > max_occurance.second) {
			max_occurance = p;
		}
	}

	if (occurences.contains(max_occurance.first)) {
		occurences.at(max_occurance.first) += occurences.at('J');
		occurences.erase('J');
	}

	if (occurences.size() == 1) {
		h->type = FIVE_OF_A_KIND;
	}
	else if (occurences.size() == 2) {
		uint16_t c1 = occurences.begin()->second;
		if (c1 == 1 || c1 == 4) {
			h->type = FOUR_OF_A_KIND;
		}
		else {
			h->type = FULL_HOUSE;
		}
	}
	else if (occurences.size() == 3) {
		for (const auto &p: occurences) {
			if (p.second == 3) {
				h->type = THREE_OF_A_KIND;
				break;
			}
			else if (p.second == 2) {
				h->type = TWO_PAIR;
				break;
			}
		}
	}
	else if (occurences.size() == 4) {
		h->type = ONE_PAIR;
	}
	else {
		h->type = HIGH_CARD;
	}
}

bool is_weaker_with_joker(hand *hand1, hand *hand2) {
	if (hand1->type == hand2->type) {
		std::unordered_map<char, uint16_t> card_values = {
			{'A', 13},
			{'K', 12},
			{'Q', 11},
			{'T', 9},
			{'9', 8},
			{'8', 7},
			{'7', 6},
			{'6', 5},
			{'5', 4},
			{'4', 3},
			{'3', 2},
			{'2', 1},
			{'J', 0},
		};
		int i;
		for (i=0; i<hand1->cards.length(); i++) {
			char c1 = hand1->cards[i];
			char c2 = hand2->cards[i];

			if (c1 != c2) {
				return card_values.at(c1) < card_values.at(c2);
			}
		}
	}
	else {
		return hand1->type < hand2->type;
	}
}

void set_type(hand* h) {
	std::unordered_map<char, uint16_t> occurences;

	for (int i=0; i<h->cards.length(); i++) {
		char c = h->cards[i];

		if (occurences.contains(c)) {
			occurences.at(c)++;
		}
		else {
			occurences.insert({c, 1});
		}
	}

	if (occurences.size() == 1) {
		h->type = FIVE_OF_A_KIND;
	}
	else if (occurences.size() == 2) {
		uint16_t c1 = occurences.at(h->cards[0]);
		if (c1 == 1 || c1 == 4) {
			h->type = FOUR_OF_A_KIND;
		}
		else {
			h->type = FULL_HOUSE;
		}
	}
	else if (occurences.size() == 3) {
		for (const auto &p: occurences) {
			if (p.second == 3) {
				h->type = THREE_OF_A_KIND;
				break;
			}
			else if (p.second == 2) {
				h->type = TWO_PAIR;
				break;
			}
		}
	}
	else if (occurences.size() == 4) {
		h->type = ONE_PAIR;
	}
	else {
		h->type = HIGH_CARD;
	}
}
 
bool is_weaker(hand* hand1, hand* hand2) {
	if (hand1->type == hand2->type) {
		std::unordered_map<char, uint16_t> card_values = {
			{'A', 13},
			{'K', 12},
			{'Q', 11},
			{'J', 10},
			{'T', 9},
			{'9', 8},
			{'8', 7},
			{'7', 6},
			{'6', 5},
			{'5', 4},
			{'4', 3},
			{'3', 2},
			{'2', 1}
		};
		int i;
		for (i=0; i<hand1->cards.length(); i++) {
			char c1 = hand1->cards[i];
			char c2 = hand2->cards[i];

			if (c1 != c2) {
				return card_values.at(c1) < card_values.at(c2);
			}
		}
	}
	else {
		return hand1->type < hand2->type;
	}
}

int main() {
	std::vector<hand*> hands;
	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		auto tokens = cpp_utils::string_utils::split_by_string(line, " ");
		hand* h = new hand;
		h->cards = tokens[0];
		h->bet = cpp_utils::string_utils::convert_string<uint64_t>(tokens[1]);
		set_type(h);
		hands.push_back(h);
	});

	std::sort(hands.begin(), hands.end(), is_weaker);

	uint64_t result = 0;

	for (int i=0; i<hands.size(); i++) {
		result += (i+1) * hands[i]->bet;
	}

	std::cout << result << std::endl;

	for (int i=0; i<hands.size(); i++) {
		set_type_with_joker(hands[i]);
	}

	std::sort(hands.begin(), hands.end(), is_weaker_with_joker);

	result = 0;

	for (int i=0; i<hands.size(); i++) {
		result += (i+1) * hands[i]->bet;
	}

	std::cout << result << std::endl;

	return 0;
}
