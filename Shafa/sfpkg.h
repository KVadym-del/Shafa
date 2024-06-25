#pragma once

#include <memory>
#include <filesystem>
#include <fstream>
#include <utility>
#include <functional>
#include <algorithm>

#include <windows.h>

#include <archive.h>
#include <archive_entry.h>
#include <toml++/toml.hpp>

#include "logger.h"
#include "sftypes.h"
#include "sfbuild.h"
#include "sftools.h"
#include "wexception.h"
#include "sfarghelper.h"
#include "sfpkg_configure.h"

namespace shafa {
	class sfpkg
	{
	public:
		sfpkg(sfConfigSetup* configSetup) : m_configSetup(configSetup) {}

		void make_pkg(const std::filesystem::path& folderPath);

		void make_pkg_config();

		void extract_pkg(const std::filesystem::path& pkgPath, const std::filesystem::path& extractPath);

		void install_pkg(const std::filesystem::path& pkgPath);
		void install_pkg(const std::wstring& pkgName);
		void compile_pkg(const std::filesystem::path& pkgConfigPath);

	private:
		void make_tar(const std::filesystem::path& folderPath, const std::filesystem::path& tarPath);
		void add_files_to_archive(struct archive* a, const std::string& dir_path, const std::vector<std::string>& exclusions, const std::string& output_filename);

		void decompress_tar(const std::filesystem::path& pkgPath, const std::filesystem::path& extractPath);
		void extract_file_form_tar(struct archive* a, struct archive* ext, struct archive_entry* entry, const std::filesystem::path& extractPath);

		static inline bool should_exclude(const std::string& path, const std::vector<std::string>& exclusions) {
			for (const auto& exclusion : exclusions) {
				if (path.find(exclusion) != std::string::npos) {
					return true;
				}
			}
			return false;
		}

		static inline std::vector<std::filesystem::path> get_paths(const std::wstring& log_file_path) {
			std::vector<std::filesystem::path> paths;
			std::wifstream log_file(log_file_path);
			if (log_file.is_open()) {
				std::wstring line;
				std::int32_t index = 1;
				while (std::getline(log_file, line)) {
					if (index % 2 != 0 && !line.empty())
						paths.push_back(line);

					index++;
				}
				log_file.close();
				return paths;
			}
		}

	private:
		sfConfigSetup* m_configSetup;

	};
};
