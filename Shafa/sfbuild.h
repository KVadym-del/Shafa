#pragma once

#include <Windows.h>

#include <iostream>
#include <string>
#include <filesystem>

#include "logger.h"
#include "sftypes.h"
#include "wexception.h"

namespace shafa {
	class sfbuild
	{
	public:
		inline sfbuild(sfConfigSetup& configSetup) : m_configSetup(configSetup)
		{}

		~sfbuild() = default;

		void build_hub();

	private:
		void compiler_check();
		void linker_check();

		void clang_build();
		void clang_link();

		static inline void ReadFromPipe(HANDLE hPipe)
		{
			char buffer[4096];
			DWORD bytesRead;
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0)
			{
				buffer[bytesRead] = '\0';
				std::wcout << buffer;
			}
		}
		void run_compiler(const std::wstring& command);


	private:
		sfConfigSetup& m_configSetup;

		std::vector<std::wstring> m_compilationCommands;
		std::wstring m_linkingCommand{};

		bool cppNonDefaultCompilerPathExist{ true };
		bool cppNonDefaultLinkerPathExist{ true };
	};
}
