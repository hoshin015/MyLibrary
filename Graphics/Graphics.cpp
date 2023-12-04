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
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// �w�i�F
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);	// renderTarget�̃N���A
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	// depthStencilView�̃N���A

	///////// View���N���A���Ă���Present�܂łɂ��ׂĂ̕`����s�� /////////
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->deviceContext->RSSetState(this->rasterizerState.Get());					// rasterizerState�̐ݒ�
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);	// depthStencilState�̐ݒ�
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

	// �V�F�[�_�[���\�[�X�̔z��̐ݒ�
	this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());

	sprites[0]->render(this->deviceContext.Get(), 10, 10, 200, 200, 1, 1, 1, 1, 0);

	// �����`��
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), L"HELLO WORLD", DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	this->swapchain->Present(1, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd, int width, int height)
{
	///////// AMD �� NVIDIA �̃O���t�B�b�N�{�[�h������ꍇ�͂�����g�p���� /////////

	// adapter�̎擾�ipc���̂��ׂẴO���t�B�b�N�{�[�h�̎擾�j
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
	if (adapters.size() < 1)	// adapter���擾�ł��Ȃ������ꍇ�̏���
	{
		ErrorLogger::Log("No DXGI Adapters found.");
		return false;
	}

	AdapterData adapter = AdapterReader::GetAdapters()[0];	// �擪�̃O���t�B�b�N�{�[�h(AMD NVIDIA ���Ȃ��ꍇ�͂�����g�p����)

	for (AdapterData& adp : adapters)
	{
		if (adp.description.VendorId == 0x1002/*AMD*/ || adp.description.VendorId == 0x10DE/*NVIDIA*/)
		{
			adapter = adp;
			break;
		}
	}

	///////// swapchain, device, deviceContext �̍쐬 /////////

	// swapchain�̏��
	DXGI_SWAP_CHAIN_DESC scd;	// �쐬�Ɏg�p����swapchain�̐���
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc.Width = width;		// �𑜓x�̕�
	scd.BufferDesc.Height = height;		// �𑜓x�̍���
	scd.BufferDesc.RefreshRate.Numerator = 60;	// ���t���b�V�����[�g�̕��q(�Ⴄ����)
	scd.BufferDesc.RefreshRate.Denominator = 1;	// ���t���b�V�����[�g�̕���(�Ⴄ����)
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	
	scd.BufferCount = 1;		
	scd.OutputWindow = hwnd;	// �o�̓E�B���h�E�̃n���h��
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	// Present���Ăяo������Ƀo�b�N�o�b�t�@�[��j������ݒ�
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// �E�B���h�E���[�h����S��ʃ��[�h�ɐ؂�ւ������ɕ\�����[�h(�܂��͉𑜓x)��ύX����
	
	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		adapter.pAdapter,					// �g�p����IDXGI Adapter
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		NULL,
		NULL,
		0,
		D3D11_SDK_VERSION,					// SDK�̃o�[�W�����B���̒l�Œ�
		&scd,								// swapchain�̏��
		this->swapchain.GetAddressOf(),		// IDXGISwapChain�̃A�h���X
		this->device.GetAddressOf(),		// ID3D11Device�̃A�h���X
		NULL,
		this->deviceContext.GetAddressOf()	// ID3D11DeviceContext�̃A�h���X
	);

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create device and swapchain.");
		return false;
	}

	///////// renderTargetView �̍쐬 /////////

	// renderTarget�̃o�b�t�@�[���擾
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

	///////// DepthStencilView�̍쐬 /////////

	// DepthStencilBuffer�̍쐬(�o�b�t�@�Ƃ��Ďg�p���邪�e�N�X�`���̐ݒ���s��)
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// depth��24bit  �E stencil��8bit
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
	
	// depthStencilView�̍쐬
	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil view.");
		return false;
	}

	// renderTarget�̐ݒ�(Optput - Merger�̐ݒ�)
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	///////// depthStencilState�̍쐬 /////////
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = true;	// �[�x�e�X�g���s�����ǂ���
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;	// �[�x�X�e���V���o�b�t�@�ɏ������݂��s�����ǂ���
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;	// �����̐[�x�f�[�^�Ƃ̔�r���@

	hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil state.");
		return false;
	}

	///////// viewport�̍쐬 /////////
	// renderTargetView�̂ǂ͈̔͂��g�p���邩�����߂�
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// viewport�̐ݒ�(Rasterizer�̐ݒ�)
	this->deviceContext->RSSetViewports(1, &viewport);


	///////// rasterizer�̍쐬 /////////
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

	 
	///////// font�֘A������ /////////
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data/Fonts/comic_sans_ms_16.spritfont");


	///////// SamplerState�̍쐬 /////////
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	// U��0�`1�͈̔͊O�������ꍇ�̏���(�J��Ԃ��w��)
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;	// V��0�`1�͈̔͊O�������ꍇ�̏���(�J��Ԃ��w��)
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;	// W��0�`1�͈̔͊O�������ꍇ�̏���(�J��Ԃ��w��)
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
	

	return true;
}

bool Graphics::InitializeScene()
{
	

	return true;
}
