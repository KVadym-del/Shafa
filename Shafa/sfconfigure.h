#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <utility>

#include <toml++/toml.hpp>
#include <openssl/evp.h>

#include "logger.h"
#include "sftypes.h"
#include "wexception.h"
#include "sfarghelper.h"

namespace shafa {
	class sfconfigure
	{
    public:
        sfconfigure(sfConfigSetup& configSetup) : m_configSetup(configSetup) 
        {
            m_configSetup.configList.buildFolder = std::filesystem::current_path().wstring() + m_configSetup.configList.buildFolder.wstring();
            m_configSetup.configList.outputDebugFolder = std::filesystem::current_path().wstring() + m_configSetup.configList.outputDebugFolder.wstring();
            m_configSetup.configList.outputReleaseFolder = std::filesystem::current_path().wstring() + m_configSetup.configList.outputReleaseFolder.wstring();
            m_configSetup.configList.cacheFilePath = std::filesystem::current_path().wstring() + m_configSetup.configList.cacheFilePath.wstring();

            m_configSetup.compilationList.defaultClangCompilerPath = 
                m_configSetup.shafaRootPath.generic_wstring() + m_configSetup.compilationList.defaultClangCompilerPath.generic_wstring();
            m_configSetup.compilationList.defaultClangLinkerPath = 
                m_configSetup.shafaRootPath.generic_wstring() + m_configSetup.compilationList.defaultClangLinkerPath.generic_wstring();

            logger::log(L"Build folder: " + m_configSetup.configList.buildFolder.wstring());
            logger::log(L"Output debug folder: " + m_configSetup.configList.outputDebugFolder.wstring());
            logger::log(L"Output release folder: " + m_configSetup.configList.outputReleaseFolder.wstring());
            logger::log(L"Default clang compiler path: " + m_configSetup.compilationList.defaultClangCompilerPath.wstring());
            logger::log(L"Default clang linker path: " + m_configSetup.compilationList.defaultClangLinkerPath.wstring() + L'\n');

        }

	public:
		void configure_hub(toml::table& sfContTable, std::vector<sfArg> args);

		void configuration_construct();

		void write_identifiers_to_file();
		
        static inline std::wstring file_hashing(const std::wstring& file_path) {
            unsigned char digest[EVP_MAX_MD_SIZE];
            std::ifstream file(file_path, std::ios::binary);

            if (!file) {
               throw wruntime_error(L"File does not exist.");
            }

            const EVP_MD* md5_digest = EVP_md5();
            unsigned int md5_len;

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

            for (unsigned int i = 0; i < md5_len; i++)
                ss << std::hex << std::setw(2) << std::setfill(L'0') << static_cast<int>(digest[i]);
             
            return ss.str();
        }

        static inline std::vector<std::wstring> get_hashes(const std::wstring& log_file_path) {
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

    private:
        sfConfigSetup& m_configSetup;
        std::vector<sfArg> m_args;

	};
}
