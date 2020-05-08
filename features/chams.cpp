#include "chams.hpp"

#include "../sourcesdk/materialsystem.hpp"
#include "../sourcesdk/keyvalues.hpp"

#include "../util/debug/labels.hpp"

#include "../config/variable.hpp"

extern Config::Bool chamsEnable;

static constexpr int MODE_DISABLE { 0 };
static constexpr int MODE_NORMAL { 1 };
static constexpr int MODE_FLAT { 2 };
static constexpr int MODE_WIREFRAME { 3 };

extern Config::Enum chamsNormalsMode;
extern Config::Bool chamsNormalsDrawZ;

extern Config::Bool chamsDangerousDrawZ;
extern Config::Enum chamsDangerousMode;

extern Config::Bool chamsFriendsDrawZ;
extern Config::Enum chamsFriendsMode;

extern Config::Bool chamsRagesDrawZ;
extern Config::Enum chamsRagesMode;

extern Config::Enum chamsArmsMode;
extern Config::Enum chamsWeaponsMode;
extern Config::Enum chamsEntitiesMode;


static SourceSDK::Material* normalChams;
static SourceSDK::Material* flatChams;

void Features::Chams::Initialize() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Building materials for chams"));

	auto keys = SourceSDK::KeyValues::Create(UTIL_CXOR("VertexLitGeneric"));
	keys->SetInt(UTIL_CXOR("$model"), 1);

	normalChams = SourceSDK::materialSystem->CreateMaterial(UTIL_CXOR("__chams_normal"), keys);

	keys = SourceSDK::KeyValues::Create(UTIL_CXOR("UnlitGeneric"));
	keys->SetInt(UTIL_CXOR("$model"), 1);

	normalChams = SourceSDK::materialSystem->CreateMaterial(UTIL_CXOR("__chams_flat"), keys);

	UTIL_LABEL_OK();
}

void Features::Chams::Shutdown() noexcept
{
	delete normalChams;
	delete flatChams;
}

bool Features::Chams::Render(const SourceSDK::DrawModelState& state,
							 const SourceSDK::ModelRenderInfo& info,
							 SourceSDK::Matrix3x4* boneToWorld) noexcept
{
	SourceSDK::modelrender->ForcedMaterialOverride(normalChams);
	SourceSDK::modelrender->DrawModelExecute(state, info, boneToWorld);
	SourceSDK::modelrender->ForcedMaterialOverride(nullptr);

	return true;
}
