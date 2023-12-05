#include "Shader.h"
#include <cstdio>
#include "../ErrorLogger.h"
#include <memory>

HRESULT Shader::createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, csoName, "rb");
	if (fp == nullptr)
	{
		ErrorLogger::Log("CSO File not found");
	}

	fseek(fp, 0, SEEK_END);
	long csoSz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoData{ std::make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);


	HRESULT hr = device->CreateVertexShader(csoData.get(), csoSz, nullptr, vertexShader);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex shader.");
	}

	if (inputLayout)
	{
		hr = device->CreateInputLayout(inputElementDesc, numElements, csoData.get(), csoSz, inputLayout);
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create input layout.");
		}
	}

	return hr;
}



HRESULT Shader::createPsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, csoName, "rb");
	if (fp == nullptr)
	{
		ErrorLogger::Log("CSO File not found");
	}

	fseek(fp, 0, SEEK_END);
	long csoSz = { ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoData = { std::make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);


	HRESULT hr = device->CreatePixelShader(csoData.get(), csoSz, nullptr, pixelShader);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create pixel shader.");
	}

	return hr;
}
