#pragma once

#include <iostream>
#include <string>

namespace shafa {
	class sfarghelper
	{
	public:
		static inline void help_args(void)
		{
			std::wcout << L"Usage: shafa [command]\n";
			std::wcout << L"Commands:\n";
			std::wcout << L"  configure\tConfigure the project\n";
			std::wcout << L"  build\t\tBuild the project\n";
			std::wcout << L"  run\t\tRun the project(unsupported)\n";
			std::wcout << L"  init\t\tInitialize the project(unsupported)\n";
			std::wcout << L"  pkg\t\tPackage the project\n";
			std::wcout << L"  help\t\tShow help\n";
			std::wcout.flush();
		}
		
		static inline void help_configure(void)
		{
			std::wcout << L"Usage: shafa configure\n";
			std::wcout << L"Options:\n";
			std::wcout << L"  help\tShow help\n";
			std::wcout.flush();
		}

		static inline void help_build(void)
		{
			std::wcout << L"Usage: shafa build [command]\n";
			std::wcout << L"Options:\n";
			std::wcout << L"  help\t\tShow help\n";
			std::wcout << L"  debug\t\tBuild the project in debug mode\n";
			std::wcout << L"  release\t\tBuild the project in release mode\n";
			std::wcout.flush();
		}

		static inline void help_run(void)
		{
			std::wcout << L"Usage: shafa run\n";
			std::wcout << L"Options:\n";
			std::wcout << L"  help\t\tShow help\n";
			std::wcout.flush();
		}

		static inline void help_pkg(void)
		{
			std::wcout << L"Usage: shafa pkg\n";
			std::wcout << L"Options:\n";
			std::wcout << L"  help\t\tShow help\n";
			std::wcout << L"  make\t\tPackage the project\n";
			std::wcout << L"  install (path to sfpkg)\tInstall the package\n";
			std::wcout.flush();
		}
	};
}
