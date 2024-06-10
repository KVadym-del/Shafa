#include "sfpkg.h"

namespace shafa {
	sfpkg::sfpkg(std::shared_ptr<sfConfigSetup> configSetup)
		: m_configSetup(configSetup)
	{}

	void sfpkg::make_pkg(const std::filesystem::path& folderPath)
	{
        const std::filesystem::path tarPath{
            m_configSetup->configList->outputFolder.string() + "\\" +
            wstring_to_string(m_configSetup->projectSettings->projectName) + ".sfpkg" 
        };
        make_tar(folderPath, tarPath);
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
        std::filesystem::path purePkgPath = sftools::remove_char(pkgPath, '\"');
        std::filesystem::path pkgProjectPath = 
            std::filesystem::current_path().wstring() +
            m_configSetup->configList->pkgFolder.wstring() +
            L"\\" + pkgPath.stem().wstring();

		std::filesystem::create_directory(pkgProjectPath);
		extract_pkg(purePkgPath, pkgProjectPath);
    }

    void sfpkg::install_pkg(const std::wstring& pkgName)
    {
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

        add_files_to_archive(a, folderPath.string().c_str(), exclusions, tarPath.string());

        if (archive_write_close(a) != ARCHIVE_OK)
            throw wexception(L"Error closing archive");

        if (archive_write_free(a) != ARCHIVE_OK)
			throw wexception(L"Error freeing archive");
        
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
            
            std::string fileName= path.filename().string();

            if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                add_files_to_archive(a, path.string(), exclusions, output_filename);
            }
            else {
                archive_entry* entry = archive_entry_new();
                archive_entry_set_pathname(entry, fileName.c_str());
                archive_entry_set_size(entry, (int64_t)find_file_data.nFileSizeLow | ((int64_t)find_file_data.nFileSizeHigh << 32));
                archive_entry_set_filetype(entry, AE_IFREG);
                archive_entry_set_perm(entry, _S_IREAD | _S_IWRITE);
                std::int32_t awhErr = archive_write_header(a, entry);
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

    void sfpkg::decompress_7z(const std::filesystem::path& pkgPath, const std::filesystem::path& extractPath)
    {
        

        LOG_INFO(L"Phase three is complete");
    }

   

}