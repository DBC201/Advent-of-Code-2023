#include <file_utils.h>
#include <string_utils.h>
#include <queue>
#include <unordered_map>
#include <math_utils.h>

struct Pulse {
	std::string source_name;
	std::string destination_name;
	bool high;
	
	Pulse(std::string source_name, std::string destination_name, bool high) {
		this->source_name = source_name;
		this->destination_name = destination_name;
		this->high = high;
	}
};

class Module {
public:
	Module(std::string name) {
		this->name = name;
	}
	virtual std::vector<Pulse*> pulse(Pulse* incoming_pulse) = 0;

	void add_parent(Module* parent) {
		parents.push_back(parent);
	}

	void add_child(Module* child) {
		children.push_back(child);
	}

	std::string get_name() {
		return name;
	}

	bool last_sent_high() {
		return m_last_sent_high;
	}

	bool has_single_low_pulse() {
		bool m_has_single_low_pulse = false;
		for (const auto& parent: parents) {
			if (!parent->last_sent_high() && !m_has_single_low_pulse) {
				return false;
			}
			else if (!parent->last_sent_high()) {
				m_has_single_low_pulse = true;
			}
		}
		return m_has_single_low_pulse;
	}

	void reset() {
		m_last_sent_high = false;
	}

	std::vector<Module*> get_parents() {
		return parents;
	}
protected:
	std::vector<Pulse*> get_broadcast_pulses(bool high) {
		std::vector<Pulse*> result;
		for (auto& child: children) {
			result.push_back(new Pulse(this->name, child->name, high));
		}
		return result;
	}
	std::vector<Module*> parents;
	std::vector<Module*> children;
	std::string name;
	bool m_last_sent_high = false;
};

class Broadcaster: public Module {
public:
	Broadcaster(std::string name): Module(name) {

	}

	std::vector<Pulse*> pulse(Pulse* incoming_pulse) override {
		m_last_sent_high = incoming_pulse->high;
		delete incoming_pulse;
		return get_broadcast_pulses(m_last_sent_high);
	}
};

class FlipFlop: public Module {
public:
	FlipFlop(std::string name): Module(name) {

	}

	std::vector<Pulse*> pulse(Pulse* incoming_pulse) override {
		if (incoming_pulse->high) {
			delete incoming_pulse;
			return {};
		}
		if (!incoming_pulse->high) {
			m_last_sent_high = !m_last_sent_high;
		}
		delete incoming_pulse;
		return get_broadcast_pulses(m_last_sent_high);
	}
};

class Conjunction: public Module {
public:
	Conjunction(std::string name): Module(name) {

	}

	std::vector<Pulse*> pulse(Pulse* incoming_pulse) override {
		delete incoming_pulse;
		bool all_high = true;
		for (auto& parent: parents) {
			if (!parent->last_sent_high()) {
				all_high = false;
			}
		}
		m_last_sent_high = !all_high;
		return get_broadcast_pulses(m_last_sent_high);
	}
};

std::pair<uint64_t, uint64_t> run_cycle(std::unordered_map<std::string, Module*>& modules) {
	std::queue<Pulse*> q;
	uint64_t low_count = 0;
	uint64_t high_count = 0;

	q.push(new Pulse("button", "broadcaster", false));

	while (q.size()) {
		Pulse* p = q.front();
		q.pop();
		if (p->high) {
			high_count++;
		}
		else {
			low_count++;
		}

		// std::cout << p->source_name << " -> " << (p->high ? "high" : "low") << " -> " << p->destination_name << std::endl;

		for (auto& pn: modules.at(p->destination_name)->pulse(p)) {
			q.push(pn);
		}
	}
	// std::cout << std::endl;
	return {low_count, high_count};
}

int main() {
	std::unordered_map<std::string, std::vector<std::string>> topology;
	std::unordered_map<std::string, Module*> modules;

	cpp_utils::file_utils::process_lines(INPUT_PATH, [&](std::string line) {
		auto tokens = cpp_utils::string_utils::split_by_string(line, " -> ");
		if (tokens[0] == "broadcaster") {
			Broadcaster* broadcaster = new Broadcaster("broadcaster");
			modules.insert({"broadcaster", broadcaster});
			topology.insert({"broadcaster", cpp_utils::string_utils::split_by_string(tokens[1], ", ")});
		}
		else {
			char module_type = tokens[0][0];
			std::string module_name = tokens[0].substr(1);
			if (module_type == '%') {
				FlipFlop* flipFlop = new FlipFlop(module_name);
				modules.insert({module_name, flipFlop});
				topology.insert({module_name, cpp_utils::string_utils::split_by_string(tokens[1], ", ")});
			}
			else if (module_type == '&') {
				Conjunction* conjunction = new Conjunction(module_name);
				modules.insert({module_name, conjunction});
				topology.insert({module_name, cpp_utils::string_utils::split_by_string(tokens[1], ", ")});
			}
			else {
				throw std::runtime_error("unrecognized module type");
			}
		}
	});

	for (const auto& p: topology) {
		Module* parent = modules.at(p.first);
		for (const auto& child_name: p.second) {
			Module *child;
			if (modules.contains(child_name)) {
				child = modules.at(child_name);
			}
			else {
				std::cout << "node with no children: " << child_name << std::endl;
				std::cout << "creating " << child_name <<  " as broadcast module" << std::endl;
				std::cout << std::endl;
				child = new Broadcaster(child_name);
				modules.insert({child_name, child});
			}
			child->add_parent(parent);
			parent->add_child(child);
		}
	}

	uint64_t low_count = 0;
	uint64_t high_count = 0;

	for (int i=0; i<1000; i++) {
		std::pair<uint64_t, uint64_t> counts = run_cycle(modules);
		low_count += counts.first;
		high_count += counts.second;
	}

	std::cout << low_count * high_count << std::endl;

	for (const auto &p: modules) {
		p.second->reset();
	}

	// uint64_t press_count = 0;

	// while (!modules.at("rx")->has_single_low_pulse()) {
	// 	press_count++;
	// 	run_cycle(modules);
	// }

	// std::cout << press_count << std::endl;

	// saw this solution on reddit, I don't understand why it works
	// Is it given that no two module won't be high, or they will converge in gcm?
	// wasn't able to find any proof, not comfortable with this solution

	Module *parent = modules.at("rx")->get_parents()[0];
	std::vector<Module*> need_to_be_high = parent->get_parents();
	std::vector<uint64_t> cycle_counts(need_to_be_high.size(), 0);
	int found_count = 0;
	uint64_t cycle_count = 0;

	while (found_count != need_to_be_high.size()) {
		cycle_count++;
		std::queue<Pulse*> q;
		q.push(new Pulse("button", "broadcaster", false));

		while (q.size()) {
			Pulse* p = q.front();
			q.pop();
			
			if (p->destination_name == parent->get_name() && p->high) {
				for(int i=0; i<cycle_counts.size(); i++) {
					if (cycle_counts[i] == 0 && need_to_be_high[i]->get_name() == p->source_name) {
						found_count++;
						cycle_counts[i] = cycle_count;
						std::cout << p->source_name << ": " << cycle_count << std::endl;
					}
				}
			}

			for (auto& pn: modules.at(p->destination_name)->pulse(p)) {
				q.push(pn);
			}
		}		
	}

	std::cout << cpp_utils::math_utils::lcm(cycle_counts) << std::endl;

	return 0;
}
