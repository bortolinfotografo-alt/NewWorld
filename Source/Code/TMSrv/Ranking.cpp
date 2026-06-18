#pragma once

#include "ProcessClientMessage.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <ctime>
#include "SendFunc.h"
#include "Functions.h"
#include "wMySQL.h"
#include "Ranking.h"

static int SafeRankingAtoi(const char* value, int fallback = 0)
{
	if (value == NULL || value[0] == 0)
		return fallback;

	return atoi(value);
}

static int ClampRankingClass(int value)
{
	if (value < 0 || value > 3)
		return 0;

	return value;
}

static int ClampRankingEvolution(int value)
{
	if (value < 1 || value > 5)
		return 1;

	return value;
}

void doRanking(int conn)
{
	if (conn <= 0 || conn >= MAX_USER)
		return;

	if (pUser[conn].Mode != USER_PLAY)
		return;

	if (pUser[conn].cSock.Sock == 0)
		return;

	auto& pc = cSQL::instance();

	int Kills = GetTotKill(conn);

	/*MORTAL			2
	ARCH			1
	CELESTIAL		3
	CELESTIALCS		4
	SCELESTIAL		5
	HARDCORE        6
	HARDCOREA		7
	HARDCORECS		8
	SHARDCORE		9*/

	int Evolution = 0;

	if (pMob[conn].extra.ClassMaster == 2)
		Evolution = 1;

	if (pMob[conn].extra.ClassMaster == 1)
		Evolution = 2;

	if (pMob[conn].extra.ClassMaster > 2)
		Evolution = pMob[conn].extra.ClassMaster;

	sprintf(xQuery, "UPDATE `characteres` SET level = '%d', class = '%d', evolution = '%d', kingdom = '%d', frags = '%d', guild_id = '%d', guildlevel = '%d' WHERE `nick` = '%s' AND `slot_char` = '%d'",
		pMob[conn].MOB.BaseScore.Level, pMob[conn].MOB.Class, Evolution, pMob[conn].MOB.Clan,
		Kills, pMob[conn].MOB.Guild, pMob[conn].MOB.GuildLevel, pMob[conn].MOB.MobName, pUser[conn].Slot);
	pc.wQuery(xQuery);
}

