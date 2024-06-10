#include "sfbuild.h"

namespace shafa {
	void sfbuild::build_hub()
	{
#ifdef _DEBUG
		logger::log_new_line();
#endif // _DEBUG

		compiler_check();
		linker_check();
		switch (m_configSetup->compilationList->cppCompiler)
		{
		case sfCppCompilers::clang:
			clang_build();
			LOG_DEBUG(L"Compilation command: \n" + [&]()
				{
					std::wstring fullCompilationCommand{};
					for (const auto& compilationCommand : m_compilationCommands)
						fullCompilationCommand += L"\t" + compilationCommand + L"\n";

					return fullCompilationCommand;
				}());
			if (m_configSetup->projectSettings->projectType != sfProjectType::dynamicLibrary)
				clang_link();
			LOG_DEBUG(L"Linking command: \n\t" + m_linkingCommand);
			break;
		case sfCppCompilers::msvc:
			break;
		case sfCppCompilers::gcc:
			break;
		}
	}

	void sfbuild::compiler_check()
	{
		if (!std::filesystem::exists(this->m_configSetup->compilationList->cppCompilerPath))
		{
			cppNonDefaultCompilerPathExist = false;
			if (!std::filesystem::exists(this->m_configSetup->compilationList->defaultClangCompilerPath))
			{
				logger::log_new_line();
				throw wexception(
					std::wstring(L"Neither the specified C++ compiler path nor the default path exists. Please check the path in the configuration file.\n")
					+ L"\tDefault Path: " + this->m_configSetup->compilationList->defaultClangCompilerPath.wstring()
					+ L"\n\tSpecified: " + this->m_configSetup->compilationList->cppCompilerPath.wstring()
				);
			}
		}
	}

	void sfbuild::linker_check()
	{
		if (!std::filesystem::exists(this->m_configSetup->compilationList->cppLinkerPath))
		{
			cppNonDefaultLinkerPathExist = false;
			if (!std::filesystem::exists(this->m_configSetup->compilationList->defaultClangLinkerPath))
			{
				logger::log_new_line();
				throw wexception(
					std::wstring(L"Neither the specified C++ linker path nor the default path exists. Please check the path in the configuration file.\n")
					+ L"\tDefault Path: " + this->m_configSetup->compilationList->defaultClangLinkerPath.wstring()
					+ L"\n\tSpecified: " + this->m_configSetup->compilationList->cppLinkerPath.wstring()
				);
			}
		}
	}

	void sfbuild::clang_build()
	{
		FunctStats functionStats{ __FUNCTIONW__, __FILEW__ };
		std::wstring compiler{};
		if (cppNonDefaultCompilerPathExist == false)
		{
			LOG_DEBUG(L"Using default C++ compiler path: " + m_configSetup->compilationList->defaultClangCompilerPath.wstring());
			compiler += m_configSetup->compilationList->defaultClangCompilerPath.wstring();
		}
		else
		{
			LOG_DEBUG(L"Using specified C++ compiler path: " + m_configSetup->compilationList->cppCompilerPath.wstring());
			compiler += m_configSetup->compilationList->cppCompilerPath.wstring();
		}
		std::wstring compilationCommand{ compiler };
		compilationCommand += L" -std=" + sfCppVersionsHelper::to_string(m_configSetup->compilationList->cppVersion);

		std::wstring buildTypeCommand{};
		std::wstring folderOutputCommand{};

		switch (m_configSetup->compilationList->projectBuildType)
		{
		case sfProjectBuildType::debug:
			buildTypeCommand += m_configSetup->compilationList->debugFlags;
			folderOutputCommand += m_configSetup->configList->outputDebugFolder.wstring() + L"\\";
			LOG_DEBUG(L"Using debug flags: " + buildTypeCommand);
			break;
		case sfProjectBuildType::release:
			buildTypeCommand += m_configSetup->compilationList->releaseFlags;
			folderOutputCommand += m_configSetup->configList->outputReleaseFolder.wstring() + L"\\";
			LOG_DEBUG(L"Using release flags: " + buildTypeCommand);
			break;
		}

		compilationCommand += buildTypeCommand;

		for (const std::filesystem::path headerDir : m_configSetup->compilerArgs->cppIncludeDirs)
			compilationCommand += L" -I\"" + headerDir.wstring() + L"\"";

		switch (m_configSetup->projectSettings->projectType)
		{
		case sfProjectType::application: case sfProjectType::staticLibrary:
		{
			for (const auto& file : m_configSetup->compilerArgs->cppFiles)
			{
				compilationCommand += L" \"" + file.wstring() + L"\"";

				compilationCommand += L" -c -o \"" + folderOutputCommand;
				if (file.extension() == ".h" || file.extension() == ".hpp")
					compilationCommand += file.filename().replace_extension(".pch").wstring() + L"\"";
				else
					compilationCommand += file.filename().replace_extension(".obj").wstring() + L"\"";

				m_compilationCommands.push_back(compilationCommand);
				compilationCommand.clear();
				compilationCommand += compiler;
				compilationCommand += L" -std=" + sfCppVersionsHelper::to_string(m_configSetup->compilationList->cppVersion);
			}
			break;
		}
		case sfProjectType::dynamicLibrary:
		{
			compilationCommand += L" -shared -undefined dynamic_lookup";
			for (const auto& file : m_configSetup->compilerArgs->cppFiles)
			{
				if (file.extension() != ".h" && file.extension() != ".hpp")
					compilationCommand += L" \"" + file.wstring() + L"\"";
			}
			compilationCommand += L" -o \"" + folderOutputCommand + m_configSetup->projectSettings->projectName;
			compilationCommand += L".dll\"";

			m_compilationCommands.push_back(compilationCommand);
			break;
		}
		}
		
		std::vector<std::future<BOOL>> processLaunchers;
		LOG_DEBUG(L"Current thread id" + std::to_wstring(GetCurrentThreadId()));
		for (const auto& compilationCommand : m_compilationCommands)
			processLaunchers.emplace_back(run_compiler(compilationCommand));

		bool result = true;
		for (auto& launcher : processLaunchers) {
			BOOL success = launcher.get();
			if (success == false)
				result = false;
			LOG_DEBUG((success ? L"Build process launched successfully" : L"Build failed to launch process"));
		}

		logger::log_new_line();
		if (result == false)
			throw wexception(L"Build failed to launch process.");
		else
			LOG_INFO(L"Build process launched successfully");
	}

