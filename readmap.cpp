#include <cmath>
#include <cstdint>
#include <cstring>
#include <endian.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

extern "C" {
#include <png.h>
#include <zlib.h>
}

/* Print PPM image format */
static std::string print_sqr_ppm(const std::vector<uint8_t[3]>& pix);
/* Convert Minecraft color to RGB */
static void decode_color(uint8_t mc_color, uint8_t color[3]);
/* Convert Minecraft color map to RGB pixmap */
static std::vector<uint8_t[3]> decode_colors(const std::vector<uint8_t>& mc_pix);
/* Convert RGB pixmap to png and save to file*/
static int libpng_export(FILE* fp, int width, int height, std::vector<uint8_t[3]>& pix);
/* Convert Minecraft color map to png and save to file */
static int MCmap_to_vec(std::string input_str, std::vector<uint8_t[3]>& RGBpix_map, uint32_t& sz);
static void print_usage(const char* prog_name);

int main(int argc, char** argv) {
	std::string input_str;
	std::vector<uint8_t[3]> RGBpix_map;
	int tmp, errnum;
	uint32_t sz;
	FILE *in_fp, *out_fp;
	/* Start parse inputs */
	if (argc > 3) {
		print_usage(argv[0]);
		return -1;
	}
	for (int i = 0; i < argc; i++)
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 ||
		    strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--h") == 0) {
			print_usage(argv[0]);
			return 0;
		}
	if (argc > 1)
		in_fp = fopen(argv[1], "r");
	if (argc == 3) {
		out_fp = fopen(argv[2], "w");
	} else if (argc == 2) {
		out_fp = fopen("map.png", "w");
	} else {
		print_usage(argv[0]);
		std::cout << "Please enter path to map.dat file: " << std::endl;
		std::getline(std::cin, input_str);
		in_fp = fopen(input_str.c_str(), "r");
	}
	if (in_fp == NULL) {
		std::cerr << "Could not open ";
		if (argc > 1)
			std::cerr << argv[2] << std::endl;
		else
			std::cerr << input_str << std::endl;
		return -1;
	}
	if (argc == 1) {
		std::cout << "Please enter output file path: " << std::endl;
		std::getline(std::cin, input_str);
		out_fp = fopen(input_str.c_str(), "w");
	}
	if (out_fp == NULL) {
		std::cerr << "Could not open ";
		if (argc > 1)
			std::cerr << argv[2] << std::endl;
		else
			std::cerr << input_str << std::endl;
		return -1;
	}
	/* End parse inputs */

	/* Read open gziped map file */
	auto gz = gzdopen(fileno(in_fp), "r");
	if (gz == NULL) {
		std::cerr << "Could not open stdin" << std::endl;
		return -1;
	}
	/* Read whole file into input_str*/
	while ((tmp = gzgetc(gz)) != -1)
		input_str += static_cast<char>(tmp);

	gzclose(gz);

	errnum = MCmap_to_vec(input_str, RGBpix_map, sz);
	if (errnum != 0)
		return errnum - 2;
	/* std::cout << print_sqr_ppm(RGBpix_map); */
	errnum = libpng_export(out_fp, sz, sz, RGBpix_map);
	if (errnum != 0)
		return errnum - 3;
	return 0;
}

/* Print PPM image format */
static std::string print_sqr_ppm(const std::vector<uint8_t[3]>& pix) {
	std::stringstream ppm;
	uint32_t sz = sqrt(pix.size());
	ppm << "P3\n" << sz << " " << sz << "\n255\n";
	for (int i = 0; i < sz; i++) {
		for (int j = 0; j < sz; j++) {
			ppm << static_cast<unsigned int>(pix[i + (j * sz)][0]) << " ";
			ppm << static_cast<unsigned int>(pix[i + (j * sz)][1]) << " ";
			ppm << static_cast<unsigned int>(pix[i + (j * sz)][2]) << " ";
		}
		ppm << "\n";
	}

	return ppm.str();
}

