# HANDOFF — Estado do código-fonte TMSrv/ClientPatch (LEIA ANTES DE MEXER/COMPILAR)

Última atualização: 2026-06-18.

## 🚨🚨 AVISO PRINCIPAL PRO CODEX — NÃO PULE ETAPAS 🚨🚨
O erro recorrente foi: **atualizar/deployar o servidor OU o site PULANDO etapas e SEM preservar
as alterações já feitas** — recompilando/rebuildando de um estado incompleto, revertendo
arquivos, ou esquecendo de incluir as features acumuladas → o servidor/site voltava sem as mudanças.

**ANTES de atualizar o servidor (TMSrv/ClientPatch) ou o site, OBRIGATÓRIO:**
1. **Sincronize o git primeiro** (`git pull`/`fetch`) e trabalhe do **working tree atual**. NUNCA
   reverter arquivos (`git checkout/reset/stash/clean`) nem usar cópia antiga.
2. **Ao recompilar o TMSrv ou rebuildar o site, confira que TODAS as features deste documento
   continuam no código** (lista na seção "Alterações recentes"). Não deployar um build que perdeu features.
3. **Não pule passos** do empacotamento/deploy (build → empacotar → aplicar na VPS / push do site).
4. **Commite e pushe DEPOIS** de cada mudança (servidor e site) — não deixar acumular uncommitted.
5. Em caso de dúvida se algo já está no build: comparar com este HANDOFF e com
   `RELATORIO-GERAL-PROBLEMAS-RESOLVIDOS.md` ANTES de subir.

## ✅ ESTADO DO GIT (atualizado 2026-06-18)
- **RESOLVIDO:** antes o HEAD era de 2025 e tudo estava uncommitted. **Agora o código está
  COMMITADO e PUSHADO** no GitHub: `NewWorld` (commit `9dbe6ac`, branch `main`, remote
  `github.com/bortolinfotografo-alt/NewWorld`) e `wydkingdom-site` (`6010e08`). Ambos em sincronia.
- Ou seja, hoje um `git checkout`/`reset` restaura o **código ATUAL** (não mais 2025). O risco
  de perder tudo foi eliminado.
- **REGRA contínua:** depois de mexer/compilar, **commitar as mudanças** (não deixar acumular
  uncommitted como antes). Compilar sempre do working tree atual.
- ⚠️ O NewWorld **não tem `.gitignore`** e tem ~10.656 arquivos UNTRACKED (lixo de build:
  `runtime/build`, `obj`, `.tlog`, deploy zips, etc.) — **NÃO** rodar `git add .` (commitaria
  gigabytes de lixo). Para commitar código, usar `git add -u` (só os tracked) ou adicionar
  arquivos específicos. (Criar um `.gitignore` no futuro ajudaria.)

## Como compilar (mantendo tudo)
**TMSrv** (servidor):
- MSBuild: `C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe`
- `MSBuild Source\Code\TMSrv\TMSrv.vcxproj /p:Configuration=Release /p:Platform=Win32 /m`
- Toolset: o Release usa **v145** (já no .vcxproj). Saída: `Source\Code\TMSrv\_production\TMSrv.exe`.
- Deploy: parar TMSrv/DBSrv, trocar `Server\TMSrv\run\TMSrv.exe`, reiniciar (`start-newworld-vps.ps1`).

