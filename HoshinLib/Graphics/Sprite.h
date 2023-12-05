#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

class Sprite
{
public:
	Sprite(ID3D11Device* device, const wchar_t* filename);
	~Sprite() {};

	void render(ID3D11DeviceContext* deviceContext, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle);
	void render(ID3D11DeviceContext* deviceContext, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh);

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2dDesc;
private:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
};