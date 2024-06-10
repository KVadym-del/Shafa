#include "sfconfigure.h"

namespace shafa {
	void sfconfigure::configure_hub(toml::table& sfContTable, std::vector<sfArg> args)
	{
        m_args = std::move(args);
        if (sfContTable.contains("settings"))
        {
            const toml::table& compilation_table = *sfContTable.at("settings").as_table();

            auto it = compilation_table.find("projectName");
            if (it != compilation_table.end())
			{
                LOG_INFO(L"Project name is set to " + string_to_wstring(it->second.as_string()->get()) + L".");
				m_configSetup->projectSettings->projectName = string_to_wstring(it->second.as_string()->get());
			}
			else
				throw wruntime_error(L"Project name not found in TOML file.");

			it = compilation_table.find("projectVersion");
            if (it != compilation_table.end())
            {
                LOG_INFO(L"Project version is set to " + string_to_wstring(it->second.as_string()->get()) + L".");
				m_configSetup->projectSettings->projectVersion = string_to_wstring(it->second.as_string()->get());
			}
            else
                throw wruntime_error(L"Project version not found in TOML file.");

            it = compilation_table.find("projectType");
            if (it != compilation_table.end())
			{
                try
                {
                    m_configSetup->projectSettings->projectType = sfProjectTypeHelper::to_enum(string_to_wstring(it->second.as_string()->get()));
                }
                catch (const wexception& err)
                {
                    throw wruntime_error(err.wwhat());
                }
                LOG_INFO(L"Project type is set to " + string_to_wstring(it->second.as_string()->get()) + L".");
			}
			else
				throw wruntime_error(L"Project type not found in TOML file.");
        }
        else
            throw wruntime_error(L"[settings] section not found in TOML file.");

        if (sfContTable.contains("compilation"))
        {
            const toml::table& compilation_table = *sfContTable.at("compilation").as_table();

            for (const auto& [key, value] : compilation_table)
            {
                if (key.str() == "cppCompilerPath")
                {
                    if (value.as_string()->get() == "auto")
                    {
						LOG_DEBUG(L"Auto C++ compiler search is enabled.")
                        auto compilerPath = foreman_finder();

                        m_configSetup->compilationList->cppCompilerPath = compilerPath;
                        LOG_DEBUG(L"Found C++ compiler: " + m_configSetup->compilationList->cppCompilerPath.wstring());

                        std::wifstream file(m_configSetup->configFilePath);
                        std::filesystem::path tempPath = m_configSetup->configFilePath.parent_path().wstring() + L"tempConfig.toml";
                        std::wofstream tempFile(tempPath);

                        std::int32_t lineToReplace = value.source().begin.line;
                        std::int32_t currentLine = 1;

                        if (file.is_open() && tempFile.is_open()) {
                            std::wstring line;
                            while (std::getline(file, line)) {
                                if (currentLine != lineToReplace) {
                                    tempFile << line << std::endl;
                                }
                                else {
                                    tempFile << L"cppCompilerPath = '" << m_configSetup->compilationList->cppCompilerPath.wstring() << '\'' << std::endl;
                                }
                                currentLine++;
                            }

                            file.close();
                            tempFile.close();

                            std::filesystem::remove(m_configSetup->configFilePath);
                            std::filesystem::rename(tempPath, m_configSetup->configFilePath);
                        }
                        else 
							throw wruntime_error(L"Failed to open config file.");
                    }
                    else
                        m_configSetup->compilationList->cppCompilerPath = string_to_wstring(value.as_string()->get());
                    LOG_INFO(L"C++ compiler path is set to " + string_to_wstring(value.as_string()->get()) + L".")                    
                }
                else if (key.str() == "cppLinkerPath")
                {
                    if (value.as_string()->get() == "auto")
					{
                        LOG_DEBUG(L"Auto C++ linker search is enabled.");
						auto linkerPath = foreman_finder(true);

						m_configSetup->compilationList->cppLinkerPath = linkerPath;
                        LOG_DEBUG(L"Found C++ linker: " + m_configSetup->compilationList->cppLinkerPath.wstring());

						std::wifstream file(m_configSetup->configFilePath);
						std::filesystem::path tempPath = m_configSetup->configFilePath.parent_path().wstring() + L"tempConfig.toml";
						std::wofstream tempFile(tempPath);

						std::int32_t lineToReplace = value.source().begin.line;
						std::int32_t currentLine = 1;

						if (file.is_open() && tempFile.is_open()) {
							std::wstring line;
							while (std::getline(file, line)) {
								if (currentLine != lineToReplace) {
									tempFile << line << std::endl;
								}
								else {
									tempFile << L"cppLinkerPath = '" << m_configSetup->compilationList->cppLinkerPath.wstring() << '\'' << std::endl;
								}
								currentLine++;
							}

							file.close();
							tempFile.close();

							std::filesystem::remove(m_configSetup->configFilePath);
							std::filesystem::rename(tempPath, m_configSetup->configFilePath);
						}
						else
							throw wruntime_error(L"Failed to open config file.");
					}
					else
						m_configSetup->compilationList->cppLinkerPath = string_to_wstring(value.as_string()->get());
                    LOG_INFO(L"C++ linker path is set to " + string_to_wstring(value.as_string()->get()) + L".", LOG_INFO);
                    m_configSetup->compilationList->cppLinkerPath = string_to_wstring(value.as_string()->get());
                }
                else if (key.str() == "cppCompiler" && !(value.as_string()->get() == "clang"))
                {
                    LOG_INFO(L"C++ compiler is set to " + string_to_wstring(value.as_string()->get()) + L".");
                    m_configSetup->compilationList->cppCompiler = sfCppCompilersHelper::to_enum(string_to_wstring(value.as_string()->get()));
                }
                else if (key.str() == "cppVersion" && !(value.as_string()->get() == "c++20"))
                {
                    LOG_INFO(L"C++ version is set to " + string_to_wstring(value.as_string()->get()) + L".");
                    m_configSetup->compilationList->cppVersion = sfCppVersionsHelper::to_enum(string_to_wstring(value.as_string()->get()));
                }
                else if (key.str() == "debugFlags")
                {
                    if (!value.as_array()->is_homogeneous(toml::node_type::string))
                    {
                        throw wruntime_error(L"Debug flags must be a string array.");
                    }
                    for (const auto& flag : *value.as_array())
					{
                        m_configSetup->compilationList->debugFlags += L" " + string_to_wstring(flag.as_string()->get());
					}

                    LOG_INFO(L"Non default debug flags are set to: " + m_configSetup->compilationList->debugFlags);
                }
                else if (key.str() == "releaseFlags")
                {
                    if (!value.as_array()->is_homogeneous(toml::node_type::string))
                    {
                        throw wruntime_error(L"Release flags must be a string array.");
                    }
                    for (const auto& flag : *value.as_array())
                    {
                        m_configSetup->compilationList->releaseFlags += L" " + string_to_wstring(flag.as_string()->get());
                    }

                    LOG_INFO(L"Non default release flags are set to: " + m_configSetup->compilationList->releaseFlags);
                }
                else if (key.str() == "libDirs")
                {
					if (!value.as_array()->is_homogeneous(toml::node_type::string))
					{
						throw wruntime_error(L"Library folders must be a string array.");
					}
					for (const auto& folder : *value.as_array())
					{
						m_configSetup->compilerArgs->cppLibDirs.push_back(string_to_wstring(folder.as_string()->get()));
					}

					LOG_INFO(L"Library folders are set.");
				}
                else if (key.str() == "libs")
                {
					if (!value.as_array()->is_homogeneous(toml::node_type::string))
					{
						throw wruntime_error(L"Libraries must be a string array.");
					}
					for (const auto& lib : *value.as_array())
					{
						m_configSetup->compilerArgs->cppLibs.push_back(string_to_wstring(lib.as_string()->get()));
					}

					LOG_INFO(L"Libraries are set.");
                }
				else if (key.str() == "includeDirs")
				{
					if (!value.as_array()->is_homogeneous(toml::node_type::string))
					{
						throw wruntime_error(L"Include folders must be a string array.");
					}
					for (const auto& folder : *value.as_array())
					{
						m_configSetup->compilerArgs->cppIncludeDirs.push_back(string_to_wstring(folder.as_string()->get()));
					}

					LOG_INFO(L"Include folders are set.");
					}
				else if (key.str() == "projectBuildType" && !(value.as_string()->get() == "debug"))
				{
					LOG_INFO(L"Project build type is set to " + string_to_wstring(value.as_string()->get()) + L".");
					m_configSetup->compilationList->projectBuildType = sfProjectBuildTypeHelper::to_enum(string_to_wstring(value.as_string()->get()));

                }
            }
        }
        else
            throw wruntime_error(L"[compilation] section not found in TOML file.");

        if (sfContTable.contains("configure")) {
            const toml::table& settings_table = *sfContTable.at("configure").as_table();

            for (const auto& [key, value] : settings_table) {
                if (key.str() == "autoSourceSearch" && value.as_boolean()->get() == false)
                {
                    LOG_INFO(L"Auto source search is disabled.");
                    m_configSetup->configList->autoSourceSearch = false;
                }
                else if (key.str() == "multiThreadedBuild" && !(value.as_boolean()->get() == true))
                {
                    LOG_INFO(L"Multi-threaded build is disabled.");
                    m_configSetup->configList->multiThreadedBuild = false;
                }
                else if (key.str() == "buildFolder" && !(value.as_string()->get() == "Build"))
                {
                    LOG_INFO(L"Build folder is set to " + string_to_wstring(value.as_string()->get()) + L".");
                    m_configSetup->configList->buildFolder = string_to_wstring(value.as_string()->get());
                }
                else if (key.str() == "outputReleaseFolder" && !(value.as_string()->get() == "Output/Release"))
                {
                    LOG_INFO(L"Output release folder is set to " + string_to_wstring(value.as_string()->get()) + L".");
                    m_configSetup->configList->outputReleaseFolder = string_to_wstring(value.as_string()->get());
				} else if (key.str() == "outputDebugFolder" && !(value.as_string()->get() == "Output/Debug"))
                {
                    LOG_INFO(L"Output debug folder is set to " + string_to_wstring(value.as_string()->get()) + L".");
                    m_configSetup->configList->outputDebugFolder = string_to_wstring(value.as_string()->get());
				}
            }
        }
        else {
            throw wruntime_error(L"[configure] section not found in TOML file.");
        }
	}
    void sfconfigure::configuration_construct()
    {
        try
        {
            if (!std::filesystem::exists(m_configSetup->configList->buildFolder))
		    {
                LOG_INFO(L"Build folder not found. Creating build folder.");
		    	std::filesystem::create_directories(m_configSetup->configList->buildFolder);
		    }
            if (!std::filesystem::exists(m_configSetup->configList->outputDebugFolder))
            {
                LOG_INFO(L"Debug output folder not found. Creating debug output folder.");
                std::filesystem::create_directories(m_configSetup->configList->outputDebugFolder);
            }
            if (!std::filesystem::exists(m_configSetup->configList->outputReleaseFolder))
		    {
                LOG_INFO(L"Release output folder not found. Creating release output folder.");
		    	std::filesystem::create_directories(m_configSetup->configList->outputReleaseFolder);
		    }

           write_identifiers_to_file();

           logger::log_new_line();
           LOG_INFO(L"Configuration completed.");

        }
        catch (const wexception& err)
        {
            LOG_ERROR(err.wwhat());
        }
    }