**ClientPatch** (cliente, ClientPatch.dll):
- Mesmo MSBuild, `ClientPatch_v759.vcxproj /p:Configuration=Release /p:Platform=Win32 /p:PlatformToolset=v145`
  + **OutDir/IntDir redirecionados** (o .vcxproj aponta OutDir pra `C:\Program Files (x86)\Wyd Kingdom - Copia\`
  que dá Acesso Negado — usar `/p:OutDir=<pasta-local>\\` e `/p:IntDir=<...>\obj\\`, com `\\` no fim por causa do espaço no path).
- Deploy: vira patch via `publicar-patch.ps1 -Version X.Y.Z -Files "ClientPatch.dll"` (lê de `Cliente\ClientPatch.dll`),
  depois `apply-wydkingdom-patch.ps1` na VPS. Versão atual do patch: **1.1.22** (subir a próxima).

## ⚠️ ENCODING (corrompe acentos e quebra o build se errar)
Cada .cpp tem encoding próprio (NÃO é uniforme):
- `_MSG_Quest.cpp` = **UTF-8** (com BOM); `Server.cpp`, `ProcessSecMinTimer.cpp`, `MobKilled.cpp`(ASCII),
  `RequestTeleport.cpp`, `CClientInfo.cpp`, `ButtonControl.cpp` = **ANSI/1252 ou ASCII**.
- **NÃO usar a ferramenta Edit nem `[IO.File]::ReadAllText`** nesses arquivos: o ReadAllText **detecta BOM**
  e relê UTF-8, e ao reescrever **corrompe ~40 acentos** (já aconteceu no _MSG_Quest, recuperado via git).
- **Método à prova:** tratar como **bytes crus** —
  `$enc=[Text.Encoding]::GetEncoding(28591); $s=$enc.GetString([IO.File]::ReadAllBytes($f)); ...; [IO.File]::WriteAllBytes($f,$enc.GetBytes($s))`
  (mapeia 1:1 por byte, sem detecção de BOM). Manter o código adicionado em **ASCII puro** (sem acentos).
- Conferir sempre: a contagem de bytes não-ASCII tem que ficar IGUAL antes/depois da edição.

## Alterações recentes já no working tree (NÃO remover ao recompilar)
Detalhes completos em `RELATORIO-GERAL-PROBLEMAS-RESOLVIDOS.md` e `RELATORIO-EVENTOS-E-DROPS-PISTA.md`.

**TMSrv (servidor):**
1. **Lojinha (auto trade) fix** — `Server.cpp`: função `ClearAutoTradeStoreState()` chamada em
   `CloseUser`, `CharLogOut` e `RemoveTrade` (loja não fica presa ao trocar de personagem). Comando `/closeloja`.
2. **Ranking sem admin** — `Ranking.cpp`: as 2 queries de `sendRanking` têm
   `WHERE nick NOT LIKE '-ADM-%%' AND ... '[GM]%%'` (o `%` É `%%` por causa do sprintf — NÃO trocar pra `%`).
3. **Drop de pedras reduzido** — `MobKilled.cpp`: `switch(Carry[i].sIndex){ case 1744..1759: droprate = droprate*200/100; }`
   (16 pedras de evolução, ~50% menos drop).
4. **Karden/Nippleheim desativado** — flag `#define NIPPLEHEIM_HABILITADO 0` em `Functions.h`.
   Bloqueios `#if !NIPPLEHEIM_HABILITADO` em: `RequestTeleport.cpp` (região), `_MSG_Quest.cpp` (JULIARZAN),
   `_MSG_UseItem.cpp` (pergaminho 3437) e `Server.cpp::DoTeleport` (redireciona pra Armia — pega-tudo).
   **Reabilitar = trocar a flag pra 1.** (Ver [[newworld-karden-desativar]].)
5. **Double XP automático fim de semana** — `ProcessSecMinTimer.cpp` (região Boss Time): liga sáb 00:00, desliga seg 00:00.
6. **Kefra** — `ProcessSecMinTimer.cpp`: spawn corrigido (guard `g_KefraLastSpawnYday`, nasce toda terça 19h).
   `MobKilled.cpp`: removidas as 3 penalidades `if(KefraLive==0) exp/=2` (XP não cai mais pela metade).
7. (Trabalho anterior do codex também está aqui: online_count em `ProcessSecMinTimer.cpp`, `ExpBaseN/M` em `Server.cpp`,
   guild/guildmark, droplist UI, evocações `CMob.cpp`, etc. — TUDO uncommitted, manter.)

**ClientPatch (cliente):**
1. **/comandos centralizado** — `ButtonControl.cpp`: `ApplyCommandsTextCenteredLayout(230)` no `OpenCommandsPanel`.
2. **Ranking centralizado, Drop List nos botões** — `CClientInfo.cpp`: DUAS funções —
   `ApplyRankingFallbackLayout` (ancora nos botões = usada pelo Drop List, `_MSG_SendDListNames`) e
   `ApplyRankingCenteredLayout` (centraliza = usada pelo Ranking, `OpenRankingFallbackPanel`).
   **Não unificar essas duas** — foi exatamente o bug que quebrou o Drop List. Título do ranking (179521) centralizado junto.

## Dados/runtime alterados (fora do código — não afetados por recompilar, mas manter)
- **npc binários** (`Server\TMSrv\run\npc\`, Carry no offset 460+): `Homem_Kalintz`/`Mulher_Kalintz` (drops do Ent_Anciao),
  `Martin_`/`Parche` (pergaminho 3437 removido), e a remoção de drops Andaluz/Unicórnio em vários mobs.
- **SQL** (`Server\banco.sql`, `Server\wydsite-dump.sql`): tabela `droplist` (Kalintz, remoção Andaluz/Unicórnio).
- Pacotes de entrega gerados em `runtime\ENTREGAR-VPS\` (o mais recente e completo: **KEFRA-FIX-20260618**, que tem o TMSrv com TUDO).

## ⚠️ Histórico: patches binários (verificar após recompilar)
Em 2026-06-06 havia um aviso de que o **TMSrv.exe em uso tinha PATCHES BINÁRIOS que não estavam
no source** (ex.: ativação de admin `/tnw`, coveiro max115). Desde então o TMSrv foi recompilado
do source e deployado várias vezes (lojinha, ranking, pedras, Karden, Double XP, Kefra) e os recursos
funcionaram — indício de que o source já está completo. **Mesmo assim, ao recompilar, TESTAR:**
ativação de admin (`/tnw adm@#2023` — ver [[newworld-comandos-gm]]) e qualquer comando GM crítico.
Se algo de admin parar, o patch binário precisa ser re-aplicado OU portado pro source. Backups do
exe antigo: `Server/TMSrv/run/TMSrv.exe.bak_*`.

## Resumo: para o Codex manter tudo
1. **Compilar do working tree atual** (não reverter nada via git).
2. Editar .cpp com o **método byte-safe (28591)**, código adicionado em ASCII.
3. Não unificar `ApplyRankingFallbackLayout`/`ApplyRankingCenteredLayout`; manter `%%` no Ranking.cpp; manter a flag `NIPPLEHEIM_HABILITADO`.
4. Idealmente, **commitar** para ter ponto de restauração (hoje o HEAD é de 2025).