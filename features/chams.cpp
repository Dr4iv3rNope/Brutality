#include "chams.hpp"
#include "playerlist.hpp"
#include "entlist.hpp"

#include "../main.hpp"

#include "../hooks/drawmodelexecute.hpp"

#include "../sourcesdk/materialsystem.hpp"
#include "../sourcesdk/player.hpp"
#include "../sourcesdk/networkable.hpp"
#include "../sourcesdk/renderable.hpp"
#include "../sourcesdk/viewrender.hpp"
#include "../sourcesdk/viewmodel.hpp"
#include "../sourcesdk/keyvalues.hpp"
#include "../sourcesdk/vrenderview.hpp"

#include "../util/debug/labels.hpp"

#include "../config/variable.hpp"

#include "../imgui/imgui.h"

#include <array>


extern Config::Bool chamsEnable;
extern Config::Bool chamsDrawDormant;

enum Chams_
{
	Chams_Disable,
	Chams_Normal,
	Chams_Flat,
	Chams_Shiny,
	Chams_Glow,
	Chams_SpawnEffect,
	Chams_AnimShield,

	Chams__Count
};

extern Config::Color chamsNormalsVis—olor;
extern Config::Enum chamsNormalsVisType;
extern Config::Color chamsNormalsOccColor;
extern Config::Enum chamsNormalsOccType;

extern Config::Color chamsDangerousVis—olor;
extern Config::Enum chamsDangerousVisType;
extern Config::Color chamsDangerousOccColor;
extern Config::Enum chamsDangerousOccType;

extern Config::Color chamsFriendsVis—olor;
extern Config::Enum chamsFriendsVisType;
extern Config::Color chamsFriendsOccColor;
extern Config::Enum chamsFriendsOccType;

extern Config::Color chamsRagesVis—olor;
extern Config::Enum chamsRagesVisType;
extern Config::Color chamsRagesOccColor;
extern Config::Enum chamsRagesOccType;


static std::array<SourceSDK::Material*, Chams__Count - 1> chamsMaterials;

static inline decltype(chamsMaterials)::value_type& GetMaterial(Chams_ chams) noexcept
{
	return chamsMaterials[chams - 1];
}

static void BuildNormalChams() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Building normal chams material"));

	auto keys = SourceSDK::KeyValues::Create("VertexLitGeneric");
	keys->SetString("$basetexture", "Models/Debug/debugwhite");
	keys->SetInt("$halflambert", 1);

	if (GetMaterial(Chams_Normal) = interfaces->materialsystem->CreateMaterial(UTIL_CXOR("__chams_normal"), keys))
		UTIL_LABEL_OK();
	else
		UTIL_LABEL_FAIL();
}

static void BuildFlatChams() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Building flat chams material"));

	auto keys = SourceSDK::KeyValues::Create("UnlitGeneric");
	keys->SetString("$basetexture", "Models/Debug/debugwhite");
	keys->SetInt("$halflambert", 1);

	if (GetMaterial(Chams_Flat) = interfaces->materialsystem->CreateMaterial(UTIL_CXOR("__chams_flat"), keys))
		UTIL_LABEL_OK();
	else
		UTIL_LABEL_FAIL();
}

static void BuildShinyChams() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Building shiny chams material"));

	auto keys = SourceSDK::KeyValues::FromBuffer(
		UTIL_CXOR("*Shiny Chams*"),

		UTIL_CXOR("\"VertexLitGeneric\"\n"
				  "{\n"
				  "\"$basetexture\" \"Models/Debug/debugwhite\"\n"
				  "\"$model\" 1\n"
				  "\"$translucent\" 1\n"
				  "\"$halflambert\" 1\n"
				  "\"$selfillum\" 0\n"
				  "\"$surfaceprop\" \"metal\"\n"
				  "\"$envmap\" \"env_cubemap\"\n"
				  "\"$envmaptint\" \"[1 1 1]\"\n"
				  "}"
		)
	);

	if (GetMaterial(Chams_Shiny) = interfaces->materialsystem->CreateMaterial(UTIL_CXOR("__chams_shiny"), keys))
		UTIL_LABEL_OK();
	else
		UTIL_LABEL_FAIL();
}

