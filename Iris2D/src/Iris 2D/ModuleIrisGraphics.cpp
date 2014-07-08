#include "ModuleIrisGraphics.h"

typedef union
{
	ARGB Color;
	struct
	{
		BYTE Blue;
		BYTE Green;
		BYTE Red;
		BYTE Alpha;
	};
}ARGBQuad, *PARGBQuad;

int ModuleIrisGraphics::width;
int ModuleIrisGraphics::height;
IDirect3DDevice9 *ModuleIrisGraphics::Device;
IDirect3DTexture9* ModuleIrisGraphics::ExchangeTexture = 0;
IDirect3DVertexBuffer9* ModuleIrisGraphics::ExchangeVertex = 0;

float ModuleIrisGraphics::frameRate = 0;
unsigned long ModuleIrisGraphics::frameCount = 0;
float ModuleIrisGraphics::brightness = 255;

list<IrisViewport*> ModuleIrisGraphics::viewports;
IrisViewport* ModuleIrisGraphics::gViewport;

HWND ModuleIrisGraphics::hwnd = 0;

bool ModuleIrisGraphics::isFreeze = false;

int ModuleIrisGraphics::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure
	GetImageEncoders(num, size, pImageCodecInfo);
	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}
	free(pImageCodecInfo);
	return -1;  // Failure
}


bool ModuleIrisGraphics::compareViewprotWithZ(IrisViewport *v1, IrisViewport *v2){
	return v1->GetZ() < v2->GetZ();
}

void ModuleIrisGraphics::sortViewports(){
	ModuleIrisGraphics::viewports.sort(ModuleIrisGraphics::compareViewprotWithZ);
}

ModuleIrisGraphics::ModuleIrisGraphics(void)
{
}

void ModuleIrisGraphics::MakeExchangeTexture(){
	Device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT, &(ExchangeTexture), NULL);
}

