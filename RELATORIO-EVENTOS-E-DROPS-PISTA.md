# Relatório de Eventos e Drops da Pista — WYD Kingdom (NewWorld)

Levantado do código-fonte do TMSrv em 2026-06-18. Horários em hora do servidor (VPS).
Dia da semana no código: 0=Domingo, 1=Segunda, ... 6=Sábado.

---

## 1. Guerra de Cidades (Guild War / "Guerra de Cidade")
**Quando:** todo **DOMINGO** às **20h** (`GuildHour = 20`, fixo em `Server.cpp:533`).
Sequência (uma cidade por vez, ~10 min cada):
| Cidade       | Aviso (3 min) | Inicia | Encerra |
|--------------|---------------|--------|---------|
| Erion        | 20:02         | 20:05  | 20:15   |
| Nipplehein   | 20:17         | 20:20  | 20:30   |
| Arzan        | 20:32         | 20:35  | 20:45   |
| Armia        | 20:47         | 20:50  | 20:59   |

- 20:00 → aviso geral "esperem nas cidades".
- **Apostas:** liberadas no **sábado**; desafiantes definidos no **domingo** antes da guerra.
- Código: `Server.cpp` função `GuildProcess()` (região "Guerra de Cidades"), `WeekMode`/`SummonGuild`.
- ⚠️ Não há guerra de cidade para Noatum nesta rotina (Noatum tem o castelo, abaixo).

## 2. Guerra de Noatum (Castelo / Torres de Noatum)
**Quando:** **DOMINGO** às **14h** (`CastleServer` é ligado no domingo em `ProcessSecMinTimer.cpp`; roda quando `tm_hour == 14`).
- ~14:05 castelo **abre** (spawn das torres `TORRE_NOATUM`, portas via `SetCastleDoor`).
- ~14:50 aviso de fechamento; ~14:55 **fecha** (`FinishCastleWar`).
- Código: `Server.cpp` `GuildProcess()` região "Guerra de Noatum".

## 3. Torre de Guerra (GTorre / "Guerra e Torre")
**Quando:** **dias úteis** (Seg–Sex; não roda Dom/Sáb), na hora **`GTorreHour`**.
- `GTorreHour` é **configurável** (lido do banco `wydsite.serverconfig` e/ou `gameconfig`; valor histórico no código = **21h**). Para ver o valor atual, checar `serverconfig` na VPS.
- :00 → aviso (5 min p/ começar); :05 → inicia (spawn do mob `GTORRE`); :35 → encerra (guilda que segura a torre ganha **+100 de fama**).
- **Só roda se** `NewbieEventServer == 1` **e** `GTorreStatus == 1`.
- Código: `CWarTower.cpp` `GuildProcess()`.

## 4. Coliseu
**Quando:** **dias úteis** (Seg–Sex), horas fixas (`Server.cpp`):
| Coliseu | Hora |
|---------|------|
| M (Mortal)   | 02:00 (`ColoHourM = 2`) |
| A (Arch)     | 03:00 (`ColoHourA = 3`) |
| N (Celestial)| 04:00 (`ColoHourN = 4`) |
- Código: `CColiseuM.cpp` / `CColiseuA.cpp` / `CColiseuN.cpp` (`tm_wday != 0 && != 6 && tm_hour == ColoHour`).
- ⚠️ Horários 2–4h da manhã (conferir se é o desejado; são fixos no código).

## 5. Batalha Real
**Quando:** **Seg–Sáb** (não domingo), registro abre a cada hora no minuto :00.
- **Só roda se** `BatalhaRealStatus == 1`.
- Avisos de contagem (180/120/60s) e início automático; cancela por falta de jogadores.
- Código: `ProcessSecMinTimer.cpp` (região "Start Batalha Real").

## 6. Bosses (renascimento automático)
| Boss              | Dias        | Horário |
|-------------------|-------------|---------|
| Kefra             | Terça       | 19:00   |
| Barão (Baron)     | Seg–Sex     | 20:00   |
| Ent Obscuro       | Seg–Sex     | 20:00   |
| Sombra Negra      | Seg–Sex     | 00, 04, 08, 12, 16, 20h (de 4 em 4h) |
| Mirror Queen (Rainha) | Seg–Sex | 19:40   |
- Código: `ProcessSecMinTimer.cpp` (blocos por `tm_wday`/`tm_hour`). Há outros bosses comentados/desativados (Verid Negro, Espelho RainhaG de Karden).

