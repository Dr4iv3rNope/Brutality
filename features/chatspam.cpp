#include "chatspam.hpp"

#include "../main.hpp"

#include "../util/xorstr.hpp"

#include "../sourcesdk/const.hpp"
#include "../sourcesdk/globals.hpp"
#include "../sourcesdk/clientstate.hpp"
#include "../sourcesdk/engineclient.hpp"

#include "../jsoncpp/reader.h"
#include "../jsoncpp/writer.h"

#include "../config/variable.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "../imgui/custom/windowmanager.hpp"
#include "../imgui/custom/specialchars.hpp"
#include "../imgui/custom/variableui.hpp"
#include "../imgui/custom/errors.hpp"

#include <string>
#include <fstream>
#include <deque>

extern Config::Bool chatSpamEnable;
extern Config::String<char> chatSpamMessage;
extern Config::LFloat chatSpamDelay;
extern Config::Bool chatSpamTeam;

enum Mode_
{
	Mode_Default,
	Mode_Animation,
};

extern Config::Enum chatSpamMode;

struct Animation
{
	std::string name;
	std::deque<std::string> sequences;
};

static std::deque<Animation> animationList;
static int currentAnimation { -1 };
static int currentSequence { -1 };

static float lastSay { 0.f };

static inline bool TryToSay(const std::string& message) noexcept
{
	if (lastSay < interfaces->globals->curTime)
	{
		const auto msg = (*chatSpamTeam ? UTIL_SXOR("say_team \"") : UTIL_SXOR("say \""))
			+ ImGui::Custom::FormatSpecialChars(message) + '\"';

		interfaces->engine->ServerCmd(msg.c_str());

		if (*chatSpamDelay == 0.f)
			lastSay = interfaces->globals->curTime + SourceSDK::TALK_INTERVAL /* add some delay */ + 0.05f;
		else
			lastSay = interfaces->globals->curTime + *chatSpamDelay;

		return true;
	}
	else
		return false;
}

void Features::ChatSpam::Think() noexcept
{
	if (*chatSpamEnable && interfaces->clientstate->IsInGame())
	{
		switch (chatSpamMode.GetCurrentItem())
		{
			case Mode_Default:
			{
				TryToSay(*chatSpamMessage);

				break;
			}

			case Mode_Animation:
			{
				if (currentAnimation != -1 && !animationList[currentAnimation].sequences.empty())
				{
					static int currentMessage { 0 };

					if (std::size_t(currentMessage) >= animationList[currentAnimation].sequences.size())
						currentMessage = 0;

					if (TryToSay(animationList[currentAnimation].sequences[currentMessage]))
						currentMessage++;
				}
				break;
			}
		}
	}
	else
		lastSay = 0.f;
}

static inline const std::wstring& GetAnimationsPath() noexcept
{
	static auto path { Main::GetLocalPath() + UTIL_SXOR(L"chatspam_animations.json") };

	return path;
}

constexpr int ERR_NOERR { -1 };
constexpr int ERR_FAILED_OPEN { 0 };
constexpr int ERR_SUCCESS_BUT_GOT_WARNS { 1 };
constexpr int ERR_NOT_VALID { 2 };

static int currentError { ERR_NOERR };
static ImGui::Custom::ErrorList errorList =
{
	ImGui::Custom::Error(
		UTIL_SXOR("Failed to open file"),
		ImVec4(1.f, 0.f, 0.f, 1.f),
		UTIL_SXOR("See logs for more information")
	),

	ImGui::Custom::Error(
		UTIL_SXOR("Success, but got errors"),
		ImVec4(1.f, 1.f, 0.f, 1.f),
		UTIL_SXOR("Check the logs for more information")
	),

	ImGui::Custom::Error(
		UTIL_SXOR("Animation file have wrong json data"),
		ImVec4(1.f, 0.f, 0.f, 1.f)
	)
};

