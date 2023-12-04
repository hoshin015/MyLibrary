//#include "Shader.h"
//
//bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
//{
//	// shaderpathのメモリを読み込む
//	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
//	if (FAILED(hr))
//	{
//		std::wstring errorMsg = L"Failed to load shader: ";
//		errorMsg += shaderpath;
//		ErrorLogger::Log(hr, errorMsg);
//		return false;
//	}
//
//	// 頂点シェーダオブジェクトの作成
//	hr = device->CreateVertexShader(this->shader_buffer.Get()->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
//	if (FAILED(hr))
//	{
//		std::wstring errorMsg = L"Failed to create vertex shader: ";
//		errorMsg += shaderpath;
//		ErrorLogger::Log(hr, errorMsg);
//		return false;
//	}
//
//	// inputLayoutオブジェクトの作成
//	hr = device->CreateInputLayout(layoutDesc, numElements, this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), this->inputLayout.GetAddressOf());
//	if (FAILED(hr))
//	{
//		ErrorLogger::Log(hr, "Failed to create input layout.");
//		return false;
//	}
//
//	return true;
//}
//
//ID3D11VertexShader* VertexShader::GetShader()
//{
//	return this->shader.Get();
//}
//
//ID3D10Blob* VertexShader::GetBuffer()
//{
//	return this->shader_buffer.Get();
//}
//
//ID3D11InputLayout* VertexShader::GetInputLayout()
//{
//	return this->inputLayout.Get();
//}
//
//
//
//
//
//bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
//{
//	// shaderpathのメモリを読み込む
//	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
//	if (FAILED(hr))
//	{
//		std::wstring errorMsg = L"Failed to load shader: ";
//		errorMsg += shaderpath;
//		ErrorLogger::Log(hr, errorMsg);
//		return false;
//	}
//
//	// ピクセルシェーダオブジェクトの作成
//	hr = device->CreatePixelShader(this->shader_buffer.Get()->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
//	if (FAILED(hr))
//	{
//		std::wstring errorMsg = L"Failed to create pixel shader: ";
//		errorMsg += shaderpath;
//		ErrorLogger::Log(hr, errorMsg);
//		return false;
//	}
//
//	return true;
//}
//
//ID3D11PixelShader* PixelShader::GetShader()
//{
//	return this->shader.Get();
//}
//
//ID3D10Blob* PixelShader::GetBuffer()
//{
//	return this->shader_buffer.Get();
//}

#include "Shader.h"
#include <cstdio>
#include "../ErrorLogger.h"
#include <memory>

HRESULT Shader::createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, csoName, "rb");
	if (FAILED(fp))
	{
		ErrorLogger::Log("CSO File not found");
	}
	//_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoData{ std::make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);


	HRESULT hr = device->CreateVertexShader(csoData.get(), csoSz, nullptr, vertexShader);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (inputLayout)
	{
		hr = device->CreateInputLayout(inputElementDesc, numElements, csoData.get(), csoSz, inputLayout);
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	return hr;
}

HRESULT Shader::createPsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, csoName, "rb");
	//_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz = { ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoData = { std::make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);


	HRESULT hr = device->CreatePixelShader(csoData.get(), csoSz, nullptr, pixelShader);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}
