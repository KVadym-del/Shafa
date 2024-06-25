#pragma once
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>

#include <toml++/toml.hpp>
#include <argparse/argparse.hpp>

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
		void analyze_hub(argparse::ArgumentParser* program);

		void start_shafa();
		void non_configured_build();

		inline const argparse::ArgumentParser* get_program() const { return m_program; }
		inline sfConfigSetup* get_config_setup() { return m_configSetup; }

	protected:
		toml::table m_sfContTable;
	
	private:
		argparse::ArgumentParser* m_program;
		sfConfigSetup* m_configSetup;
	};
}