static bool LoadAnimations() noexcept
{
	UTIL_XLOG(L"Loading chat spam animations");

	std::ifstream file(GetAnimationsPath());

	if (file)
	{
		bool got_warnings { false };

		Json::Value root;
		file >> root;

		if (root.isArray())
		{
			animationList.clear();

			currentAnimation = -1;

			for (auto& animation : root)
			{
				if (!animation.isObject())
				{
					UTIL_XLOG(L"Animation is not a object!");
					got_warnings = true;

					continue;
				}

				if (!animation[UTIL_SXOR("name")].isString())
				{
					UTIL_XLOG(L"\"name\" of the animation must be a string");
					got_warnings = true;

					continue;
				}

				std::string name = animation[UTIL_SXOR("name")].asString();

				if (animation[UTIL_SXOR("sequences")].isArray())
				{
					std::deque<std::string> parsedSequences;

					for (auto& sequences : animation[UTIL_SXOR("sequences")])
					{

						if (sequences.isString())
							parsedSequences.push_back(sequences.asString());
						else
						{
							UTIL_XLOG(L"In sequence found non-string value");
							got_warnings = true;

							continue;
						}
					}

					Animation anim;
					anim.name = name;
					anim.sequences = parsedSequences;

					animationList.push_back(anim);
				}
				else
				{
					UTIL_XLOG(L"Failed to get \"sequences\" of the animation");
					got_warnings = true;
				}
			}
		}
		else
		{
			UTIL_XLOG(L"Waited for array of animations, but got other value!");
			currentError = ERR_NOT_VALID;

			file.close();
			return false;
		}

		if (got_warnings)
			currentError = ERR_SUCCESS_BUT_GOT_WARNS;
		else
			currentError = ERR_NOERR;

		UTIL_XLOG(L"Successfully loaded animations");

		file.close();
		return true;
	}
	else
	{
		UTIL_XLOG(L"Failed to open file");
		currentError = ERR_FAILED_OPEN;

		return false;
	}
}

static bool SaveAnimations() noexcept
{
	UTIL_XLOG(L"Saving animations");

	std::ofstream file(GetAnimationsPath());

	if (file)
	{
		auto root = Json::Value(Json::arrayValue);

		for (const auto& animation : animationList)
		{
			auto json_anim = Json::Value(Json::objectValue);

			json_anim[UTIL_SXOR("name")] = animation.name;
			json_anim[UTIL_SXOR("sequences")] = Json::Value(Json::arrayValue);

			for (const auto& sequence : animation.sequences)
				json_anim[UTIL_SXOR("sequences")].append(sequence);

			root.append(json_anim);
		}

		file << root;
		file.close();
		
		UTIL_XLOG(L"Successfully saved animations");
		return true;
	}
	else
	{
		UTIL_XLOG(L"Failed to open file");
		currentError = ERR_FAILED_OPEN;

		return false;
	}
}

