#include "Sprite.h"
#include "Shader.h"
#include "Texture.h"
#include "../ErrorLogger.h"

Sprite::Sprite(ID3D11Device* device, const wchar_t* filename)
{
	// ���_���
	vertex vertices[]
	{
		{{ -0.5f,  0.5f, 0 }, { 1, 1, 1, 1 }, { 0, 0 }},
		{{  0.5f,  0.5f, 0 }, { 1, 0, 0, 1 }, { 1, 0 }},
		{{ -0.5f, -0.5f, 0 }, { 0, 1, 0, 1 }, { 0, 1 }},
		{{  0.5f, -0.5f, 0 }, { 0, 0, 1, 1 }, { 1, 1 }},
	};

	// ���_�o�b�t�@�I�u�W�F�N�g�̐���
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
	}

	// ���_�V�F�[�_�[�I�u�W�F�N�g�̐����E���̓��C�A�E�g�I�u�W�F�N�g�̐���
	const char* csoName{ "./Shader/SpriteVs.cso" };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Shader shader;
	shader.createVsFromCso(device, csoName, &vertexShader, &inputLayout, inputElementDesc, _countof(inputElementDesc));

	// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
	csoName = { "./Shader/SpritePs.cso" };

	shader.createPsFromCso(device, csoName, &pixelShader);

	// �摜�t�@�C���̃��[�h�ƃV�F�[�_�[���\�[�X�r���[�̐���
	Texture texture;
	texture.loadTextureFromFile(device, filename, &shaderResourceView, &texture2dDesc);
}

void Sprite::render(ID3D11DeviceContext* deviceContext, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle)
{
	render(deviceContext, dx, dy, dw, dh, r, g, b, a, angle, 0, 0, texture2dDesc.Width, texture2dDesc.Height);
}

void Sprite::render(ID3D11DeviceContext* deviceContext, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)
{
	auto rotate = [](float& x, float& y, float cx, float cy, float angle)
	{
		x -= cx;
		y -= cy;

		float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
		float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
		float tx{ x }, ty{ y };
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;

		x += cx;
		y += cy;
	};


	// �X�N���[��(�r���[�|�[�g)�̃T�C�Y���擾����
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	deviceContext->RSGetViewports(&num_viewports, &viewport);

	// dx, dy, dw, dh ����X�N���[�����W�n�̌v�Z

	// left-top
	float x0{ dx };
	float y0{ dy };
	// right-top
	float x1{ dx + dw };
	float y1{ dy };
	// left-bottom
	float x2{ dx };
	float y2{ dy + dh };
	// right-bottom
	float x3{ dx + dw };
	float y3{ dy + dh };

	// ��]�̒��S����`�̒��S�_�ɂ����ꍇ
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	rotate(x0, y0, cx, cy, angle);
	rotate(x1, y1, cx, cy, angle);
	rotate(x2, y2, cx, cy, angle);
	rotate(x3, y3, cx, cy, angle);

	// �X�N���[�����W�n����NDC�ւ̍��W�ϊ����s��
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	// 
	float u0{ sx / texture2dDesc.Width };
	float v0{ sy / texture2dDesc.Height };
	float u1{ (sx + sw) / texture2dDesc.Width };
	float v1{ (sy + sh) / texture2dDesc.Height };


	// �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g�̍X�V
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = deviceContext->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex* vertices{ reinterpret_cast<vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0, 0 };
		vertices[1].position = { x1, y1, 0 };
		vertices[2].position = { x2, y2, 0 };
		vertices[3].position = { x3, y3, 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };


		vertices[0].texcoord = { u0, v0 };
		vertices[1].texcoord = { u1, v0 };
		vertices[2].texcoord = { u0, v1 };
		vertices[3].texcoord = { u1, v1 };
	}
	deviceContext->Unmap(vertexBuffer.Get(), 0);


	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };

	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	deviceContext->IASetInputLayout(inputLayout.Get());

	deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	deviceContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());


	deviceContext->Draw(4, 0);
}
