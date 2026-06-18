#pragma once

#include "ProcessClientMessage.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <ctime>
#include <string.h>
#include "SendFunc.h"
#include "Functions.h"
#include "wMySQL.h"

static void CopyDropListName(char* dest, int destSize, const char* src, bool spacesToUnderscore)
{
	if (destSize <= 0)
		return;

	dest[0] = 0;

	if (!src)
		return;

	int out = 0;
	for (int i = 0; src[i] != 0 && out < destSize - 1; i++)
	{
		char ch = src[i];

		if (ch == '\'' || ch == '"' || ch == '\\' || ch == ';')
			continue;

		if (spacesToUnderscore && ch == ' ')
			ch = '_';

		dest[out++] = ch;
	}

	dest[out] = 0;
}

static bool IsLichCruntName(const char* mobname)
{
	char normalized[32];
	CopyDropListName(normalized, sizeof(normalized), mobname, true);

	return _stricmp(normalized, "Lich_Crunt") == 0;
}

static bool FillLichCruntDropList(MSG_UpdateDropList* sm, const char* mobname)
{
	if (!IsLichCruntName(mobname))
		return false;

	const int items[] = { 2400, 2405, 2441, 2442, 2443, 2444, 412, 413, 1741, 671, 670 };

	for (int i = 0; i < sizeof(items) / sizeof(items[0]); i++)
		sm->Item[i] = items[i];

	return true;
}

static bool DropListHasMobName(const MSG_SendDListNames* packet, const char* mobname)
{
	for (int i = 0; i < 30; i++)
	{
		if (packet->MobName[i][0] != 0 && _stricmp(packet->MobName[i], mobname) == 0)
			return true;
	}

	return false;
}

static void ForceMobName(MSG_SendDListNames* packet, int* mobCount, const char* mobname)
{
	if (DropListHasMobName(packet, mobname))
		return;

	int index = *mobCount;
	if (index >= 30)
		index = 29;

	strncpy(packet->MobName[index], mobname, sizeof(packet->MobName[index]) - 1);
	packet->MobName[index][sizeof(packet->MobName[index]) - 1] = 0;

	if (*mobCount < 30)
		(*mobCount)++;
}

void UpdateDropList(int conn, const char* mobname, int region)
{
	MSG_UpdateDropList sm;
	memset(&sm, 0, sizeof(MSG_UpdateDropList));
	sm.Type = _MSG_UpdateDropList;
	sm.Size = sizeof(MSG_UpdateDropList);
	sm.ID = conn;

	auto& pc = cSQL::instance();

	char safeName[32];
	char safeNameAlt[32];
	CopyDropListName(safeName, sizeof(safeName), mobname, false);
	CopyDropListName(safeNameAlt, sizeof(safeNameAlt), mobname, true);

	sprintf(xQuery,
		"SELECT * FROM `droplist` WHERE `nome` = '%s' OR `nome` = '%s' OR REPLACE(`nome`, '_', ' ') = '%s' LIMIT 1",
		safeName,
		safeNameAlt,
		safeName);

	MYSQL_ROW row;
	MYSQL* wSQL = pc.wStart();
	MYSQL_RES* result = pc.wRes(wSQL, xQuery);

	if (result == NULL && !FillLichCruntDropList(&sm, mobname))
		return;

	int Slot = 0;
	int Index = 0;
	bool found = false;

	while (result != NULL && (row = mysql_fetch_row(result)) != NULL)
	{
		found = true;

		for (int k = 0; k < MAX_CARRY; k++) {

			Index = atoi(row[k + 3]);

			if (Index == 0)
				continue;

			if (Index > MAX_ITEMLIST)
				continue;

			if (Index < 400)
				continue;

			if (Slot >= 64)
				break;

			sm.Item[Slot] = Index;
			Slot++;
		}
	}	

	if (!found)
		FillLichCruntDropList(&sm, mobname);

	int Size = sm.Size;

	if (Size > sizeof(MSG_UpdateDropList))
	{
		sm.Size = sizeof(MSG_UpdateDropList);
		return;
	}

	if (!pUser[conn].cSock.AddMessage((char*)&sm, sizeof(MSG_UpdateDropList)))
		CloseUser(conn);

}

void ReqDropList(int conn, char* pMsg)
{
	if (conn <= 0 || conn >= MAX_USER)
		return;

	if (pUser[conn].Mode != USER_PLAY)
		return;

	if (pUser[conn].cSock.Sock == 0)
		return;

	MSG_ReqDropList* m = (MSG_ReqDropList*)pMsg;

	if (m->Region < 0 || m->Region >= 13)
		return;

	if (pUser[conn].DropListDelay != 0)
	{
		int isTime = GetTickCount64() - pUser[conn].DropListDelay; // ATRASO PACOTE

		if (isTime < 300)
		{
			SendClientMessage(conn, "Aguarde 1 segundo para uma nova Tentativa.");
			return;
		}

	}
	pUser[conn].DropListDelay = GetTickCount64();

	UpdateDropList(conn, m->MobName, m->Region);
	return;
}

