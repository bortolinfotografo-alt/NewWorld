
#include "ProcessClientMessage.h"

void Exec_MSG_ReqBuy(int conn, char *pMsg)
{
	MSG_ReqBuy* m = (MSG_ReqBuy*)pMsg;

	if (pMob[conn].MOB.CurrentScore.Hp == 0 || pUser[conn].Mode != USER_PLAY)
	{
		SendHpMode(conn);
		//AddCrackError(conn, 10, 86);
		RemoveTrade(conn);
		return;
	}

	if (pUser[conn].TradeMode)
	{
		RemoveTrade(conn);
		return;
	}

	if (pUser[conn].Trade.OpponentID)
	{
		RemoveTrade(conn);
		return;
	}

	int targetID = m->TargetID;
	int ownerID = GetAutoTradeOwner(targetID);
	int shopID = GetAutoTradeDisplayMob(ownerID);
	int itemPrice = m->Price;
	int itemTax = m->Tax;
	int itemPos = m->Pos;

	if (itemPrice < 0 || itemPrice > 2000000000)
		return;

	if (itemPos < 0 || itemPos > 60)
		return;

	if (itemTax < 0)
		return;

	if (ownerID <= 0 || ownerID >= MAX_USER || shopID <= 0 || shopID >= MAX_MOB)
	{
		RemoveTrade(conn);
		return;
	}

	if (pUser[conn].Mode != USER_PLAY)
	{
		RemoveTrade(conn);
		return;
	}

	if (ownerID <= 0 || ownerID >= MAX_USER || shopID <= 0 || shopID >= MAX_MOB)
	{
		RemoveTrade(conn);
		return;
	}

	if (pUser[conn].Mode != USER_PLAY)
	{
		RemoveTrade(conn);
		return;
	}

	if (ownerID == conn)
		return;

	if (pMob[conn].TargetX >= pMob[shopID].TargetX - VIEWGRIDX && pMob[conn].TargetX <= pMob[shopID].TargetX + VIEWGRIDX && pMob[conn].TargetY >= pMob[shopID].TargetY - VIEWGRIDY && pMob[conn].TargetY <= pMob[shopID].TargetY + VIEWGRIDY)
	{
		if (itemPos < 0 || itemPos >= MAX_AUTOTRADE)
			return;

		int StorageSlot = pUser[ownerID].AutoTrade.CarryPos[itemPos];

		if (StorageSlot < 0 || StorageSlot >= MAX_CARGO)
			return;

		if (itemTax != pUser[ownerID].AutoTrade.Tax)
			return;

		if (itemPrice != pUser[ownerID].AutoTrade.Coin[itemPos])
			return;

		if (memcmp(&m->item, &pUser[ownerID].AutoTrade.Item[itemPos], sizeof(STRUCT_ITEM)))
		{
			RemoveTrade(conn);
			return;
		}

		if (memcmp(&pUser[ownerID].Cargo[StorageSlot], &pUser[ownerID].AutoTrade.Item[itemPos], sizeof(STRUCT_ITEM)))
		{
			RemoveTrade(conn);
			return;
		}


		if (pMob[conn].MOB.Coin < itemPrice)
		{
			SendClientMessage(conn, g_pMessageStringTable[_NN_Not_Enough_Money]);
			return;
		}

		unsigned int xcoin = pUser[ownerID].Coin + itemPrice;

		if (xcoin > 2000000000)
		{
			SendClientMessage(conn, g_pMessageStringTable[_NN_Cant_get_more_than_2G]);
			return;
		}

		int i = 0;

		for (i = 0; i < MAX_CARRY - 4; i++)
		{
			if (pMob[conn].MOB.Carry[i].sIndex != 0)
				continue;

			break;
		}

		if (i == MAX_CARRY)
		{ 
			SendMsgExp(conn, g_pMessageStringTable[_NN_You_Have_No_Space_To_Trade], TNColor::Red, false);
			return;
		}

		int target_village = BASE_GetVillage(pMob[shopID].TargetX, pMob[shopID].TargetY);

		if (target_village < 0 || target_village >= 5)
		{
			SendClientMessage(conn, g_pMessageStringTable[_NN_OnlyVillage]);
			return;
		}

		memcpy(&pMob[conn].MOB.Carry[i], &pUser[ownerID].Cargo[StorageSlot], sizeof(STRUCT_ITEM));
		SendItem(conn, ITEM_PLACE_CARRY, i, &pMob[conn].MOB.Carry[i]);

		char tmplog[2048];
		BASE_GetItemCode(&m->item, tmplog);

		sprintf_s(temp, "autotrade_buy,target_name:%s price:%d item:%s", pUser[ownerID].AccountName, itemPrice, tmplog);
		ItemLog(pUser[conn].AccountName, pUser[conn].MacAddress, pUser[conn].IP, temp);

		int imposto = 0;
		int price_end = itemPrice;

		if (itemPrice >= 100000)
		{
			imposto = (itemPrice / 100) *  itemTax;
			price_end = itemPrice - imposto;
		}

		pUser[ownerID].AutoTrade.CarryPos[itemPos] = -1;

		memset(&pUser[ownerID].AutoTrade.Item[itemPos], 0, sizeof(STRUCT_ITEM));

		pUser[ownerID].AutoTrade.Coin[itemPos] = 0;

		memset(&pUser[ownerID].Cargo[StorageSlot], 0, sizeof(STRUCT_ITEM));

		SendItem(ownerID, ITEM_PLACE_CARGO, StorageSlot, &pUser[ownerID].Cargo[StorageSlot]);

		pMob[conn].MOB.Coin -= itemPrice;

		if (pUser[ownerID].Coin < 2000000000)
			pUser[ownerID].Coin += price_end;

		SendEtc(conn);
		SendCargoCoin(ownerID);
		SaveUser(ownerID, 1);

		const char* soldItemName = "Item";
		if (m->item.sIndex > 0 && m->item.sIndex < MAX_ITEMLIST)
			soldItemName = g_pItemList[m->item.sIndex].Name;

		char soldMessage[128];
		snprintf(soldMessage, sizeof(soldMessage), "Lojinha: %s comprou [%s] por %d gold.",
			pMob[conn].MOB.MobName,
			soldItemName,
			itemPrice);
		SendClientMessage(ownerID, soldMessage);

		if (imposto > 0)
		{
			char taxMessage[128];
			snprintf(taxMessage, sizeof(taxMessage), "Lojinha: imposto %d%% = %d. Voce recebeu %d gold.",
				itemTax,
				imposto,
				price_end);
			SendClientMessage(ownerID, taxMessage);
		}

		// DESCOMENTAR AQUI PARA QUANDO ALGUEM COMPRAR ALGUMA ITEM NA LOJA A LOJA FECHAR PINHEIRO
		//if (pUser[ownerID].AutoTrade.Item[0].sIndex == 0 && pUser[ownerID].AutoTrade.Item[1].sIndex == 0 &&
		//	pUser[ownerID].AutoTrade.Item[2].sIndex == 0 && pUser[ownerID].AutoTrade.Item[3].sIndex == 0 &&
		//	pUser[ownerID].AutoTrade.Item[4].sIndex == 0 && pUser[ownerID].AutoTrade.Item[5].sIndex == 0 &&
		//	pUser[ownerID].AutoTrade.Item[6].sIndex == 0 && pUser[ownerID].AutoTrade.Item[7].sIndex == 0 &&
		//	pUser[ownerID].AutoTrade.Item[8].sIndex == 0 && pUser[ownerID].AutoTrade.Item[9].sIndex == 0 &&
		//	pUser[ownerID].AutoTrade.Item[10].sIndex == 0) 
		//{ 
		//	RemoveTrade(targetID);
		//	RemoveTrade(conn);
		//}

		if (target_village >= 0 && target_village < 5)
		{
			if (GuildImpostoID[target_village] >= MAX_USER && GuildImpostoID[target_village] < MAX_MOB && imposto > 0
				&& pMob[GuildImpostoID[target_village]].Mode != MOB_EMPTY && pMob[GuildImpostoID[target_village]].MOB.Guild == g_pGuildZone[target_village].ChargeGuild)
			{
				if (pMob[GuildImpostoID[target_village]].MOB.Exp < 200000000000)
					pMob[GuildImpostoID[target_village]].MOB.Exp += imposto;
			}
			if (GuildImpostoID[4] >= MAX_USER && GuildImpostoID[4] < MAX_MOB && imposto > 0 && pMob[GuildImpostoID[4]].Mode != MOB_EMPTY
				&& pMob[GuildImpostoID[4]].MOB.Guild == g_pGuildZone[4].ChargeGuild)
			{
				if (pMob[GuildImpostoID[4]].MOB.Exp < 200000000000)
					pMob[GuildImpostoID[4]].MOB.Exp += imposto;

			}
		}

		MSG_STANDARDPARM2 sm_is;
		memset(&sm_is, 0, sizeof(MSG_STANDARDPARM2));

		sm_is.Type = _MSG_ItemSold;
		sm_is.ID = ESCENE_FIELD;
		sm_is.Parm1 = shopID;
		sm_is.Parm2 = itemPos;
		sm_is.Size = sizeof(MSG_STANDARDPARM2);

		GridMulticast(pMob[shopID].TargetX, pMob[shopID].TargetY, (MSG_STANDARD*)&sm_is, 0);

		if (pMob[ownerID].TargetX < pMob[shopID].TargetX - VIEWGRIDX || pMob[ownerID].TargetX > pMob[shopID].TargetX + VIEWGRIDX ||
			pMob[ownerID].TargetY < pMob[shopID].TargetY - VIEWGRIDY || pMob[ownerID].TargetY > pMob[shopID].TargetY + VIEWGRIDY)
		{
			if (!pUser[ownerID].cSock.AddMessage((char*)&sm_is, sizeof(MSG_STANDARDPARM2)))
				CloseUser(ownerID);
		}

		SendClientMessage(ownerID, g_pMessageStringTable[_NN_ItemSold]);
	}

	else
	{
		//Log("err,too far from autotrade - _MSG_ReqBuy", pUser[conn].AccountName, pUser[conn].IP);
		return;
	}
}
