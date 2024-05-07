#pragma once

#include <iostream>
#include <string>

namespace shafa {
	class sfarghelper
	{
	public:
		static inline void help_args(void)
		{
			std::wcout << L"Usage: shafa [command] [options]\n";
			std::wcout << L"Commands:\n";
			std::wcout << L"  configure\tConfigure the project\n";
			std::wcout << L"  build\t\tBuild the project\n";
			std::wcout << L"  run\t\tRun the project\n";
			std::wcout << L"Options:\n";
			std::wcout << L"  version\tShow version\n";
			std::wcout << L"  help\t\tShow help\n";
			std::wcout.flush();
		}

		static inline void help_configure(void)
		{
			std::wcout << L"Usage: shafa configure [options]\n";
			std::wcout << L"Options:\n";
			std::wcout << L"  \"build folder name\"\n";
			std::wcout << L"  help\tShow help\n";
			std::wcout.flush();
		}
	};
}
