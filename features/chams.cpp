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


static std::array<SourceSDK::Material*, int(Features::Chams::Type::_Count) - 1> chamsMaterials;

static inline decltype(chamsMaterials)::value_type& GetChamsMaterial(Features::Chams::Type chams) noexcept
{
	return chamsMaterials[int(chams) - 1];
}

static void BuildNormalChams() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Building normal chams material"));

	auto keys = SourceSDK::KeyValues::Create(UTIL_CXOR("VertexLitGeneric"));
	keys->SetString(UTIL_CXOR("$basetexture"), UTIL_CXOR("Models/Debug/debugwhite"));
	keys->SetInt(UTIL_CXOR("$halflambert"), 1);

	if (::GetChamsMaterial(Features::Chams::Type::Normal) = interfaces->materialsystem->CreateMaterial(UTIL_CXOR("__chams_normal"), keys))
		UTIL_LABEL_OK();
	else
		UTIL_LABEL_FAIL();
}

static void BuildFlatChams() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Building flat chams material"));

	auto keys = SourceSDK::KeyValues::Create("UnlitGeneric");
	keys->SetString(UTIL_CXOR("$basetexture"), UTIL_CXOR("Models/Debug/debugwhite"));
	keys->SetInt(UTIL_CXOR("$halflambert"), 1);

	if (::GetChamsMaterial(Features::Chams::Type::Flat) = interfaces->materialsystem->CreateMaterial(UTIL_CXOR("__chams_flat"), keys))
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

	if (::GetChamsMaterial(Features::Chams::Type::Shiny) = interfaces->materialsystem->CreateMaterial(UTIL_CXOR("__chams_shiny"), keys))
		UTIL_LABEL_OK();
	else
		UTIL_LABEL_FAIL();
}

#if BUILD_GAME_IS_GMOD

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

	if (::GetChamsMaterial(Features::Chams::Type::Glow) = interfaces->materialsystem->CreateMaterial(UTIL_CXOR("__chams_glow"), keys))
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

	if (::GetChamsMaterial(Features::Chams::Type::SpawnEffect) = interfaces->materialsystem->CreateMaterial(UTIL_CXOR("__chams_spawneffect"), keys))
		UTIL_LABEL_OK();
	else
		UTIL_LABEL_FAIL();
}

#endif

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

	#if BUILD_GAME_IS_GMOD
	BuildGlowChams();
	BuildSpawnEffectChams();
	#endif

	#ifdef _DEBUG
	dbg_initialized = true;
	#endif
	UTIL_LABEL_OK();
}

void Features::Chams::Shutdown() noexcept
{
	UTIL_DEBUG_ASSERT(dbg_initialized);

	for (auto& chams : chamsMaterials)
		chams->DecrementReferenceCount();

	chamsMaterials.fill(nullptr);

	#ifdef _DEBUG
	dbg_initialized = false;
	#endif
}

static bool OverrideMaterial(const SourceSDK::DrawModelExecuteArgs& args, 
							 SourceSDK::VecColor4 color,
							 Features::Chams::Type type,
							 bool ignorez = false) noexcept
{
	if (type == Features::Chams::Type::Disable)
		return false;

	auto material = ::GetChamsMaterial(type);
	UTIL_DEBUG_ASSERT(material);


	material->SetMaterialVarFlag(SourceSDK::MaterialVar_Translucent, color.Alpha() < 1.f);
	material->SetMaterialVarFlag(SourceSDK::MaterialVar_IgnoreZ, ignorez);

	interfaces->renderview->SetColorModulation(
		SourceSDK::VecColor3(color.Red(), color.Green(), color.Blue())
	);
	interfaces->renderview->SetBlend(color.Alpha());

	interfaces->modelrender->ForcedMaterialOverride(material);
	Hooks::OldDrawModelExecute(args);
	return true;
}

