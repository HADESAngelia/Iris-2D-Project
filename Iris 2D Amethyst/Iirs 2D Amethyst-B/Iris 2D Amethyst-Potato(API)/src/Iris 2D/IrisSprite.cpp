#include "IrisSprite.h"
#include <cmath>
using namespace Gdiplus;

const double PI = 3.14159;

void IrisSprite::SetInitData(IIrisViewport *viewport){
	if (viewport != NULL)
		this->SetViewport(viewport);
}

IrisSprite::IrisSprite(IIrisViewport *viewport){
	this->Device = this->Device = ModuleIrisGraphics::Device;

	this->angle = 0.0f;
	this->blendType = 0;
	this->bushDepth = 0;
	this->bushOpacity = 255;
	this->mirror = false;
	this->opacity = 255;
	this->ox = 0.0f;
	this->oy = 0.0f;
	this->visible = true;
	this->waveAmp = 0.0f;
	this->waveLength = 0.0f;
	this->wavePhase = 0.0f;
	this->waveSpeed = 0.0f;
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->zoomX = 1.0f;
	this->zoomY = 1.0f;

	if (viewport != NULL)
		this->viewport = viewport;
	else
		this->viewport = ModuleIrisGraphics::getGViewport();
	((IrisViewport*)this->viewport)->addSprite(this);
	((IrisViewport*)this->viewport)->sortSprites();

	this->srcRect = new IrisRect(0, 0, 0, 0);
	this->color = NULL;
	this->bitmap = NULL;
	this->tone = new IrisTone(0, 0, 0, 0);

	this->isFlash = false;
	this->flashColor = NULL;
	this->flashDuration = 0;
	this->realFlashDuration = 0;
}

void IrisSprite::SetDevice(IDirect3DDevice9 *Device){
	this->Device = Device;
}

void IrisSprite::Dispose(){
	this->bitmap->Dispose();
	delete this->tone;
	this->tone = NULL;
	delete this->srcRect;
	this->srcRect = NULL;
	if (this->color != NULL){
		delete this->color;
		this->color = NULL;
	}
	delete this->bitmap;
	this->bitmap = NULL;
	((IrisViewport*)this->viewport)->deleteSprite(this);
	this->viewport = NULL;
	this->Device = NULL;
}

bool IrisSprite::Disposed(){
	if(this->Device == NULL)
		return true;
	return false;
}

void IrisSprite::AutoDispose(){
	this->bitmap->Dispose();
	delete this->bitmap;
	this->bitmap = NULL;
	this->viewport = NULL;
	this->Device = NULL;
}

void IrisSprite::Flash(const IIrisColor *color, int duration){
	this->isFlash = true;
	if(this->flashColor != NULL)
		delete this->flashColor;
	this->flashColor = new IrisColor(((IrisColor*)color)->red, ((IrisColor*)color)->green, ((IrisColor*)color)->blue, ((IrisColor*)color)->alpha);
	this->flashDuration = duration;
	if (this->flashColor != NULL){
		this->flashDuration = this->flashDuration * ((IrisColor*)this->flashColor)->alpha / 255;
		this->realFlashDuration = this->flashDuration;// alpha������˸ʱ��
		((IrisBitmap*)this->bitmap)->PrevFlash(color);
	}
	else{
		this->realFlashDuration = this->flashDuration;
	}
}

void IrisSprite::Update(){
	// ���ش� this->flashColor ����/�ݼ� �� ��ǰ������
	// �Ƚ�ԭͼÿ�����ر�ΪflashColor
	// ÿһ֡�仯������ flashColor -= (flashColor - ԭcolor) / ֡��
	// ����仯������� flashColor != ԭcolor������Ϊԭcolor
	// ���
	int durationTime = this->flashDuration;

	if (this->isFlash){
		if (this->flashDuration == 0){
			if (this->flashColor != NULL){
				delete this->flashColor;
				((IrisBitmap*)this->bitmap)->EndFlash();
			}
			else
				this->visible = true;
			this->isFlash = false;
		}
		else {
			if (this->flashColor == NULL){
				this->visible = false;
			}
			else{
				((IrisBitmap*)this->bitmap)->DoFlash(this->flashColor, this->realFlashDuration);
			}
			this->flashDuration--;
		}
	}
}

