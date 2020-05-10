#pragma once
#include <string>
#include <deque>
#include <map>
#include <utility>
#include <optional>
#include <numeric>
#include <array>
#include <type_traits>

#include "../nlohmann/json.hpp"

#include "../imgui/custom/keys.hpp"

#include "../util/xorstr.hpp"
#include "../util/flags.hpp"
#include "../util/debug/assert.hpp"

namespace Config
{
	class IVariable;
	
	using Variables = std::deque<IVariable*>;

	// key is group of variables
	using SortedVariables = std::map<std::string, Variables>;

	// registers new variable
	//
	// return false if variable with same group and key already presented
	extern bool RegisterVariable(IVariable& variable) noexcept;

	// return true if variable with this group and key already presented
	extern bool IsVariableRegistered(const std::string& group, const std::string& key) noexcept;

	// imports (loads) values to variables
	extern void ImportVariables(const nlohmann::json& root);
	
	// exports (saves) values to root
	extern void ExportVariables(nlohmann::json& root);
	
	// return all saved variables
	extern SortedVariables& GetSortedVariables() noexcept;

	// register all variables in window manager
	extern void RegisterVariablesInWindowManager() noexcept;

	UTIL_FLAGS(VariableFlags)
	{
		UTIL_MAKE_BIT_FLAG(VariableFlags, DontRegister, 0),		// dont register variable when created
		UTIL_MAKE_BIT_FLAG(VariableFlags, NotVisible, 1),		// prevent render element in menu
		UTIL_MAKE_BIT_FLAG(VariableFlags, DontSave, 2)			// prevent save variable in config
	};

	enum class VariableType
	{
		Invalid = -1,
		
		// basic types
		
		Signed,
		Unsigned,
		Float,
		Boolean,

		// custom types

		String,
		Enum,
		Color,
		Key,
		Flags
	};

	// basic variable that used to create unique variable
	class IVariable
	{
	private:
		std::string _group, _key;
		VariableFlags _flags;

	public:
		inline IVariable(const std::string& group, const std::string& key, VariableFlags flags = 0)
			: _group { group }, _key { key }, _flags { flags }
		{
			if (!flags.HasFlag(VariableFlags_DontRegister))
				RegisterVariable(*this);
		}

		// disallow copying setting
		IVariable(const IVariable&) = delete;

		inline auto GetGroup() const noexcept { return _group; }
		inline auto GetKey() const noexcept { return _key; }
		inline auto GetFlags() const noexcept { return _flags; }

		virtual bool Export(nlohmann::json& value) const = 0;
		virtual bool Import(const nlohmann::json& value) = 0;

		virtual VariableType GetType() const = 0;
		virtual bool IsLimitedVariable() const = 0;
	};

	// return true if variable with same group and key already presented
	inline bool IsVariableRegistered(IVariable& variable)
	{
		return IsVariableRegistered(variable.GetGroup(), variable.GetKey());
	}


	// base variable that can be used in most cases
	//
	// includes all stuff that required for variables
	// but you need implement SetValue, Save, Load methods
	// yourself
	template <typename T>
	class IBaseVariable : public IVariable
	{
		static_assert(std::is_copy_constructible<T>(), "T must be copiable");

	protected:
		T _value;

		inline IBaseVariable(const std::string& group, const std::string& key, VariableFlags flags = 0, T default_value = T {})
			: IVariable(group, key, flags), _value { default_value } {}

	public:
		using Type = T;
				
		virtual VariableType GetType() const override
		{
			if constexpr (std::is_same<T, bool>())
			{
				return VariableType::Boolean;
			}
			else if constexpr (std::is_floating_point<T>())
			{
				return VariableType::Float;
			}
			else if constexpr (std::is_unsigned<T>())
			{
				return VariableType::Unsigned;
			}
			else
			{
				if constexpr (std::is_signed<T>())
					return VariableType::Signed;
				else
					return VariableType::Invalid;
			}
		}

		inline const T& GetValue() const noexcept { return this->_value; }
		virtual void SetValue(T value) = 0;

		inline auto operator=(T value) { SetValue(value); }
		inline bool operator==(T value) const noexcept { return GetValue(value) == value; }
		inline bool operator!=(T value) const noexcept { return !operator==(value); }

		inline explicit operator T() const noexcept { return this->GetValue(); }
		inline const T& operator*() const noexcept { return this->GetValue(); }
	};

	// variable class that can hold basic types
	// (like int, float, etc.)
	//
	// not recommended if you want create own
	// variable class
	template <typename T>
	class Variable : public IBaseVariable<T>
	{
		static_assert(!std::is_pointer<T>(), "T cannot be pointer");
		
