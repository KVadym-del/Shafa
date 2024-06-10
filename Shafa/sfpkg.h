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

#include "logger.h"
#include "sftypes.h"
#include "sftools.h"
#include "wexception.h"
#include "sfarghelper.h"
#include "sftools.h"

namespace shafa {
	class sfpkg
	{
	public:
		sfpkg() = default;
		sfpkg(std::shared_ptr<sfConfigSetup> configSetup);

		void make_pkg(const std::filesystem::path& folderPath);

        void extract_pkg(const std::filesystem::path& pkgPath, const std::filesystem::path& extractPath);

		void install_pkg(const std::filesystem::path& pkgPath);
		void install_pkg(const std::wstring& pkgName);

	private:
		void make_tar(const std::filesystem::path& folderPath, const std::filesystem::path& tarPath);
		void add_files_to_archive(archive* a, const std::string& dir_path, const std::vector<std::string>& exclusions, const std::string& output_filename);

		void decompress_tar(const std::filesystem::path& pkgPath, const std::filesystem::path& extractPath);
		void extract_file_form_tar(struct archive* a, struct archive* ext, struct archive_entry* entry, const std::filesystem::path& extractPath);

		void decompress_7z(const std::filesystem::path& pkgPath, const std::filesystem::path& extractPath);

		static inline bool should_exclude(const std::string& path, const std::vector<std::string>& exclusions) {
			for (const auto& exclusion : exclusions) {
				if (path.find(exclusion) != std::string::npos) {
					return true;
				}
			}
			return false;
		}

	private:
		std::shared_ptr<sfConfigSetup> m_configSetup;

	};
}
