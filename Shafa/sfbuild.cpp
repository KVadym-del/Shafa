#include "sfbuild.h"

namespace shafa {
	void sfbuild::build_hub()
	{
		compiler_check();
		linker_check();
		switch (m_configSetup.compilationList.cppCompiler)
		{
		case sfCppCompilers::clang:
			clang_build();
			logger::log(L"Compilation command: \n" + [&]() 
				{
					std::wstring fullCompilationCommand{};
					for (const auto& compilationCommand : m_compilationCommands)
						fullCompilationCommand += L"\t" + compilationCommand + L"\n";

					return fullCompilationCommand;
				}()
			);
			if (m_configSetup.projectSettings.projectType != sfProjectType::dynamicLibrary)
				clang_link();
			logger::log(L"Linking command: \n\t" + m_linkingCommand);
			break;
		case sfCppCompilers::msvc:
			break;
		case sfCppCompilers::gcc:
			break;
		}
	}

	void sfbuild::compiler_check()
	{
		if (!std::filesystem::exists(this->m_configSetup.compilationList.cppCompilerPath))
		{
			cppNonDefaultCompilerPathExist = false;
			if (!std::filesystem::exists(this->m_configSetup.compilationList.defaultClangCompilerPath))
			{
				throw wexception(
					std::wstring(L"Neither the specified C++ compiler path nor the default path exists. Please check the path in the configuration file.\n")
					+ L"\tDefault Path: " + this->m_configSetup.compilationList.defaultClangCompilerPath.wstring()
					+ L"\n\tSpecified: " + this->m_configSetup.compilationList.cppCompilerPath.wstring()
				);
			}
		}
	}

	void sfbuild::linker_check()
	{
		if (!std::filesystem::exists(this->m_configSetup.compilationList.cppLinkerPath))
		{
			cppNonDefaultLinkerPathExist = false;
			if (!std::filesystem::exists(this->m_configSetup.compilationList.defaultClangLinkerPath))
			{
				throw wexception(
					std::wstring(L"Neither the specified C++ linker path nor the default path exists. Please check the path in the configuration file.\n")
					+ L"\tDefault Path: " + this->m_configSetup.compilationList.defaultClangLinkerPath.wstring()
					+ L"\n\tSpecified: " + this->m_configSetup.compilationList.cppLinkerPath.wstring()
				);
			}
		}
	}

	void sfbuild::clang_build()
	{
		std::wstring compiler{};
		if (cppNonDefaultCompilerPathExist == false)
		{
			logger::log(L"Using default C++ compiler path: " + m_configSetup.compilationList.defaultClangCompilerPath.wstring());
			compiler += m_configSetup.compilationList.defaultClangCompilerPath.wstring();
		}
		else
		{
			logger::log(L"Using specified C++ compiler path: " + m_configSetup.compilationList.cppCompilerPath.wstring());
			compiler += m_configSetup.compilationList.cppCompilerPath.wstring();
		}
		std::wstring compilationCommand{ compiler };
		compilationCommand += L" -std=" + sfCppVersionsHelper::to_string(m_configSetup.compilationList.cppVersion);

		switch (m_configSetup.projectSettings.projectType)
		{
		case sfProjectType::application: case sfProjectType::staticLibrary:
		{
			for (const auto& file : m_configSetup.compilerArgs.cppFiles)
			{
				compilationCommand += L" \"" + file.wstring() + L"\"";

				compilationCommand += L" -c -o \"" + m_configSetup.configList.outputDebugFolder.wstring() + L"\\";
				if (file.extension() == ".h" || file.extension() == ".hpp")
					compilationCommand += file.filename().replace_extension(".pch").wstring() + L"\"";
				else
					compilationCommand += file.filename().replace_extension(".obj").wstring() + L"\"";

				m_compilationCommands.push_back(compilationCommand);
			}
			break;
		}
		case sfProjectType::dynamicLibrary:
		{
			compilationCommand += L" -shared -undefined dynamic_lookup";
			for (const auto& file : m_configSetup.compilerArgs.cppFiles)
			{
				if (file.extension() != ".h" && file.extension() != ".hpp")
					compilationCommand += L" \"" + file.wstring() + L"\"";
			}
			compilationCommand += L" -o \"" + m_configSetup.configList.outputDebugFolder.wstring() + L"\\" + m_configSetup.projectSettings.projectName;
			compilationCommand += L".dll\"";

			m_compilationCommands.push_back(compilationCommand);
			break;
		}
		}
		
		for (const auto& compilationCommand : m_compilationCommands)
			run_compiler(compilationCommand);
	}

	void sfbuild::clang_link()
	{
		if (m_configSetup.projectSettings.projectType != sfProjectType::dynamicLibrary)
		{
			if (cppNonDefaultLinkerPathExist == false)
			{
				logger::log(L"Using default C++ linker path: " + m_configSetup.compilationList.defaultClangLinkerPath.wstring());
				m_linkingCommand += m_configSetup.compilationList.defaultClangLinkerPath.wstring();
			}
			else
			{
				logger::log(L"Using specified C++ linker path: " + m_configSetup.compilationList.cppLinkerPath.wstring());
				m_linkingCommand += m_configSetup.compilationList.cppLinkerPath.wstring();
			}

			m_linkingCommand += L" /defaultlib:libcmt";

			m_linkingCommand += L" -out:\"" + m_configSetup.configList.outputDebugFolder.wstring() + L"\\" + m_configSetup.projectSettings.projectName;
			switch (m_configSetup.projectSettings.projectType)
			{
			case sfProjectType::application:
				m_linkingCommand += L".exe\"";
				break;
			case sfProjectType::staticLibrary:
				m_linkingCommand += L".lib\"";
				break;
			default:
				break;
			}

			for (const auto& entry : std::filesystem::recursive_directory_iterator(m_configSetup.configList.outputDebugFolder)) {
				if (std::filesystem::is_regular_file(entry)) {

					std::wstring extension = entry.path().extension().wstring();

					if (extension == L".obj") {
						m_linkingCommand += L" \"" + entry.path().wstring() + L"\"";
					}
				}
			}

			run_compiler(m_linkingCommand);
		}
	}

	void sfbuild::run_compiler(const std::wstring& command)
	{
		SECURITY_ATTRIBUTES saAttr;
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = nullptr;

		HANDLE hOutputRead, hOutputWrite;
		CreatePipe(&hOutputRead, &hOutputWrite, &saAttr, 0);
		SetHandleInformation(hOutputRead, HANDLE_FLAG_INHERIT, 0);

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.hStdError = hOutputWrite;
		si.hStdOutput = hOutputWrite;
		si.dwFlags |= STARTF_USESTDHANDLES;

		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

		BOOL success = CreateProcess(nullptr, (LPWSTR)command.c_str(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi);

		if (success)
		{
			CloseHandle(hOutputWrite);
			ReadFromPipe(hOutputRead);
			CloseHandle(hOutputRead);
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		else
		{
			throw wexception(L"Failed to create process.");
		}
	}
}
