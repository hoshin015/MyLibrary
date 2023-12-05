#include "Font.h"
#include "Shader.h"
#include "Texture.h"
#include "../ErrorLogger.h"

Font::Font(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	// 頂点情報
	vertex vertices[]
	{
		{{ -0.5f,  0.5f, 0 }, { 1, 1, 1, 1 }, { 0, 0 }},
		{{  0.5f,  0.5f, 0 }, { 1, 1, 1, 1 }, { 1, 0 }},
		{{ -0.5f, -0.5f, 0 }, { 1, 1, 1, 1 }, { 0, 1 }},
		{{  0.5f, -0.5f, 0 }, { 1, 1, 1, 1 }, { 1, 1 }},
	};

	// 頂点バッファオブジェクトの生成
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

	// 頂点シェーダーオブジェクトの生成・入力レイアウトオブジェクトの生成
	const char* csoName{ "./Data/Shader/SpriteVs.cso" };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Shader shader;
	shader.createVsFromCso(device, csoName, &vertexShader, &inputLayout, inputElementDesc, _countof(inputElementDesc));

	// ピクセルシェーダーオブジェクトの生成
	csoName = { "./Data/Shader/SpritePs.cso" };

	shader.createPsFromCso(device, csoName, &pixelShader);


	// フォントを使えるようにする
	DESIGNVECTOR design;
	AddFontResourceEx(
		TEXT("./Data/Fonts/APJapanesefontF.ttf"), //ttfファイルへのパス
		FR_PRIVATE,
		&design
	);

	// フォントの生成
	int fontSize = 32;

	LOGFONT lf = {
		fontSize,0,0,0,0,0,0,0,
		SHIFTJIS_CHARSET,
		OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY,
		FIXED_PITCH | FF_MODERN,
		TEXT("あんずもじ湛")
	};
	HFONT hFont = CreateFontIndirect(&lf);


	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
	UINT code = static_cast<UINT>('い');     // テクスチャに書き込む文字

	// フォントビットマップ取得
	TEXTMETRIC TM;
	GetTextMetrics(hdc, &TM);
	GLYPHMETRICS GM;
	CONST MAT2 Mat = { {0,1}, {0,0}, {0,0}, {0,1} };
	DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
	BYTE* ptr = new BYTE[size];
	GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

	// デバイスコンテキストとフォントハンドルの開放
	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, hdc);


	//--------------------------------
	// 書き込み可能テクスチャ作成
	//--------------------------------

	// CPUで書き込みができるテクスチャを作成

	// テクスチャ作成
	memset(&texture2dDesc, 0, sizeof(texture2dDesc));
	texture2dDesc.Width = GM.gmCellIncX;
	texture2dDesc.Height = TM.tmHeight;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	// RGBA(255,255,255,255)タイプ
	texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2dDesc.SampleDesc.Count = 1;
	// 動的（書き込みするための必須条件）
	texture2dDesc.Usage = D3D11_USAGE_DYNAMIC;
	// シェーダリソースとして使う	
	texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	// CPUからアクセスして書き込みOK
	texture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	hr = device->CreateTexture2D(&texture2dDesc, 0, texture2d.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create texture2d");
	}

	// フォント情報をテクスチャに書き込む部分
	D3D11_MAPPED_SUBRESOURCE hMappedResource{};
	if (texture2d.Get() != nullptr)
	{
		hr = deviceContext->Map(
			texture2d.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&hMappedResource);
		// ここで書き込む
		BYTE* pBits = (BYTE*)hMappedResource.pData;

		// フォント情報の書き込み
		// iOfs_x, iOfs_y : 書き出し位置(左上)
		// iBmp_w, iBmp_h : フォントビットマップの幅高
		// Level : α値の段階 (GGO_GRAY4_BITMAPなので17段階)
		int iOfs_x = GM.gmptGlyphOrigin.x;
		int iOfs_y = TM.tmAscent - GM.gmptGlyphOrigin.y;
		int iBmp_w = GM.gmBlackBoxX + (4 - (GM.gmBlackBoxX % 4)) % 4;
		int iBmp_h = GM.gmBlackBoxY;
		int Level = 17;
		int x, y;
		DWORD Alpha, Color;
		memset(pBits, 0, hMappedResource.RowPitch * TM.tmHeight);
		for (y = iOfs_y; y < iOfs_y + iBmp_h; y++)
		{
			for (x = iOfs_x; x < iOfs_x + iBmp_w; x++)
			{
				Alpha = (255 * ptr[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);
				Color = 0x00ffffff | (Alpha << 24);

				memcpy((BYTE*)pBits + hMappedResource.RowPitch * y + 4 * x, &Color, sizeof(DWORD));
			}
		}
		deviceContext->Unmap(texture2d.Get(), 0);
	}


	// シェーダーリソースビューの生成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = texture2dDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = texture2dDesc.MipLevels;

	hr = device->CreateShaderResourceView(texture2d.Get(), &srvDesc, shaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create shaderResourceView");
	}
}

Font::~Font()
{
	// リソース削除
	DESIGNVECTOR design;
	RemoveFontResourceEx(
		TEXT("./Data/Fonts/APJapanesefontF.ttf"), //ttfファイルへのパス
		FR_PRIVATE,
		&design
	);
}

void Font::render(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle)
{
	render(device, deviceContext, dx, dy, dw, dh, r, g, b, a, angle, 0, 0, texture2dDesc.Width, texture2dDesc.Height);
}

void Font::render(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)
{
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
