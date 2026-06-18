#include "main.h"
#include "UISlot.h"
#define DROPLIST 3
#define RANKING 5

static int NormalizeRankingClass(int value)
{
	if (value < 0 || value > 3)
		return 0;

	return value;
}

static int NormalizeRankingEvolution(int value)
{
	if (value < 1 || value > 5)
		return 1;

	return value;
}

static const char* GetRankingClassName(int value)
{
	static const char* Classe[4] = { "TK", "FM", "BM", "HT" };
	return Classe[NormalizeRankingClass(value)];
}

static const char* GetRankingEvolutionName(int value)
{
	static const char* Evolution[5] = { "Mortal", "Arch", "Celestial", "CelestialCS", "SubCelestial" };
	return Evolution[NormalizeRankingEvolution(value) - 1];
}

static void SetRankingText(int Handle, const char* Text)
{
	auto Label = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(Handle);
	if (Label)
		Label->setConstString("%s", Text);
}

static void SetRankingPageText(int Page)
{
	auto Label = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919420);
	if (Label)
		Label->setConstString("%d", Page);
}

static void SetRankingRow(int Handle, int Position, const char* Name, int Value, int Evolution, int Classe)
{
	auto Label = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(Handle);
	if (!Label)
		return;

	const char* SafeName = Name && Name[0] ? Name : "-";
	Label->setConstString("%d. [%s][%d][%s][%s]",
		Position, SafeName, Value, GetRankingEvolutionName(Evolution), GetRankingClassName(Classe));
}

static void RenderRankingPage(int StartIndex, int State)
{
	if (StartIndex < 0)
		StartIndex = 0;

	if (StartIndex > 40)
		StartIndex = 40;

	SetRankingText(1919401, "Pos");
	SetRankingText(1919402, "Nick");
	SetRankingText(1919403, State == 1 ? "PvP" : "Level");
	SetRankingText(1919404, "Evolucao");
	SetRankingText(1919405, "Classe");

	for (int i = 0; i < 10; i++)
	{
		int Index = StartIndex + i;
		int Value = State == 1 ? g_pClientInfo->Ranking.PvP[Index] : g_pClientInfo->Ranking.RankLevel[Index];

		SetRankingRow(1919410 + i, Index + 1, g_pClientInfo->Ranking.RankName[Index], Value,
			g_pClientInfo->Ranking.RankEvolution[Index], g_pClientInfo->Ranking.RankClasse[Index]);
	}
}

static void SetRankingFallbackText(int Handle, const char* Format, ...)
{
	auto Label = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(Handle);
	if (!Label)
		return;

	char Buffer[128] = { 0 };
	va_list Args;
	va_start(Args, Format);
	int Size = vsprintf_s(Buffer, Format, Args);
	va_end(Args);

	if (Size < 0 || Size >= sizeof(Buffer))
		return;

	Label->setText(Buffer);
}

static void AnchorFallbackPageControlToPager()
{
	auto Page = g_pInterface->Instance()->getGuiFromHandle<UIControl>(179521);
	auto Prev = g_pInterface->Instance()->getGuiFromHandle<UIControl>(179530);
	auto Next = g_pInterface->Instance()->getGuiFromHandle<UIControl>(179531);
	if (!Page || !Prev || !Next)
		return;

	float Left = Prev->Left + Prev->Width;
	float Width = Next->Left - Left;
	if (Width < 12.0f || Width > 80.0f)
		return;

	Page->Left = Left;
	Page->Width = Width;
	Page->ViewLeft = Left;
	Page->ViewWidth = Width;
	Page->PosX = Prev->PosX + (UINT32)Prev->Width;
}

static void ApplyRankingFallbackLayout()
{
	for (int i = 0; i < 10; i++)
	{
		auto Button = g_pInterface->Instance()->getGuiFromHandle<UIControl>(179501 + i);
		auto Label = g_pInterface->Instance()->getGuiFromHandle<UIControl>(179511 + i);
		if (!Button || !Label)
			continue;

		float Left = Button->Left + 7.0f;
		float Width = Button->Width > 14.0f ? Button->Width - 14.0f : Button->Width;

		Label->Left = Left;
		Label->Width = Width;
		Label->ViewLeft = Left;
		Label->ViewWidth = Width;
		Label->PosX = Button->PosX + 7;
	}

	AnchorFallbackPageControlToPager();
}

