#include "sfpkg.h"

namespace shafa {
	void sfpkg::make_pkg(const std::filesystem::path& folderPath)
	{
        if (m_configSetup->projectSettings->projectType == sfProjectType::application)
			throw wexception(L"Cannot create package for application project");

        const std::filesystem::path tarPath{
            m_configSetup->configList->outputFolder.wstring() + L"\\" +
            m_configSetup->projectSettings->projectName + L"-" + m_configSetup->projectSettings->projectVersion + L".sfpkg"
        };
        make_tar(folderPath, tarPath);
	}

    void sfpkg::make_pkg_config()
    {
		std::ofstream configFile;
        std::filesystem::path tempPkgConfigPath = std::filesystem::current_path().wstring() + L"\\pkgConfig.toml";
        configFile.open(tempPkgConfigPath, std::ios::out | std::ios::trunc);
		if (!configFile.is_open())
			throw wexception(L"Could not open file");

        toml::table pkgConfigTable{
            {"settings", toml::table {
                {"projectName", m_configSetup->projectSettings->projectName},
                {"projectVersion", m_configSetup->projectSettings->projectVersion},
                {"projectType", sfProjectTypeHelper::to_string(m_configSetup->projectSettings->projectType)}
                },
			},
			{"compilation", toml::table {
				{"cppCompilerPath", "auto"},
				{"cppLinkerPath", "auto"},
				{"cppLibLinkerPath", "auto"},
                {"cppVersion", sfCppVersionsHelper::to_string(m_configSetup->compilationList->cppVersion)},
                {"cppCompiler", sfCppCompilersHelper::to_string(m_configSetup->compilationList->cppCompiler)},
                {"debugFlags", sftools::wstring_to_toml_array(m_configSetup->compilationList->debugFlags)},
				{"releaseFlags", sftools::wstring_to_toml_array(m_configSetup->compilationList->releaseFlags)},
				},
			},
            {"configure", toml::table {}}
        };

		configFile << pkgConfigTable;
		configFile.close();
    }

	void sfpkg::extract_pkg(const std::filesystem::path& pkgPath, const std::filesystem::path& extractPath)
	{
        std::filesystem::path purePkgPath = sftools::remove_char(pkgPath, '\"');
        std::filesystem::path pureExtractPath = sftools::remove_char(extractPath, '\"');
        if (!std::filesystem::exists(purePkgPath))
            throw wexception(L"Package does not exist");

        if (!std::filesystem::exists(extractPath))
			throw wexception(L"Extract path does not exist");

		if (purePkgPath.extension() == ".sfpkg") {
			decompress_tar(purePkgPath, extractPath);
		} else {
			LOG_ERROR(L"Unknown package format");
		}
	}

    void sfpkg::install_pkg(const std::filesystem::path& pkgPath)
    {
        auto table = toml::parse_file(m_configSetup->configFilePath.string());

        if (auto value = table["compilation"]["packages"].as_array(); value) {
			for (const auto& pkg : *value) {
				if (pkg.as_string()->get().contains(pkgPath.stem().string())) {
					LOG_ERROR(L"Package " + pkgPath.stem().wstring() + L" already exists");
					return;
				}
			}
        }

        std::filesystem::path purePkgPath = sftools::remove_char(pkgPath, '\"');
        std::filesystem::path pkgProjectPath = 
            m_configSetup->configList->pkgFolder.wstring() +
            L"\\" + pkgPath.stem().wstring();

		std::filesystem::create_directories(pkgProjectPath);
		extract_pkg(purePkgPath, pkgProjectPath);
		compile_pkg(pkgProjectPath / "pkgConfig.toml");
    }

    void sfpkg::install_pkg(const std::wstring& pkgName)
    {
    }

