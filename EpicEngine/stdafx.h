#pragma once

#define WIN32_LEAN_AND_MEAN //���� ������ ���� ������ ������� ����

#include<Windows.h>

#include<stdio.h>
#include<memory.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include<d3d11.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<d3dx10math.h>
using namespace DirectX;