#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <memory>

#include "sftypes.h"
#include "sfhub.h"
#include "wexception.h"
#include "logger.h"

namespace shafa
{
	using std::filesystem::path;

	class sffile : public sfhub
	{
	public:
		sffile() = default;
		sffile(sfConfigSetup* configSetup);
		~sffile() = default;

		void check_validity(void);

	public:
		inline path get_path(void) const { return m_sfPath; }
		inline toml::table& get_content_table(void) { return sfhub::m_sfContTable; }

	private:
		path m_sfPath;
		sfConfigSetup* m_configSetup;
	};
}