static inline bool SetupPlayerMaterial(const SourceSDK::DrawModelExecuteArgs& args,
									   Features::PlayerList::PlayerType type) noexcept
{
	switch (type)
	{
		case Features::PlayerList::PlayerType::Normal:
			return Features::Chams::RenderChams(args,
												Features::Chams::Type(chamsNormalsVisType.GetCurrentItem()),
												SourceSDK::VecColor4(
													chamsNormalsVis—olor.RedF(),
													chamsNormalsVis—olor.GreenF(),
													chamsNormalsVis—olor.BlueF()
												),
												Features::Chams::Type(chamsNormalsOccType.GetCurrentItem()),
												SourceSDK::VecColor4(
													chamsNormalsOccColor.RedF(),
													chamsNormalsOccColor.GreenF(),
													chamsNormalsOccColor.BlueF()
												));

		case Features::PlayerList::PlayerType::Dangerous:
			return Features::Chams::RenderChams(args,
												Features::Chams::Type(chamsDangerousVisType.GetCurrentItem()),
												SourceSDK::VecColor4(
													chamsDangerousOccColor.RedF(),
													chamsDangerousOccColor.GreenF(),
													chamsDangerousOccColor.BlueF()
												),
												Features::Chams::Type(chamsDangerousOccType.GetCurrentItem()),
												SourceSDK::VecColor4(
													chamsDangerousOccColor.RedF(),
													chamsDangerousOccColor.GreenF(),
													chamsDangerousOccColor.BlueF()
												));

		case Features::PlayerList::PlayerType::Friend:
			return Features::Chams::RenderChams(args,
												Features::Chams::Type(chamsFriendsVisType.GetCurrentItem()),
												SourceSDK::VecColor4(
													chamsFriendsOccColor.RedF(),
													chamsFriendsOccColor.GreenF(),
													chamsFriendsOccColor.BlueF()
												),
												Features::Chams::Type(chamsFriendsOccType.GetCurrentItem()),
												SourceSDK::VecColor4(
													chamsFriendsOccColor.RedF(),
													chamsFriendsOccColor.GreenF(),
													chamsFriendsOccColor.BlueF()
												));

		case Features::PlayerList::PlayerType::Rage:
			return Features::Chams::RenderChams(args,
												Features::Chams::Type(chamsRagesVisType.GetCurrentItem()),
												SourceSDK::VecColor4(
													chamsRagesOccColor.RedF(),
													chamsRagesOccColor.GreenF(),
													chamsRagesOccColor.BlueF()
												),
												Features::Chams::Type(chamsRagesOccType.GetCurrentItem()),
												SourceSDK::VecColor4(
													chamsRagesOccColor.RedF(),
													chamsRagesOccColor.GreenF(),
													chamsRagesOccColor.BlueF()
												));
		default:
			UTIL_DEBUG_ASSERT(false);
			return false;
	}
}

SourceSDK::Material* Features::Chams::GetChamsMaterial(Type chams_type) noexcept
{
	return chamsMaterials[int(chams_type)];
}

bool Features::Chams::RenderChams(const SourceSDK::DrawModelExecuteArgs& args,
								  Type vis_type, SourceSDK::VecColor4 vis_color,
								  Type occ_type, SourceSDK::VecColor4 occ_color) noexcept
{
	return OverrideMaterial(args, occ_color, occ_type, true) ||
		OverrideMaterial(args, vis_color, vis_type, false);
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
					SourceSDK::VecColor4(color.x, color.y, color.z, color.w),
					Features::Chams::Type(info->chamsOccType),
					true
				);
			}

			if (info->chamsVisType)
			{
				auto color = ImGui::ColorConvertU32ToFloat4(info->chamsVisColor);

				return OverrideMaterial(
					args,
					SourceSDK::VecColor4(color.x, color.y, color.z, color.w),
					Features::Chams::Type(info->chamsVisType),
					false
				);
			}
		}
	}
	
	return false;
}