    void sfpkg::compile_pkg(const std::filesystem::path& pkgConfigPath)
    {
        std::shared_ptr<shafa::sfConfigSetup> pkgConfigSetup = std::make_shared<shafa::sfConfigSetup>();
		pkgConfigSetup->shafaRootPath = m_configSetup->shafaRootPath;
		pkgConfigSetup->configFilePath = pkgConfigPath;
		LOG_INFO(L"Compiling package: " + pkgConfigPath.wstring());
        sfpkg_configure m_pkgConfig{ pkgConfigSetup };

		toml::table pkgContTable = toml::parse_file(pkgConfigPath.c_str());
		m_pkgConfig.pkg_configure_hub(pkgContTable);
		m_pkgConfig.pkg_configuration_construct();
        m_pkgConfig.non_configured_build();

		sfbuild pkgBuild(pkgConfigSetup);
		pkgBuild.build_hub();
		LOG_INFO(L"Debug package compiled successfully");
		pkgConfigSetup->compilationList->projectBuildType = sfProjectBuildType::release;
		pkgBuild.build_hub();
		LOG_INFO(L"Release package compiled successfully");
		LOG_INFO(L"Package compiled successfully");

        auto table = toml::parse_file(m_configSetup->configFilePath.string());

        auto& section = *table["compilation"].as_table();

        if (section.contains("packages")) {
            auto& array = *section["packages"].as_array();
            array.push_back(pkgConfigSetup->projectSettings->projectName + L'-' + pkgConfigSetup->projectSettings->projectVersion);
        }
        else {
            section.insert_or_assign("packages", toml::array{ pkgConfigSetup->projectSettings->projectName + L'-' + pkgConfigSetup->projectSettings->projectVersion });
        }

		std::ofstream configFile;
		configFile.open(m_configSetup->configFilePath, std::ios::out | std::ios::trunc);
		if (!configFile.is_open())
			throw wexception(L"Could not open file");

		configFile << table;
		configFile.close();
    }

	void sfpkg::make_tar(const std::filesystem::path& folderPath, const std::filesystem::path& tarPath)
	{
        const std::vector<std::string> exclusions { "config.toml", "Build" };

        struct archive* a;
        a = archive_write_new();
        if(a == nullptr) 
			throw wexception(L"Error creating archive");
		
        if (archive_write_add_filter_none(a) != ARCHIVE_OK) 
            throw wexception(L"Error adding filter to archive");

        if (archive_write_set_format_pax_restricted(a) != ARCHIVE_OK) 
			throw wexception(L"Error setting format to archive");

        if (archive_write_open_filename(a, tarPath.string().c_str()) != ARCHIVE_OK)
            throw wexception(L"Error opening archive");

        make_pkg_config();

        add_files_to_archive(a, folderPath.string().c_str(), exclusions, tarPath.string());

        if (archive_write_close(a) != ARCHIVE_OK)
            throw wexception(L"Error closing archive");

        if (archive_write_free(a) != ARCHIVE_OK)
			throw wexception(L"Error freeing archive");
        
        std::filesystem::remove(std::filesystem::current_path().wstring() + L"\\pkgConfig.toml");
        logger::log_new_line();
        LOG_INFO(L"Phase one is complete (1/1)");
		LOG_INFO(L"Package created at: " + tarPath.wstring());
	}