static void ApplyRankingCenteredLayout()
{
	auto Grid = g_pInterface->Instance()->getGuiFromHandle<UIControl>(179600);

	float GrayLeft = 0.0f;
	float GrayWidth = 0.0f;

	if (Grid && Grid->Width > 20.0f)
	{
		GrayLeft = Grid->Left;
		GrayWidth = Grid->Width;
	}
	else
	{
		auto Panel = g_pInterface->Instance()->getGuiFromHandle<UIControl>(179500);
		auto Button = g_pInterface->Instance()->getGuiFromHandle<UIControl>(179501);
		if (Panel && Button)
		{
			float ButtonsRight = Button->Left + Button->Width;
			GrayLeft = ButtonsRight;
			GrayWidth = (Panel->Left + Panel->Width) - ButtonsRight;
		}
	}

	if (GrayWidth >= 40.0f)
	{
		const float ColWidth = 250.0f;
		float Left = GrayLeft + (GrayWidth - ColWidth) * 0.5f;
		if (Left < GrayLeft)
			Left = GrayLeft;

		for (int i = 0; i < 11; i++)
		{
			int Handle = (i < 10) ? (179511 + i) : 179521;
			auto Label = g_pInterface->Instance()->getGuiFromHandle<UIControl>(Handle);
			if (!Label)
				continue;

			Label->Left = Left;
			Label->Width = ColWidth;
			Label->ViewLeft = Left;
			Label->ViewWidth = ColWidth;
			Label->PosX = (UINT32)Left;
		}
	}
}

static void ClearRankingFallbackGrid()
{
	auto DropListGrid = g_pInterface->Instance()->getGuiFromHandle<UISlot>(179600);
	if (!DropListGrid)
		return;

	*(int*)((int)DropListGrid + 0x400) = 0x5;
	DropListGrid->deleteItems();
}

static void RenderRankingFallbackPage(int StartIndex, int State)
{
	if (StartIndex < 0)
		StartIndex = 0;

	if (StartIndex > 40)
		StartIndex = 40;

	SetRankingFallbackText(179521, "Ranking %s  %d/5", State == 1 ? "PvP" : "Level", ConfigR::RankingPage);

	for (int i = 0; i < 10; i++)
	{
		int Index = StartIndex + i;
		int Value = State == 1 ? g_pClientInfo->Ranking.PvP[Index] : g_pClientInfo->Ranking.RankLevel[Index];
		const char* SafeName = g_pClientInfo->Ranking.RankName[Index][0] ? g_pClientInfo->Ranking.RankName[Index] : "-";

		SetRankingFallbackText(179511 + i, "%02d. %s  %s %d  %s  %s",
			Index + 1,
			SafeName,
			State == 1 ? "PvP" : "Lv",
			Value,
			GetRankingEvolutionName(g_pClientInfo->Ranking.RankEvolution[Index]),
			GetRankingClassName(g_pClientInfo->Ranking.RankClasse[Index]));
	}
}

static bool OpenRankingFallbackPanel(int State)
{
	auto DropList = g_pInterface->Instance()->getGuiFromHandle<UIControl>(179500);
	if (!DropList)
		return false;

	ConfigR::WindowControl = RANKING;
	ConfigR::DropListPage = 0;
	ConfigR::DropListRegion = 0;

	ApplyRankingCenteredLayout();
	ClearRankingFallbackGrid();
	RenderRankingFallbackPage((ConfigR::RankingPage - 1) * 10, State);

	DropList->IsVisible = true;
	return true;
}

void CClientInfo::Init()
{
	this->Info.Cash = 0;
	this->Server.Double = 0;
	this->Server.DropRate = 0;
	this->Server.ExpRate = 0;
	this->Ranking.State = 0;

	for (int i = 0; i < 50; i++) {
		this->Info.Keys[i];
		this->Ranking.RankLevel[i] = 0;
		this->Ranking.RankEvolution[i] = 1;
		this->Ranking.RankClasse[i] = 0;
		this->Ranking.PvP[i] = 0;
		strncpy(this->Ranking.RankName[i], "", sizeof(this->Ranking.RankName[i]));
	}
	for (int i = 0; i < 10; i++) {
		strncpy(this->DropList.MobName[i], "", sizeof(this->DropList.MobName[i]));
	}
	for (int i = 0; i < 64; i++) {
		this->DropList.Item[i] = 0;
	}
	for (int i = 0; i < 3; i++) {
		for (int x = 0; x < 3; x++) {
			for (int z = 0; z < 15; z++) {
				for (int c = 0; c < 3; c++) {
					this->Info.cStoreItens[i][x][z][c] = 0;
				}
			}
		}
	}
	strncpy(this->Info.Pix, "", sizeof(this->Info.Pix));
}

