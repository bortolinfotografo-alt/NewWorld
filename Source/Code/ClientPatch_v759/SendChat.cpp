#include "main.h"

/* N�o finalizado */
void CreateMessagePanel(char* msg, int color)
{
	static int CallBack = 0x04073CF;

	*(int*)0x04073CA = color;

	int* dat = *(int**)0x6F0AB0;

	__asm
	{
		PUSH msg
		MOV ECX, dat
		CALL CallBack
	}

	*(int*)0x04073CA = 0xFFFFAAAA;
}

// Macro do Pergaminho da Agua (ver MacroPergaminhoAgua.cpp)
extern bool MacroPergaAgua;
extern int MacroDebugScroll();
extern void MacroScanPos(int tx, int ty);
extern void MacroPosDump();

int HKD_SendChat(char* command)
{
	//	if (*command != '@')
	//		return TRUE;
	UINT16 X = 0;
	UINT16 Y = 0;
	UINT16 X1 = 0;
	UINT16 Y1 = 0;

	// --- Macro do Pergaminho da Quest da Agua ---
	if (strcmp(command, "/macropergaon") == 0)
	{
		if (!MacroPergaAgua)
		{
			MacroPergaAgua = true;
			SendMsgExp(TNColor::GreenYellow, (char*)"Macro de Pergaminho da Agua ATIVADO. Va para a entrada/sala da agua.");
		}
		else
		{
			SendMsgExp(TNColor::GreenYellow, (char*)"Macro de Pergaminho ja estava ativado.");
		}
		return FALSE; // consome o comando (nao envia ao servidor)
	}

	if (strcmp(command, "/macropergaoff") == 0)
	{
		MacroPergaAgua = false;
		SendMsgExp(TNColor::Red, (char*)"Macro de Pergaminho DESATIVADO.");
		return FALSE;
	}

	if (strcmp(command, "/pergastatus") == 0)
	{
		int sc = MacroDebugScroll();
		SendMsgExp(TNColor::NewYellow, (char*)"Macro: %s | Pergaminho no inventario (ID do item): %d", MacroPergaAgua ? "LIGADO" : "desligado", sc);
		return FALSE;
	}

	if (strcmp(command, "/pergapos") == 0)
	{
		MacroPosDump();
		return FALSE;
	}

	// Diagnostico de posicao: /pergascan <X> <Y>  (use os valores que aparecem no canto da tela)
	{
		int scanX = 0, scanY = 0;
		if (sscanf(command, "/pergascan %d %d", &scanX, &scanY) == 2)
		{
			MacroScanPos(scanX, scanY);
			return FALSE;
		}
	}

	if (strcmp(command, "/comandos") == 0 || strcmp(command, "/cmds") == 0)
	{
		SendMsgExp(TNColor::NewYellow,   (char*)"=== COMANDOS DO SERVIDOR ===");
		SendMsgExp(TNColor::GreenYellow, (char*)"/novato - aviso da recompensa de iniciante");
		SendMsgExp(TNColor::GreenYellow, (char*)"/online - jogadores online");
		SendMsgExp(TNColor::GreenYellow, (char*)"/saldo - sua honra e fama");
		SendMsgExp(TNColor::GreenYellow, (char*)"/filtro - liga/desliga filtro de drop   |   /limpar - limpa o filtro");
		SendMsgExp(TNColor::GreenYellow, (char*)"/grupo - cria senha do grupo   |   /entrar <nick> <senha> - entra no grupo");
		SendMsgExp(TNColor::GreenYellow, (char*)"/sair - sai da guilda   |   /transferir <nick> - transfere lideranca da guilda");
		SendMsgExp(TNColor::GreenYellow, (char*)"/closeloja - fechar lojinha aberta   |   /tab <txt> - titulo (lv70+)");
		SendMsgExp(TNColor::GreenYellow, (char*)"/guildinfo - info da guild (lider)   |   /fimcidadao - remove cidadania");
		SendMsgExp(TNColor::GreenYellow, (char*)"/@aposta1 a /@aposta4 - apostas   |   /nt - contador de troca de nome");
		SendMsgExp(TNColor::GreenYellow, (char*)"/macropergaon /macropergaoff /pergastatus - macro do pergaminho da agua");
		SendMsgExp(TNColor::NewYellow,   (char*)"Teclas: D = painel   |   F = fada (teleport/filtro)   |   U = modo foto");
		return FALSE;
	}

	/*int Filtro = 0;

	if (sscanf_s(command, "/addfiltro %d", &Filtro))
	{
		SendFiltro(Filtro);
		return false;
	}*/

	return TRUE;
}