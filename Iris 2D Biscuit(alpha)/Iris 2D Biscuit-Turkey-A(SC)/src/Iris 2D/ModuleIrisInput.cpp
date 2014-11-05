#include "ModuleIrisInput.h"

IDirectInputDevice8 * ModuleIrisInput::KeyBoardInputDevice = 0;
IDirectInputDevice8 * ModuleIrisInput::MouseInputDevice = 0;

//DIDEVICEOBJECTDATA ModuleIrisInput::kbdidod[DINPUT_BUFFERSIZE];
//DIDEVICEOBJECTDATA ModuleIrisInput::msdidod[DINPUT_BUFFERSIZE];

char ModuleIrisInput::KeyStateBuffer[256] = { 0 };
char ModuleIrisInput::LastKeyStateBuffer[256] = { 0 };

DIMOUSESTATE ModuleIrisInput::MouseStateBuffer = { 0 };
DIMOUSESTATE ModuleIrisInput::LastMouseStateBuffer = { 0 };
KeyState ModuleIrisInput::TimeSecStateBufferLeftKey = { 0 };
KeyState ModuleIrisInput::TimeSecStateBufferRightKey = { 0 };
KeyState ModuleIrisInput::TimeSecStateBufferKeyBoard[256] = { 0 };

ModuleIrisInput::ModuleIrisInput(void)
{
}

void ModuleIrisInput::SecKeyStateUpdate(){
	if (TimeSecStateBufferLeftKey.state != MouseStateBuffer.rgbButtons[0]){
		if (TimeSecStateBufferLeftKey.state == 0){
			TimeSecStateBufferLeftKey.state = MouseStateBuffer.rgbButtons[0];
			TimeSecStateBufferLeftKey.trigger = FALSE;
			TimeSecStateBufferLeftKey.secTime = ModuleIrisGraphics::frameCount;
		}
		else{
			if (MouseStateBuffer.rgbButtons[0] & 0x80){
				TimeSecStateBufferLeftKey.state = MouseStateBuffer.rgbButtons[0];
				TimeSecStateBufferLeftKey.trigger = FALSE;
				TimeSecStateBufferLeftKey.secTime = ModuleIrisGraphics::frameCount;
			}
			else{
				TimeSecStateBufferLeftKey.state = MouseStateBuffer.rgbButtons[0];
				TimeSecStateBufferLeftKey.trigger = TRUE;
				TimeSecStateBufferLeftKey.secTime = ModuleIrisGraphics::frameCount;
			}
		}
	}

	if (TimeSecStateBufferRightKey.state != MouseStateBuffer.rgbButtons[1]){
		if (TimeSecStateBufferRightKey.state == 0){
			TimeSecStateBufferRightKey.state = MouseStateBuffer.rgbButtons[1];
			TimeSecStateBufferRightKey.trigger = FALSE;
			TimeSecStateBufferRightKey.secTime = ModuleIrisGraphics::frameCount;
		}
		else{
			if (MouseStateBuffer.rgbButtons[1] & 0x80){
				TimeSecStateBufferRightKey.state = MouseStateBuffer.rgbButtons[1];
				TimeSecStateBufferRightKey.trigger = FALSE;
				TimeSecStateBufferRightKey.secTime = ModuleIrisGraphics::frameCount;
			}
			else{
				TimeSecStateBufferRightKey.state = MouseStateBuffer.rgbButtons[1];
				TimeSecStateBufferRightKey.trigger = TRUE;
				TimeSecStateBufferRightKey.secTime = ModuleIrisGraphics::frameCount;
			}
		}
	}

	for (int i = 0; i < 256; i++){
		if (TimeSecStateBufferKeyBoard[i].state != KeyStateBuffer[i]){
			if (TimeSecStateBufferKeyBoard[i].state == 0){
				TimeSecStateBufferKeyBoard[i].state = KeyStateBuffer[i];
				TimeSecStateBufferKeyBoard[i].trigger = FALSE;
				TimeSecStateBufferKeyBoard[i].secTime = ModuleIrisGraphics::frameCount;
			}
			else{
				if (KeyStateBuffer[i] & 0x80){
					TimeSecStateBufferKeyBoard[i].state = KeyStateBuffer[i];
					TimeSecStateBufferKeyBoard[i].trigger = FALSE;
					TimeSecStateBufferKeyBoard[i].secTime = ModuleIrisGraphics::frameCount;
				}
				else{
					TimeSecStateBufferKeyBoard[i].state = KeyStateBuffer[i];
					TimeSecStateBufferKeyBoard[i].trigger = TRUE;
					TimeSecStateBufferKeyBoard[i].secTime = ModuleIrisGraphics::frameCount;
				}
			}
		}
	}

}

