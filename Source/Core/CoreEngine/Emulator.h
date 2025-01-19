#pragma once 

#include <stdint.h>
#include <math.h>
#include <glm/glm.hpp>
#include <stdlib.h>
#include <cstring>

#include "Common.h"


namespace NESEmu {

	uint8_t* GetFramebuffer();
	void Update(float Time, float DeltaTime);
}