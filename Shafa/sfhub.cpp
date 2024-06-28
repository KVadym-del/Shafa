#include "sfhub.h"
namespace shafa {
	void sfhub::non_configured_build()
	{
		
		std::vector<std::pair<std::filesystem::path, std::wstring>> cppFileIdentity;
		for (auto entry = std::filesystem::recursive_directory_iterator(std::filesystem::current_path());
			entry != std::filesystem::recursive_directory_iterator(); ++entry)
		{
			if (entry->path() == m_configSetup->configList->pkgFolder) {
				entry.disable_recursion_pending();
				continue;
			}

			if (
				entry->path().extension() == ".cpp" ||
				entry->path().extension() == ".cxx" ||
				entry->path().extension() == ".cc" ||
				entry->path().extension() == ".h" ||
				entry->path().extension() == ".hpp"
				)
			{
				cppFileIdentity.push_back(std::make_pair(entry->path(), file_hashing(entry->path().wstring())));
			}
		}
		std::vector<std::wstring> hashes;
		for (const auto& pair : cppFileIdentity) {
			hashes.push_back(pair.second);
		}
		std::vector <std::wstring> oldCppFileIdentity = get_hashes(m_configSetup->configList->buildFolder.wstring() + L"\\data.info");
        if (!sftools::compare_vectors<std::wstring>(oldCppFileIdentity, hashes) ||
            ![&]() {
                switch (m_configSetup->compilationList->projectBuildType) {
                    case sfProjectBuildType::debug:
                        switch (m_configSetup->projectSettings->projectType) {
                            case sfProjectType::application:
                                return std::filesystem::exists(m_configSetup->configList->outputDebugFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".exe");
                            case sfProjectType::staticLibrary:
                                return std::filesystem::exists(m_configSetup->configList->outputDebugFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".lib");
                            case sfProjectType::dynamicLibrary:
                                return std::filesystem::exists(m_configSetup->configList->outputDebugFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".dll");
                        }
                        break;
                    case sfProjectBuildType::release:
                        switch (m_configSetup->projectSettings->projectType) {
                            case sfProjectType::application:
                                return std::filesystem::exists(m_configSetup->configList->outputReleaseFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".exe");
                            case sfProjectType::staticLibrary:
                                return std::filesystem::exists(m_configSetup->configList->outputReleaseFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".lib");
                            case sfProjectType::dynamicLibrary:
                                return std::filesystem::exists(m_configSetup->configList->outputReleaseFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".dll");
                        }
                        break;
                }
                return false;
            }())
        {
			std::vector<std::filesystem::path> paths;
			for (const auto& pair : cppFileIdentity) {
				if (pair.first.extension() != ".h" || pair.first.extension() != ".hpp")
				{
					paths.push_back(pair.first);
				}
			}
			m_configSetup->compilerArgs->cppFiles = paths;
			try
			{
				build_hub();
			}
			catch (const wexception& err)
			{
				throw err;
			}

		}
		else
		{
			LOG_INFO(L"Files have not changed. Skipping build.");
		}
	}
}
		