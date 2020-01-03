#include <Windows.h>
#include <iostream>
#include <string>
#include <string_view>
#include <cstring>
#include <algorithm>
#include <iterator>


int main()
{
	auto mmf_handle = OpenFileMapping(FILE_MAP_READ, FALSE, L"AIDA64_SensorValues");
	if(!mmf_handle)
	{
		std::cout << "Handle error\n";
		return 1;
	}
	auto mmf_view = static_cast<char*>(MapViewOfFile(mmf_handle, FILE_MAP_READ, 0, 0, 0));
	if(!mmf_view)
	{
		std::cout << "Cannot map view of file" << GetLastError() << "\n";
		CloseHandle(mmf_handle);
		return -1;
	}

	auto current_label_begin = strstr(mmf_view, "<label>");
	do {
		current_label_begin += strlen("<label>");
		auto current_label_end = strstr(current_label_begin, "</label>");		
		auto current_value_begin = strstr(current_label_begin, "<value>") + strlen("<value>");
		auto current_value_end = strstr(current_label_begin, "</value>");
		std::copy(current_label_begin, current_label_end, std::ostream_iterator<char>(std::cout));
		std::cout << '\t';
		std::copy(current_value_begin, current_value_end, std::ostream_iterator<char>(std::cout));
		std::cout << '\n';
		if (*(current_value_end + strlen("</value>")) == '\0')
			break;
		current_label_begin = strstr(current_value_end, "<label>");
	} while (current_label_begin != NULL);
}