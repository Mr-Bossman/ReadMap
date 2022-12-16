#include <iostream>
#include <iterator>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <cmath>
#include <endian.h>

std::string print_sqr_ppm(const std::vector<uint8_t[3]>& pix) {
	std::stringstream ppm;
	uint32_t sz = sqrt(pix.size());
	ppm << "P3\n" << sz << " " << sz << "\n255\n";
	for(int i = 0;i < sz;i++) {
		for(int j = 0;j < sz;j++) {
			ppm << static_cast<unsigned int>(pix[i+(j*sz)][0]) << " ";
			ppm << static_cast<unsigned int>(pix[i+(j*sz)][1]) << " ";
			ppm << static_cast<unsigned int>(pix[i+(j*sz)][2]) << " ";
		}
		ppm << "\n";
	}

	return ppm.str();
}

void decode_color(uint8_t mc_color,uint8_t color[3]) {
	static uint8_t color_maps[][3] = {{0, 0, 0},{127, 178, 56},{247, 233, 163},{199, 199, 199},{255, 0, 0},{160, 160, 255},{167, 167, 167},{0, 124, 0},{255, 255, 255},{164, 168, 184},{151, 109, 77},{112, 112, 112},{64, 64, 255},{143, 119, 72},{255, 252, 245},{216, 127, 51},{178, 76, 216},{102, 153, 216},{229, 229, 51},{127, 204, 25},{242, 127, 165},{76, 76, 76},{153, 153, 153},{76, 127, 153},{127, 63, 178},{51, 76, 178},{102, 76, 51},{102, 127, 51},{153, 51, 51},{25, 25, 25},{250, 238, 77},{92, 219, 213},{74, 128, 255},{0, 217, 58},{129, 86, 49},{112, 2, 0},{209, 177, 161},{159, 82, 36},{149, 87, 108},{112, 108, 138},{186, 133, 36},{103, 117, 53},{160, 77, 78},{57, 41, 35},{135, 107, 98},{87, 92, 92},{122, 73, 88},{76, 62, 92},{76, 50, 35},{76, 82, 42},{142, 60, 46},{37, 22, 16},{189, 48, 49},{148, 63, 97},{92, 25, 29},{22, 126, 134},{58, 142, 140},{86, 44, 62},{20, 180, 133},{100, 100, 100},{216, 175, 147},{127, 167, 150}};
	static uint8_t mult[4] = {180, 220, 255, 135};
	uint16_t multiplier = static_cast<uint16_t>(mult[mc_color&3]);
	if ((mc_color>>2) >= (sizeof(color_maps)/3)){
		std::cerr << "Color map is invalid" << std::endl;
		exit(-3);
	}
	auto base_color = color_maps[mc_color>>2];
	for(int i =0; i < 3;i++)
		color[i] = (static_cast<uint16_t>(base_color[i])*multiplier)/255;
}
std::vector<uint8_t[3]> decode_colors(const std::vector<uint8_t>& mc_pix) {
	std::vector<uint8_t[3]> pix_map(mc_pix.size());
	for(int i = 0; i < mc_pix.size();i++) {
		uint8_t color[3] = {0};
		decode_color(mc_pix[i],color);
		pix_map[i][0] = color[0];
		pix_map[i][1] = color[1];
		pix_map[i][2] = color[2];
	}
	return pix_map;
}

int main() {
	std::istreambuf_iterator<char> begin(std::cin), end;
	std::string input_str(begin, end);
	std::size_t fnd = input_str.find_last_of("colors");
	if(fnd == std::string::npos) {
		std::cerr << "could not find color map. Did you un zip it?" << std::endl;
		return -1;
	}
	input_str = input_str.substr(fnd+1);
	auto total_len = htobe32(*reinterpret_cast<const int32_t*>(input_str.substr(0,4).c_str()));
	uint32_t sz = sqrt(total_len);
	if(sz*sz != total_len) {
		std::cerr << "Map isnt square are you sure minecraft generated it?" << std::endl;
		return -2;
	}
	input_str = input_str.substr(4);
	std::vector<uint8_t> byte_arr;
	std::copy(input_str.begin(),input_str.begin()+total_len,std::back_inserter(byte_arr));
	std::cout << print_sqr_ppm(decode_colors(byte_arr));
	return 0;
}