static void DrawMenu(ImGui::Custom::Window&) noexcept
{
	ImGui::Custom::Variable::Boolean(chatSpamEnable);
	ImGui::Custom::Variable::Boolean(chatSpamTeam);
	ImGui::Custom::Variable::LimitedFloat(chatSpamDelay);
	ImGui::NewLine();
	ImGui::Custom::Variable::Enum(chatSpamMode);

	ImGui::Separator();

	switch (chatSpamMode.GetCurrentItem())
	{
		case Mode_Default:
		{
			ImGui::Custom::Variable::String(chatSpamMessage);

			if (ImGui::IsItemHovered())
				ImGui::Custom::ShowSpecialChars();

			break;
		}

		case Mode_Animation:
		{
			ImGui::Custom::StatusError(currentError, errorList);

			if (ImGui::Button(UTIL_CXOR("Load Animations")))
				LoadAnimations();

			ImGui::SameLine();

			if (ImGui::Button(UTIL_CXOR("Save Animations")))
				SaveAnimations();

			ImGui::Separator();

			static std::string name_buffer, msg_buffer;

			ImGui::Columns(2);

			//
			// animation list
			//

			ImGui::Text(UTIL_CXOR("Current Animation Index: %i"), currentAnimation);

			ImGui::PushID(UTIL_CXOR("##ANIM_LIST"));
			ImGui::PushItemWidth(-1.f);
			if (ImGui::ListBox("", &currentAnimation, [] (void*, int idx, const char** out) -> bool
			{
				*out = animationList[idx].name.c_str();

				return true;
			}, nullptr, animationList.size(), 5))
			{
				name_buffer = animationList[currentAnimation].name;

				currentSequence = -1;
			}
			ImGui::PopID(); // ##ANIM_LIST

			ImGui::TextUnformatted(UTIL_CXOR("Name"));

			ImGui::PushID(UTIL_CXOR("##ANIM_INPUT"));
			ImGui::InputText("", &name_buffer);
			ImGui::PopID(); // ##ANIM_INPUT

			ImGui::PushID(UTIL_CXOR("##BUTTONANIM_NEW"));
			if (ImGui::Button(UTIL_CXOR("New")))
			{
				Animation anim;
				anim.name = name_buffer;

				animationList.push_back(anim);
				currentAnimation = animationList.size() - 1;
			}
			ImGui::PopID(); // ##BUTTONANIM_NEW

			SKIP_SEQUENCE:
			if (currentAnimation != -1)
			{
				ImGui::SameLine();

				ImGui::PushID(UTIL_CXOR("##BUTTONANIM_OVERRIDE"));
				if (ImGui::Button(UTIL_CXOR("Override")))
					animationList[currentAnimation].name = name_buffer;
				ImGui::PopID(); // ##BUTTONANIM_OVERRIDE

				ImGui::SameLine();

				ImGui::PushID(UTIL_CXOR("##BUTTONANIM_DELETE"));
				if (ImGui::Button(UTIL_CXOR("Delete")))
				{
					animationList.erase(animationList.begin() + currentAnimation);

					currentAnimation = -1;

					ImGui::PopID(); // ##BUTTONANIM_DELETE
					goto SKIP_SEQUENCE;
				}
				ImGui::PopID(); // ##BUTTONANIM_DELETE

				//
				// sequence list
				//
				auto& sequences = animationList[currentAnimation].sequences;

				ImGui::NextColumn();
				ImGui::Text(UTIL_CXOR("Current Sequence Index: %i"), currentSequence);

				ImGui::PushID(UTIL_CXOR("##SEQ_LIST"));
				ImGui::PushItemWidth(-1.f);
				if (ImGui::ListBox("", &currentSequence, [] (void* sequence, int idx, const char** out) -> bool
				{
					const auto& str = reinterpret_cast<std::deque<std::string>*>(sequence)->at(idx);

					*out = str.c_str();
					return true;
				}, &sequences, sequences.size(), 5))
					msg_buffer = sequences[currentSequence];
				ImGui::PopID(); // ##SEQ_LIST

				ImGui::TextUnformatted(UTIL_CXOR("Message"));

				ImGui::PushID(UTIL_CXOR("##SEQ_INPUT"));
				ImGui::InputText("", &msg_buffer);

				if (ImGui::IsItemHovered())
					ImGui::Custom::ShowSpecialChars();
				ImGui::PopID(); // ##SEQ_INPUT

				ImGui::PushID(UTIL_CXOR("##BUTTON_NEW"));
				if (ImGui::Button(UTIL_CXOR("New")))
					sequences.push_back(msg_buffer);
				ImGui::PopID(); // ##BUTTON_NEW

				if (currentSequence != -1)
				{
					ImGui::SameLine();

					ImGui::PushID(UTIL_CXOR("##BUTTON_OVERRIDE"));
					if (ImGui::Button(UTIL_CXOR("Override")))
						sequences[currentSequence] = msg_buffer;
					ImGui::PopID(); // ##BUTTON_OVERRIDE

					ImGui::SameLine();

					ImGui::PushID(UTIL_CXOR("##BUTTON_DELETE"));
					if (ImGui::Button(UTIL_CXOR("Delete")))
					{
						sequences.erase(sequences.begin() + currentSequence);

						currentSequence = -1;
					}
					ImGui::PopID(); // ##BUTTON_OVERRIDE
				}

				ImGui::NewLine();

				ImGui::Text(
					UTIL_CXOR("Total time to print: %f s."),
					*chatSpamDelay != 0.f
					? *chatSpamDelay * float(sequences.size())
					: SourceSDK::TALK_INTERVAL * float(sequences.size())
				);

				//
				// END sequence list
				//
			}

			//
			// END animation list
			//

			ImGui::Columns();
			break;
		}
	}
}

void Features::ChatSpam::RegisterWindow() noexcept
{
	ImGui::Custom::windowManager->RegisterWindow(
		ImGui::Custom::Window(
			UTIL_SXOR("Chat Spammer"),
			0,
			DrawMenu
		)
	);
}
