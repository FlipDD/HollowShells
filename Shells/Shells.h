#pragma once

#include "Core.h"
#include "UnrealNetwork.h"
#include "Online.h"

#define SURFACE_FLESHDEFAULT		SurfaceType1
#define SURFACE_FLESHVULNERABLE		SurfaceType2

#define COLLISION_WEAPON			ECC_GameTraceChannel1

#define guard(val, ret) if(!ensure((val))) return ret;