static void BuildGlowChams() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Building glow chams material"));

	auto keys = SourceSDK::KeyValues::FromBuffer(
		UTIL_CXOR("*Normal Chams*"),

		UTIL_CXOR("\"VertexLitGeneric\"\n"
				  "{\n"
				  "\"$basetexture\" \"models/spawn_effect\"\n"
				  "\"$model\" 1\n"
				  "\"$translucent\" 1\n"
				  "\"$additive\" 1\n"
				  "\"halflambert\" 1\n"
				  "}"
		)
	);

	if (GetMaterial(Chams_Glow) = interfaces->materialsystem->CreateMaterial(UTIL_CXOR("__chams_glow"), keys))
		UTIL_LABEL_OK();
	else
		UTIL_LABEL_FAIL();
}

static void BuildSpawnEffectChams() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Building spawn effect chams material"));

	auto keys = SourceSDK::KeyValues::FromBuffer(
		UTIL_CXOR("*Spawn effect chams*"),

		UTIL_CXOR("\"UnlitGeneric\""
				  "{\n"
				  "$model \"1\"\n"
				  "$basetexture \"models/spawn_effect\"\n"
				  "$additive \"1\"\n"
				  "$halflambert \"1\"\n"
				  "$basetexturetransform \"center .2 .6 scale 100 100 rotate 0 translate 0 0\"\n"
				  "\"$translucent\" 1\n"
				  "\"Proxies\" {\n"
				  "\"TextureScroll\" {\n"
				  "\"texturescrollvar\" \"$basetexturetransform\"\n"
				  "\"texturescrollrate\" 1.0\n"
				  "\"texturescrollangle\" 36\n"
				  "}\n"
				  "}\n"
				  "}\n"
		)
	);

	if (GetMaterial(Chams_SpawnEffect) = interfaces->materialsystem->CreateMaterial(UTIL_CXOR("__chams_spawneffect"), keys))
		UTIL_LABEL_OK();
	else
		UTIL_LABEL_FAIL();
}

static void BuildAnimShieldChams() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Building animated shield chams material"));

	auto keys = SourceSDK::KeyValues::FromBuffer(
		UTIL_CXOR("*Anim Shield Chams*"),

		UTIL_CXOR("\"Unlittwotexture\"\n"
				  "{\n"
				  "\"$basetexture\" \"Effects/com_shield002a\"\n"
				  "\"$texture2\" \"Effects/com_shield004b\"\n"
				  "\"$translucent\" 1\n"
				  "\"$additive\" 1\n"
				  "\"$nodecal\" 1\n"
				  "\"Proxies\"\n"
				  "{\n"
				  "\"TextureScroll\""
				  "{\n"
				  "\"texturescrollvar\" \"$basetexturetransform\"\n"
				  "\"texturescrollrate\" .02\n"
				  "\"texturescrollangle\" 90\n"
				  "}\n"
				  "\"TextureScroll\""
				  "{\n"
				  "\"texturescrollvar\" \"$texture2transform\"\n"
				  "\"texturescrollrate\" .04\n"
				  "\"texturescrollangle\" 20\n"
				  "}\n"
				  "}\n"
				  "}"
		)
	);

	if (GetMaterial(Chams_AnimShield) = interfaces->materialsystem->CreateMaterial(UTIL_CXOR("__chams_animshield"), keys))
		UTIL_LABEL_OK();
	else
		UTIL_LABEL_FAIL();
}

#ifdef _DEBUG
static bool dbg_initialized = false;
#endif

void Features::Chams::Initialize() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Building materials for chams"));
	UTIL_DEBUG_ASSERT(!dbg_initialized);

	BuildNormalChams();
	BuildFlatChams();
	BuildShinyChams();
	BuildGlowChams();
	BuildSpawnEffectChams();
	BuildAnimShieldChams();

	#ifdef _DEBUG
	dbg_initialized = true;
	#endif
	UTIL_LABEL_OK();
}

void Features::Chams::Shutdown() noexcept
{
	UTIL_DEBUG_ASSERT(dbg_initialized);

	#ifdef _DEBUG
	dbg_initialized = false;
	#endif
}

