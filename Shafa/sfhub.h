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
#include "sfarghelper.h"


namespace shafa {
	template<typename T>
	bool compareVectors(const std::vector<T>& vec1, const std::vector<T>& vec2)
	{
		if (vec1.size() != vec2.size())
		{
			return false;
		}

		for (std::size_t i = 0; i < vec1.size(); i++)
		{
			if (vec1[i] != vec2[i])
			{
				return false;
			}
		}

		return true;
	}

	class sfhub : public sfinit, public sfconfigure, public sfbuild
	{
	public:
		sfhub(std::shared_ptr<sfConfigSetup> configSetup) : m_configSetup(configSetup), sfinit(configSetup), sfconfigure(configSetup), sfbuild(configSetup)
		{}
		~sfhub() = default;

	public:
		void analyze_hub(const std::vector<std::wstring_view>& args);

		void start_shafa();
		void non_configured_build();

		inline std::vector<sfArg> get_args() const { return m_args; }
		inline std::shared_ptr<sfConfigSetup> get_config_setup() { return m_configSetup; }

	protected:
		toml::table m_sfContTable;
	
	private:
		std::vector<sfArg> m_args{};
		std::shared_ptr<sfConfigSetup> m_configSetup;
	};
}