    void sfpkg::add_files_to_archive(struct archive* a, const std::string& dir_path, const std::vector<std::string>& exclusions, const std::string& output_filename)
    {
        WIN32_FIND_DATA find_file_data;
        HANDLE h_find = FindFirstFile((string_to_wstring(dir_path) + L"\\*").c_str(), &find_file_data);

        if (h_find == INVALID_HANDLE_VALUE) {
            throw wexception(L"Error finding first file");
        }

        do {
            std::wstring ws_filename(find_file_data.cFileName);
            std::string filename = wstring_to_string(ws_filename);

            if (filename == "." || filename == "..") {
                continue;
            }

            std::filesystem::path path = dir_path + "\\" + filename;

            if (should_exclude(path.string(), exclusions) || path == output_filename) {
                continue;
            }

            std::string relative_path = std::filesystem::relative(path, std::filesystem::current_path().string()).string();

            if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                archive_entry* entry = archive_entry_new();
                std::string dirEntry = relative_path + "/";
                archive_entry_set_pathname(entry, dirEntry.c_str());
                archive_entry_set_filetype(entry, AE_IFDIR);
                archive_entry_set_perm(entry, _S_IREAD | _S_IWRITE);
                archive_entry_set_size(entry, 0);
                int awhErr = archive_write_header(a, entry);
                if (awhErr != ARCHIVE_OK) {
                    LOG_ERROR(L"Error writing directory header to archive");
                    archive_entry_free(entry);
                    continue;
                }
                archive_entry_free(entry);

                add_files_to_archive(a, path.string(), exclusions, output_filename);
            }
            else {
                archive_entry* entry = archive_entry_new();
                archive_entry_set_pathname(entry, relative_path.c_str());
                archive_entry_set_size(entry, (int64_t)find_file_data.nFileSizeLow | ((int64_t)find_file_data.nFileSizeHigh << 32));
                archive_entry_set_filetype(entry, AE_IFREG);
                archive_entry_set_perm(entry, _S_IREAD | _S_IWRITE);
                int awhErr = archive_write_header(a, entry);
                if (awhErr != ARCHIVE_OK) {
                    LOG_ERROR(L"Error writing header to archive");
                    archive_entry_free(entry);
                    continue;
                }

                HANDLE h_file = CreateFileA(path.string().c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (h_file == INVALID_HANDLE_VALUE) {
                    LOG_ERROR(L"Could not open file " + string_to_wstring(path.string()));
                    archive_entry_free(entry);
                    continue;
                }

                char buff[8192];
                DWORD bytes_read;
                BOOL result;

                do {
                    result = ReadFile(h_file, buff, sizeof(buff), &bytes_read, nullptr);
                    if (result && bytes_read > 0) {
                        archive_write_data(a, buff, bytes_read);
                    }
                } while (result && bytes_read > 0);

                CloseHandle(h_file);
                archive_entry_free(entry);
            }
        } while (FindNextFile(h_find, &find_file_data) != 0);

        FindClose(h_find);
    }




    void sfpkg::decompress_tar(const std::filesystem::path& pkgPath, const std::filesystem::path& extractPath)
    {
        struct archive* a = archive_read_new();
        struct archive* ext = archive_write_disk_new();
        struct archive_entry* entry;
        int r;

        archive_read_support_format_tar(a);

        if ((r = archive_read_open_filename(a, pkgPath.string().c_str(), 10240))) {
            throw wexception(L"Error opening archive");
        }

        while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
            extract_file_form_tar(a, ext, entry, extractPath);
        }

        if (archive_read_close(a) != ARCHIVE_OK) {
            throw wexception(L"Error closing archive");
        }
        if (archive_read_free(a) != ARCHIVE_OK) {
            throw wexception(L"Error freeing archive");
        }
        if (archive_write_close(ext) != ARCHIVE_OK) {
            throw wexception(L"Error closing archive writer");
        }
        if (archive_write_free(ext) != ARCHIVE_OK) {
            throw wexception(L"Error freeing archive writer");
        }

        logger::log_new_line();
        LOG_INFO(L"Pkg extracting is complete");
        LOG_INFO(L"Pkg extracted to: " + extractPath.wstring());
    }

    void sfpkg::extract_file_form_tar(struct archive* a, struct archive* ext, struct archive_entry* entry, const std::filesystem::path& extractPath) {
        const char* currentFile = archive_entry_pathname(entry);
        std::filesystem::path fullOutputPath = extractPath / currentFile;

        archive_entry_set_pathname(entry, fullOutputPath.string().c_str());

        if (archive_write_header(ext, entry) != ARCHIVE_OK) {
            throw wexception(L"Error writing header to file");
        }

        const void* buff;
        size_t size;
        la_int64_t offset;
        while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK) {
            if (archive_write_data_block(ext, buff, size, offset) != ARCHIVE_OK) {
                throw wexception(L"Error writing data to file");
            }
        }

        if (archive_write_finish_entry(ext) != ARCHIVE_OK) {
            throw wexception(L"Error finishing entry");
        }
    }
}