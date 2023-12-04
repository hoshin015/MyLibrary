#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	if (!InitializeDirectX(hwnd, width, height))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;


	return true;
}

void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// 背景色
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);	// renderTargetのクリア
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	// depthStencilViewのクリア

	///////// ViewをクリアしてからPresentまでにすべての描画を行う /////////
	
	// inputLayoutの設定
	//this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->deviceContext->RSSetState(this->rasterizerState.Get());					// rasterizerStateの設定
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);	// depthStencilStateの設定
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

	//// 頂点シェーダーの設定
	//this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	//// ピクセルシェーダーの設定
	//this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);

	// シェーダーリソースの配列の設定
	this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());
	
	// 頂点バッファの配列を入力アセンブラーステージにバインドする
	//UINT offset = 0;

	//// constantバッファの更新
	//static float yOff = 0.5f;
	//yOff -= 0.01f;
	//constantBuffer.data.xOffset = 0.0f;
	//constantBuffer.data.yOffset = sinf(yOff);
	//if (!constantBuffer.ApplyChanges())
	//	return;
	//this->deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());	// constantBufferのバインド

	// 四角形
	//this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	//this->deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//this->deviceContext->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);

	sprites[0]->render(this->deviceContext.Get(), 10, 10, 200, 200, 1, 1, 1, 1, 0);

	// 文字描画
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), L"HELLO WORLD", DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	this->swapchain->Present(1, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd, int width, int height)
{
	///////// AMD か NVIDIA のグラフィックボードがある場合はそれを使用する /////////

	// adapterの取得（pc内のすべてのグラフィックボードの取得）
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
	if (adapters.size() < 1)	// adapterが取得できなかった場合の処理
	{
		ErrorLogger::Log("No DXGI Adapters found.");
		return false;
	}

	AdapterData adapter = AdapterReader::GetAdapters()[0];	// 先頭のグラフィックボード(AMD NVIDIA がない場合はこれを使用する)

	for (AdapterData& adp : adapters)
	{
		if (adp.description.VendorId == 0x1002/*AMD*/ || adp.description.VendorId == 0x10DE/*NVIDIA*/)
		{
			adapter = adp;
			break;
		}
	}

	///////// swapchain, device, deviceContext の作成 /////////

	// swapchainの情報
	DXGI_SWAP_CHAIN_DESC scd;	// 作成に使用するswapchainの説明
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc.Width = width;		// 解像度の幅
	scd.BufferDesc.Height = height;		// 解像度の高さ
	scd.BufferDesc.RefreshRate.Numerator = 60;	// リフレッシュレートの分子(違うかも)
	scd.BufferDesc.RefreshRate.Denominator = 1;	// リフレッシュレートの分母(違うかも)
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	
	scd.BufferCount = 1;		
	scd.OutputWindow = hwnd;	// 出力ウィンドウのハンドル
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	// Presentを呼び出した後にバックバッファーを破棄する設定
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// ウィンドウモードから全画面モードに切り替えた時に表示モード(または解像度)を変更する
	
	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		adapter.pAdapter,					// 使用するIDXGI Adapter
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		NULL,
		NULL,
		0,
		D3D11_SDK_VERSION,					// SDKのバージョン。この値固定
		&scd,								// swapchainの情報
		this->swapchain.GetAddressOf(),		// IDXGISwapChainのアドレス
		this->device.GetAddressOf(),		// ID3D11Deviceのアドレス
		NULL,
		this->deviceContext.GetAddressOf()	// ID3D11DeviceContextのアドレス
	);

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create device and swapchain.");
		return false;
	}

	///////// renderTargetView の作成 /////////

	// renderTargetのバッファーを取得
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "GetBuffer Failed.");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create render target view.");
		return false;
	}

	///////// DepthStencilViewの作成 /////////

	// DepthStencilBufferの作成(バッファとして使用するがテクスチャの設定を行う)
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// depthに24bit  ・ stencilに8bit
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil buffer.");
		return false;
	}
	
	// depthStencilViewの作成
	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil view.");
		return false;
	}

	// renderTargetの設定(Optput - Mergerの設定)
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	///////// depthStencilStateの作成 /////////
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = true;	// 深度テストを行うかどうか
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;	// 深度ステンシルバッファに書きこみを行うかどうか
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;	// 既存の深度データとの比較方法

	hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil state.");
		return false;
	}

	///////// viewportの作成 /////////
	// renderTargetViewのどの範囲を使用するかを決める
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// viewportの設定(Rasterizerの設定)
	this->deviceContext->RSSetViewports(1, &viewport);


	///////// rasterizerの作成 /////////
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	//rasterizerDesc.FrontCounterClockwise = TRUE;
	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}

	 
	///////// font関連初期化 /////////
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data/Fonts/comic_sans_ms_16.spritfont");


	///////// SamplerStateの作成 /////////
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	// Uが0～1の範囲外だった場合の処理(繰り返し指定)
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;	// Vが0～1の範囲外だった場合の処理(繰り返し指定)
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;	// Wが0～1の範囲外だった場合の処理(繰り返し指定)
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create sampler state.");
		return false;
	}

	sprites[0] = new Sprite(this->device.Get(), L"Data/Texture/piano.png");

	return true;
}