/* Convert Minecraft color to RGB */
static void decode_color(uint8_t mc_color, uint8_t color[3]) {
	static constexpr uint8_t color_maps[][3] = {
	    {0, 0, 0},       {127, 178, 56},  {247, 233, 163}, {199, 199, 199}, {255, 0, 0},
	    {160, 160, 255}, {167, 167, 167}, {0, 124, 0},     {255, 255, 255}, {164, 168, 184},
	    {151, 109, 77},  {112, 112, 112}, {64, 64, 255},   {143, 119, 72},  {255, 252, 245},
	    {216, 127, 51},  {178, 76, 216},  {102, 153, 216}, {229, 229, 51},  {127, 204, 25},
	    {242, 127, 165}, {76, 76, 76},    {153, 153, 153}, {76, 127, 153},  {127, 63, 178},
	    {51, 76, 178},   {102, 76, 51},   {102, 127, 51},  {153, 51, 51},   {25, 25, 25},
	    {250, 238, 77},  {92, 219, 213},  {74, 128, 255},  {0, 217, 58},    {129, 86, 49},
	    {112, 2, 0},     {209, 177, 161}, {159, 82, 36},   {149, 87, 108},  {112, 108, 138},
	    {186, 133, 36},  {103, 117, 53},  {160, 77, 78},   {57, 41, 35},    {135, 107, 98},
	    {87, 92, 92},    {122, 73, 88},   {76, 62, 92},    {76, 50, 35},    {76, 82, 42},
	    {142, 60, 46},   {37, 22, 16},    {189, 48, 49},   {148, 63, 97},   {92, 25, 29},
	    {22, 126, 134},  {58, 142, 140},  {86, 44, 62},    {20, 180, 133},  {100, 100, 100},
	    {216, 175, 147}, {127, 167, 150}};
	static constexpr uint8_t mult[4] = {180, 220, 255, 135};
	uint16_t multiplier = static_cast<uint16_t>(mult[mc_color & 3]);
	if ((mc_color >> 2) >= (sizeof(color_maps) / 3)) {
		std::cerr << "Color map is invalid!" << std::endl;
		mc_color = 0;
	}
	auto base_color = color_maps[mc_color >> 2];
	for (int i = 0; i < 3; i++)
		color[i] = (static_cast<uint16_t>(base_color[i]) * multiplier) / 255;
}

/* Convert Minecraft color map to RGB pixmap */
static std::vector<uint8_t[3]> decode_colors(const std::vector<uint8_t>& mc_pix) {
	std::vector<uint8_t[3]> pix_map(mc_pix.size());
	for (int i = 0; i < mc_pix.size(); i++) {
		uint8_t color[3] = {0};
		decode_color(mc_pix[i], color);
		pix_map[i][0] = color[0];
		pix_map[i][1] = color[1];
		pix_map[i][2] = color[2];
	}
	return pix_map;
}

/* Convert RGB pixmap to png and save to file*/
static int libpng_export(FILE* fp, int width, int height, std::vector<uint8_t[3]>& pix) {
	int code = 0;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	if (fp == NULL)
		return -4;
	if (width * height != pix.size())
		return -5;

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		code = -1;
		goto finalise;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		code = -2;
		goto finalise;
	}
	if (setjmp(png_jmpbuf(png_ptr))) {
		code = -3;
		goto finalise;
	}
	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);

	for (int y = 0; y < height; y++) {
		png_bytep row = ((png_bytep)pix.data()) + (y * width * 3);
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);
finalise:
	if (info_ptr != NULL)
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL)
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	return code;
}

/* Convert Minecraft color map to png and save to file */
static int MCmap_to_vec(std::string input_str, std::vector<uint8_t[3]>& RGBpix_map, uint32_t& sz) {
	std::vector<uint8_t> byte_arr;
	std::size_t fnd = input_str.find_last_of("colors");
	if (fnd == std::string::npos) {
		std::cerr << "could not find color map." << std::endl;
		return -1;
	}
	/* Only data after our node is needed */
	input_str = input_str.substr(fnd + 1);
	/* First 4 bytes are the length of the map*/
	auto total_len = htobe32(*reinterpret_cast<const int32_t*>(input_str.substr(0, 4).c_str()));
	sz = sqrt(total_len);
	if (sz * sz != total_len) {
		std::cerr << "Map isn't square are you sure minecraft generated it?" << std::endl;
		return -2;
	}
	/* Data after our node and size is the pixmap*/
	input_str = input_str.substr(4);
	std::copy(input_str.begin(), input_str.begin() + total_len, std::back_inserter(byte_arr));
	RGBpix_map = decode_colors(byte_arr);
	return 0;
}

static void print_usage(const char* prog_name) {
	std::cout << "Usage: " << std::endl;
	std::cout << prog_name << " (map.dat)" << std::endl;
	std::cout << prog_name << " (map.dat) [output.png]" << std::endl;
	std::cout << prog_name << " -h" << std::endl;
	std::cout << "Run with no inputs for a prompt." << std::endl;
}