void IrisSprite::Draw(){
	if(this->bitmap == NULL)
		return;

	if(this->visible == false)
		return;

	((IrisBitmap*)this->bitmap)->Draw(this->opacity, this->viewport, this->srcRect, this->tone);

	Device->SetTexture(0, ((IrisBitmap*)this->bitmap)->GetTexture());

	//-------------Matrix Math-------------

	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetFVF(Iris2DVertex::FVF);

	D3DXMATRIX translationMatrix;
	D3DXMATRIX rotationMatrix;
	D3DXMATRIX scalingMatrix;
	D3DXMATRIX resultMatrix;

	D3DXMatrixIdentity(&translationMatrix);
	D3DXMatrixIdentity(&rotationMatrix);
	D3DXMatrixIdentity(&scalingMatrix);

	if(this->angle != 0.0f)
		D3DXMatrixRotationZ(&rotationMatrix, this->angle);

	D3DXMatrixTranslation(&translationMatrix, this->x - this->ox, this->y - this->oy, 0.0f);

	if(this->zoomX != 1.0f || this->zoomY != 1.0f)
		D3DXMatrixScaling(&scalingMatrix, this->zoomX, this->zoomY, 1.0f);

	resultMatrix = rotationMatrix * translationMatrix * scalingMatrix;
	Device->SetTransform(D3DTS_WORLD, &resultMatrix);

	D3DMATERIAL9 mtrl;// = d3d::WHITE_MTRL;

	int x = ModuleIrisGraphics::getBrightness();

	mtrl.Ambient = D3DXCOLOR(D3DCOLOR_XRGB(x, x, x));
	mtrl.Diffuse = D3DXCOLOR(D3DCOLOR_XRGB(x, x, x));
	mtrl.Specular = D3DXCOLOR(D3DCOLOR_XRGB(x, x, x));
	mtrl.Emissive = d3d::BLACK;
	mtrl.Power = 5.0f;

	Device->SetMaterial(&mtrl);

	Device->SetStreamSource(0, ((IrisBitmap*)this->bitmap)->GetVb(), 0, sizeof(Iris2DVertex));
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

}

void IrisSprite::SetViewport(IIrisViewport *tviewport){
	((IrisViewport*)this->viewport)->deleteSprite(this);
	this->viewport = tviewport;
	((IrisViewport*)this->viewport)->addSprite(this);
}

void IrisSprite::AutoSetViewport(IIrisViewport *tviewport){
	this->viewport = tviewport;
	((IrisViewport*)this->viewport)->addSprite(this);
}

IIrisViewport* IrisSprite::GetViewport(){
	return this->viewport;
}

void IrisSprite::SetOpacity(int tOpacity){
	this->opacity = tOpacity;
	((IrisBitmap*)this->bitmap)->SetOpacity(this->opacity);
}

int IrisSprite::GetOpacity(){
	return this->opacity;
}

void IrisSprite::SetZ(float tZ){
	this->z = tZ;
	((IrisViewport*)this->viewport)->sortSprites();
}
float IrisSprite::GetZ(){
	return this->z;
}

IIrisColor* IrisSprite::GetColor(){
	return this->color;
}

void IrisSprite::SetColor(IIrisColor* color){
	if (this->color != color){
		if (this->color != NULL)
			delete this->color;
		this->color = color;
	}
}

IIrisTone* IrisSprite::GetTone(){
	return this->tone;
}

void IrisSprite::SetTone(IIrisTone* tone){
	if (this->tone != tone){
		delete this->tone;
		this->tone = tone;
	}
}

IIrisRect* IrisSprite::GetSrcRect(){
	return this->srcRect;
}

void IrisSprite::SetSrcRect(IIrisRect* srcRect){
	if (this->srcRect != srcRect){
		delete this->srcRect;
		this->srcRect = srcRect;
	}
}

IIrisBitmap* IrisSprite::GetBitmap(){
	return this->bitmap;
}

void IrisSprite::SetBitmap(IIrisBitmap* bitmap){
	if (this->bitmap != bitmap){
		delete this->bitmap;
		this->bitmap = bitmap;
	}
	this->SetSrcRect(new IrisRect(0.0f, 0.0f, (float)((IrisBitmap*)bitmap)->width, (float)((IrisBitmap*)bitmap)->height));
	((IrisBitmap*)this->bitmap)->Draw(this->opacity, this->viewport, this->srcRect, this->tone);
}

int IrisSprite::GetWidth(){
	return (int)((IrisRect*)this->srcRect)->width;
}

int IrisSprite::GetHeight(){
	return (int)((IrisRect*)this->srcRect)->height;
}

IrisSprite::~IrisSprite(void)
{
	if(!this->Disposed())
		this->Dispose();
	//if(!this->bitmap->Disposed())
	//	this->bitmap->Dispose();
}
