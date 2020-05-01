#include "vector.hpp"
#include "worldtoscreen.hpp"

SourceSDK::W2SOutput SourceSDK::Vector::ToScreen() const noexcept
{
	return WorldToScreen(*this);
}