	public:
		inline Variable(const std::string& group, const std::string& key,
						VariableFlags flags = 0, T default_value = T {})
			: IBaseVariable<T>(group, key, flags, default_value)
		{}

		virtual void SetValue(T value) override { this->_value = value; }

		virtual bool Export(nlohmann::json& value) const override
		{
			value = this->_value;
			return true;
		}

		virtual bool Import(const nlohmann::json& value) override
		{
			if constexpr (std::is_same<T, bool>())
			{
				if (!value.is_boolean())
					return false;
			}
			else if constexpr (std::is_floating_point<T>())
			{
				if (!value.is_number_float())
					return false;
			}
			else if constexpr (std::is_unsigned<T>())
			{
				if (!value.is_number_unsigned())
					return false;
			}
			else
			{
				if constexpr (std::is_signed<T>())
				{
					if (!value.is_number())
						return false;
				}
			}
			
			// anyway, we should use try catch
			try
			{
				this->_value = value;
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		virtual bool IsLimitedVariable() const override { return false; }
	};

	// limited variable that can clamp input variable
	//
	// not working with bool (for obvious reasons)
	template <typename T>
	class LimitedVariable : public IBaseVariable<T>
	{
		static_assert(!std::is_pointer<T>(), "T cannot be pointer");
		static_assert(!std::is_same<T, bool>(), "cannot be bool");

	public:
		using Limits = std::pair<T, T>;

	protected:
		std::optional<Limits> _limits;

	public:
		inline LimitedVariable(const std::string& group, const std::string& key,
							   T default_value, T min, T max,
							   VariableFlags flags = 0)
			: IBaseVariable<T>(group, key, flags, default_value),
			_limits { std::make_pair(min, max) }
		{}
		
		inline T GetMin() const noexcept
		{
			return this->_limits.has_value()
				? this->_limits.value().first
				: std::numeric_limits<T>().min();
		}
		
		inline T GetMax() const noexcept
		{
			return this->_limits.has_value()
				? this->_limits.value().second
				: std::numeric_limits<T>().max();
		}

		inline bool Clamp(T& value) const noexcept
		{
			if (!this->_limits.has_value())
				return false;

			auto& limits = this->_limits.value();

			if (limits.first > value)
			{
				value = limits.first;
				return true;
			}
			else if (limits.second < value)
			{
				value = limits.second;
				return true;
			}
			else
				return false;
		}

		virtual void SetValue(T value) override
		{
			this->Clamp(value);

			this->_value = value;
		}

		virtual bool Export(nlohmann::json& value) const override
		{
			value = this->_value;
			return true;
		}

		virtual bool Import(const nlohmann::json& value) override
		{
			if constexpr (std::is_floating_point<T>())
			{
				if (!value.is_number_float())
					return false;
			}
			else if constexpr (std::is_unsigned<T>())
			{
				if (!value.is_number_unsigned())
					return false;
			}
			else
			{
				if constexpr (std::is_signed<T>())
				{
					if (!value.is_number())
						return false;
				}
			}

			// anyway, we should use try catch
			try
			{
				this->_value = value;
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		virtual bool IsLimitedVariable() const override { return true; }
	};

	//
	// supported types
	//
	// limited variables have prefix L
	// like LFloat - Limited Float
	//
	// if you want use custom data type
	// create own Variable class using
	// IBaseVariable<T> or IVariable
	// interfaces
	//
	// example of this class you can find
	// below (class String)
	//

	using Int32 = Variable<int>;
	using LInt32 = LimitedVariable<int>;

	using UInt32 = Variable<std::uint32_t>;
	using LUInt32 = LimitedVariable<std::uint32_t>;

	using Float = Variable<float>;
	using LFloat = LimitedVariable<float>;

	using Bool = Variable<bool>;

	template <typename T>
	class LimitedString : public IBaseVariable<std::basic_string<T>>
	{
	private:
		std::size_t _max_length;

	public:
		inline LimitedString(const std::string& group, const std::string& key,
							 std::size_t max_length,
							 VariableFlags flags = 0,
							 std::basic_string<T> default_value = std::basic_string<T>())
			: IBaseVariable<std::basic_string<T>>(group, key, flags, default_value),
			_max_length { max_length }
		{
			this->_value.resize(_max_length);
		}

		virtual VariableType GetType() const override { return VariableType::String; }
		inline std::size_t GetMaxLength() const noexcept { return this->_max_length; }

		virtual bool Export(nlohmann::json& value) const override
		{
			value = this->_value;
			return true;
		}

		virtual bool Import(const nlohmann::json& value) override
		{
			if (!value.is_string())
				return false;

			this->_value = value;
			return true;
		}

		virtual void SetValue(std::basic_string<T> value) noexcept override
		{
			this->_value = value;
			this->_value.resize(this->_max_length);
		}

		virtual bool IsLimitedVariable() const override { return true; }

		inline char* GetBuffer() noexcept { return (char*)this->GetValue().data(); }
	};

	template <typename T>
	class String : public IBaseVariable<std::basic_string<T>>
	{
	public:
		inline String(const std::string& group, const std::string& key,
					  VariableFlags flags = 0,
					  std::basic_string<T> default_value = std::basic_string<T>())
			: IBaseVariable<std::basic_string<T>>(group, key, flags, default_value)
		{}

		virtual VariableType GetType() const override { return VariableType::String; }

		virtual bool Export(nlohmann::json& value) const override
		{
			value = this->_value;
			return true;
		}

		virtual bool Import(const nlohmann::json& value) override
		{
			if (!value.is_string())
				return false;

			this->_value = value;
			return true;
		}

		virtual void SetValue(std::basic_string<T> value) noexcept override
		{
			this->_value = value;
		}

		virtual bool IsLimitedVariable() const override { return false; }

		inline char* GetBuffer() noexcept { return (char*)this->GetValue().data(); }
		inline void Resize(std::size_t size) noexcept { return (char*)this->GetValue().resize(size); }
	};

	class Enum : public IVariable
	{
	public:
		using Items = std::deque<std::string>;

	protected:
		Items _items;

		std::size_t _typehash = typeid(Enum).hash_code();
		std::size_t _currentItem { 0 };

	public:
		inline Enum(const std::string& group, const std::string& key,
					std::initializer_list<Items::value_type> items,
					VariableFlags flags = 0)
			: IVariable(group, key, flags), _items(items)
		{
			// empty item list disallow
			UTIL_DEBUG_ASSERT(items.size() > 0);
		}

		virtual bool Export(nlohmann::json& value) const override
		{
			value = _currentItem;
			return true;
		}

		virtual bool Import(const nlohmann::json& value) override
		{
			if (!value.is_number_integer())
				return false;

			_currentItem = value;
			return true;
		}

		virtual VariableType GetType() const override { return VariableType::Enum; }
		virtual bool IsLimitedVariable() const override { return false; }


		inline auto GetCurrentItem() const noexcept { return _currentItem; }
		inline explicit operator std::size_t() const noexcept { return GetCurrentItem(); }

		inline void SetCurrentItem(std::size_t idx) noexcept { _currentItem = idx; }
		inline void operator=(std::size_t idx) noexcept { SetCurrentItem(idx); }

		inline const Items::value_type& GetStringItem() const noexcept{ return _items[_currentItem]; }

		inline const auto& GetItems() const noexcept { return this->_items; }
	};

	class Color : public IBaseVariable<std::array<std::uint8_t, 4>>
	{
	protected:
		std::array<std::uint8_t, 4> _defaultColor;

	public:
		inline Color(const std::string& group, const std::string& key,
					 std::array<std::uint8_t, 4> colors,
					 VariableFlags flags = 0)
			: IBaseVariable<std::array<std::uint8_t, 4>>(group, key, flags, colors)
		{
			_defaultColor = colors;
		}

		inline Color(const std::string& group, const std::string& key,
					 float r, float g, float b, float a = 1.f,
					 VariableFlags flags = 0)
			: Color(group, key,
					{
						std::uint8_t(r * 255.f),
						std::uint8_t(g * 255.f),
						std::uint8_t(b * 255.f),
						std::uint8_t(a * 255.f)
					}, flags)
		{}

		inline Color(const std::string& group, const std::string& key,
					 std::uint32_t hex, VariableFlags flags = 0)
			: Color(group, key, *reinterpret_cast<std::array<std::uint8_t, 4>*>(&hex), flags)
		{}

		virtual VariableType GetType() const override { return VariableType::Color; }

		virtual bool Export(nlohmann::json& value) const override
		{
			value[0] = this->_value[0];
			value[1] = this->_value[1];
			value[2] = this->_value[2];
			value[3] = this->_value[3];
			return true;
		}

		virtual bool Import(const nlohmann::json& value) override
		{
			if (!value.is_array())
				return false;

			try
			{
				this->_value[0] = value[0];
				this->_value[1] = value[1];
				this->_value[2] = value[2];
				this->_value[3] = value[3];

				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		virtual bool IsLimitedVariable() const override { return false; }

		virtual void SetValue(std::array<std::uint8_t, 4> value) noexcept override { this->_value = value; }
		
		inline void SetValue(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255) noexcept
		{
			this->SetValue({ r, g, b, a });
		}
		
		inline void SetValue(std::uint32_t hex) noexcept
		{
			this->SetValue(*reinterpret_cast<std::array<std::uint8_t, 4>*>(&hex));
		}
		
		inline void SetValue(float r, float g, float b, float a = 1.f) noexcept
		{
			this->SetValue({
				std::uint8_t(r * 255.f),
				std::uint8_t(g * 255.f),
				std::uint8_t(b * 255.f),
				std::uint8_t(a * 255.f)
			});
		}

		inline auto Red() const noexcept { return this->_value[0]; }
		inline auto Green() const noexcept { return this->_value[1]; }
		inline auto Blue() const noexcept { return this->_value[2]; }
		inline auto Alpha() const noexcept { return this->_value[3]; }

		inline float RedF() const noexcept { return float(this->Red()) / 255.f; }
		inline float GreenF() const noexcept { return float(this->Green()) / 255.f; }
		inline float BlueF() const noexcept { return float(this->Blue()) / 255.f; }
		inline float AlphaF() const noexcept { return float(this->Alpha()) / 255.f; }

		inline auto Hex() const noexcept { return *(uint32_t*)this->_value.data(); }

		inline const auto& GetDefaultColor() const noexcept { return _defaultColor; }
		inline void SetDefaultColor() noexcept { _value = _defaultColor; }
	};

	// used for key bindings
	class Key : public IVariable
	{
	private:
		ImGui::Custom::Key _key_value;

	public:
		inline Key(const std::string& group, const std::string& key,
				   ImGui::Custom::Key key_value = ImGui::Custom::Keys::INVALID,
				   VariableFlags flags = 0)
			: IVariable(group, key, flags), _key_value { key_value }
		{}

		inline void SetKeyValue(ImGui::Custom::Key key_value) noexcept { _key_value = key_value; }
		inline auto GetKeyValue() const noexcept { return _key_value; }

		inline bool HasKeyValue() const noexcept { return _key_value != -1; }
		inline void ResetKeyValue() noexcept { _key_value = -1; }


		virtual bool Export(nlohmann::json& value) const override
		{
			value = _key_value;

			return true;
		}

		virtual bool Import(const nlohmann::json& value) override
		{
			if (!value.is_number_unsigned())
				return false;

			_key_value = value;
			return true;
		}

		virtual VariableType GetType() const override { return VariableType::Key; }
		virtual bool IsLimitedVariable() const override { return false; }
	};

	class Flags : public IVariable
	{
	private:
		std::uint32_t _flags;
		std::deque<std::string> _bits_info;

	public:
		inline Flags(const std::string& group, const std::string& key,
					 std::initializer_list<std::string> bits_info,
					 VariableFlags flags = 0)
			: IVariable(group, key, flags), _bits_info(bits_info)
		{
			// empty item list disallow
			UTIL_DEBUG_ASSERT(bits_info.size() > 0);
			UTIL_DEBUG_ASSERT(bits_info.size() <= GetMaxBit());
		}

		template <typename V = int, typename F = int>
		inline Util::Flags<V, F> GetFlags() const noexcept { return Util::Flags<V, F>(_flags); }

		template <typename V = int, typename F = int>
		inline void SetFlags(Util::Flags<V, F> flags) noexcept { _flags = flags.flags; }

		inline std::uint32_t GetFlags() noexcept { return _flags; }
		inline void SetFlags(std::uint32_t flags) noexcept { _flags = flags; }

		inline bool HasBitInfo(std::size_t bit) const noexcept
		{
			return bit < _bits_info.size();
		}

		inline bool BitInfo(std::size_t bit, std::string& info) const noexcept
		{
			if (HasBitInfo(bit))
			{
				info = _bits_info[bit];
				return true;
			}
			else
				return false;
		}

		inline std::size_t GetBitCount() const noexcept
		{
			return _bits_info.size();
		}

		static constexpr inline std::size_t GetMaxBit() noexcept
		{
			return sizeof(std::uint32_t) * 8;
		}


		virtual bool Export(nlohmann::json& value) const
		{
			value = _flags;
			return true;
		}

		virtual bool Import(const nlohmann::json& value)
		{
			if (!value.is_number())
				return false;

			_flags = value;
			return true;
		}

		virtual VariableType GetType() const { return VariableType::Flags; }

		virtual bool IsLimitedVariable() const { return false; }
	};
}