void ModuleIrisInput::Update(){

	KeyBoardInputDevice->Acquire();
	// ��¼�ϴΰ���״̬
	memcpy(LastKeyStateBuffer, KeyStateBuffer, sizeof(KeyStateBuffer));
	memcpy(&LastMouseStateBuffer, &MouseStateBuffer, sizeof(MouseStateBuffer));

	SecKeyStateUpdate();

	::ZeroMemory(KeyStateBuffer, sizeof(KeyStateBuffer));
	DeviceRead(KeyBoardInputDevice, (LPVOID)KeyStateBuffer, sizeof(KeyStateBuffer));

	::ZeroMemory(&MouseStateBuffer, sizeof(MouseStateBuffer));
	DeviceRead(MouseInputDevice, (LPVOID)&MouseStateBuffer, sizeof(MouseStateBuffer));
}

bool ModuleIrisInput::DeviceRead(IDirectInputDevice8*pDIDevice, void* pBuffer, long lSize){
	HRESULT hr;
	
	while (true)
	{
		pDIDevice->Poll();              // ��ѯ�豸  
		pDIDevice->Acquire();          // ��ȡ�豸�Ŀ���Ȩ  
		if (SUCCEEDED(hr = pDIDevice->GetDeviceState(lSize, pBuffer))) break;
		if (hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED) return false;
		if (FAILED(pDIDevice->Acquire())) return false;
	}
	return true;
}

bool ModuleIrisInput::KeyBoardKeyPress(DWORD key){
	return (KeyStateBuffer[key] & 0x80) == 0 ? false : true;
}	

bool ModuleIrisInput::KeyBoardKeyTrigger(DWORD key){
	if (!(LastKeyStateBuffer[key] & 0x80)){
		if (KeyStateBuffer[key] & 0x80){
			return true;
		}
	}
	return false;
}

bool ModuleIrisInput::KeyBoardKeyRepeat(DWORD key){
	if (TimeSecStateBufferKeyBoard[key].trigger == TRUE
		&& !(TimeSecStateBufferKeyBoard[key].state & 0x80)
		&& KeyStateBuffer[key] & 0x80
		&& ModuleIrisGraphics::frameCount - TimeSecStateBufferKeyBoard[key].secTime < 10)
		return true;
	return false;
}

bool ModuleIrisInput::MousePress(MouseKey botton){
	switch (botton){
	case Left:
		if (MouseStateBuffer.rgbButtons[0] & 0x80)
			return true;
		break;
	case Right:
		if (MouseStateBuffer.rgbButtons[1] & 0x80)
			return true;
		break;
	}
	return false;
}

bool ModuleIrisInput::MouseTrigger(MouseKey botton){
	switch (botton){
	case Left:
		if (MouseStateBuffer.rgbButtons[0] & 0x80 && !(LastMouseStateBuffer.rgbButtons[0] & 0x80))
			return true;
		break;
	case Right:
		if (MouseStateBuffer.rgbButtons[1] & 0x80 && !(LastMouseStateBuffer.rgbButtons[1] & 0x80))
			return true;
		break;
	}
	return false;
}

bool ModuleIrisInput::MouseRepeat(MouseKey botton){
	switch (botton){
	case Left:
		if (TimeSecStateBufferLeftKey.trigger == TRUE
			&& !(TimeSecStateBufferLeftKey.state & 0x80)
			&& MouseStateBuffer.rgbButtons[0] & 0x80
			&& ModuleIrisGraphics::frameCount - TimeSecStateBufferLeftKey.secTime < 10)
			return true;
		break;
	case Right:
		if (TimeSecStateBufferRightKey.trigger == TRUE
			&& !(TimeSecStateBufferRightKey.state & 0x80)
			&& MouseStateBuffer.rgbButtons[0] & 0x80
			&& ModuleIrisGraphics::frameCount - TimeSecStateBufferRightKey.secTime < 10)
			return true;
		break;
	}

	return false;
}

long ModuleIrisInput::MouseMiddleRoll(){
	return MouseStateBuffer.lZ;
}

long ModuleIrisInput::MouseXMove(){
	return MouseStateBuffer.lX;
}
long ModuleIrisInput::MouseYMove(){
	return MouseStateBuffer.lY;
}

bool ModuleIrisInput::CursorInRect(const IrisRect* rect){
	POINT  pt;
	::GetCursorPos(&pt);
	::ScreenToClient(ModuleIrisGraphics::hwnd, &pt);

	if (pt.x >= rect->x && pt.x <= rect->x + rect->width && pt.y >= rect->y && pt.y <= rect->y + rect->height)
		return true;
	return false;
}

int ModuleIrisInput::GetCursorX(){
	POINT  pt;
	GetCursorPos(&pt);
	ScreenToClient(ModuleIrisGraphics::hwnd, &pt);
	return pt.x;
}

int ModuleIrisInput::GetCursorY(){
	POINT  pt;
	GetCursorPos(&pt);
	ScreenToClient(ModuleIrisGraphics::hwnd, &pt);
	return pt.y;
}

ModuleIrisInput::~ModuleIrisInput(void)
{
}
