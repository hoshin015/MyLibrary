#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <Shlwapi.h>

class Font
{
public:
	Font(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~Font();

	void initialize();

	void render(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle);
	void render(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh);

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	D3D11_TEXTURE2D_DESC texture2dDesc;
private:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
};