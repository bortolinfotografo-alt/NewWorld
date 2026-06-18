#pragma once

#include <stdio.h>
#include "..\Basedef.h"

void UpdateDropList(int conn, const char* mobname, int region);
void FiltroDrop(int conn, int Index);
void ReqDropList(int conn, char* pMsg);
void ReqdListNames(int conn, char* pMsg);
void ReqTeleport(int conn, char* pMsg);
void ReqDonateShop(int conn, char* pMsg);
void EventoClick(int conn, int npc);
void Jephi(int conn, char* pMsg);
void ItemLevel(int conn, char* pMsg);
void TradeDonate(int conn, char* pMsg);
void UpdateShopDonate1();
void UpdateShopDonate2();
void UpdateShopDonate3();
void SendShopDonate(int conn);
void RequestAlias(int conn, char* pMsg);
void MailItem(int conn);
void SendMailItem(int conn);
void doRanking(int conn);
void sendRanking(int conn, int state);
void GerarPix(int conn);
void GerarRecaptcha();
void VerificarRecaptcha(int conn, int word);
void SendRecaptcha(int conn, int territorio);

void attMountTraje(int conn, char* pMsg);
void SetMountTraje(int conn, int slot);
void RemoveMountTraje(int conn);
bool MountTrajeBuy(int conn, int Index);
void RequestTraje(int conn);

enum ACCOUNT_STAFF_ROLE
{
	ACCOUNT_STAFF_NONE = 0,
	ACCOUNT_STAFF_GM = 1,
	ACCOUNT_STAFF_ADMIN = 2
};

// Cargos por conta, lidos ao vivo da pasta run do TMSrv.
int GetAccountStaffRole(const char* account);
bool IsAdminAccount(const char* account);
bool IsGameMasterAccount(const char* account);
bool CanUseStaffCharacterName(const char* account, const char* characterName);
bool ApplyAccountStaffPrefix(const char* account, char* characterName);

// ============================================================================
// Cidade de gelo Nippleheim/Karden: 0 = DESABILITADA (em breve), 1 = habilitada.
// REABILITAR portal de Noatun (JULIARZAN) + botao Painel D (Nipple): trocar para
// 1 e recompilar o TMSrv. (O pergaminho 3437 tambem volta; pra vender, re-adicionar nas lojas.)
// ============================================================================
#define NIPPLEHEIM_HABILITADO 0

int NumbGen(int Initial, int Final);
void LoadEvent();
void ReadServerConfigs();
void ReadExpTableN();
void ReadExpTableM();
//void ReadExpTableA();

int ControleDropGold(int conn, int target, int DropedGold);
STRUCT_ITEM* ControleDropItem(int conn, int target, int bonus, int PosX, int PosY, STRUCT_ITEM* Item);
void LanDrop(int conn, int target);
void CartaDrop(int conn, int target, int type);
bool AutoDrop(int conn, int itemId);
bool AgroupList(int conn, int itemId);
bool LogList(int itemId);

