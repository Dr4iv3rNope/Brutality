#include "variableui.hpp"
#include "numeric.hpp"
#include "colors.hpp"

#include "../imgui.h"
#include "../imgui_stdlib.h"

bool ImGui::Custom::Variable::Unsigned(Config::UInt32& value) noexcept
{
	ImGui::PushID(value.GetKey().c_str());
	if (auto temp = value.GetValue();
		ImGui::Custom::InputAny<std::uint32_t, ImGuiDataType_U32>(
			value.GetKey().c_str(),
			temp
			))
	{
		value.SetValue(temp);

		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}

bool ImGui::Custom::Variable::LimitedUnsigned(Config::LUInt32& value) noexcept
{
	ImGui::PushID(value.GetKey().c_str());
	if (auto temp = value.GetValue();
		ImGui::Custom::InputAny<std::uint32_t, ImGuiDataType_U32>(
			value.GetKey().c_str(),
			temp,
			std::optional(value.GetMin()),
			std::optional(value.GetMax())
			))
	{
		value.SetValue(temp);

		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}

bool ImGui::Custom::Variable::Signed(Config::Int32& value) noexcept
{
	ImGui::PushID(value.GetKey().c_str());
	if (auto temp = value.GetValue();
		ImGui::Custom::InputAny<std::int32_t, ImGuiDataType_S32>(
			value.GetKey().c_str(),
			temp
			))
	{
		value.SetValue(temp);

		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}

bool ImGui::Custom::Variable::LimitedSigned(Config::LInt32& value) noexcept
{
	ImGui::PushID(value.GetKey().c_str());
	if (auto temp = value.GetValue();
		ImGui::Custom::InputAny<std::int32_t, ImGuiDataType_S32>(
			value.GetKey().c_str(),
			temp,
			std::optional(value.GetMin()),
			std::optional(value.GetMax())
			))
	{
		value.SetValue(temp);

		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}

bool ImGui::Custom::Variable::Float(Config::Float& value) noexcept
{
	ImGui::PushID(value.GetKey().c_str());
	if (auto temp = value.GetValue();
		ImGui::Custom::InputAny<float, ImGuiDataType_Float>(
			value.GetKey().c_str(),
			temp
			))
	{
		value.SetValue(temp);

		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}

bool ImGui::Custom::Variable::LimitedFloat(Config::LFloat& value) noexcept
{
	ImGui::PushID(value.GetKey().c_str());
	if (auto temp = value.GetValue();
		ImGui::Custom::InputAny<float, ImGuiDataType_Float>(
			value.GetKey().c_str(),
			temp,
			std::optional(value.GetMin()),
			std::optional(value.GetMax())
			))
	{
		value.SetValue(temp);

		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}

template <typename T>
static inline bool AnyString(Config::String<T>& value) noexcept
{
	ImGui::PushID(value.GetKey().c_str());
	if (std::basic_string<T> temp = value.GetValue();
		ImGui::InputText(value.GetKey().c_str(), (std::string*)&temp))
	{
		value.SetValue(temp);
		
		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}

template <typename T>
static inline bool AnyLimitedString(Config::LimitedString<T>& value) noexcept
{
	return ImGui::InputText(value.GetKey().c_str(), value.GetBuffer(), value.GetMaxLength());
}

bool ImGui::Custom::Variable::String(Config::String<char>& value) noexcept
{
	return AnyString(value);
}

bool ImGui::Custom::Variable::LimitedString(Config::LimitedString<char>& value) noexcept
{
	return AnyLimitedString(value);
}

bool ImGui::Custom::Variable::Boolean(Config::Bool& boolean) noexcept
{
	ImGui::PushID(boolean.GetKey().c_str());
	if (bool temp = boolean.GetValue();
		ImGui::Checkbox(
			boolean.GetKey().c_str(),
			&temp
		))
	{
		boolean.SetValue(temp);
		
		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}

bool ImGui::Custom::Variable::Enum(Config::Enum& value) noexcept
{
	static auto callback = [] (void* data, int idx, const char** out) -> bool
	{
		auto items = (Config::Enum::Items*)data;

		*out = items->at(idx).c_str();
		return true;
	};

	ImGui::PushID(value.GetKey().c_str());
	if (int temp = value.GetCurrentItem();
		ImGui::Combo(
			value.GetFlags().HasFlag(Config::VariableFlags_AlignToRight)
			? ""
			: value.GetKey().c_str(),
			&temp,
			callback,
			(void*)&value.GetItems(),
			value.GetItems().size()
		))
	{
		value.SetCurrentItem(temp);
		
		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}

bool ImGui::Custom::Variable::Color(Config::Color& value) noexcept
{
	ImGui::PushID(value.GetKey().c_str());
	if (ImVec4 temp = ImGui::ColorConvertU32ToFloat4(value.Hex());
		ImGui::Custom::ColorPicker(
			value.GetKey().c_str(),
			temp,
			ImGui::ColorConvertU32ToFloat4(*(std::uint32_t*) & value.GetDefaultColor())
		))
	{
		value.SetValue(ImGui::ColorConvertFloat4ToU32(temp));
		
		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}

bool ImGui::Custom::Variable::Key(Config::Key& value) noexcept
{
	ImGui::PushID(value.GetKey().c_str());
	if (auto temp = value.GetKeyValue();
		ImGui::Custom::InputKey(value.GetKey().c_str(), temp))
	{
		value.SetKeyValue(temp);

		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}

bool ImGui::Custom::Variable::Flags(Config::Flags& flags) noexcept
{
	ImGui::PushID(flags.GetKey().c_str());
	if (auto iflags = flags.GetFlags<int, std::uint32_t>();
		FlagInput(flags.GetKey().c_str(), iflags.flags, [&flags, iflags] (unsigned int bit, std::string& desc) -> bool
	{
		return flags.BitInfo(bit, desc);
	}, flags.GetBitCount()))
	{
		flags.SetFlags(iflags);

		ImGui::PopID();
		return true;
	}
	else
	{
		ImGui::PopID();
		return false;
	}
}
