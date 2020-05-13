#pragma once
#include <string>

#include "../util/stringhash.hpp"

namespace Features
{
	namespace EntityList
	{
		struct EntitySettings
		{
			Util::HashType classnameHash;
			std::string classname;

			bool espDraw { false };
			std::uint32_t espColor { 0xFFFFFFFF };

			std::uint32_t chamsVisColor { 0xFFFFFFFF }; // visible color
			std::uint32_t chamsOccColor { 0xFFFFFFFF }; // occluded color
			int chamsVisType { 0 }; // 0 - disabled
			int chamsOccType { 0 }; // 0 - disabled

			inline EntitySettings(const std::string& classname)
				: classname{ classname }, classnameHash { Util::MakeRuntimeHash(classname) }
			{}
		};

		// can be null!
		extern const EntitySettings* GetEntitySettings(const std::string& classname) noexcept;

		void RegisterEntityListWindow() noexcept;
	}
}
