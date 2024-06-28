#pragma once

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <utility>
#include <Windows.h>

#include <toml++/toml.hpp>
#include <openssl/evp.h>

#include "logger.h"
#include "sftypes.h"
#include "wexception.h"
#include "sfarghelper.h"
#include "sftools.h"

namespace shafa {
	class sfpkg_configure
	{
    public:
        sfpkg_configure(sfConfigSetup* pkgConfigSetup) : m_pkgConfigSetup(pkgConfigSetup)
        {
            m_pkgConfigSetup->configList->buildFolder = m_pkgConfigSetup->configFilePath.parent_path().wstring() + m_pkgConfigSetup->configList->buildFolder.wstring();
            m_pkgConfigSetup->configList->outputFolder = m_pkgConfigSetup->configFilePath.parent_path().wstring() + m_pkgConfigSetup->configList->outputFolder.wstring();
            m_pkgConfigSetup->configList->outputDebugFolder = m_pkgConfigSetup->configFilePath.parent_path().wstring() + m_pkgConfigSetup->configList->outputDebugFolder.wstring();
            m_pkgConfigSetup->configList->outputReleaseFolder = m_pkgConfigSetup->configFilePath.parent_path().wstring() + m_pkgConfigSetup->configList->outputReleaseFolder.wstring();
            m_pkgConfigSetup->configList->cacheFilePath = m_pkgConfigSetup->configFilePath.parent_path().wstring() + m_pkgConfigSetup->configList->cacheFilePath.wstring();
			m_pkgConfigSetup->configList->pkgFolder = m_pkgConfigSetup->configFilePath.parent_path().wstring() + m_pkgConfigSetup->configList->pkgFolder.wstring();

            m_pkgConfigSetup->compilationList->defaultClangCompilerPath =
                m_pkgConfigSetup->shafaRootPath.generic_wstring() + m_pkgConfigSetup->compilationList->defaultClangCompilerPath.generic_wstring();
            m_pkgConfigSetup->compilationList->defaultClangLinkerPath =
                m_pkgConfigSetup->shafaRootPath.generic_wstring() + m_pkgConfigSetup->compilationList->defaultClangLinkerPath.generic_wstring();
			m_pkgConfigSetup->compilationList->defaultClangLibLinkerPath =
				m_pkgConfigSetup->shafaRootPath.generic_wstring() + m_pkgConfigSetup->compilationList->defaultClangLibLinkerPath.generic_wstring();

			LOG_DEBUG(L"Pkg Build folder: " + m_pkgConfigSetup->configList->buildFolder.wstring())
			LOG_DEBUG(L"Pkg Cache file path: " + m_pkgConfigSetup->configList->cacheFilePath.wstring())
			LOG_DEBUG(L"Pkg Output debug folder: " + m_pkgConfigSetup->configList->outputDebugFolder.wstring())
			LOG_DEBUG(L"Pkg Output release folder: " + m_pkgConfigSetup->configList->outputReleaseFolder.wstring())
			LOG_DEBUG(L"Pkg Default clang compiler path: " + m_pkgConfigSetup->compilationList->defaultClangCompilerPath.wstring())
			LOG_DEBUG(L"Pkg Default clang linker path: " + m_pkgConfigSetup->compilationList->defaultClangLinkerPath.wstring())
			LOG_DEBUG(L"Pkg Default clang lib linker path: " + m_pkgConfigSetup->compilationList->defaultClangLibLinkerPath.wstring())
        }

	public:
		void pkg_configure_hub(toml::table& sfContTable);

		void pkg_configuration_construct();

		void write_identifiers_to_file_for_pkg();

        std::filesystem::path foreman_finder_for_pkg(bool isLinker = false);
		
