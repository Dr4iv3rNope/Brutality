#pragma once
#include <string>
#include <deque>
#include <map>
#include <utility>
#include <optional>
#include <numeric>
#include <array>
#include <functional>
#include <type_traits>

#include "../jsoncpp/value.h"

#include "../imgui/custom/keys.hpp"

#include "../util/xorstr.hpp"
#include "../util/flags.hpp"
#include "../util/debug/assert.hpp"

#pragma warning(disable : 4244)

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

	// unregisters variable
	//
	// return false if variable not registered
	extern bool UnregisterVariable(IVariable& variable) noexcept;

	// return true if variable with this group and key already presented
	extern bool IsVariableRegistered(const std::string& group, const std::string& key) noexcept;

	// imports (loads) values to variables
	extern void ImportVariables(const Json::Value& root);

	// exports (saves) values to root
	extern void ExportVariables(Json::Value& root);

	// resets all values
	extern void ResetAllVariables() noexcept;
	
	// return all saved variables
	extern SortedVariables& GetSortedVariables() noexcept;

	// register all variables in window manager
	extern void RegisterVariablesInWindowManager() noexcept;

	UTIL_FLAGS(VariableFlags)
	{
		UTIL_MAKE_BIT_FLAG(VariableFlags, DontRegister, 0),		// dont register variable when created
		UTIL_MAKE_BIT_FLAG(VariableFlags, NotVisible, 1),		// prevent render element in menu
		UTIL_MAKE_BIT_FLAG(VariableFlags, DontSave, 2),			// prevent save variable in config
		UTIL_MAKE_BIT_FLAG(VariableFlags, AtNewLine, 3),		// will place variable at new line
		UTIL_MAKE_BIT_FLAG(VariableFlags, AtSameLine, 4),		// will place variable at same line
		UTIL_MAKE_BIT_FLAG(VariableFlags, AlignToRight, 5)		// align variable to rhe right
	};

	enum class VariableType
	{
		Invalid = -1,
		
		// basic types
		
		Signed, Unsigned, Float, Boolean,

		// custom types

		String, Enum, Color, Key, Flags
	};

	// basic variable that used to create unique variable
	class IVariable
	{
	private:
		std::string _group, _key;
		VariableFlags _flags;

	protected:
		inline IVariable(const std::string& group, const std::string& key, VariableFlags flags = 0)
			: _group { group }, _key { key }, _flags { flags }
		{
			if (!flags.HasFlag(VariableFlags_DontRegister))
				RegisterVariable(*this);
		}

	public:

		inline auto GetGroup() const noexcept { return _group; }
		inline auto GetKey() const noexcept { return _key; }
		inline auto GetFlags() const noexcept { return _flags; }

		virtual bool Export(Json::Value& value) const = 0;
		virtual bool Import(const Json::Value& value) = 0;
		virtual void Reset() noexcept = 0;

		virtual VariableType GetType() const = 0;
		virtual bool IsLimitedVariable() const = 0;
	};

	// return true if variable with same group and key already presented
	inline bool IsVariableRegistered(IVariable& variable)
	{
		return IsVariableRegistered(variable.GetGroup(), variable.GetKey());
	}

	enum class VariableAction
	{
		Import,			// called when we successfully imported value
		Export,			// called when we successfully exported value
		ValueChanged,	// called when value is changed
		Reset			// called when value is reset
	};

	template <typename T>
	class IBaseVariable;

	// for devs: call when value is changed ALREADY
	template <typename T>
	using VariableCallback = std::function<void(const IBaseVariable<T>*, VariableAction)>;

	// base variable that can be used in most cases
	//
	// includes all stuff that required for variables
	// but you need implement SetValue, Save, Load methods
	// yourself
	template <typename T>
	class IBaseVariable : public IVariable
	{
		static_assert(std::is_copy_constructible<T>(), "T must be copiable");
		static_assert(!std::is_pointer<T>(), "T cannot be pointer");

	protected:
		T _value;
		const T _default_value;
		VariableCallback<T> _callback;

		inline void Callback(VariableAction action) const noexcept
		{
			if (_callback)
				_callback(this, action);
		}

		inline IBaseVariable(const std::string& group, const std::string& key,
							 T default_value = T {},
							 VariableFlags flags = 0,
							 VariableCallback<T> callback = nullptr)
			: IVariable(group, key, flags), _value { default_value }, _callback { callback }, _default_value { _value }
		{}

	public:
		IBaseVariable(const IBaseVariable&) = delete;
		IBaseVariable(const IBaseVariable&&) = delete;

		virtual void SetCallback(VariableCallback<T> callback) noexcept { _callback = callback; }
		
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

		inline const T& GetDefaultValue() const noexcept { return _default_value; }
		inline const T& GetValue() const noexcept { return this->_value; }
		virtual void SetValue(T value) = 0;

		virtual void Reset() noexcept override
		{
			_value = _default_value;
			Callback(Config::VariableAction::Reset);
		}

		inline auto operator=(T value) { SetValue(value); }
		inline bool operator==(T value) const noexcept { return GetValue(value) == value; }
		inline bool operator!=(T value) const noexcept { return !operator==(value); }

		inline explicit operator T() const noexcept { return this->GetValue(); }
		inline const T& operator*() const noexcept { return this->GetValue(); }
	};

	// variable class that can hold basic types
	// (like int, float, etc.)
	//
	// not recommended if you want use custom class
	template <typename T>
	class Variable : public IBaseVariable<T>
	{	
	public:
		inline Variable(const std::string& group, const std::string& key,
						T default_value = T {}, VariableFlags flags = 0, VariableCallback<T> callback = nullptr)
			: IBaseVariable<T>(group, key, default_value, flags, callback) {}

		virtual void SetValue(T value) override
		{
			this->_value = value;
			this->Callback(VariableAction::ValueChanged);
		}

		virtual bool Export(Json::Value& value) const override
		{
			value = this->_value;
			this->Callback(VariableAction::Export);

			return true;
		}

		virtual bool Import(const Json::Value& value) override
		{
			bool success = false;

			if constexpr (std::is_same<T, bool>())
			{
				if (value.isBool())
				{
					this->_value = value.asBool();
					success = true;
				}
			}
			else if constexpr (std::is_floating_point<T>())
			{
				if (value.isDouble())
				{
					if constexpr (sizeof(T) == sizeof(double))
					{
						this->_value = value.asDouble();
						success = true;
					}
					else
					{
						this->_value = value.asFloat();
						success = true;
					}
				}
			}
			else if constexpr (std::is_unsigned<T>())
			{
				if constexpr (sizeof(T) == sizeof(unsigned long))
				{
					if (value.isUInt64())
					{
						this->_value = value.asUInt64();
						success = true;
					}
				}
				else
				{
					if (value.isUInt())
					{
						this->_value = value.asUInt();
						success = true;
					}
				}
			}
			else
			{
				if constexpr (std::is_signed<T>())
				{
					if constexpr (sizeof(T) == sizeof(unsigned long))
					{
						if (value.isInt64())
						{
							this->_value = value.asInt64();
							success = true;
						}
					}
					else
					{
						if (value.isInt())
						{
							this->_value = value.asInt();
							success = true;
						}
					}
				}
			}

			if (success)
				this->Callback(VariableAction::Import);

			return success;
		}

		virtual bool IsLimitedVariable() const override { return false; }
	};

	// limited variable that can clamp input variable
	//
	// not working with bool (for obvious reasons)
	template <typename T>
	class LimitedVariable : public Variable<T>
	{
		static_assert(!std::is_same<T, bool>(), "cannot be bool");

	public:
		using Limits = std::pair<T, T>;

	protected:
		Limits _limits;

	public:
		inline LimitedVariable(const std::string& group, const std::string& key,
							   T default_value, T min, T max,
							   VariableFlags flags = 0, VariableCallback<T> callback = nullptr)
			: Variable<T>(group, key, default_value, flags, callback),
			_limits { std::make_pair(min, max) }
		{}
		
		inline T GetMin() const noexcept
		{
			return this->_limits.first;
		}
		
		inline T GetMax() const noexcept
		{
			return this->_limits.second;
		}

		inline bool Clamp(T& value) const noexcept
		{
			if (GetMin() > value)
			{
				value = GetMin();
				return true;
			}
			else if (GetMax() < value)
			{
				value = GetMax();
				return true;
			}
			else
				return false;
		}

		virtual void SetValue(T value) override
		{
			this->Clamp(value);
			Variable<T>::SetValue(value);
		}

		virtual bool Export(Json::Value& value) const override
		{
			return Variable<T>::Export(value);
		}

		virtual bool Import(const Json::Value& value) override
		{
			if (Variable<T>::Import(value))
			{
				this->Clamp(this->_value);
				return true;
			}
			else
				return false;
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

	using Int8 = Variable<std::int8_t>;
	using Int16 = Variable<std::int16_t>;
	using Int32 = Variable<std::int32_t>;
	using Int64 = Variable<std::int64_t>;
	using LInt8 = LimitedVariable<std::int8_t>;
	using LInt16 = LimitedVariable<std::int16_t>;
	using LInt32 = LimitedVariable<std::int32_t>;
	using LInt64 = LimitedVariable<std::int64_t>;

	using UInt8 = Variable<std::uint8_t>;
	using UInt16 = Variable<std::uint16_t>;
	using UInt32 = Variable<std::uint32_t>;
	using UInt64 = Variable<std::uint64_t>;
	using LUInt8 = LimitedVariable<std::uint8_t>;
	using LUInt16 = LimitedVariable<std::uint16_t>;
	using LUInt32 = LimitedVariable<std::uint32_t>;
	using LUInt64 = LimitedVariable<std::uint64_t>;

	using Float = Variable<float>;
	using LFloat = LimitedVariable<float>;

	using Bool = Variable<bool>;

	template <typename T>
	class String : public IBaseVariable<std::basic_string<T>>
	{
	public:
		inline String(const std::string& group, const std::string& key,
					  std::basic_string<T> default_value = std::basic_string<T>(),
					  VariableFlags flags = 0,
					  VariableCallback<std::basic_string<T>> callback = nullptr)
			: IBaseVariable<std::basic_string<T>>(group, key, default_value, flags, callback) {}

		virtual VariableType GetType() const override { return VariableType::String; }

		virtual bool Export(Json::Value& value) const override
		{
			value = this->_value;
			this->Callback(VariableAction::Export);
			return true;
		}

		virtual bool Import(const Json::Value& value) override
		{
			if (!value.isString())
				return false;

			this->_value = value.asString();
			this->Callback(VariableAction::Import);
			return true;
		}

		virtual void SetValue(std::basic_string<T> value) noexcept override
		{
			this->_value = value;
			this->Callback(VariableAction::ValueChanged);
		}

		virtual bool IsLimitedVariable() const override { return false; }

		inline char* GetBuffer() noexcept { return (char*)this->GetValue().data(); }
		inline void Resize(std::size_t size) noexcept { return (char*)this->GetValue().resize(size); }
	};

	template <typename T>
	class LimitedString : public String<T>
	{
	private:
		std::size_t _max_length;

	public:
		inline LimitedString(const std::string& group, const std::string& key,
							 std::size_t max_length,
							 std::basic_string<T> default_value = std::basic_string<T>(),
							 VariableFlags flags = 0,
							 VariableCallback<std::basic_string<T>> callback = nullptr)
			: String<T>(group, key, default_value.substr(0, _max_length), flags, callback),
			_max_length { max_length }
		{}

		virtual VariableType GetType() const override { return VariableType::String; }
		inline std::size_t GetMaxLength() const noexcept { return this->_max_length; }

		virtual void SetValue(std::basic_string<T> value) noexcept override
		{
			String<T>::SetValue(value.substr(0, _max_length));
		}

		virtual bool IsLimitedVariable() const override { return true; }

		// we cant handle if value has been changed
		// so it's your turn
		inline char* GetBuffer() noexcept { return (char*)this->GetValue().data(); }
	};

	class Enum : public IBaseVariable<std::deque<std::string>>
	{
	protected:
		std::size_t _currentItem;

	public:
		inline Enum(const std::string& group, const std::string& key,
					std::initializer_list<std::string> items,
					VariableFlags flags = 0,
					VariableCallback<std::deque<std::string>> callback = nullptr)
			: IBaseVariable<std::deque<std::string>>(group, key, items, flags, callback),
			_currentItem { 0 }
		{
			// empty item list disallow
			UTIL_DEBUG_ASSERT(items.size() > 0);
		}

		virtual void SetValue(std::deque<std::string>) noexcept override {}

		virtual bool Export(Json::Value& value) const override
		{
			value = _currentItem;
			this->Callback(VariableAction::Export);
			return true;
		}

		virtual bool Import(const Json::Value& value) override
		{
			if constexpr (sizeof(std::size_t) == sizeof(int))
			{
				if (!value.isUInt())
					return false;

				_currentItem = value.asUInt();
				this->Callback(VariableAction::Import);
				return true;
			}
			else
			{
				if (!value.isUInt64())
					return false;

				_currentItem = value.asUInt64();
				this->Callback(VariableAction::Import);
				return true;
			}
		}

		virtual VariableType GetType() const override { return VariableType::Enum; }
		virtual bool IsLimitedVariable() const override { return false; }


		inline auto GetCurrentItem() const noexcept { return _currentItem; }
		inline explicit operator std::size_t() const noexcept { return GetCurrentItem(); }

		inline void SetCurrentItem(std::size_t idx) noexcept
		{
			_currentItem = idx;
			this->Callback(VariableAction::ValueChanged);
		}
		inline void operator=(std::size_t idx) noexcept { SetCurrentItem(idx); }

		inline const std::string& GetStringItem() const noexcept{ return this->_value[_currentItem]; }
	};

	class Color : public IBaseVariable<std::array<std::uint8_t, 4>>
	{
	public:
		inline Color(const std::string& group, const std::string& key,
					 std::array<std::uint8_t, 4> colors,
					 VariableFlags flags = 0,
					 VariableCallback<std::array<std::uint8_t, 4>> callback = nullptr)
			: IBaseVariable<std::array<std::uint8_t, 4>>(group, key, colors, flags, callback)
		{}

		virtual VariableType GetType() const override { return VariableType::Color; }

		virtual bool Export(Json::Value& value) const override
		{
			value[0] = this->_value[0];
			value[1] = this->_value[1];
			value[2] = this->_value[2];
			value[3] = this->_value[3];
			this->Callback(VariableAction::Export);

			return true;
		}

		virtual bool Import(const Json::Value& value) override
		{
			if (!value.isArray())
				return false;

			if (value.size() < 4)
				return false;

			this->_value[0] = std::uint8_t(value[0].asUInt());
			this->_value[1] = std::uint8_t(value[1].asUInt());
			this->_value[2] = std::uint8_t(value[2].asUInt());
			this->_value[3] = std::uint8_t(value[3].asUInt());
			this->Callback(VariableAction::Import);

			return true;
		}

		virtual bool IsLimitedVariable() const override { return false; }

		virtual void SetValue(std::array<std::uint8_t, 4> value) noexcept override
		{
			this->_value = value;
			this->Callback(VariableAction::Import);
		}
		
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
			this->SetValue(
				std::uint8_t(r * 255.f),
				std::uint8_t(g * 255.f),
				std::uint8_t(b * 255.f),
				std::uint8_t(a * 255.f)
			);
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
	};

	// used for key bindings
	class Key : public IBaseVariable<ImGui::Custom::Key>
	{
	public:
		inline Key(const std::string& group, const std::string& key,
				   ImGui::Custom::Key key_value = ImGui::Custom::Key::_Invalid,
				   VariableFlags flags = 0,
				   VariableCallback<ImGui::Custom::Key> callback = nullptr)
			: IBaseVariable<ImGui::Custom::Key>(group, key, key_value, flags, callback)
		{}

		virtual void SetValue(ImGui::Custom::Key value) noexcept override
		{
			this->_value = value;
			this->Callback(VariableAction::ValueChanged);
		}

		inline bool HasValue() const noexcept { return this->_value != ImGui::Custom::Key::_Invalid; }
		inline void ResetValue() noexcept { this->SetValue(ImGui::Custom::Key::_Invalid); }

		// if key haven't "key value" then it will return 0
		inline short GetAsyncKeyState() const noexcept
		{
			return HasValue() ? ImGui::Custom::GetAsyncKeyState(this->_value) : 0;
		}

		// if key haven't "key value" then it will return false
		inline bool IsPressed(bool repeat = true) const noexcept
		{
			return HasValue() && ImGui::Custom::IsKeyPressed(this->_value, repeat);
		}

		// if key haven't "key value" then it will return false
		inline bool IsKeyReleased() const noexcept
		{
			return HasValue() && ImGui::Custom::IsKeyReleased(this->_value);
		}

		// if key haven't "key value" then it will return false
		inline bool IsKeyDown() const noexcept
		{
			return HasValue() && ImGui::Custom::IsKeyDown(this->_value);
		}


		inline bool ToVirtualKey(ImGui::Custom::VirtualKey& key) const noexcept
		{
			return ImGui::Custom::KeyToVirtualKey(this->_value, &key);
		}

		inline bool ToButtonCode(SourceSDK::ButtonCode& key) const noexcept
		{
			return ImGui::Custom::KeyToButtonCode(this->_value, &key);
		}

		virtual bool Export(Json::Value& value) const override
		{
			value = Json::Value(int(this->_value));
			this->Callback(VariableAction::Export);

			return true;
		}

		virtual bool Import(const Json::Value& value) override
		{
			if (!value.isNumeric())
				return false;

			this->_value = ImGui::Custom::Key(value.asInt());
			this->Callback(VariableAction::Import);
			return true;
		}

		virtual VariableType GetType() const override { return VariableType::Key; }
		virtual bool IsLimitedVariable() const override { return false; }
	};

	class Flags : public UInt32
	{
	private:
		std::deque<std::string> _bits_info;

	public:
		inline Flags(const std::string& group, const std::string& key,
					 std::initializer_list<std::string> bits_info,
					 VariableFlags flags = 0,
					 VariableCallback<std::uint32_t> callback = nullptr)
			: UInt32(group, key, 0, flags, callback), _bits_info(bits_info)
		{
			// empty item list disallow
			UTIL_DEBUG_ASSERT(bits_info.size() > 0);
			UTIL_DEBUG_ASSERT(bits_info.size() <= GetMaxBit());
		}

		template <typename V = int, typename F = int>
		inline Util::Flags<V, F> GetFlags() const noexcept { return Util::Flags<V, F>(this->_value); }

		template <typename V = int, typename F = int>
		inline void SetFlags(Util::Flags<V, F> flags) noexcept { this->SetValue(flags.flags); }

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

		virtual VariableType GetType() const { return VariableType::Flags; }
	};
}
