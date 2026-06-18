#include "main.h" 
#include "VirtualizerSDK.h"


static void LogCreateCharacterPacket(const MSG_CreateCharacter* msg, int a_iSize)
{
	char mobName[NAME_LENGTH + 1] = { 0 };
	memcpy(mobName, msg->MobName, NAME_LENGTH);

	FILE* fp = nullptr;
	if (fopen_s(&fp, "createchar-send.log", "a") != 0 || fp == nullptr)
		return;

	SYSTEMTIME now;
	GetLocalTime(&now);
	fprintf(fp, "%04u-%02u-%02u %02u:%02u:%02u type=0x%04X size=%d slot=%d class=%d name=%s\n",
		now.wYear, now.wMonth, now.wDay,
		now.wHour, now.wMinute, now.wSecond,
		msg->Type, a_iSize, msg->Slot, msg->MobClass, mobName);
	fclose(fp);
}

void ClientSended(MSG_STANDARD* pBuffer, int a_iSize)
{  
	VIRTUALIZER_TIGER_WHITE_START
	if (pBuffer->Type == _MSG_CreateCharacter && a_iSize >= sizeof(MSG_CreateCharacter))
	{
		LogCreateCharacterPacket(reinterpret_cast<MSG_CreateCharacter*>(pBuffer), a_iSize);
	}

	if (pBuffer->Type == 0x20D)
	{
		UUID uuid;
		UuidCreateSequential(&uuid);
		int sContexto = g_pInterface->SceneContext();
		auto old = reinterpret_cast<MSG_AccountLogin*>(pBuffer);

		old->Version = APP_VERSION;

		memset(&old->MacAddres[0], 0, 52);

		sprintf_s(old->MacAddres, 18, "%02X:%02X:%02X:%02X:%02X:%02X", uuid.Data4[2], uuid.Data4[3], uuid.Data4[4], uuid.Data4[5], uuid.Data4[6], uuid.Data4[7]);

		int len = strlen(old->MacAddres); 

		if (len < 12)
			ExitProcess(1);  
		 //18/11
		old->Encode(); 
	}		
	VIRTUALIZER_TIGER_WHITE_END
}
