#pragma once
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>

#include <toml++/toml.hpp>

#include "sftypes.h"
#include "sfinit.h"
#include "sfconfigure.h"
#include "sfbuild.h"
#include "sfpkg.h"
#include "sfarghelper.h"
#include "sftools.h"


namespace shafa {
	class sfhub : public sfinit, public sfconfigure, public sfbuild, public sfpkg
	{
	public:
		sfhub(sfConfigSetup* configSetup) 
			: m_configSetup(configSetup),
			sfinit(configSetup),
			sfconfigure(configSetup),
			sfbuild(configSetup),
			sfpkg(configSetup)
		{}
		~sfhub() = default;

	public:
		void non_configured_build();

		inline sfConfigSetup* get_config_setup() { return m_configSetup; }

	public:
		toml::table m_sfContTable;
	
	private:
		sfConfigSetup* m_configSetup;
	};
}
