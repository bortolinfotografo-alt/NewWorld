/*
*   Copyright (C) {2015}  {Victor Klafke, Charles TheHouse}
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see [http://www.gnu.org/licenses/].
*
*   Contact at: victor.klafke@ecomp.ufsm.br
*/
#include "ProcessClientMessage.h"
#include <regex>
#include <mysql.h>
#include "wMySQL.h"
#include "Functions.h"

char g_PendingCreateCharacterName[MAX_USER][NAME_LENGTH];
int g_PendingCreateCharacterSlot[MAX_USER];
int g_PendingCreateCharacterAccountId[MAX_USER];

static void ClearPendingCreateCharacter(int conn)
{
	g_PendingCreateCharacterName[conn][0] = 0;
	g_PendingCreateCharacterSlot[conn] = -1;
	g_PendingCreateCharacterAccountId[conn] = 0;
}

static int GetCreateCharacterAccountId(int conn)
{
	auto& pc = cSQL::instance();

	sprintf(xQuery, "SELECT `id` FROM `accounts` WHERE UPPER(`username`) = UPPER('%s') LIMIT 1", pUser[conn].AccountName);

	MYSQL* wSQL = pc.wStart();
	MYSQL_RES* result = pc.wRes(wSQL, xQuery);

	if (result == NULL)
		return 0;

	MYSQL_ROW row;
	int accountId = 0;

	if ((row = mysql_fetch_row(result)) != NULL && row[0] != NULL)
		accountId = atoi(row[0]);

	mysql_free_result(result);

	return accountId;
}

void Exec_MSG_CreateCharacter(int conn, char *pMsg)
{
	MSG_CreateCharacter* m = (MSG_CreateCharacter*)pMsg;

	if (m->Size > sizeof(MSG_CreateCharacter))
		return;


	if (pUser[conn].Mode != USER_CHARWAIT)
	{
		SystemLog(pUser[conn].AccountName, pUser[conn].MacAddress, pUser[conn].IP, strFmt("err,createchar not user_selchar %d %d", conn, pUser[conn].Mode));

		SendClientSignal(conn, 0, _MSG_NewCharacterFail);
		return;
	}

	if (pUser[conn].Atraso != 0)
	{
		int isTime = GetTickCount64() - pUser[conn].Atraso; // ATRASO PACOTE

		if (isTime < 5000)
		{
			SendClientMessage(conn, "Aguarde 5 segundos para uma nova Tentativa.");
			return;
		}

	}
	pUser[conn].Atraso = GetTickCount64();

	m->MobName[NAME_LENGTH - 1] = 0;
	m->MobName[NAME_LENGTH - 2] = 0;

	if (pUser[conn].WaitDB) {
		SendClientMessage(conn, "Aguarde um momento");
		return;
	}

	//validação de string
	std::string name = { m->MobName };
	std::regex int_regex(GetAccountStaffRole(pUser[conn].AccountName) == ACCOUNT_STAFF_NONE ? "^[A-Za-z0-9-]{4,12}$" : "^[A-Za-z0-9-]{4,9}$");

	if (!std::regex_match(name, int_regex))
	{
		SendClientSignal(conn, 0, _MSG_NewCharacterFail);
		SystemLog(pUser[conn].AccountName, pUser[conn].MacAddress, pUser[conn].IP, strFmt("err,createchar invalid regex name:%s len:%d mode:%d", m->MobName, (int)strlen(m->MobName), pUser[conn].Mode));
		return;
	}

	if (!ApplyAccountStaffPrefix(pUser[conn].AccountName, m->MobName))
	{
		SendClientMessage(conn, "Nome de equipe invalido ou reservado.");
		SendClientSignal(conn, 0, _MSG_NewCharacterFail);
		SystemLog(pUser[conn].AccountName, pUser[conn].MacAddress, pUser[conn].IP,
			strFmt("err,createchar invalid staff name:%s", m->MobName));
		return;
	}


	if (BASE_CheckValidString(m->MobName))
	{
		m->Type = _MSG_DBCreateCharacter;
		m->ID = conn;

		int accountId = GetCreateCharacterAccountId(conn);
		if (accountId <= 0)
		{
			SystemLog(pUser[conn].AccountName, pUser[conn].MacAddress, pUser[conn].IP,
				strFmt("warn,createchar account id unavailable, continuing name:%s", m->MobName));
		}

		ClearPendingCreateCharacter(conn);
		strncpy(g_PendingCreateCharacterName[conn], m->MobName, NAME_LENGTH);
		g_PendingCreateCharacterName[conn][NAME_LENGTH - 1] = 0;
		g_PendingCreateCharacterName[conn][NAME_LENGTH - 2] = 0;
		g_PendingCreateCharacterSlot[conn] = m->Slot;
		g_PendingCreateCharacterAccountId[conn] = accountId;

		pUser[conn].Mode = USER_WAITDB;
		pUser[conn].WaitDB = true;

		DBServerSocket.SendOneMessage((char*)m, sizeof(MSG_CreateCharacter));
		SystemLog(pUser[conn].AccountName, pUser[conn].MacAddress, pUser[conn].IP, strFmt("etc,createchar name:%s %d %d", m->MobName, conn, pUser[conn].Mode));
	}

	else
	{
		SystemLog(pUser[conn].AccountName, pUser[conn].MacAddress, pUser[conn].IP, strFmt("err,createchar invalid basedef name:%s len:%d mode:%d", m->MobName, (int)strlen(m->MobName), pUser[conn].Mode));
		SendClientSignal(conn, 0, _MSG_NewCharacterFail);
	}
}