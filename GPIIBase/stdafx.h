// stdafx.h 

#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

// DirectX
#define _XM_NO_INTRINSICS_
#include <dxgi.h>
#include <d3d11.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include <libstb.h>
#pragma comment(lib, "libstb.lib")

#include "uMath.h"
#include "Utils.h"
#include "Config.h"