    void sfconfigure::write_identifiers_to_file()
    {
        LOG_DEBUG(L"Log path: " + m_configSetup->configList->cacheFilePath.wstring());
        if (!std::filesystem::exists(m_configSetup->configList->cacheFilePath)) {
            LOG_INFO(L"Data.info file not found. Creating data.info file.");
        }
        else
            std::wofstream(m_configSetup->configList->cacheFilePath, std::wios::out | std::wios::trunc).close();

        std::vector<std::pair<std::wstring, std::wstring>> cppFileIdentity;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::current_path()))
        {
            if (
                entry.path().extension() == ".cpp" ||
                entry.path().extension() == ".cxx" ||
                entry.path().extension() == ".h" ||
                entry.path().extension() == ".hpp"
                )
            {
                cppFileIdentity.push_back(std::make_pair(entry.path().wstring(), file_hashing(entry.path().wstring())));
            }
        }

        try
        {
            std::wofstream log_file(m_configSetup->configList->cacheFilePath);
            if (log_file.is_open()) {
                for (const auto& [key, value] : cppFileIdentity)
                {
                    log_file << key << L"\n";
                    log_file << value << std::endl;
                }
                log_file.close();
                return;
            }
        }
        catch (const std::exception& err)
        {
            std::cout << err.what() << std::endl;
        }
        
        throw wruntime_error(L"Failed to open log file.");
    }

    std::filesystem::path sfconfigure::foreman_finder(bool isLinker)
    {
        const DWORD buffSize = 65535;
        static char pathBuf[buffSize];
        if (!GetEnvironmentVariableA("PATH", pathBuf, buffSize))
            throw wexception(L"Failed to get PATH environment variable.");

        std::wstring pathStr(string_to_wstring(pathBuf));
        std::vector<std::wstring> paths;
        size_t start = 0, end;
        while ((end = pathStr.find(';', start)) != std::wstring::npos) {
            paths.emplace_back(pathStr.substr(start, end - start));
            start = end + 1;
        }
        paths.emplace_back(pathStr.substr(start));

        std::wstring compilerName{};
        if (!isLinker)
            switch (m_configSetup->compilationList->cppCompiler)
            {
            case sfCppCompilers::clang:
                compilerName = L"clang++.exe";
                break;
            case sfCppCompilers::msvc:
                compilerName = L"clang-cl.exe";
                break;
            case sfCppCompilers::gcc:
                compilerName = L"g++.exe";
                break;
            default:
                break;
            }
        else
            compilerName = L"lld-link.exe";

        std::vector<std::filesystem::path> compilers;
        for (const auto& path : paths) {
            std::wstring appPath = path + L"\\" + compilerName.c_str();
            DWORD fileAttrs = GetFileAttributes(appPath.c_str());
            if (fileAttrs != INVALID_FILE_ATTRIBUTES && !(fileAttrs & FILE_ATTRIBUTE_DIRECTORY))
                compilers.emplace_back(appPath);
        }

        if (compilers.empty())
            throw wexception(L"Failed to find C++ compiler in PATH environment variable.");

        std::filesystem::path compilerPath;
        if (compilers.size() >= 2)
        {
            std::uint16_t index = sftools::make_option(compilers);
            compilerPath = compilers[index];
        }
        else
            compilerPath = compilers[0];

        return compilerPath;
    }
}
