#include "ProcessClientMessage.h"

// NPC Freyja (Aposta da Pista +6): a aposta NAO e feita pela NPC, e sim por COMANDO.
// Em vez de "em manutencao", instruimos o jogador a usar /@aposta1 a /@aposta4
// (logica em _MSG_MessageWhisper.cpp: dentro da zona da pista +6, somente o lider,
//  paga gold e reduz os mobs da pista +6).
void Exec_MSG_onPaymentGold(int conn, char* pMsg)
{
	if (conn <= 0 || conn >= MAX_USER)
		return;

	if (pUser[conn].Mode != USER_PLAY)
		return;

	// Mesma zona da pista +6 onde o NPC Freyja fica.
	if (pMob[conn].TargetX >= 3332 && pMob[conn].TargetY >= 1471 &&
		pMob[conn].TargetX <= 3449 && pMob[conn].TargetY <= 1529)
	{
		SendClientMessage(conn, "A aposta da pista +6 e feita por COMANDO: digite /@aposta1 a /@aposta4.");
		SendClientMessage(conn, "Fique dentro da zona da pista +6 e use o comando. Somente o lider do grupo pode apostar.");
	}
}