	void sfbuild::clang_link()
	{
		if (m_configSetup->projectSettings->projectType != sfProjectType::dynamicLibrary)
		{
			if (cppNonDefaultLinkerPathExist == false)
			{
				LOG_DEBUG(L"Using default C++ linker path: " + m_configSetup->compilationList->defaultClangLinkerPath.wstring());
				m_linkingCommand += m_configSetup->compilationList->defaultClangLinkerPath.wstring();
			}
			else
			{
				LOG_DEBUG(L"Using specified C++ linker path: " + m_configSetup->compilationList->cppLinkerPath.wstring());
				m_linkingCommand += m_configSetup->compilationList->cppLinkerPath.wstring();
			}


			if (m_configSetup->compilationList->projectBuildType == sfProjectBuildType::debug)
				m_linkingCommand += L" /defaultlib:libucrtd";
			else
				m_linkingCommand += L" /defaultlib:libucrt";

			for (const std::filesystem::path dir :  m_configSetup->compilerArgs->cppLibDirs)
				m_linkingCommand += L" /LIBPATH:\"" + dir.wstring() + L"\"";

			for (const std::filesystem::path lib : m_configSetup->compilerArgs->cppLibs)
				m_linkingCommand += L" /DEFAULTLIB:\"" + lib.wstring() + L"\"";

			std::wstring folderOutputCommand{};

			switch (m_configSetup->compilationList->projectBuildType)
			{
			case sfProjectBuildType::debug:
				folderOutputCommand += m_configSetup->configList->outputDebugFolder.wstring() + L"\\";
				break;
			case sfProjectBuildType::release:
				folderOutputCommand += m_configSetup->configList->outputReleaseFolder.wstring() + L"\\";
				break;
			}

			m_linkingCommand += L" -out:\"" + folderOutputCommand + m_configSetup->projectSettings->projectName;

			switch (m_configSetup->projectSettings->projectType)
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

			for (const auto& entry : std::filesystem::recursive_directory_iterator(folderOutputCommand)) {
				if (std::filesystem::is_regular_file(entry)) {

					std::wstring extension = entry.path().extension().wstring();

					if (extension == L".obj") {
						m_linkingCommand += L" \"" + entry.path().wstring() + L"\"";
					}
				}
			}

			bool result = true;
			BOOL success = run_compiler(m_linkingCommand).get();
			if (success == false)
				result = false;
			LOG_DEBUG((success ? L"Link process launched successfully" : L"Link failed to launch process"));

			if (result == false)
				throw wexception(L"Link failed to launch process.");
			else
				LOG_INFO(L"Link process launched successfully");
		}
	}

	std::future<BOOL> sfbuild::run_compiler(const std::wstring& command)
	{
		std::launch launchType;
		if (m_configSetup->configList->multiThreadedBuild == true)
			launchType = std::launch::async;
		else
			launchType = std::launch::deferred;

		return std::async(launchType, [command]() {
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
				LOG_DEBUG(L"Process id: " + std::to_wstring(pi.dwProcessId));

				return success;
			}
			else
			{
				throw wexception(L"Failed to create process.");
			}
			});
	}
}
