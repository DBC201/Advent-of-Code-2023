/**
 * @file 09.cpp
 * @author Deniz Cakiroglu
 * @brief 
 * I had this exact problem in mind years ago, when I first started programming.
 * Github repo link: https://github.com/DBC201/sequence-interpolation
 * @date 2023-12-09
 *  
 */
#include <file_utils.h>
#include <string_utils.h>
#include <extrapolation_utils.h>

using cpp_utils::file_utils::FileUtils;
using cpp_utils::extrapolation_utils::ExtrapolationUtils;
using cpp_utils::extrapolation_utils::Addition;
using cpp_utils::extrapolation_utils::Subtraction;
using cpp_utils::extrapolation_utils::Operation;

int main() {
	std::vector<std::vector<int32_t>> sequences;
	FileUtils fileUtils(INPUT_PATH);

	fileUtils.process_lines([&] (std::string line) {
		auto tokens = cpp_utils::string_utils::split_by_string(line, " ");
		auto nums = cpp_utils::string_utils::convert_string_vector<int32_t>(tokens);
		sequences.push_back(nums);
	});
	fileUtils.close();

	int32_t result = 0;

	for (auto& sequence: sequences) {
		Addition<int32_t> addition;
		Subtraction<int32_t> subtraction;
		std::vector<Operation<int32_t>*> o1 = {&addition};
		std::vector<Operation<int32_t>*> o2 = {&subtraction};	
		ExtrapolationUtils<int32_t> extrapolationUtils(sequence, o1, o2);
		result += extrapolationUtils.extrapolate(0);
	}

	std::cout << result << std::endl;

	result = 0;

	for (auto sequence: sequences) {
		Addition<int32_t> addition;
		Subtraction<int32_t> subtraction;
		std::vector<Operation<int32_t>*> o1 = {&addition};
		std::vector<Operation<int32_t>*> o2 = {&subtraction};
		std::reverse(sequence.begin(), sequence.end());	
		ExtrapolationUtils<int32_t> extrapolationUtils(sequence, o1, o2);
		result += extrapolationUtils.extrapolate(0);
	}

	std::cout << result << std::endl;


	return 0;
}