void ReqdListNames(int conn, char* pMsg)
{
	if (conn <= 0 || conn >= MAX_USER)
		return;

	if (pUser[conn].Mode != USER_PLAY)
		return;

	if (pUser[conn].cSock.Sock == 0)
		return;

	if (pUser[conn].Atraso != 0)
	{
		int isTime = GetTickCount64() - pUser[conn].Atraso; // ATRASO PACOTE

		if (isTime < 300)
		{
			SendClientMessage(conn, "Aguarde 1 segundo para uma nova Tentativa.");
			return;
		}

	}
	pUser[conn].Atraso = GetTickCount64();

	MSG_ReqdListNames* m = (MSG_ReqdListNames*)pMsg;

	MSG_SendDListNames p;
	memset(&p, 0, sizeof(MSG_SendDListNames));
	p.Type = _MSG_SendDListNames;
	p.Size = sizeof(MSG_SendDListNames);
	p.ID = conn;

	if (m->Page < 0 || m->Page > 15)
		return;

	int region = m->Page;
	int mob = 0;

	auto& pc = cSQL::instance();

	sprintf(xQuery, "SELECT * FROM `droplist` WHERE `region` = %d ORDER BY `nome` ASC LIMIT 30", region);

	MYSQL_ROW row;
	MYSQL* wSQL = pc.wStart();
	MYSQL_RES* result = pc.wRes(wSQL, xQuery);

	if (result == NULL)
		return;

	while ((row = mysql_fetch_row(result)) != NULL)
	{
		if (mob >= 30)
			break;

		strncpy(p.MobName[mob], row[1], sizeof(p.MobName[mob]) - 1);
		p.MobName[mob][sizeof(p.MobName[mob]) - 1] = 0;
		mob++;
	}	

	if (region == 8)
		ForceMobName(&p, &mob, "Lich_Crunt");

	int Size = p.Size;
	if (Size > sizeof(MSG_SendDListNames))
	{
		p.Size = sizeof(MSG_SendDListNames);
		return;
	}

	if (!pUser[conn].cSock.AddMessage((char*)&p, sizeof(MSG_SendDListNames)))
		CloseUser(conn);
}



//void ReqDropList(int conn, char* pMsg)
//{
//	if (conn <= 0 || conn >= MAX_USER)
//		return;
//
//	if (pUser[conn].Mode != USER_PLAY)
//		return;
//
//	if (pUser[conn].cSock.Sock == 0)
//		return;
//
//	MSG_ReqDropList* m = (MSG_ReqDropList*)pMsg;
//
//	if (m->State == 1)
//	{
//		if (pUser[conn].DroplistTime != 0)
//		{
//			int isTime = GetTickCount64() - pUser[conn].DroplistTime;
//
//			pUser[conn].DroplistTime = GetTickCount64();
//
//			if (isTime < 10000)
//			{
//				SendClientMessage(conn, "Aguarde 10 segundos para pesquisar novamente");
//				return;
//			}
//		}
//		pUser[conn].DroplistTime = GetTickCount64();
//
//		for (int i = 1000; i < 12800; i++)
//		{
//			int LastFace = 0;
//			int View = GetInView(conn, i);
//			if (View == 1) {
//				auto Mob = GetMobFromIndex(i);
//
//				if (Mob->MOB.Merchant != 0)
//					continue;
//
//				if ((Mob->GenerateIndex >= 3391 && Mob->GenerateIndex <= 3435) ||
//					(Mob->GenerateIndex >= 982 && Mob->GenerateIndex <= 986) ||
//					(Mob->GenerateIndex >= 3784 && Mob->GenerateIndex <= 3792) ||
//					Mob->GenerateIndex == 4531)
//					continue;
//
//				if ((strcmp(Mob->MOB.MobName, "Andaluz B") == 0) || (strcmp(Mob->MOB.MobName, "Andaluz N") == 0) ||
//					(strcmp(Mob->MOB.MobName, "Condor") == 0) || (strcmp(Mob->MOB.MobName, "Dente de Sabre") == 0) ||
//					(strcmp(Mob->MOB.MobName, "Dragao") == 0) || (strcmp(Mob->MOB.MobName, "Dragao Menor") == 0) ||
//					(strcmp(Mob->MOB.MobName, "Grifo") == 0) || (strcmp(Mob->MOB.MobName, "Grifo Sangrento") == 0) ||
//					(strcmp(Mob->MOB.MobName, "Gorila") == 0) || (strcmp(Mob->MOB.MobName, "Hipogrifo") == 0) ||
//					(strcmp(Mob->MOB.MobName, "Javali") == 0) || (strcmp(Mob->MOB.MobName, "Javali ") == 0) ||
//					(strcmp(Mob->MOB.MobName, "Succubus") == 0) || (strcmp(Mob->MOB.MobName, "Tigre") == 0) ||
//					(strcmp(Mob->MOB.MobName, "Urso") == 0) || (strcmp(Mob->MOB.MobName, "Vinha") == 0))
//					continue;
//
//				int Face = Mob->MOB.Equip[0].sIndex;
//				int Effect = Mob->GenerateIndex;
//
//				for (int x = 0; x < 10; x++) {
//					if (pUser[conn].Ingame.MobFace[x] != 0)
//						continue;
//
//					bool MobList = false;
//					for (int y = 0; y < 10; y++) {
//						if (pUser[conn].Ingame.MobFace[y] == Face)
//							MobList = true;
//					}
//
//					if (pUser[conn].Ingame.MobFace[x] == 0 && MobList == false) {
//						pUser[conn].Ingame.MobFace[x] = Face;
//						pUser[conn].Ingame.MobEffect[x] = Effect;
//						strncpy(pUser[conn].Ingame.MobName[x], Mob->MOB.MobName, sizeof(pUser[conn].Ingame.MobName[x]));
//						break;
//					}
//				}
//			}
//		}
//		UpdateDropList(conn);
//		m->State = 0;
//		return;
//	}
//}
