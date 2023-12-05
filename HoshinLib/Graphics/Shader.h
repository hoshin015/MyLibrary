#pragma once
#include <wrl.h>
#include <d3d11.h>

class Shader
{
public:
	HRESULT createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);

	HRESULT createPsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader);
};