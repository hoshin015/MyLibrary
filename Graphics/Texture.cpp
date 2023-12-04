#include "texture.h"
#include "../ErrorLogger.h"

static map<wstring, ComPtr<ID3D11ShaderResourceView>> resources;
HRESULT Texture::loadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc)
{
	HRESULT hr{ S_OK };
	ComPtr<ID3D11Resource> resource;

	auto it = resources.find(filename);
	if (it != resources.end())
	{
		*shaderResourceView = it->second.Get();
		(*shaderResourceView)->AddRef();
		(*shaderResourceView)->GetResource(resource.GetAddressOf());
	}
	else
	{
		hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shaderResourceView);
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create wic texture from file.");
		}
		resources.insert(make_pair(filename, *shaderResourceView));
	}

	ComPtr<ID3D11Texture2D> texture2d;
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	texture2d->GetDesc(texture2dDesc);

	return hr;
}

void Texture::releaseAllTextures()
{
	resources.clear();
}