bool Graphics::InitializeShaders()
{
	// modeによって読み込むshaderのパスを変更する
//	std::wstring shaderfolder;
//#pragma region DetermineShaderPath
//	if (IsDebuggerPresent() == TRUE)
//	{
//#ifdef _DEBUG // Degbu Mode
//
//	#ifdef _WIN64 // x64
//		shaderfolder = L"./x64/Debug/";
//	#else // x86(Win32)
//		shaderfolder = L"./Debug/";
//	#endif
//
//#else // Release Mode
//
//	#ifdef _WIN64 // x64
//			shaderfolder = L"./x64/Release/";
//	#else // x86(Win32)
//			shaderfolder = L"./Release/";
//#endif
//#endif
//	}
//#pragma endregion
//
//	// InputLayoutの情報作成
//	D3D11_INPUT_ELEMENT_DESC layout[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//
//	UINT numElements = ARRAYSIZE(layout);	
//
//	// verteshaderオブジェクトとinputLayoutオブジェクトの作成
//	if (!vertexshader.Initialize(this->device, shaderfolder +  L"vertexshader.cso", layout, numElements))
//		return false;
//
//	// pixelshaderオブジェクトの作成
//	if (!pixelshader.Initialize(this->device, shaderfolder + L"pixelshader.cso"))
//		return false;

	return true;
}

bool Graphics::InitializeScene()
{
	//-------- < 頂点バッファの作成　> --------//

	// 頂点配列
	//Vertex v[] =
	//{
	//	Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f),		// Bottom Left	- [0]
	//	Vertex(-0.5f,  0.5f, 0.0f, 0.0f, 0.0f),		// Top Left		- [1]
	//	Vertex( 0.5f,  0.5f, 1.0f, 1.0f, 0.0f),		// Top Right	- [2]
	//	Vertex( 0.5f, -0.5f, 1.0f, 1.0f, 1.0f),		// Bottom Right	- [3]
	//};

	//// バッファの作成
	//HRESULT hr = this->vertexBuffer.Initialize(this->device.Get(), v, ARRAYSIZE(v));
	//if (FAILED(hr))
	//{
	//	ErrorLogger::Log(hr, "Failed to create vertex buffer.");
	//	return false;
	//}

	/////////// テクスチャ読み込み /////////

	//// ID3D11Resource と　ID3D11ShaderResourceView を生成する
	//// ID3D11Resourceのポインタにはnullptrを渡しているため作られない
	//hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Texture/piano.png", nullptr, myTexture.GetAddressOf());
	//if (FAILED(hr))
	//{
	//	ErrorLogger::Log(hr, "Failed to create wic texture from file.");
	//	return false;
	//}

	//-------- < constantバッファの作成　> --------//
	/*hr = this->constantBuffer.Initialize(device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to initialize constant buffer.");
		return false;
	}*/

	return true;
}