void ModuleIrisGraphics::MakeExchangeVertex(){
	HRESULT r = Device->CreateVertexBuffer(6 * sizeof(Iris2DVertex), D3DUSAGE_WRITEONLY, Iris2DVertex::FVF, D3DPOOL_MANAGED, &(ExchangeVertex), 0);

	if (FAILED(r))
	{
		MessageBox(NULL, L"Create buffer failed!", L"Error", 0);
	}

	Iris2DVertex* v;
	ExchangeVertex->Lock(0, 0, (void**)&v, 0);
	v[0] = Iris2DVertex(0.0f, (float)height, 0.0f, 0.0f, 1.0f);
	v[1] = Iris2DVertex(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Iris2DVertex((float)width, 0.0f, 0.0f, 1.0f, 0.0f);

	v[3] = Iris2DVertex(0.0f, (float)height, 0.0f, 0.0f, 1.0f);
	v[4] = Iris2DVertex((float)width, 0.0f, 0.0f, 1.0f, 0.0f);
	v[5] = Iris2DVertex((float)width, (float)height, 0.0f, 1.0f, 1.0f);

	ExchangeVertex->Unlock();
}

void ModuleIrisGraphics::Init(){
	gViewport = new IrisViewport(0, 0, (float)ModuleIrisGraphics::width, (float)ModuleIrisGraphics::height);
	MakeExchangeTexture();
	MakeExchangeVertex();
}

void ModuleIrisGraphics::addViewport(IrisViewport *viewport){
	viewports.push_back(viewport);
}

void ModuleIrisGraphics::deleteViewport(IrisViewport *viewport){
	viewports.remove(viewport);
}

IrisViewport* ModuleIrisGraphics::getGViewport(){
	return gViewport;
}

void ModuleIrisGraphics::setDevice(IDirect3DDevice9 *tDevice){
	Device = tDevice;
}

void ModuleIrisGraphics::Update(){

	// Can Display
	//while (!IrisApp::Instance()->CanDisplay == true)
	//	//Win32 Message
	//	if (IrisApp::Instance()->MessageLoop() == WM_QUIT){
	//		IrisApp::Instance()->GoQuit();
	//		return;
	//	}

	//Win32 Message
	while (!IrisApp::Instance()->CanDisplay == true){
		if (!IrisApp::Instance()->IsQuited())
			IrisApp::Instance()->MessageLoop();
		else
			return;
	}
	
	IrisApp::Instance()->CanDisplay = false;

	//Alpha enabled
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	// ��Viewport��Sprite��������Viewport��Texure��
	list<IrisViewport*>::iterator it;
	for (it = viewports.begin(); it != viewports.end(); it++){
		((IrisViewport*)(*it))->RenderSurface();
	}

	// ��Viewport��Texture������̨����ҳ��
	D3DXMATRIX proj;
	D3DXMatrixOrthoOffCenterLH(&proj, 0.0f, (float)ModuleIrisGraphics::getWidth(), (float)ModuleIrisGraphics::getHeight(), 0.0f, 0, 9999.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	IDirect3DSurface9* eSurface;
	IDirect3DSurface9* oldSurface;
	ModuleIrisGraphics::ExchangeTexture->GetSurfaceLevel(0, &(eSurface));
	Device->GetRenderTarget(0, &oldSurface);
	Device->SetRenderTarget(0, eSurface);

	Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	Device->BeginScene();
	for (it = viewports.begin(); it != viewports.end(); it++){
		((IrisViewport*)(*it))->RenderToBackbuffer();
	}
	Device->EndScene();
	Device->SetRenderTarget(0, oldSurface);

	// ������ڶ��ᣬ��ô���Ѻ�̨����ҳ����ȥ
	if (isFreeze)
		return;

	// ���Ѻ�̨����ҳ������̨������
	Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	Device->BeginScene();

	Device->SetTexture(0, ModuleIrisGraphics::ExchangeTexture);
	Device->SetFVF(Iris2DVertex::FVF);
	Device->SetStreamSource(0, ModuleIrisGraphics::ExchangeVertex, 0, sizeof(Iris2DVertex));
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	Device->EndScene();
	//Alpha disabled
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	Device->Present(0, 0, 0, 0);
}

void ModuleIrisGraphics::disposeAllGraphicsSource(){
	ModuleIrisGraphics::deleteViewport(ModuleIrisGraphics::gViewport);

	list<IrisViewport*>::iterator it;
	for(it = viewports.begin(); it != viewports.end(); it++){
		(*it)->AutoDispose();
	}

	list<IrisSprite*> sprites = ModuleIrisGraphics::gViewport->GetSprites();
	list<IrisSprite*>::iterator its = sprites.begin();
	for(its = sprites.begin(); its != sprites.end(); its++){
		(*its)->AutoDispose();
	}
}

void ModuleIrisGraphics::Wait(int duration){
	for (int i = 0; i < duration; i++)
		Update();
}

void ModuleIrisGraphics::freeze(){
	isFreeze = true;
}

void ModuleIrisGraphics::fadeOut(int duration){
	if(brightness == 0.0f)
		return;
	for (int i = 0; i < duration; i++){
		brightness -= 255.0f / duration;
		if (brightness < 0.0f) brightness = 0.0f;
		Update();
	}
}

void ModuleIrisGraphics::fadeIn(int duration){
	if (brightness == 255.0f)
		return;
	for (int i = 0; i < duration; i++){
		brightness += 255.0f / duration;
		if (brightness > 255.0f) brightness = 255.0f;
		Update();
	}
}

void ModuleIrisGraphics::transition(int duration, LPSTR filename, int vague){
	isFreeze = false;
}

void ModuleIrisGraphics::snap2Bitmap(IrisBitmap *bitmap){
	IDirect3DSurface9* EXSurface;
	IDirect3DSurface9* ExchangeSurface;
	ExchangeTexture->GetSurfaceLevel(0, &EXSurface);	
	Device->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &ExchangeSurface, 0);
	Device->GetRenderTargetData(EXSurface, ExchangeSurface);

	D3DLOCKED_RECT rc;
	memset(&rc, 0, sizeof(rc));
	ExchangeSurface->LockRect(&rc, 0, D3DLOCK_NOOVERWRITE);
	PARGBQuad p2 = (PARGBQuad)rc.pBits;
	PARGBQuad c2 = 0;
	IrisColor color(0, 0, 0, 0);
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){
			c2 = p2 + x + y * rc.Pitch / sizeof(ARGBQuad);
			color.set(c2->Red, c2->Green, c2->Blue, c2->Alpha);
			bitmap->SetPixel(x, y, &color);
		}
	}

	ExchangeSurface->UnlockRect();
	EXSurface->Release();
	ExchangeSurface->Release();
}

int ModuleIrisGraphics::getBrightness(){
	return (int)ModuleIrisGraphics::brightness;
}

void ModuleIrisGraphics::setBrightness(int bs){
	float tbrightness = (float)(bs > 255 ? 255 : bs < 0 ? 0 : bs);
	ModuleIrisGraphics::brightness = tbrightness;
}

void ModuleIrisGraphics::frameReset(){
	ModuleIrisGraphics::frameCount = 0;
}

void ModuleIrisGraphics::resizeScreen(int width, int height){
}

int ModuleIrisGraphics::getWidth(){
	return width;
}

int ModuleIrisGraphics::getHeight(){
	return height;
}

ModuleIrisGraphics::~ModuleIrisGraphics(void)
{
}
