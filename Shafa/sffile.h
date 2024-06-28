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

	class sffile 
	{
	public:
		sffile() = default;
		~sffile() = default;

		void check_validity(sfConfigSetup* configSetup);

	public:
		inline path get_path(void) const { return m_sfPath; }
		inline sfConfigSetup* get_config_setup(void) const { return m_configSetup; }
		inline sfhub* get_sf_hub(void) const { return m_sfHub.get(); }

	private:
		std::unique_ptr<sfhub> m_sfHub;

		path m_sfPath;
		sfConfigSetup* m_configSetup;
	};
}