void CClientInfo::ReceiveInfo(char* Packet)
{
	auto player = GetChar();

	auto sm = reinterpret_cast<MSG_STANDARD*>(Packet);
	switch (sm->Type)
	{
			
	case _MSG_UpdateEtc: {
		auto p = reinterpret_cast<MSG_UpdateEtc*>(Packet);
		
		this->Info.chave1 = p->chave1;
		if (this->Info.chave1 < 0)
			this->Info.chave1 = 0;

		for (int i = 0; i < 50; i++) {
			this->Info.Keys[i] = p->Keys[i];

			if (this->Info.Keys[i] < 0)
				this->Info.Keys[i] = 0;
		}
		ExecItemLevel();

	}break;		
	case _MSG_SendTraje: {
		auto p = reinterpret_cast<MSG_SendTraje*>(Packet);
		
		if (p->Tipo == 1) {
			auto PainelTrajes = g_pInterface->Instance()->getGuiFromHandle<UIControl>(1882000);
			PainelTrajes->IsVisible = true;

			STRUCT_ITEM item[15];
			int TrajeIndex = 4500 - 22;
			for (int i = 0; i < 15; i++) {
				auto TrajeGrid = g_pInterface->Instance()->getGuiFromHandle<UISlot>(1882001 + i);
				*(int*)((int)TrajeGrid + 0x400) = 0x5;
				TrajeGrid->deleteItemGui(0, 0);

				memset(&item[i], 0, sizeof(STRUCT_ITEM));
				item[i].sIndex = g_pClientInfo->Info.Keys[1 + i] + TrajeIndex;

				if (item[i].sIndex != 0 && (item[i].sIndex >= 4500 && item[i].sIndex <= 4549))
					TrajeGrid->setGui((int)TrajeGrid->New(item[i], 0, 0), 0, 0);
			}
		}	

	}break;		
	case _MSG_UpdateDonateStore:
	{
		auto p = reinterpret_cast<MSG_UpdateDonateStore*>(Packet);

		for (int i = 0; i < 3; i++) {
			for (int x = 0; x < 3; x++) {
				for (int z = 0; z < 15; z++) {
					for (int c = 0; c < 3; c++) {
						this->Info.cStoreItens[i][x][z][c] = p->cStoreItens[i][x][z][c];
					}
				}
			}
		}

	} break;
	case _MSG_Recaptcha: {
		auto p = reinterpret_cast<MSG_Recaptcha*>(Packet);

		char Word[7];
		strncpy(Word, "", 7);
		
		if (p->state == 0) {
			char Keywords[30][7] = {
			"AVIÃO", "TREM", "AVIÃO", "CARRO", "MOTO",
			"MOTO", "AVIÃO", "CARRO", "BIKE", "BIKE",
			"AVIÃO", "BIKE", "NAVIO", "BIKE", "NAVIO",
			"MOTO", "AVIÃO", "NAVIO", "CARRO", "MOTO",
			"NAVIO", "NAVIO", "TREM", "BIKE", "BIKE",
			"CARRO", "BIKE", "NAVIO", "AVIÃO", "TREM"
			};

			strncpy(Word, Keywords[p->word], 7);
		}
		if (p->state == 1) {
			char Keywords1[30][7] = {
			"MOTO", "NAVIO", "BIKE", "TREM", "CARRO",
			"TREM", "MOTO", "MOTO", "AVIÃO", "AVIÃO",
			"CARRO", "BIKE", "AVIÃO", "AVIÃO", "CARRO",
			"AVIÃO", "NAVIO", "TREM", "CARRO", "NAVIO",
			"AVIÃO", "BIKE", "NAVIO", "CARRO", "TREM",
			"BIKE", "TREM", "NAVIO", "BIKE", "MOTO"
			};
			strncpy(Word, Keywords1[p->word], 7);
		}
		if (p->state == 2) {
			char Keywords2[30][7] = {
			"BIKE", "TREM", "BIKE", "BIKE", "TREM",
			"CARRO", "MOTO", "CARRO", "NAVIO", "MOTO",
			"NAVIO", "BIKE", "BIKE", "AVIÃO", "TREM",
			"MOTO", "CARRO", "AVIÃO", "TREM", "BIKE",
			"CARRO", "TREM", "BIKE", "CARRO", "AVIÃO",
			"MOTO", "MOTO", "AVIÃO", "AVIÃO", "NAVIO",
			};
			strncpy(Word, Keywords2[p->word], 7);
		}
		auto PainelCaptcha = g_pInterface->Instance()->getGuiFromHandle<UIControl>(1319001);
		PainelCaptcha->IsVisible = true;	

		auto Desc = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1319002);
		Desc->setConstString("Clique na imagem que represente um(a): [%s]", Word);

		auto Desc2 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1319003);
		Desc2->setConstString("[ATENÇÃO] A próxima tentativa será somente em 5 minutos");

		auto PainelInv = g_pInterface->Instance()->getGuiFromHandle<UIControl>(589832);
		PainelInv->IsVisible = false;

	}break;
	case _MSG_SendRanking:
	{
		auto p = reinterpret_cast<MSG_SendRanking*>(Packet);
		
		for (int i = 0; i < 50; i++) {
			strncpy(this->Ranking.RankName[i], p->RankName[i], sizeof(this->Ranking.RankName[i]) - 1);
			this->Ranking.RankName[i][sizeof(this->Ranking.RankName[i]) - 1] = 0;
			this->Ranking.RankLevel[i] = p->RankLevel[i];
			this->Ranking.RankClasse[i] = NormalizeRankingClass(p->RankClasse[i]);
			this->Ranking.RankEvolution[i] = NormalizeRankingEvolution(p->RankEvolution[i]);
			this->Ranking.PvP[i] = p->PvP[i];
		}
		this->Ranking.State = p->State == 1 ? 1 : 0;

		auto PainelRanking = g_pInterface->Instance()->getGuiFromHandle<UIControl>(1919400);

		char Evolution[5][15] = { "Mortal", "Arch", "Celestial", "CelestialCS", "SubCelestial" };
		char Classe[4][4] = { "TK", "FM", "BM", "HT" };

		if (false && p->State == 0) {
			auto Pos = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919401);
			Pos->setConstString("Pos");
			auto Name = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919402);
			Name->setConstString("Nick");
			auto Level = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919403);
			Level->setConstString("Level");
			auto Evo = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919404);
			Evo->setConstString("Evolução");
			auto Class = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919404);
			Class->setConstString("Classe");

			auto Pos1 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919410);
			auto Pos2 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919411);
			auto Pos3 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919412);
			auto Pos4 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919413);
			auto Pos5 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919414);
			auto Pos6 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919415);
			auto Pos7 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919416);
			auto Pos8 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919417);
			auto Pos9 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919418);
			auto Pos10 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919419);
			auto Page = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919420);

			ConfigR::RankingPage = 1;

			Page->setConstString("%d", ConfigR::RankingPage);
			Pos1->setConstString("1. [%s][%d][%s][%s]", this->Ranking.RankName[0], this->Ranking.RankLevel[0], Evolution[this->Ranking.RankEvolution[0] - 1], Classe[this->Ranking.RankClasse[0]]);
			Pos2->setConstString("2. [%s][%d][%s][%s]", this->Ranking.RankName[1], this->Ranking.RankLevel[1], Evolution[this->Ranking.RankEvolution[1] - 1], Classe[this->Ranking.RankClasse[1]]);
			Pos3->setConstString("3. [%s][%d][%s][%s]", this->Ranking.RankName[2], this->Ranking.RankLevel[2], Evolution[this->Ranking.RankEvolution[2] - 1], Classe[this->Ranking.RankClasse[2]]);
			Pos4->setConstString("4. [%s][%d][%s][%s]", this->Ranking.RankName[3], this->Ranking.RankLevel[3], Evolution[this->Ranking.RankEvolution[3] - 1], Classe[this->Ranking.RankClasse[3]]);
			Pos5->setConstString("5. [%s][%d][%s][%s]", this->Ranking.RankName[4], this->Ranking.RankLevel[4], Evolution[this->Ranking.RankEvolution[4] - 1], Classe[this->Ranking.RankClasse[4]]);
			Pos6->setConstString("6. [%s][%d][%s][%s]", this->Ranking.RankName[5], this->Ranking.RankLevel[5], Evolution[this->Ranking.RankEvolution[5] - 1], Classe[this->Ranking.RankClasse[5]]);
			Pos7->setConstString("7. [%s][%d][%s][%s]", this->Ranking.RankName[6], this->Ranking.RankLevel[6], Evolution[this->Ranking.RankEvolution[6] - 1], Classe[this->Ranking.RankClasse[6]]);
			Pos8->setConstString("8. [%s][%d][%s][%s]", this->Ranking.RankName[7], this->Ranking.RankLevel[7], Evolution[this->Ranking.RankEvolution[7] - 1], Classe[this->Ranking.RankClasse[7]]);
			Pos9->setConstString("9. [%s][%d][%s][%s]", this->Ranking.RankName[8], this->Ranking.RankLevel[8], Evolution[this->Ranking.RankEvolution[8] - 1], Classe[this->Ranking.RankClasse[8]]);
			Pos10->setConstString("10. [%s][%d][%s][%s]", this->Ranking.RankName[9], this->Ranking.RankLevel[9], Evolution[this->Ranking.RankEvolution[9] - 1], Classe[this->Ranking.RankClasse[9]]);			
		}
		if (false && p->State == 1) {
			auto Pos = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919401);
			Pos->setConstString("Pos");
			auto Name = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919402);
			Name->setConstString("Nick");
			auto PvP = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919403);
			PvP->setConstString("PvP");
			auto Evo = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919404);
			Evo->setConstString("Evolução");
			auto Class = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919404);
			Class->setConstString("Classe");

			auto Pos1 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919410);
			auto Pos2 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919411);
			auto Pos3 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919412);
			auto Pos4 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919413);
			auto Pos5 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919414);
			auto Pos6 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919415);
			auto Pos7 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919416);
			auto Pos8 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919417);
			auto Pos9 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919418);
			auto Pos10 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919419);
			auto Page = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(1919420);

			ConfigR::RankingPage = 1;

			Page->setConstString("%d", ConfigR::RankingPage);
			Pos1->setConstString("1. [%s][%d][%s][%s]", this->Ranking.RankName[0], this->Ranking.PvP[0], Evolution[this->Ranking.RankEvolution[0] - 1], Classe[this->Ranking.RankClasse[0]]);
			Pos2->setConstString("2. [%s][%d][%s][%s]", this->Ranking.RankName[1], this->Ranking.PvP[1], Evolution[this->Ranking.RankEvolution[1] - 1], Classe[this->Ranking.RankClasse[1]]);
			Pos3->setConstString("3. [%s][%d][%s][%s]", this->Ranking.RankName[2], this->Ranking.PvP[2], Evolution[this->Ranking.RankEvolution[2] - 1], Classe[this->Ranking.RankClasse[2]]);
			Pos4->setConstString("4. [%s][%d][%s][%s]", this->Ranking.RankName[3], this->Ranking.PvP[3], Evolution[this->Ranking.RankEvolution[3] - 1], Classe[this->Ranking.RankClasse[3]]);
			Pos5->setConstString("5. [%s][%d][%s][%s]", this->Ranking.RankName[4], this->Ranking.PvP[4], Evolution[this->Ranking.RankEvolution[4] - 1], Classe[this->Ranking.RankClasse[4]]);
			Pos6->setConstString("6. [%s][%d][%s][%s]", this->Ranking.RankName[5], this->Ranking.PvP[5], Evolution[this->Ranking.RankEvolution[5] - 1], Classe[this->Ranking.RankClasse[5]]);
			Pos7->setConstString("7. [%s][%d][%s][%s]", this->Ranking.RankName[6], this->Ranking.PvP[6], Evolution[this->Ranking.RankEvolution[6] - 1], Classe[this->Ranking.RankClasse[6]]);
			Pos8->setConstString("8. [%s][%d][%s][%s]", this->Ranking.RankName[7], this->Ranking.PvP[7], Evolution[this->Ranking.RankEvolution[7] - 1], Classe[this->Ranking.RankClasse[7]]);
			Pos9->setConstString("9. [%s][%d][%s][%s]", this->Ranking.RankName[8], this->Ranking.PvP[8], Evolution[this->Ranking.RankEvolution[8] - 1], Classe[this->Ranking.RankClasse[8]]);
			Pos10->setConstString("10. [%s][%d][%s][%s]", this->Ranking.RankName[9], this->Ranking.PvP[9], Evolution[this->Ranking.RankEvolution[9] - 1], Classe[this->Ranking.RankClasse[9]]);
		}

		ConfigR::RankingPage = 1;
		SetRankingPageText(ConfigR::RankingPage);
		RenderRankingPage(0, this->Ranking.State);

		// Abre o painel ao receber os dados (necessario para o NPC de ranking,
		// que dispara o sendRanking pelo servidor sem passar pelo botao).
		if (PainelRanking)
			PainelRanking->IsVisible = true;
		else
			OpenRankingFallbackPanel(this->Ranking.State);

	}break;
	case _MSG_MailItem:
	{
		auto MailButton = g_pInterface->Instance()->getGuiFromHandle<UIControl>(1212300);
		MailButton->IsVisible = false;

		auto p = reinterpret_cast<MSG_MailItem*>(Packet);
		this->MailItem.Index = p->Index;
		this->MailItem.sEffect[0] = p->sEffect[0];
		this->MailItem.sValue[0] = p->sValue[0];
		this->MailItem.sEffect[1] = p->sEffect[1];
		this->MailItem.sValue[1] = p->sValue[1];
		this->MailItem.sEffect[2] = p->sEffect[2];
		this->MailItem.sValue[2] = p->sValue[2];

		STRUCT_ITEM item;
		memset(&item, 0, sizeof(STRUCT_ITEM));
		item.sIndex = this->MailItem.Index;
		item.stEffect[0].cEffect = this->MailItem.sEffect[0];
		item.stEffect[0].cValue = this->MailItem.sValue[0];
		item.stEffect[1].cEffect = this->MailItem.sEffect[1];
		item.stEffect[1].cValue = this->MailItem.sValue[1];
		item.stEffect[2].cEffect = this->MailItem.sEffect[2];
		item.stEffect[2].cValue = this->MailItem.sValue[2];


		auto MailItem = g_pInterface->Instance()->getGuiFromHandle<UISlot>(121202);
		*(int*)((int)MailItem + 0x400) = 0x5;
		MailItem->deleteItemGui(0, 0);

		if (this->MailItem.Index != 0) {			
			MailButton->IsVisible = true;
			MailItem->setGui((int)MailItem->New(item, 0, 0), 0, 0);
		}
		

	}break;
	case _MSG_ServerInfos:
	{
		auto p = reinterpret_cast<MSG_ServerInfos*>(Packet);
		this->Server.Double = p->Double;
		this->Server.DropRate = p->DropRate;
		this->Server.ExpRate = p->ExpRate;

		auto Tx1 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(981403);
		if (this->Server.Double == 0) {
			Tx1->setConstString("DOUBLE DESATIVADO");
			Tx1->setTextColor(TNColor::Orange);
		}
		if (this->Server.Double == 1) {
			Tx1->setConstString("DOUBLE ATIVO");
			Tx1->setTextColor(TNColor::Speak);
		}

		auto Tx2 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(981404);
		Tx2->setConstString("Drop Bonus: %d", this->Server.DropRate);
		Tx2->setTextColor(TNColor::CornBlue);

		auto Tx3 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(981405);
		Tx3->setConstString("Exp Bonus: %d", this->Server.ExpRate);
		Tx3->setTextColor(TNColor::DeepPink);
	}break;
	case _MSG_SendDListNames:
	{
		auto p = reinterpret_cast<MSG_SendDListNames*>(Packet);

		ConfigR::DropListTpages = 1;
		int nMobs = 30;

		for (int i = 0; i < 30; i++)
		{
			strncpy(this->DropList.MobName[i], p->MobName[i], sizeof(this->DropList.MobName[i]) - 1);
			this->DropList.MobName[i][sizeof(this->DropList.MobName[i]) - 1] = 0;
		}

		for (int y = 0; y < 30; y++) {
			if (strcmp(p->MobName[y], "") == 0) {
				nMobs = y;
				break;
			}
		}

		ApplyRankingFallbackLayout();

		auto NewWindowTX1 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(179511);
		auto NewWindowTX2 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(179512);
		auto NewWindowTX3 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(179513);
		auto NewWindowTX4 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(179514);
		auto NewWindowTX5 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(179515);
		auto NewWindowTX6 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(179516);
		auto NewWindowTX7 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(179517);
		auto NewWindowTX8 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(179518);
		auto NewWindowTX9 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(179519);
		auto NewWindowTX10 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(179520);

		int page = (ConfigR::DropListPage * 10);

		NewWindowTX1->setConstString("%s", g_pClientInfo->DropList.MobName[page + 0]);
		NewWindowTX2->setConstString("%s", g_pClientInfo->DropList.MobName[page + 1]);
		NewWindowTX3->setConstString("%s", g_pClientInfo->DropList.MobName[page + 2]);
		NewWindowTX4->setConstString("%s", g_pClientInfo->DropList.MobName[page + 3]);
		NewWindowTX5->setConstString("%s", g_pClientInfo->DropList.MobName[page + 4]);
		NewWindowTX6->setConstString("%s", g_pClientInfo->DropList.MobName[page + 5]);
		NewWindowTX7->setConstString("%s", g_pClientInfo->DropList.MobName[page + 6]);
		NewWindowTX8->setConstString("%s", g_pClientInfo->DropList.MobName[page + 7]);
		NewWindowTX9->setConstString("%s", g_pClientInfo->DropList.MobName[page + 8]);
		NewWindowTX10->setConstString("%s", g_pClientInfo->DropList.MobName[page + 9]);
		
		if (nMobs >= 11)
			ConfigR::DropListTpages = 2;

		if (nMobs >= 21)
			ConfigR::DropListTpages = 3;

		auto PageTx = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(179521);
		PageTx->setConstString("%d/%d", (ConfigR::DropListPage + 1), ConfigR::DropListTpages);
	}break;
	case _MSG_UpdateDropList: {
		auto p = reinterpret_cast<MSG_UpdateDropList*>(Packet);

		int Slot = 0;

		for (int i = 0; i < 64; i++) {
			this->DropList.Item[i] = 0;

			bool exist = false;

			if (p->Item[i] == 0)
				continue;

			for (int y = 0; y < 64; y++) {
				if (p->Item[i] == this->DropList.Item[y])
					exist = true;
			}

			if (exist)
				continue;

			this->DropList.Item[Slot] = p->Item[i];
			Slot++;
		}

		STRUCT_ITEM item;
		memset(&item, 0, sizeof(STRUCT_ITEM));

		auto DropListGrid = g_pInterface->Instance()->getGuiFromHandle<UISlot>(179600);
		*(int*)((int)DropListGrid + 0x400) = 0x5;
		DropListGrid->deleteItems();

		for (int i = 0; i < 64; i++) {
			item.sIndex = this->DropList.Item[i];

			if (item.sIndex == 0)
				continue;

			DropListGrid->setGui((int)DropListGrid->New(item, 0, 0), i % 8, i / 8);
		}
	} break;
	case _MSG_UpdateDonate:
	{
		auto p = reinterpret_cast<MSG_UpdateDonate*>(Packet);

		this->Info.Cash = p->Cash;
		if (this->Info.Cash < 0)
			this->Info.Cash = 0;

		strncpy(this->Info.Pix, p->Pix, sizeof(this->Info.Pix));

		auto DonateInv = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(250099);
		auto PixTx4 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(188517);
				
		DonateInv->setConstString("Diamantes: %d", g_pClientInfo->Info.Cash);
		DonateInv->setTextColor(TNColor::CornBlue);

		PixTx4->setConstString("Diamantes: %d", g_pClientInfo->Info.Cash);
		PixTx4->setTextColor(TNColor::CornBlue);

		auto PixTx3 = g_pInterface->Instance()->getGuiFromHandle<UITextControl>(188516);
		PixTx3->setConstString("Sua Chave: %s", g_pClientInfo->Info.Pix);
		PixTx3->setTextColor(TNColor::CornBlue);
	}break;
	default:
		break;
	}
}
