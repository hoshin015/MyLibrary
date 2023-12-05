#include "AdapterReader.h"

std::vector<AdapterData> AdapterReader::adapters;

std::vector<AdapterData> AdapterReader::GetAdapters()
{
	// adapter�����ɏ���������Ă���ꍇ�͂����Ԃ�
	if (adapters.size() > 0)
		return adapters;


	//--------- �܂�����������Ă��Ȃ��ꍇ��adapter��񋓂��� ---------//


	// DXGI�t�@�N�g���̍쐬
	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to craete DXGIFactory for enumerating adapters.");
		exit(-1);
	}

	// adapter�̎擾
	IDXGIAdapter* pAdapter;
	UINT index = 0;

	// EnumAdapters��index��adapter���擾�ł���
	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
	{
		// �����ł�adapter�{�̂ł͂Ȃ��R���X�g���N�^�Ŏ擾����������push���Ă���
		adapters.push_back(AdapterData(pAdapter));
		index += 1;
	}
	return adapters;
}

AdapterData::AdapterData(IDXGIAdapter* pAdapter)
{
	this->pAdapter = pAdapter;
	HRESULT hr = pAdapter->GetDesc(&this->description);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to Get Description for IDXGIAdapter.");
	}
}