        static inline std::wstring file_hashing_for_pkg(const std::wstring& file_path) {
            unsigned char digest[EVP_MAX_MD_SIZE];
            std::ifstream file(file_path, std::ios::binary);

            if (!file) {
               throw wruntime_error(L"File does not exist.");
            }

            const EVP_MD* md5_digest = EVP_md5();
            std::uint32_t md5_len;

            EVP_MD_CTX* md5_ctx = EVP_MD_CTX_new();

            EVP_DigestInit_ex(md5_ctx, md5_digest, nullptr);

            constexpr size_t buffer_size = 4096;
            char buffer[buffer_size];

            while (file.read(buffer, buffer_size)) {
                EVP_DigestUpdate(md5_ctx, buffer, file.gcount());
            }

            if (file.gcount() > 0) {
                EVP_DigestUpdate(md5_ctx, buffer, file.gcount());
            }

            EVP_DigestFinal_ex(md5_ctx, digest, &md5_len);

            EVP_MD_CTX_free(md5_ctx);

            
            std::wstringstream ss;

            for (std::uint32_t i = 0; i < md5_len; i++)
                ss << std::hex << std::setw(2) << std::setfill(L'0') << static_cast<std::int32_t>(digest[i]);
             
            return ss.str();
        }

        static inline std::vector<std::wstring> get_hashes_for_pkg(const std::wstring& log_file_path) {
			std::vector<std::wstring> hashes;
			std::wifstream log_file(log_file_path);
			if (log_file.is_open()) {
				std::wstring line;
                std::int32_t index = 1;
				while (std::getline(log_file, line)) {
                    if (index % 2 == 0 && !line.empty())
					    hashes.push_back(line);

                    index++;
				}
				log_file.close();
				return hashes;
			}
			throw wruntime_error(L"Failed to open log file.");
		}

        inline void non_configured_build()
        {
            std::vector<std::pair<std::filesystem::path, std::wstring>> cppFileIdentity;
			for (const auto& entry : std::filesystem::recursive_directory_iterator(m_pkgConfigSetup->configFilePath.parent_path()))
            {
                if (
                    entry.path().extension() == ".cpp" ||
                    entry.path().extension() == ".cxx" ||
                    entry.path().extension() == ".cc" ||
                    entry.path().extension() == ".h" ||
                    entry.path().extension() == ".hpp"
                    )
                {
                    cppFileIdentity.push_back(std::make_pair(entry.path(), file_hashing_for_pkg(entry.path().wstring())));
                }
            }
            std::vector<std::wstring> hashes;
            for (const auto& pair : cppFileIdentity) {
                hashes.push_back(pair.second);
            }
            std::vector <std::wstring> oldCppFileIdentity = get_hashes_for_pkg(m_pkgConfigSetup->configList->buildFolder.wstring() + L"\\data.info");
            if (!sftools::compare_vectors<std::wstring>(oldCppFileIdentity, hashes) ||
                ![&]() {
                    switch (m_pkgConfigSetup->compilationList->projectBuildType) {
                    case sfProjectBuildType::debug:
                        switch (m_pkgConfigSetup->projectSettings->projectType) {
                        case sfProjectType::application:
                            return std::filesystem::exists(m_pkgConfigSetup->configList->outputDebugFolder.wstring() + L"\\" + m_pkgConfigSetup->projectSettings->projectName + L".exe");
                        case sfProjectType::staticLibrary:
                            return std::filesystem::exists(m_pkgConfigSetup->configList->outputDebugFolder.wstring() + L"\\" + m_pkgConfigSetup->projectSettings->projectName + L".lib");
                        case sfProjectType::dynamicLibrary:
                            return std::filesystem::exists(m_pkgConfigSetup->configList->outputDebugFolder.wstring() + L"\\" + m_pkgConfigSetup->projectSettings->projectName + L".dll");
                        }
                        break;
                    case sfProjectBuildType::release:
                        switch (m_pkgConfigSetup->projectSettings->projectType) {
                        case sfProjectType::application:
                            return std::filesystem::exists(m_pkgConfigSetup->configList->outputReleaseFolder.wstring() + L"\\" + m_pkgConfigSetup->projectSettings->projectName + L".exe");
                        case sfProjectType::staticLibrary:
                            return std::filesystem::exists(m_pkgConfigSetup->configList->outputReleaseFolder.wstring() + L"\\" + m_pkgConfigSetup->projectSettings->projectName + L".lib");
                        case sfProjectType::dynamicLibrary:
                            return std::filesystem::exists(m_pkgConfigSetup->configList->outputReleaseFolder.wstring() + L"\\" + m_pkgConfigSetup->projectSettings->projectName + L".dll");
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
                m_pkgConfigSetup->compilerArgs->cppFiles = paths;
            }
            else
            {
                LOG_INFO(L"Files have not changed. Skipping build.");
            }
        }

    private:
        sfConfigSetup* m_pkgConfigSetup;

	};
}