void sendRanking(int conn, int state) {

	if (conn <= 0 || conn >= MAX_USER)
		return;

	if (pUser[conn].Mode != USER_PLAY)
		return;

	if (pUser[conn].cSock.Sock == 0)
		return;

	if (pUser[conn].RankingDelay != 0)
	{
		int isTime = GetTickCount64() - pUser[conn].RankingDelay; // ATRASO PACOTE

		if (isTime < 200)
		{
			SendClientMessage(conn, "Aguarde 1 segundo para uma nova Tentativa.");
			return;
		}

	}
	pUser[conn].RankingDelay = GetTickCount64();

	auto& pc = cSQL::instance();

	std::string mobName[50];
	int RankLevel[50] = {};
	int RankClasse[50] = {};
	int RankEvolution[50] = {};
	int PvP[50] = {};
	int i = 0;

	for (int j = 0; j < 50; j++)
	{
		RankClasse[j] = 0;
		RankEvolution[j] = 1;
	}

	if (state == 0) {
		sprintf(xQuery, "SELECT COALESCE(`nick`, ''), COALESCE(`level`, 0), COALESCE(`class`, 0), GREATEST(LEAST(COALESCE(`evolution`, 1), 5), 1), COALESCE(`frags`, 0) FROM `characteres` WHERE `nick` NOT LIKE '-ADM-%%' AND `nick` NOT LIKE 'ADM-%%' AND `nick` NOT LIKE '[ADM]%%' AND `nick` NOT LIKE '-GM-%%' AND `nick` NOT LIKE 'GM-%%' AND `nick` NOT LIKE '[GM]%%' ORDER BY COALESCE(`evolution`, 0) DESC, COALESCE(`level`, 0) DESC LIMIT 50");
		MYSQL_ROW row;
		MYSQL* wSQL = pc.wStart();
		MYSQL_RES* result = pc.wRes(wSQL, xQuery);

		if (result == NULL)
		{
			SendClientMessage(conn, "Ranking indisponivel. Verifique a conexao com o banco.");
			return;
		}

		while (i < 50 && (row = mysql_fetch_row(result)) != NULL)
		{
			mobName[i] = row[0] ? row[0] : "";
			RankLevel[i] = SafeRankingAtoi(row[1]) + 1;
			RankClasse[i] = ClampRankingClass(SafeRankingAtoi(row[2]));
			RankEvolution[i] = ClampRankingEvolution(SafeRankingAtoi(row[3], 1));
			PvP[i] = SafeRankingAtoi(row[4]);
			i++;
		}

		MSG_SendRanking sm;
		memset(&sm, 0, sizeof(MSG_SendRanking));

		sm.Type = _MSG_SendRanking;
		sm.Size = sizeof(MSG_SendRanking);
		sm.ID = conn;
		sm.State = state;

		for (int i = 0; i < 50; i++) {
			strncpy(sm.RankName[i], mobName[i].c_str(), sizeof(sm.RankName[i]) - 1);
			sm.RankName[i][sizeof(sm.RankName[i]) - 1] = 0;
			sm.RankLevel[i] = RankLevel[i];
			sm.RankClasse[i] = ClampRankingClass(RankClasse[i]);
			sm.RankEvolution[i] = ClampRankingEvolution(RankEvolution[i]);
			sm.PvP[i] = PvP[i];
		}

		int Size = sm.Size;

		if (Size > sizeof(MSG_SendRanking))
		{
			sm.Size = 0;
			return;
		}

		if (!pUser[conn].cSock.AddMessage((char*)&sm, sizeof(MSG_SendRanking)))
			CloseUser(conn);

		mysql_free_result(result);
	}

	if (state == 1) {
		sprintf(xQuery, "SELECT COALESCE(`nick`, ''), COALESCE(`level`, 0), COALESCE(`class`, 0), GREATEST(LEAST(COALESCE(`evolution`, 1), 5), 1), COALESCE(`frags`, 0) FROM `characteres` WHERE `nick` NOT LIKE '-ADM-%%' AND `nick` NOT LIKE 'ADM-%%' AND `nick` NOT LIKE '[ADM]%%' AND `nick` NOT LIKE '-GM-%%' AND `nick` NOT LIKE 'GM-%%' AND `nick` NOT LIKE '[GM]%%' ORDER BY COALESCE(`frags`, 0) DESC LIMIT 50");
		MYSQL_ROW row;
		MYSQL* wSQL = pc.wStart();
		MYSQL_RES* result = pc.wRes(wSQL, xQuery);

		if (result == NULL)
		{
			SendClientMessage(conn, "Ranking indisponivel. Verifique a conexao com o banco.");
			return;
		}

		while (i < 50 && (row = mysql_fetch_row(result)) != NULL)
		{
			mobName[i] = row[0] ? row[0] : "";
			RankLevel[i] = SafeRankingAtoi(row[1]) + 1;
			RankClasse[i] = ClampRankingClass(SafeRankingAtoi(row[2]));
			RankEvolution[i] = ClampRankingEvolution(SafeRankingAtoi(row[3], 1));
			PvP[i] = SafeRankingAtoi(row[4]);
			i++;
		}

		MSG_SendRanking sm;
		memset(&sm, 0, sizeof(MSG_SendRanking));

		sm.Type = _MSG_SendRanking;
		sm.Size = sizeof(MSG_SendRanking);
		sm.ID = conn;
		sm.State = state;

		for (int i = 0; i < 50; i++) {
			strncpy(sm.RankName[i], mobName[i].c_str(), sizeof(sm.RankName[i]) - 1);
			sm.RankName[i][sizeof(sm.RankName[i]) - 1] = 0;
			sm.RankLevel[i] = RankLevel[i];
			sm.RankClasse[i] = ClampRankingClass(RankClasse[i]);
			sm.RankEvolution[i] = ClampRankingEvolution(RankEvolution[i]);
			sm.PvP[i] = PvP[i];
		}

		int Size = sm.Size;

		if (Size > sizeof(MSG_SendRanking))
		{
			sm.Size = 0;
			return;
		}

		if (!pUser[conn].cSock.AddMessage((char*)&sm, sizeof(MSG_SendRanking)))
			CloseUser(conn);

		mysql_free_result(result);
	}
}