static bool OverrideMaterial(const SourceSDK::DrawModelExecuteArgs& args,
							 float r, float g, float b, float a,
							 Chams_ type,
							 bool ignorez = false) noexcept
{
	if (type == Chams_Disable)
		return false;

	auto material = GetMaterial(type);
	UTIL_DEBUG_ASSERT(material);


	material->SetMaterialVarFlag(SourceSDK::MaterialVar_Translucent, a < 1.f);
	material->SetMaterialVarFlag(SourceSDK::MaterialVar_IgnoreZ, ignorez);

	interfaces->renderview->SetColorModulation(SourceSDK::VecColor3(r, g, b));
	interfaces->renderview->SetBlend(a);

	interfaces->modelrender->ForcedMaterialOverride(material);
	Hooks::OldDrawModelExecute(args);
	return true;
}

static inline bool OverrideMaterial(const SourceSDK::DrawModelExecuteArgs& args,
									const Config::Color& color,
									const Config::Enum& enum_type,
									bool ignorez = false) noexcept
{
	return OverrideMaterial(args,
							color.RedF(), color.GreenF(), color.BlueF(), color.AlphaF(),
							(Chams_)enum_type.GetCurrentItem(),
							ignorez);
}

static inline bool OverrideMaterial(const SourceSDK::DrawModelExecuteArgs& args,
									const Config::Enum& vis_type,
									const Config::Color& vis_color,
									const Config::Enum& occ_type,
									const Config::Color& occ_color) noexcept
{
	OverrideMaterial(args, occ_color, occ_type, true);
	return OverrideMaterial(args, vis_color, vis_type, false);
}

static inline bool SetupPlayerMaterial(const SourceSDK::DrawModelExecuteArgs& args,
									   Features::PlayerList::PlayerType type) noexcept
{
	switch (type)
	{
		case Features::PlayerList::PlayerType::Normal:
			return OverrideMaterial(args,
									chamsNormalsVisType,
									chamsNormalsVis—olor,
									chamsNormalsOccType,
									chamsNormalsOccColor);

		case Features::PlayerList::PlayerType::Dangerous:
			return OverrideMaterial(args,
									chamsDangerousVisType,
									chamsDangerousVis—olor,
									chamsDangerousOccType,
									chamsDangerousOccColor);

		case Features::PlayerList::PlayerType::Friend:
			return OverrideMaterial(args,
									chamsFriendsVisType,
									chamsFriendsVis—olor,
									chamsFriendsOccType,
									chamsFriendsOccColor);

		case Features::PlayerList::PlayerType::Rage:
			return OverrideMaterial(args,
									chamsRagesVisType,
									chamsRagesVis—olor,
									chamsRagesOccType,
									chamsRagesOccColor);
		default:
			UTIL_DEBUG_ASSERT(false);
			return false;
	}
}

bool Features::Chams::Render(const SourceSDK::DrawModelExecuteArgs& args) noexcept
{
	if (!chamsEnable)
		return false;

	auto entity = SourceSDK::BaseEntity::GetByIndex(args.info->entityIndex);

	if (!entity)
		return false;

	auto client_class = entity->ToNetworkable()->GetClientClass();

	if (client_class->IsPlayer())
	{
		auto player = (SourceSDK::BasePlayer*)entity;

		if (player->IsDead())
			return false;

		if (!chamsDrawDormant && player->ToNetworkable()->IsDormant())
			return false;

		if (PlayerList::PlayerType type;
			PlayerList::GetPlayerType(args.info->entityIndex - 1, type))
			return SetupPlayerMaterial(args, type);
	}
	else // default entity
	{
		if (auto info = EntityList::GetEntitySettings(entity->GetClassname()))
		{
			if (info->chamsOccType)
			{
				auto color = ImGui::ColorConvertU32ToFloat4(info->chamsOccColor);

				return OverrideMaterial(
					args,
					color.x, color.y, color.z, color.w,
					(Chams_)info->chamsOccType,
					true
				);
			}

			if (info->chamsVisType)
			{
				auto color = ImGui::ColorConvertU32ToFloat4(info->chamsVisColor);

				return OverrideMaterial(
					args,
					color.x, color.y, color.z, color.w,
					(Chams_)info->chamsVisType,
					false
				);
			}
		}
	}
	
	return false;
}
