#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <string_view>
#include <chrono>
#include "indicators/progress_bar.hpp"
#include "indicators/multi_progress.hpp"

using namespace std::chrono_literals;

struct Utilization
{
	indicators::ProgressBar util_bar;
	const float min;
	const float max;
public:
	Utilization(float min=0, float max=100):min(min), max(max)
	{
		util_bar.set_bar_width(50);
		util_bar.start_bar_with("[");
		util_bar.end_bar_with("]");
		util_bar.fill_bar_progress_with("*");
		util_bar.set_foreground_color(indicators::Color::CYAN);
	}
	void set_value(float value)
	{
		util_bar.set_progress((value-min)/(max-min));
	}

	void set_label(std::string_view label)
	{
		util_bar.set_prefix_text(std::string(label));
	}
};

#define class struct

class Info
{
	struct info
	{
		std::string label;
		int value;
		size_t offset;
		size_t length;
	};
	std::vector<info> infos;
public:
	Info(const char* mmf_view)
	{
		auto current_label_begin = strstr(mmf_view, "<label>");
		do {
			current_label_begin += strlen("<label>");
			auto current_label_end = strstr(current_label_begin, "</label>");
			auto current_value_begin = strstr(current_label_begin, "<value>") + strlen("<value>");
			auto current_value_end = strstr(current_label_begin, "</value>");
			infos.push_back({ std::string{current_label_begin, current_label_end}, 0, 
				static_cast<size_t>(current_value_begin-mmf_view), static_cast<size_t>(current_value_end-current_value_begin) });
			if (*(current_value_end + strlen("</value>")) == '\0')
				break;
			current_label_begin = strstr(current_value_end, "<label>");
		} while (current_label_begin != NULL);
	}

	void update(const char* mmf_view)
	{
		for (auto& info : infos)
		{
			//info.value = std::stoi(std::string(mmf_view + info.offset, 2));
			info.value = atoi(mmf_view + info.offset);
		}
	}

	void print()
	{
		for (const auto& info : infos)
			std::cout << info.label<<" "<<info.value<< '\n';
	}
};

int main()
{
	
	auto mmf_handle = OpenFileMapping(FILE_MAP_READ, FALSE, L"AIDA64_SensorValues");
	if (!mmf_handle)
	{
		std::cout << "Handle error\n";
		return 1;
	}
	auto mmf_view = static_cast<char*>(MapViewOfFile(mmf_handle, FILE_MAP_READ, 0, 0, 0));
	if (!mmf_view)
	{
		std::cout << "Cannot map view of file" << GetLastError() << "\n";
		CloseHandle(mmf_handle);
		return -1;
	}

	Info info(mmf_view);
	Utilization bars[17];
	for (auto i = size_t{}; i < 17; ++i)
	{
		bars[i].set_label(info.infos[i].label);
	}
	indicators::MultiProgress<indicators::ProgressBar, 17> bar_obj(bars[0].util_bar, bars[1].util_bar, bars[2].util_bar, bars[3].util_bar, bars[4].util_bar, bars[5].util_bar, bars[6].util_bar, bars[7].util_bar,
		bars[8].util_bar, bars[9].util_bar, bars[10].util_bar, bars[11].util_bar, bars[12].util_bar, bars[13].util_bar, bars[14].util_bar, bars[15].util_bar, bars[16].util_bar);
	do {
		info.update(mmf_view);
		bar_obj.set_progress<0>(info.infos[0].value);
		bar_obj.set_progress<1>(info.infos[1].value);
		bar_obj.set_progress<2>(info.infos[2].value);
		bar_obj.set_progress<3>(info.infos[3].value);
		bar_obj.set_progress<4>(info.infos[4].value);
		bar_obj.set_progress<5>(info.infos[5].value);
		bar_obj.set_progress<6>(info.infos[6].value);
		bar_obj.set_progress<7>(info.infos[7].value);
		bar_obj.set_progress<8>(info.infos[8].value);
		bar_obj.set_progress<9>(info.infos[9].value);
		bar_obj.set_progress<10>(info.infos[10].value);
		bar_obj.set_progress<11>(info.infos[11].value);
		bar_obj.set_progress<12>(info.infos[12].value);
		bar_obj.set_progress<13>(info.infos[13].value);
		bar_obj.set_progress<14>(info.infos[14].value);
		bar_obj.set_progress<15>(info.infos[15].value);
		bar_obj.set_progress<16>(info.infos[16].value);
		std::this_thread::sleep_for(1s);
	} while (1);
}