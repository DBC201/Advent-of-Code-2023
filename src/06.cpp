#include <iostream>
#include <vector>

int main() {
	std::vector<uint32_t> times = {51, 69, 98, 78};
	std::vector<uint32_t> distances = {377, 1171, 1224, 1505};
	std::vector<uint32_t> win_counts(times.size(), 0);

	for (int i=0; i<times.size(); i++) {
		int distance = distances[i];
		for (int j=1; j<times[i]; j++) {
			int movement_dur = times[i] - j;
			int speed = j;

			if (movement_dur * speed > distance) {
				win_counts[i]++;
			}
		}
	}

	uint32_t multiplication = 1;

	for (const auto& win_count: win_counts) {
		if (win_count == 0) {
			multiplication = 0;
		}
		else {
			multiplication *= win_count;
		}
	}

	std::cout << multiplication << std::endl;

	uint64_t time = 0;
	uint64_t distance = 0;
	uint64_t win_count = 0;
	uint16_t time_curr_power = 0;
	uint16_t distance_curr_power = 0;

	for (int i=times.size()-1; i>=0; i--) {
		time += pow(10, time_curr_power) * times[i];
		distance += pow(10, distance_curr_power) * distances[i];

		time_curr_power += ceil(log(times[i])/log(10));
		distance_curr_power += ceil(log(distances[i])/log(10));
	}

	for (uint64_t wait_duration=1; wait_duration<time; wait_duration++) {
		uint64_t movement_duration = time-wait_duration;
		uint64_t speed = wait_duration;
		if (movement_duration * speed > distance) {
			win_count++;
		}
	}

	std::cout << win_count << std::endl;

	return 0;
}