### Kefra (detalhe) — terça 19:00
- **Spawn:** corrigido em 2026-06-18. Antes tinha um gate `&& KefraLive` que falhava quando `KefraLive==0` (zera em todo restart) → Kefra não nascia. Agora nasce **toda terça 19h** de forma confiável (guard por dia `g_KefraLastSpawnYday`), com aviso global. Spawna `KEFRA_BOSS` + mobs `KEFRA_MOB_INITIAL..END`.
- **Drops do boss** (sorteados e espalhados pela arena ~2335-2395 / 3896-3955): `413` (Poeira de Lactolerium), `1740` (Alma do Unicórnio), `1741` (Alma da Fênix), `1760`, `1761`, `1762`, `1763`, `3172`, `3140` (Pedra da Luz).
- **Recompensa por matar:** a guild que mata ganha **+100 fama**; e o servidor ganha **+10 de XP** (bônus em `SendFunc.cpp`, ativo enquanto `KefraLive != 0`, ou seja, depois de morto até o próximo spawn).
- **Penalidade XP/2 REMOVIDA (2026-06-18):** antes, enquanto o Kefra não era morto (`KefraLive==0`), TODO o XP de mob do servidor caía pela metade (`exp /= 2` em `MobKilled.cpp`) — e isso ficava ativo "escondido" após cada restart. Removido a pedido: agora o XP é normal sempre; matar o Kefra só ADICIONA o +10 de bônus.

## 7. Double XP (fim de semana) — implementado em 2026-06-18
**Quando:** liga **sábado 00:00**, desliga **segunda 00:00** (fim de semana inteiro). Avisa todos. Liga/desliga manual continua nos outros dias (`/gm +set double 1/0`). Código: `ProcessSecMinTimer.cpp`. Ver [memória newworld-taxas-xp-drop-eventos].

## 8. Evento Novato (Newbie)
Ativado por servidor conforme o dia do mês (`NewbieServerID = (tm_mday-1) % NumServerInGroup`). Mensagens de boas-vindas / "/novato".

---

## Drops dos mobs da PISTA (RuneQuest)
A "Pista" é a RuneQuest: **Pista +1 = Lugefer_Inf**, **Pista +2 = Amon_Inf** (mobs renascem ao morrer dentro da pista). Os drops saem do **Carry** de cada mob (offset 460+ no binário do npc). IDs abaixo (nomes conhecidos entre parênteses; demais ficam só o ID):

| Mob (npc)        | Itens que dropa (IDs) |
|------------------|------------------------|
| Lugefer_Inf      | 419, 420, 578(P.Adamantita), 4127, 670, 2570 |
| Lugefer_Inf_     | 419, 578(P.Adamantita), 4127, 670, 2570 |
| Cav._Lugefer     | 810, 825, 855, 1349, 1649, 1214, 1208, 1493, 1343, 2444(Garnet), **1758(Pedra do Lugefer)**, 1740(Alma do Unicornio), 2827 |
| Chefe_Amon_Inf   | 578(P.Adamantita), 3140(Pedra da Luz), 419, 420, 4127 |
| Capitao_Amon     | 419, 4020, 413(Poeira Lactolerium) |
| Soldado_Amon     | 4020, 3140(Pedra da Luz), 419, 3855 |
| Guerreiro_Amon   | 413(Poeira Lactolerium) |
| Lich_Infernal    | 2441(Diamante), 2442(Esmeralda), 2443(Coral), 2444(Garnet), 1710, 840, 810, 870, 885, 911, 855, 825, 419/420, 578(Adamantita), 4127, 413/412(Poeiras), 668, 669, 670, 671, 5140 |
| Batorero_Inf     | 4127, 3140(Pedra da Luz), 578(Adamantita), 670, 2570 |
| Gargula_Inf      | 591, 592, 593, 594, 595, 4122, 2796 |
| Aranha_Inferno   | 420, 412(Poeira Oriharcon), 1285 |
| Lagarto_Inferno  | 612, 419, 420, 4019, 412/413(Poeiras), 3940, 3855 |
| Zakum_Inf / Zakum_Inf_ | 3140(Pedra da Luz), 419, 420, 2570 |
| Simio_Inf        | 4108, 2570 |
| Taurus_Maligno   | 4080 |
| Mago_Inf / Ranger_Inf / Shaman_Inf / Templario_Inf | 578(Adamantita), 4127 |
| Lugefer / Lugefer_ / Lugefer_Maligno / Lugefer__ | vários (697, 2441 Diamante, 900, 810, 840, 855, 870, 885, 902, 911, 936, 1712, 668, 578, 420, 3855, 671) |

Notas:
- Itens 419/420 são consumíveis/poeira comuns; 578 = Pedra de Adamantita; 2441–2444 = Diamante/Esmeralda/Coral/Garnet; 3140 = Pedra da Luz; 412/413 = Poeira de Oriharcon/Lactolerium; 1740–1742 = Alma do Unicórnio/Fênix/Pedra da Imortalidade.
- As pedras de evolução de boss (1752–1759, ex.: 1758 do Cav. Lugefer) tiveram o drop reduzido ~50% (×2.0) em 2026-06-18 — ver [memória newworld-droplist-sistema].
- IDs sem nome podem ser decodificados depois pelo `ItemName.bin` se precisar.

### Como conferir/alterar
- Horários de guerra/coliseu: `GuildHour` (fixo 20), `ColoHour*` (fixos 2/3/4), `GTorreHour` (config no banco). `GuildProcess`/`CWarTower`/`CColiseu*`.
- Drops da pista: Carry no binário do npc (`Server/TMSrv/run/npc/<Mob>`, offset 460+) — ver [memória newworld-droplist-sistema].