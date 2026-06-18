# Relatorio geral - problemas resolvidos NewWorld

Atualizado em: 2026-06-16

Este arquivo e o indice central para consultar problemas ja resolvidos no projeto.
Antes de mexer em um sistema que ja foi corrigido, procurar o nome aqui e abrir
os arquivos citados em "Onde conferir".

Formato usado em cada caso:

- Sintoma: o que aparecia no jogo ou no cliente.
- Causa: o ponto tecnico encontrado.
- Solucao: o que foi alterado.
- Onde conferir: arquivos, backups e relatorios especificos.
- Validacao: como confirmar que continua correto.

## Indice rapido

- Tela de selecao: nomes do servidor e canais.
- Painel D: botao Comandos, layout e ESC voltando para janela anterior.
- /macroperga: macro dos pergaminhos da quest da Agua.
- Quest do Coveiro: visual, composicao, Jephi e limite de level.
- Evento Hit: Passe do Evento controla double hit e agrupamento.
- Montaria adulta: vida do item sincronizada com dano recebido.
- Rebranding visual: titulo da janela, tela de login (logos removidos) e rodape.
- Arte do login (.wyt): por que nao da pra trocar a imagem (e como removo).
- Painel D: botao Modo Foto ligado (= tecla U) e mapa de handles dos botoes.
- Sistema da Agua: teleporte clicando no pergaminho fora d'agua (sem consumir) e pack.
- Double XP: anuncio no servidor ao ligar/desligar + painel de eventos (PowerShell).
- Taxas de XP e Drop do servidor (onde ficam e o que cada uma faz).
- Acesso remoto: jogar de outra casa via Radmin VPN.
- Conta, personagem e admin: regras de login, bug ENOENT de pastas e /wydkingdom on.
- Toolchain de compilacao (cliente/servidor) e armadilhas de encoding.
- Documentacao de apoio criada (comandos-adm/usuario, Painel D, painel-eventos).
- Helriohdon (item 346): drop removido de todos os mobs.
- Refino +12 de acessorio: brinco/pingente/anel sumindo no NPC Odin (corrigido).
- Poeira de Fada Celestial (item 5600): level-up de celestial, loja BETA A e icone.
- Hidra Dourada: brilho dourado de verdade (aura EF_SANC no struct do mob).
- Destrave celestial Lv90 e comando /gm +set destravecel (XP travado em 0).
- Lojas dos reinos e BETAs: Mithril, Barra de Prata (preco), Refinacao Abencoada, Emblema.
- Pocao Revigorante: montaria invulneravel a dano E fome (corrigido).
- Balanceamento das classes: ajustes V3, telemetria e automacao dos testes.
- Launcher WYD Kingdom: criado, compilado e publicado no cliente.
- Sistema de Drop: o painel D (tecla D) e fiel? e mapa de TODAS as fontes de drop.
- Servidor publico na VPS (Battlehost) e ferramentas de deploy.
- Admin por CONTA (nao mais por IP): admin-accounts.txt / IsAdminAccount.
- Site WYD Kingdom no ar (Cloudflare) + cadastro/login/painel reais.
- Backend de contas (api-backend, Node) na VPS: cadastro, login, troca senha/e-mail.
- DBSrv: troca de senha e contas novas reconhecidas sem reiniciar o servidor.
- Site: Discord substituido pelo grupo de comercio no WhatsApp.
- Crash do TMSrv ao matar mob (Mortal/Arch Lv256+): ExpBaseN/M zerados (divisao por zero).
- Caixas/Bolsas aleatorias (Bolsa da Sorte 4104 etc.): tabela de premios -> RELATORIO-CAIXAS-ALEATORIAS.md.
- Guildmark no site, API e cliente: upload do lider da guild e patch 1.1.9 confirmado no jogo.
- Recompensa de level do cavalo: Cavalo Leve N/B trocado por escolha Cavalo s/Sela N/B.
- Cliente antigo preso conectando: reparo automatico de duplo clique para jogador.
- Anuncio global de drop: Alma do Unicornio e Alma da Fenix.
- Launcher 1.1.12: botao JOGAR passa a abrir `WYD.exe` com permissao de administrador.
- Reparo ClientPatch/Admin V3 publicado no site para cliente que abre como `Wyd Gc 7662`.

## 2026-06-16 - Reparo V3 para cliente abrindo como Wyd Gc 7662

Sintoma:

- Mesmo com launcher atualizado para `1.1.12`, um jogador clicava em JOGAR e o
  cliente abria como `Wyd Gc 7662`, sem carregar `ClientPatch.dll`.
- O launcher detectava o problema e avisava que o jogo abriu sem carregar o
  `ClientPatch.dll`.

Causa:

- A atualizacao do launcher chegou, mas a instalacao local do jogador ainda tinha
  `WYD.exe`/`ClientPatch.dll` incorretos, bloqueados ou em outra pasta de jogo.

Solucao:

- Criado reparo auto-contido `REPARAR-WYD-KINGDOM-CLIENTPATCH-V3.bat`.
- O reparo localiza a instalacao real do jogo por atalhos, processos e pastas
  comuns, troca `WYD.exe`, `ClientPatch.dll` e os binarios do launcher `1.1.12`,
  cria backup, adiciona exclusoes do Defender, marca `WYD.exe` como administrador
  e recria o atalho `WYD Kingdom - Launcher`.
- ZIP de entrega para jogador:
  `runtime/ENTREGAR-CLIENTE/REPARO-CLIENTPATCH-ADMIN-V3-20260616/`.
- Site oficial atualizado: o botao de reparo em Downloads agora aponta para
  `/downloads/WYD-Kingdom-Reparo-ClientPatch-Admin-V3-20260616.zip`.

Validacao:

- Payload interno do `.bat` foi extraido em teste e contem `WYD.exe`,
  `ClientPatch.dll`, launcher `1.1.12` e `launcher-config.json`.
- `auto-install-v3.ps1` passou no parser do PowerShell.
- Build do site passou com `npm run build`.
- Deploy no Cloudflare Pages concluido.
- URL direta do ZIP retornou `200`, `application/zip`, tamanho `1290642`.
- Home do site contem o link V3 e nao contem mais o link V2.
- SHA-256 do ZIP: `73BE0062CF6D17B6896CE1C8B967772097370C2CCD9A41A4D9E16AE2AA54CD64`.

## 2026-06-16 - Launcher abre cliente sem ClientPatch no PC de jogador

Sintoma:

- No PC de um jogador, clicar em JOGAR abria o cliente com titulo antigo
  (`Wyd Gc 7662`) e depois o launcher acusava que o jogo abriu sem carregar
  `ClientPatch.dll`.
- No PC do dono, o mesmo clique mostrava a permissao do Windows para `WYD.exe`
  e o cliente abria com o nome correto (`Wyd Kingdom`).

Causa:

- O fluxo antigo dependia do Windows/manifest do `WYD.exe` para pedir elevacao.
  Em alguns computadores o jogo abria sem permissao de administrador, o
  `ClientPatch.dll` nao carregava e o cliente ficava preso na conexao.

Solucao:

- Launcher atualizado para `1.1.12`.
- O botao JOGAR agora inicia o `WYD.exe` com `Verb = "runas"`, forçando a janela
  de permissao do Windows.
- A mensagem de erro foi ajustada para orientar o jogador a clicar em SIM na
  permissao do Windows.

Onde conferir:

- `Launcher/MainWindow.xaml.cs`
- `Launcher/WydKingdom.Launcher.csproj`
- `Launcher/Services/LauncherConfig.cs`
- Entrega VPS: `runtime/ENTREGAR-VPS/LAUNCHER-ADMIN-JOGAR-1.1.12-20260616/`

Validacao:

- `dotnet publish` gerou os binarios do launcher `1.1.12.0`.
- Patch local `WYD-Kingdom-Patch-1.1.12.zip` criado com os 4 binarios do launcher.
- `COMANDO-POWERSHELL-VPS.txt` passou no parser do PowerShell.
- SHA-256 do ZIP: `2D10EDCB237BC8676D84EE4B6B1F1BC70C1FD3EAC5FCCF5CF347DE47233D9B85`.

## 2026-06-16 - Anuncio global de drop das Almas

Sintoma:

- O servidor nao avisava todos os jogadores quando alguem dropava itens raros de evolucao:
  `Alma do Unicornio` e `Alma da Fenix`.

Causa:

- O `DropControl.cpp` ja tinha os IDs `1740` e `1741` nas listas de filtro/log, mas o aviso global estava comentado.

Solucao:

- Ativado anuncio global no fluxo central `ControleDropItem`, depois da entrega do item pelo agrupador automatico ou pelo `PutItem`.
- IDs anunciados:
  - `1740`: Alma do Unicornio.
  - `1741`: Alma da Fenix.
- Mensagem enviada:
  `[DROP RARO] NomeDoPlayer dropou Alma do Unicornio/Fenix!`

Onde conferir:

- `Source/Code/TMSrv/DropControl.cpp`
- `runtime/ENTREGAR-VPS/ANUNCIO-DROP-ALMAS-20260616/`

Validacao:

- `TMSrv.vcxproj` compilou Release Win32 sem erros.
- Entrega limpa criada com `WYD-Kingdom-Anuncio-Drop-Almas-20260616.zip`.
- `INSTALAR-NA-VPS.ps1` e `COMANDO-POWERSHELL-VPS.txt` passaram no parser do PowerShell.
- SHA-256 do ZIP: `3A9BF14354693A87439E0C1E9542A6B64EE3BD60E8D70C8E239973805E0F43D9`.

## 2026-06-16 - Cliente antigo preso conectando

Sintoma:

- Em outro computador, a janela aparecia como `Wyd Gc 7662` em vez de `Wyd Kingdom`.
- A tela de selecao mostrava `The / Kingdom` ou canal vazio.
- Depois de digitar login e senha, ficava apenas conectando e nao entrava.

Causa:

- O computador estava usando cliente antigo, pasta errada, launcher antigo ou `ClientPatch.dll` sem atualizar/carregar.
- O cliente correto exibe `Wyd Kingdom` no titulo e aplica os hooks do `ClientPatch.dll`.

Solucao:

- Criado reparo automatico de duplo clique `REPARAR-WYD-KINGDOM.bat`.
- O `.bat` e autocontido: extrai os arquivos centrais, procura a pasta do jogo, fecha processos antigos, faz backup, copia arquivos atuais, cria atalho e abre o launcher.
- Payload inclui `WYD.exe`, `ClientPatch.dll`, `serverlist.bin`, launcher, configs e arquivos visuais/texto centrais.
- V2 criada apos caso em que o launcher abriu certo, mas mostrou `O jogo abriu sem carregar o ClientPatch.dll`.
- V2 `REPARAR-WYD-KINGDOM-ADMIN.bat` pede UAC, roda como administrador, desbloqueia arquivos, adiciona exclusao no Windows Defender, cria regras de firewall, marca `WYD.exe`/launcher como administrador e inclui DLLs/BINs raiz como `PS.dll`, `JProtectDll.dll`, `JPGLib.dll` e similares.
- Site atualizado com botao `Baixar reparo do cliente` logo abaixo do download do instalador oficial.
- O ZIP do reparo V2 foi publicado em `/downloads/WYD-Kingdom-Reparo-Admin-Defender-V2-20260616.zip`.

Onde conferir:

- `runtime/ENTREGAR-CLIENTE/REPARO-AUTOMATICO-WYD-20260616/`
- `runtime/ENTREGAR-CLIENTE/REPARO-ADMIN-DEFENDER-V2-20260616/`
- `runtime/build/reparo-automatico-cliente-20260616/auto-install.ps1`
- `runtime/build/reparo-admin-defender-cliente-v2-20260616/auto-install-v2.ps1`
- `wydkingdom-site/components/DownloadsSection.jsx`
- `wydkingdom-site/public/downloads/WYD-Kingdom-Reparo-Admin-Defender-V2-20260616.zip`

Validacao:

- Payload interno do `.bat` foi extraido e listado com sucesso.
- `auto-install.ps1` passou no parser do PowerShell sem erro de sintaxe.
- SHA-256 do ZIP: `42BD8B4CC441BF7B950A09AAA51E3BCD90ADA867AE6590113E92DD6190773CB3`.
- V2: payload interno validado com `ClientPatch.dll`, `WYD.exe`, `PS.dll` e `JProtectDll.dll`.
- `auto-install-v2.ps1` passou no parser do PowerShell sem erro de sintaxe.
- SHA-256 do ZIP V2: `C1CAFF76F5FCC2416C99A633DE984BF39C787509E98F64040225C0DE1B35B752`.
- `npm run build` do `wydkingdom-site` passou sem erros.
- Deploy Cloudflare Pages concluido no projeto `wydkingdom-site`.
- Dominio `https://wydkingdom.com.br/` retornou `200` com o botao e a URL do reparo no HTML.
- Link direto do ZIP retornou `application/zip` e tamanho `24190054` bytes.

## 2026-06-16 - Guildmark no site, API e cliente

Sintoma:

- O jogador lider da guild precisava enviar a imagem da guildmark pelo site.
- O upload era aceito no painel, mas a imagem nao aparecia no jogo porque o cliente buscava em `/img_guilds/`.

Causa:

- O site/API salvavam os BMPs em `https://api.wydkingdom.com.br/guildmarks/`.
- O `ClientPatch.dll` ainda montava a URL antiga `https://wydkingdom.com.br/img_guilds/`.

Solucao:

- Criada pagina `https://wydkingdom.com.br/conta/guildmark/` dentro da conta.
- O painel so libera envio para lider de guild.
- Criado redirect Cloudflare Pages de `/img_guilds/*` para `https://api.wydkingdom.com.br/guildmarks/:splat`.
- Atualizado o `ClientPatch.dll` para buscar direto em `https://api.wydkingdom.com.br/guildmarks/`.
- Publicado patch do launcher `1.1.9` com `ClientPatch.dll`.

Onde conferir:

- `wydkingdom-site/app/conta/guildmark/page.jsx`
- `wydkingdom-site/app/conta/page.jsx`
- `wydkingdom-site/lib/api.js`
- `wydkingdom-site/public/_redirects`
- `Source/Code/ClientPatch_v759/hooknaked.cpp`
- `runtime/deploy/WYD-Kingdom-Patch-1.1.9.zip`

Validacao:

- Upload do arquivo `CABOCRO_WYD_16x12_24bit.bmp` pelo site retornou sucesso.
- `https://wydkingdom.com.br/img_guilds/b01000003.bmp` redirecionou para a API e retornou `image/bmp`.
- VPS publicou o manifesto `https://api.wydkingdom.com.br/patch/manifest.json` na versao `1.1.9`.
- Usuario confirmou que a guildmark apareceu no jogo.

## 2026-06-16 - Recompensa de level: Cavalo s/Sela N/B

Sintoma:

- Na etapa de recompensa do cavalo, o jogador recebia Cavalo Leve.
- A nova regra desejada e o jogador escolher entre Cavalo s/Sela N ou Cavalo s/Sela B.
- Na primeira versao, alguns clientes ainda mostravam Cavalo Leve; ao clicar, nao recebiam nada e a recompensa voltava no proximo level.

Causa:

- O painel do ClientPatch mostrava os IDs `2373` e `2368` (Cavalo Leve B/N).
- O TMSrv aceitava somente esses dois IDs na validacao da recompensa.
- O comando antigo `@levelitem` tambem podia entregar Cavalo Leve automaticamente.
- Depois da troca inicial, cliente sem patch atualizado ainda podia enviar os IDs antigos `2368`/`2373`, enquanto o TMSrv ja esperava somente `2366`/`2371`; isso recusava a entrega e nao avancava a etapa `chave1`.

Solucao:

- ClientPatch passou a mostrar `2366` (Cavalo s/Sela N) e `2371` (Cavalo s/Sela B).
- TMSrv passou a aceitar apenas `2366` ou `2371` na etapa da recompensa.
- O comando antigo `@levelitem` agora exige escolha explicita `N` ou `B`.
- Gerado `TMSrv.exe` novo e patch do launcher `1.1.10`.
- V2: TMSrv agora converte envio antigo `2368` -> `2366` e `2373` -> `2371` na etapa do cavalo, entrega Cavalo s/Sela e avanca a recompensa mesmo se o cliente ainda estiver desatualizado.
- V2: publicado patch do launcher `1.1.11` com `ClientPatch.dll` mostrando Cavalo s/Sela na interface.
- V3: instalador ajustado para nao abortar quando `taskkill` encontra `TMSrv.exe` ou `DBSrv.exe` ja fechado.

Onde conferir:

- `Source/Code/ClientPatch_v759/cItemLevel.cpp`
- `Source/Code/TMSrv/ItemLevel.cpp`
- `Source/Code/TMSrv/_MSG_MessageWhisper.cpp`
- `runtime/deploy/WYD-Kingdom-Patch-1.1.10.zip`
- `runtime/deploy/WYD-Kingdom-Patch-1.1.11.zip`
- `runtime/ENTREGAR-VPS/CAVALO-SSELA-ESCOLHA-20260616/`
- `runtime/ENTREGAR-VPS/RECOMPENSA-CAVALO-SSELA-V2-20260616/`
- `runtime/ENTREGAR-VPS/RECOMPENSA-CAVALO-SSELA-V3-20260616/`

Validacao:

- `TMSrv.vcxproj` compilou Release Win32 sem erros.
- `ClientPatch_v759.vcxproj` compilou Release Win32 sem erros usando `PlatformToolset=v145`.
- Patch local do launcher publicado na versao `1.1.10`.
- Entrega limpa criada com `WYD-Kingdom-Cavalo-SSela-Escolha-20260616.zip` e comando PowerShell da VPS.
- V2 compilou `TMSrv.exe` Release Win32 sem erros e publicou patch local do launcher `1.1.11`.
- Entrega limpa V2 criada com `WYD-Kingdom-Recompensa-Cavalo-SSela-V2-20260616.zip`.
- SHA-256 do pacote V2: `BFBEE3E78F7CF19764323136D623026B14DC662FDFBFF444457ED6F46BC0BF4C`.
- Entrega limpa V3 criada com `WYD-Kingdom-Recompensa-Cavalo-SSela-V3-20260616.zip`.
- SHA-256 do pacote V3: `2B0000EE84B9B1778A908D6D5A2E47E63D1501B85CA87B8981CFD830D0E159FB`.

## 2026-06-07 - Tela de selecao: servidor e canais

Sintoma:

- A janela do cliente ja aparecia como `Wyd Kingdom`.
- Os canais foram corrigidos para `Canal-1` e `Canal-2`.
- O servidor ainda aparecia como `The` em uma linha e `Kingdom` em outra.

Causa:

- `serverlist.bin`, `serv00.htm` e `UIString.txt` nao carregam o nome visual
  do servidor. Eles carregam URL/IP, status e rotulos da UI.
- O `WYD.exe` monta/decifra os nomes em runtime.
- A tela de selecao nao passava sempre `Wyd Kingdom` como uma string unica:
  para o servidor ela passava `The` e `Kingdom` em textos separados.
- Varredura/substituicao de memoria nao resolveu porque a lista/cache de texto
  nao relia as copias alteradas.

Solucao:

- Criado hook no ClientPatch para interceptar textos da UI em runtime.
- Pontos usados:
  - `setText` em `0x0040232B`;
  - cache/render de texto em `0x0042231F`.
- Adicionado trampolim manual para preservar o prologo original antes do JMP.
- `ReplaceServerNameText` troca:
  - `Wyd Kingdom` -> `Wyd Kingdom`;
  - `The` -> `WYD`;
  - `Kingdom` -> `Kingdom`;
  - `Canal-1` -> `Canal-1`;
  - `Canal-2` -> `Canal-2`.
- O build Release Win32 do `ClientPatch.dll` foi aplicado em `Cliente/ClientPatch.dll`.

Onde conferir:

- `Source/Code/ClientPatch_v759/hooknaked.cpp`
- `Source/Code/ClientPatch_v759/hooknaked.h`
- `RELATORIO-nomes-servidor.md`
- Backup antes do hook: `backups/fix-nome-servidor-settext-20260607-114404/`
- Backup antes do ajuste do nome dividido: `backups/fix-nome-servidor-split-20260607-120038/`

Validacao:

- Abrir o cliente na tela de selecao.
- Esperado:
  - servidor exibido como `WYD` / `Kingdom` na estrutura de duas linhas;
  - canais exibidos como `Canal-1` e `Canal-2`.

## 2026-06-07 - Painel D: Comandos, layout e ESC

Sintoma:

- O botao `Modo Stream` precisava virar `Comandos`.
- Ao clicar, primeiro aparecia mensagem vermelha de painel nao encontrado.
- Depois os textos ficavam fora do enquadramento ou surgiam artefatos roxos.
- Em alguns subpaineis, `ESC` fechava o painel todo em vez de voltar para a
  tela anterior.
- No `Drop List`, ao entrar nos mobs/drops de uma cidade, nao havia retorno
  natural para a lista de cidades.

Causa:

- O botao reaproveitou a estrutura do painel de `DropList`, que ja tinha
  posicoes e controles proprios.
- Ao transformar esse painel em lista de comandos, era necessario salvar e
  restaurar o layout original antes de voltar para DropList/cidades.
- O fluxo de `ESC` nao mantinha claramente a janela anterior: Pix, Donate Store,
  Filtro e DropList precisavam voltar para o painel principal da tecla D ou para
  a etapa anterior do proprio DropList.

Solucao:

- Renomeado o botao para `Comandos`.
- Criado fluxo `OpenCommandsPanel()` para listar comandos de usuario em ordem
  comum de uso.
- Criados controles auxiliares para salvar/restaurar layout do painel de comandos.
- Ajustado retorno de `ESC`:
  - Pix -> painel D;
  - Donate Store -> painel D;
  - Filtro -> painel D;
  - DropList detalhe de mob/drop -> DropList cidades;
  - DropList cidades -> painel D.
- Removido o artefato roxo ajustando o reaproveitamento dos controles.

Onde conferir:

- `Source/Code/ClientPatch_v759/ButtonControl.cpp`
- `Source/Code/ClientPatch_v759/SendChat.cpp`
- `Cliente/UI/command.txt`
- Backups:
  - `backups/pre-comandos-painel-20260607-000646/`
  - `backups/fix-comandos-esc-20260607-004047/`
  - `backups/fix-pix-esc-20260607-005105/`
  - `backups/fix-esc-stack-20260607-010639/`
  - `backups/fix-store-filter-esc-20260607-012257/`
  - `backups/fix-comandos-duas-colunas-20260607-012815/`
  - `backups/fix-comandos-remove-roxo-20260607-013558/`

Validacao:

- Abrir painel com a tecla D.
- Clicar em `Comandos`: os comandos devem aparecer organizados, sem texto
  estourando e sem artefato roxo.
- Abrir Pix, Donate Store, Filtro e DropList; usar `ESC` e confirmar que volta
  para a janela anterior correta.

## 2026-06-05 - /macroperga: pergaminhos da quest da Agua

Sintoma:

- Era necessario automatizar o uso dos pergaminhos da quest da Agua sem depender
  de leitura instavel de posicao do cliente.

Causa:

- A leitura direta de posicao no cliente era instavel e podia causar crash.
- O fluxo confiavel esta no servidor: ao limpar cada sala da Agua, o servidor
  entrega ao lider o pergaminho da proxima sala.
- Assim, o cliente nao precisa adivinhar posicao; precisa apenas usar o proximo
  pergaminho correto quando ele aparecer no inventario.

Solucao:

- Criado macro em `MacroPergaminhoAgua.cpp`.
- O macro usa uma maquina de estados:
  - usa LV1 para entrar;
  - confirma consumo/entrada;
  - aguarda LV2 ate Neses entregues pelo servidor;
  - espera o boss antes de reentrar;
  - rearma sozinho apos tempo ocioso.
- Comandos interceptados no cliente:
  - `/macropergaon` ativa;
  - `/macropergaoff` desativa;
  - `/pergastatus` mostra estado e pergaminho detectado.
- O comando e consumido pelo ClientPatch e nao precisa ser enviado ao servidor.

Onde conferir:

- `Source/Code/ClientPatch_v759/MacroPergaminhoAgua.cpp`
- `Source/Code/ClientPatch_v759/SendChat.cpp`
- `Cliente/UI/command.txt`
- `comandos-usuario.txt`
- Backups:
  - `Cliente/ClientPatch.dll.bak_macroperga_20260605_163219`
  - `Cliente/ClientPatch.pdb.bak_macroperga_20260605_163219`

Observacao importante:

- A thread antiga de varredura de nomes do servidor (`ThreadServerName`) ficou
  desativada em `StartMacroPergaAgua()`. Nao reativar: ela era pesada e nao
  resolveu nomes da tela de selecao.

Validacao:

- Entrar no jogo com pergaminhos da Agua.
- Usar `/macropergaon`.
- Usar `/pergastatus` para ver se detecta o pergaminho.
- O macro deve aguardar quando nao houver pergaminho e se desligar se o servidor
  recusar varias tentativas.

## 2026-06-04 - Quest do Coveiro

Sintoma:

- Clicar no Coveiro abria `Sistema de Composicao`.
- Em tentativas anteriores, o Coveiro ficou com visual de outro NPC ou perdeu
  o capacete.
- Havia risco de conflito visual/funcional com Jephi.
- Jogador fora da faixa de level conseguia entrar na quest clicando direto no
  NPC, mesmo quando a `Vela_do_Coveiro` bloqueava.

Causa:

- O Coveiro usava identificador visual `58`, que tambem acionava comportamento
  nativo de NPCs de composicao/forja.
- O hook do Jephi tambem dependia de assinatura visual proxima.
- O item `Vela_do_Coveiro` e o clique direto em `QUEST_COVEIRO` tinham regra de
  level divergente.

Solucao:

- Coveiro reconstruido com base original e capacete preservado.
- Identificador visual principal alterado de `58` para `125`.
- Assets do visual original clonados para o novo ID visual.
- Marcador do Jephi separado.
- `QUEST_COVEIRO` alinhada para `maxlevel = 115`.
- `TMSrv.exe` ativo recebeu patch binario do limite quando necessario.

Onde conferir:

- `RELATORIO_BUGS_COVEIRO.md`
- `Server/TMSrv/run/npc/Coveiro`
- `Source/Code/TMSrv/_MSG_Quest.cpp`
- `Source/Code/ClientPatch_v759/ProcessNpc.cpp`
- Backups principais:
  - `Server/TMSrv/run/npc/Coveiro.before_face125_visualclone_20260604_210731.bak`
  - `Cliente/Mesh/backup_coveiro_face125_20260604_210731/`
  - `Server/TMSrv/run/TMSrv.exe.before_coveiro_max115_20260604_211917.bak`

Validacao:

- Coveiro deve aparecer com visual correto e capacete.
- Clicar no Coveiro nao deve abrir composicao.
- Personagem fora do limite deve ser bloqueado tanto pela vela quanto pelo
  clique direto no NPC.

## 2026-06-06 - Evento Hit: Passe do Evento e agrupamento

Sintoma:

- Bau/drop do Evento Hit estava agrupando mesmo sem o usuario usar o Passe do
  Evento.
- O comportamento desejado e agrupar e ativar double hit somente quando o passe
  estiver ativo.

Causa:

- O ponto de entrega do premio do Evento Hit precisa respeitar `pUser[conn].Keys[49]`.
- Sem essa trava, o item pode ser entregue via agrupamento mesmo para personagem
  sem passe ativo.

Solucao:

- No fluxo atual de `SendEventCount`, o agrupamento chama `SendItemagrupar`
  somente quando `pUser[conn].Keys[49] == 1`.
- Sem passe, o item e entregue via `PutItem`.
- Com passe, o contador tambem incrementa extra, mantendo o efeito de double hit.

Onde conferir:

- `Source/Code/TMSrv/SendFunc.cpp`
- `Source/Code/TMSrv/_MSG_UseItem.cpp`, regiao `Entrada Cassino // Passe Evento`
- Backup: `backups/pre-event-hit-pass-fix-20260606-134412/`

Validacao:

- Personagem sem passe ativo: dropar bau do Evento Hit e confirmar que nao agrupa
  automaticamente.
- Usar Passe do Evento: confirmar double hit e agrupamento.

## 2026-06-06 - Montaria adulta: vida e vitalidade

Sintoma:

- A vitalidade da montaria funcionava, mas havia duvida se a vida do item estava
  baixando de forma gradual quando a montaria recebia dano.
- Em teste longo, a montaria morreu de fome, indicando que vitalidade/fome estava
  ativa.

Causa:

- A montaria adulta tem duas representacoes:
  - o mob summon visivel no mundo;
  - o item equipado no slot 14.
- Se o HP do mob e o HP gravado no item nao forem sincronizados, a barra/tooltip
  pode parecer parada ate ocorrer morte ou atualizacao tardia.

Solucao:

- `LinkMountHp(int mobConn)` sincroniza o HP do mob da montaria para o item
  equipado do dono e envia `SendItem` do slot 14.
- Em ataques, parte do dano e desviada para a montaria adulta; depois
  `ProcessAdultMount(Target, damage)` atualiza a vida do item.
- Quando o HP chega a zero, a vitalidade/fome e tratada e o item e atualizado.

Onde conferir:

- `Source/Code/TMSrv/Server.cpp`
- `Source/Code/TMSrv/Server.h`
- Backup: `backups/pre-mount-hp-sync-20260606-234602/`

Validacao:

- Equipar montaria adulta viva.
- Receber dano de player/monstro.
- Conferir tooltip/equipamento: a vida do item deve refletir o dano recebido.
- Se a montaria morrer, a vida minima apos reviver pode ser comportamento normal,
  mas vitalidade/fome nao deve ficar incoerente.

## 2026-06-07 - Rebranding visual: titulo, tela de login e rodape

Sintoma:

- Cliente exibia a marca antiga: titulo "Wyd Kingdom", tela de login com o logo
  grande "WYD KINGDOM" (templo) + um logo laranja "Ascension to Victory", e
  rodape "Copyright 2023 Wyd Kingdom".

Causa:

- Titulo: string estatica no .data do WYD.exe em 0x622CDA.
- Tela de login: a imagem principal eh `UI/logo1.wyT` (1444x1080, contem o templo
  + "WYD KINGDOM"); por baixo havia `UI/logo2.wyt` (256x256, logo laranja). O
  `NewLogin.wyt` (2000x2000) esta quase vazio (NAO eh o fundo). A animacao classica
  de montarias eh o fundo real, sempre por tras.
- Rodape: texto no indice 225 de `Cliente/UI/UIString.txt` (arquivo Latin-1).

Solucao:

- Titulo -> "Wyd Kingdom": `strcpy((char*)0x622CDA, "  Wyd Kingdom")` em
  hooknaked.cpp (mesmo tamanho do original "  Wyd Kingdom").
- Tela de login limpa: zerei os pixels de `logo1.wyT` e `logo2.wyt` mantendo o
  cabecalho (WT10), deixando-os transparentes -> somem o templo e o logo laranja,
  fica so a animacao classica.
- Rodape -> "Wyd Kingdom": troca em UIString.txt (edicao byte-safe, Latin-1).

Onde conferir:

- `Source/Code/ClientPatch_v759/hooknaked.cpp` (strcpy 0x622CDA)
- `Cliente/UI/logo1.wyT`, `Cliente/UI/logo2.wyt` (backups `*.bak_remove_*`)
- `Cliente/UI/UIString.txt` (indice 225; backup `*.bak_*`)

Validacao:

- Abrir o cliente: titulo "Wyd Kingdom", login sem templo/logo antigo (so animacao),
  rodape "Copyright 2023 Wyd Kingdom".

## 2026-06-07 - Arte do login (.wyt): por que NAO da pra trocar a imagem

Sintoma:

- Tentativa de colocar uma arte nova (fundo/logo "Wyd Kingdom") na tela de login.

Causa:

- As texturas de UI sao `.wyt` (assinatura "WT10"): cabecalho + pixels, mas com
  codificacao interna nao-trivial. As 4 ordens de canal testadas decodificaram so
  monocromatico/cinza e o alpha do arquivo nunca passa de ~110-180.
- Sem o conversor correto (ou o fonte do loader, que esta no WYD.exe fechado) nao
  da pra gerar um `.wyt` valido na mao. Gravar errado quebra a tela (testado: ficou
  transparente / espelhado / cor trocada).

Solucao (contornada):

- Para REMOVER arte: basta zerar os pixels (transparente) -> foi assim que limpei o
  login. Para COLOCAR arte nova: precisa de um conversor `.wyt` CONFIAVEL; o unico
  achado na web (Legacy.Tools.TextureConverter, WebCheats) tem ~40 deteccoes de
  antivirus -> NAO usar. Caminho seguro: ferramenta/fonte com a origem do cliente.

Onde conferir:

- `RELATORIO-nomes-servidor.md` (mesma classe de bloqueio: binario do cliente)
- Backups dos `.wyt` originais: `Cliente/UI/*.bak_*`

## 2026-06-06 - Painel D (tecla D): botao Modo Foto e mapa de handles

Sintoma:

- Os botoes "Modo Foto" e "Modo Stream" do painel da tecla D nao faziam nada.

Causa:

- O handler `HKD_ControlEventClick` (ButtonControl.cpp) mapeia o handle de cada
  botao para uma acao. Modo Foto (handle 981310) e Modo Stream (981311) estavam
  mapeados para `OPENPVPRANKING`/`OPENRANKING`, cujos `case` estao VAZIOS.
- Descoberto com um diagnostico que loga o handle de cada clique.

Solucao:

- 981310 (Modo Foto) -> nova acao `MODOFOTO` (novo `#define` + `case`) que replica
  o toggle da tecla U (`ConfigR::gModoFoto`). Botao Modo Foto agora = tecla U.
- (O 981311 "Modo Stream" foi depois reaproveitado pelo codex como "Comandos".)
- Adicionado `/comandos` (e `/cmds`) em SendChat.cpp listando os comandos de usuario.

Mapa de handles do Painel D (GamePainel = 981300):

- 981301-305 cidades (Armia/Arzan/Erion/Noatum/Nipple)
- 981306 Droplist | 981307 Filtro | 981308 Donate Store | 981309 Painel Pix
- 981310 Modo Foto | 981311 Modo Stream

Onde conferir:

- `Source/Code/ClientPatch_v759/ButtonControl.cpp`
- `Source/Code/ClientPatch_v759/TeclaPressNewButton*.cpp` (toggle da tecla U)
- `Source/Code/ClientPatch_v759/SendChat.cpp` (/comandos)
- `Painel D.txt`, `comandos-usuario.txt`

Validacao:

- Tecla D -> Modo Foto esconde a interface (volta com U). `/comandos` lista comandos.

## 2026-06-06 - Sistema da Agua: teleporte pelo pergaminho (sem consumir) e pack

Sintoma:

- Clicar no pergaminho da agua FORA d'agua nao fazia nada; queria que teleportasse
  para a frente da quest SEM consumir (como os itens de quest Vela/Colheita).
- Tambem: como criar um pack grande de pergaminhos.

Solucao:

- Em `_MSG_UseItem.cpp`: se o item usado eh um pergaminho de agua (3 tipos: M/N/A)
  E o jogador esta FORA das salas da agua E nao na grade de entrada -> `DoTeleport`
  para a frente da agua e devolve o item (`SendItem`), SEM consumir.
- Pack: a quantidade vai no efeito 61 (EF_AMOUNT):
  `/gm +set item 3173 61 120` = 120 pergaminhos Agua N.
- O macro da agua tambem ganhou auto-recuperacao apos reconectar (nao precisa mais
  ligar/desligar manualmente).

Onde conferir:

- `Source/Code/TMSrv/_MSG_UseItem.cpp`
- `Source/Code/ClientPatch_v759/MacroPergaminhoAgua.cpp`
- `comandos-adm.txt`, `comandos-usuario.txt`

Validacao:

- Fora da agua, clicar no pergaminho -> teleporta para a frente sem gastar o item.

## 2026-06-06 - Double XP: anuncio no servidor e painel de eventos

Sintoma:

- Ligar/desligar Double XP por comando de GM nao avisava os jogadores (saia
  "SETDOUBLEMODE"). Faltava um jeito facil de ligar/desligar, mudar drop e agendar.

Causa:

- O comando `double` so trocava o modo interno; nao mandava aviso global.
- O modo admin ativa com `/wydkingdom on` (NAO `/adm on`): o exe usava "wydkingdom" mas o fonte
  estava "adm", causando divergencia ao recompilar.

Solucao:

- Em `_MSG_Imple.cpp` (comando double): `SendNotice` global:
  - ON  = "> > > Evento DOUBLE XP ATIVADO! XP em dobro, aproveitem! < < <"
  - OFF = "> > > Evento DOUBLE XP encerrado. < < <"
- Corrigido o ativador admin "adm" -> "wydkingdom" em `_MSG_MessageWhisper.cpp` (comando
  real `/wydkingdom on`).
- Criado `painel-eventos.ps1`: liga/desliga Double XP (banco `serverconfig.doubleexp`
  + `gameconfig.txt`), muda/volta a taxa de drop e agenda Double XP (ex.: fim de
  semana). Reinicia o TMSrv quando necessario.

Onde conferir:

- `Source/Code/TMSrv/_MSG_Imple.cpp` (double / notice)
- `Source/Code/TMSrv/_MSG_MessageWhisper.cpp` (adm -> wydkingdom)
- `painel-eventos.ps1`, `comandos-adm.txt`

Validacao:

- `/wydkingdom on` e depois `/gm +set double 1` -> aparece o anuncio verde para todos.

## 2026-06-05 - Taxas de XP e Drop do servidor (referencia)

Resumo:

- Drop: bonus geral = 450 (em `gameconfig.txt`, secao Drop Bonus, 64 valores).
- Double XP: por banco (`serverconfig.doubleexp`) + `gameconfig.txt` ("double"),
  com anuncio (entrada acima). `globalexp` no banco eh vestigial; `freeexp 35` no
  gameconfig eh limite de BILLING, NAO taxa de exp.

Onde conferir:

- `Server/TMSrv/run/gameconfig.txt`, `painel-eventos.ps1`

## 2026-06-05 - Acesso remoto: jogar de outra casa (Radmin VPN)

Sintoma:

- Permitir que o primo jogasse de outra casa, sem IP publico nem abrir portas.

Solucao:

- Radmin VPN (LAN virtual, faixa 26.x.x.x): servidor e clientes entram na mesma
  rede virtual e usam o IP 26.x do host.
- `configurar-ip.ps1` ajustado para preferir o adaptador 26.x.x.x (Radmin).
- Confirmado: primo conectou (IP 26.x nos logs).

Onde conferir:

- `configurar-ip.ps1`, `COMO-LIGAR-LOCAL.md`

## 2026-06-05 - Conta, personagem e tornar conta admin

Estado atual:

- Esta entrada preserva o diagnostico historico de conta e personagem.
- A parte de admin por IP foi substituida em 12 de junho de 2026 pelo controle
  por conta em `admin-accounts.txt`. Nao seguir hoje as instrucoes de
  `admin.txt` descritas abaixo.

Sintoma:

- "Senha incorreta" com a senha certa; "Data too long for column username".
- Criar personagem dava "inadequado ou existente" para quase todos os nomes.
- Como tornar a conta do primo admin.

Causa:

- Login/senha: o cliente poe tudo em MAIUSCULAS; a conta precisa estar UPPERCASE no
  banco. `username` eh `varchar(12)` e o jogo limita a 11 chars.
- Personagem: a pasta `DBSrv/run/char` so tinha a subpasta de uma letra; sem as
  subpastas A-Z (+etc) o registro de nome falha (ENOENT) e quase tudo vira
  "inadequado/existente".
- Admin: eh por IP. O TMSrv le `admin.txt` (`../../DBSRV/Run/Admin.txt`) so no
  startup; a ativacao eh `/wydkingdom on` apos logar de um IP listado.

Solucao:

- Conta: criar/recriar em MAIUSCULAS, username <= 11 chars.
- Personagem: rodar o `GeradorDePastas.exe` (cria as subpastas A-Z+etc em char/).
- Admin do primo: adicionar o IP Radmin dele em `admin.txt` (DBSrv/run e TMSrv/run),
  formato `<indice> <IP>`; REINICIAR o TMSrv; depois o primo loga e usa `/wydkingdom on`.

Onde conferir:

- `Server/DBSrv/run/admin.txt`, `Server/TMSrv/run/admin.txt` (backups `*.bak_*`)
- `Source/Code/TMSrv/CReadFiles.cpp` (ReadAdmin), `comandos-adm.txt`

Validacao:

- Conta em maiusculas loga; nome valido eh aceito; primo com IP no admin.txt +
  TMSrv reiniciado consegue `/wydkingdom on`.

## 2026-06-04/06 - Toolchain de compilacao e armadilhas de encoding (REFERENCIA)

Como compilar:

- ClientPatch (cliente): MSBuild do VS18, `ClientPatch_v759.vcxproj`,
  `/p:Configuration=Debug /p:Platform=Win32 /p:PlatformToolset=v145` (o vcxproj
  pede v143 que NAO esta instalado), OutDir `_macrobuild/bin`. Saida -> copiar para
  `Cliente/ClientPatch.dll` (FECHAR o WYD.exe antes; ele trava a DLL). Cliente Debug
  usa /MT. Usar barras `/` nos paths p/ evitar problema de aspas.
- TMSrv (servidor): precisa de `libmysql.lib` 32-bit (pacote
  mysql-connector-c-6.1.11-win32) em `Source/Code/include_mysql`; no vcxproj o
  include `include_mysql` + opcao `/source-charset:.1252` (o fonte eh Latin-1).
- MSBuild: `C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe`.

Armadilhas de encoding (IMPORTANTE):

- Varios .cpp sao Latin-1 (ButtonControl.cpp, _MSG_*.cpp, SendChat.cpp);
  TeclaPressNewButton.cpp eh UTF-8 BOM. Editar com ferramenta que reescreve em UTF-8
  CORROMPE os acentos (viram EF BF BD) e quebra o build (C3872).
- Edicao segura: ler/gravar com Latin-1 (code page 28591) preservando bytes, e
  manter o codigo ADICIONADO em ASCII puro.
- Logar texto na tela ANTES de logar (tela de selecao) CRASHA o cliente; restringir
  hooks/varreduras ao contexto in-game.

Onde conferir:

- `Source/Code/ClientPatch_v759/*.vcxproj`, `Source/Code/TMSrv/TMSrv.vcxproj`
- `Source/Code/include_mysql/`

## 2026-06-06 - Documentacao de apoio criada

Arquivos criados na raiz `NewWorld/`:

- `comandos-adm.txt` - comandos de GM corrigidos (criar item eh
  `/gm +set item <id> 61 <qtd>`, NAO `/gm +item`).
- `comandos-usuario.txt` - comandos de usuario comum (custom), por etapa + atalhos.
- `Painel D.txt` - documentacao do painel da tecla D (cidades, botoes, Painel Pix).
- `painel-eventos.ps1` - painel de eventos (Double XP / drop / agendamento).

## 2026-06-07/12 - Launcher WYD Kingdom e atualizador automatico

Pedido:

- Criar um launcher proprio, com a identidade visual do WYD Kingdom, capaz de
  verificar arquivos e abrir o cliente real.

Solucao:

- Criado em WPF/.NET 8 com janela adaptada ao monitor, status do servidor,
  noticias e links oficiais.
- O launcher verifica automaticamente o manifesto ao abrir. O botao `JOGAR`
  repete a verificacao e abre `Cliente/WYD.exe`.
- O botao de configuracoes abre `Resolution WYD Kingdom.exe`.
- O manifesto oficial e
  `https://api.wydkingdom.com.br/patch/manifest.json`.
- A verificacao compara tamanho e SHA-256, baixa somente arquivos ausentes ou
  diferentes e impede caminhos fora da pasta do cliente.
- Os downloads sao validados antes da troca. Arquivos comuns usam backup e
  rollback.
- O proprio launcher agora pode atualizar EXE/DLLs: prepara `.update`, fecha,
  substitui os arquivos por um helper, reabre e confirma a nova execucao. Se a
  versao nova falhar, restaura os binarios anteriores.
- O backend Node serve o manifesto sem cache e os arquivos versionados com cache
  imutavel em `/patch/versions/...`.
- Criado `publicar-patch.ps1`, que mantem um manifesto cumulativo e gera o ZIP
  pronto para instalar na VPS.
- Build framework-dependent publicado em `Cliente/` como
  `Launcher WYD Kingdom.exe` e DLLs auxiliares.
- Versao atual: `1.1.0`.
- Patch inicial com 18 arquivos centrais gerado em
  `runtime/deploy/WYD-Kingdom-Patch-1.1.0.zip`.
- SHA-256 do ZIP:
  `FF3840A5302FBAF25BDB10445A7996EED82CC00A6E40E104D11C3BF132CBFCC5`.

Onde conferir:

- `DOCUMENTO-LAUNCHER-WYD-KINGDOM.md`
- `GUIA-PUBLICAR-ATUALIZACAO-LAUNCHER.md`
- `Launcher/`
- `publicar-patch.ps1`
- `api-backend/src/server.js`
- `Cliente/Launcher WYD Kingdom.exe`
- `Cliente/launcher-config.json`

Validacao:

- Build Release concluido sem avisos ou erros.
- Teste real de auto-update encerrou o PID antigo, atualizou EXE/DLLs, abriu um
  novo PID e removeu todos os arquivos temporarios.
- Backend, cache HTTP, manifesto e hashes do pacote foram validados.
- Teste incremental publicou apenas um arquivo e preservou as outras 17 entradas.
- Pendente somente instalar o ZIP na VPS e validar o download publico antes de
  criar o instalador oficial.

## 2026-06-07 - Anuncio global de refino +12 a +15

Sintoma:

- Nao havia anuncio global quando um player tinha sucesso em refinar um item para
  +12/+13/+14/+15 (so a mensagem local "Voce obteve sucesso na refinacao").

Causa:

- O refino abencado (+12 a +15) eh tratado em `_MSG_CombineItemOdin.cpp`, bloco
  `combine == 2` (Item +12+). No sucesso, ele calcula `NewSanc` (12/13/14/15) e
  manda mensagem so para o jogador; nao usava `SendNotice` (anuncio global).

Solucao:

- Inserido `SendNotice` no bloco de sucesso (antes do log de sucesso, onde
  `NewSanc` ja esta definido e o item ja foi refinado):
  - `> > > [Nick] refinou [Item] para +15! Parabens! < < <`
  - `> > > [Nick] refinou [Item] para +14/+13/+12! < < <`
- Recompilado o TMSrv e aplicado (servidor reiniciado).

Onde conferir:

- `Source/Code/TMSrv/_MSG_CombineItemOdin.cpp` (bloco combine==2, var `NewSanc`)
- Backup do exe: `Server/TMSrv/run/TMSrv.exe.bak_refino_*`

Validacao:

- Refinar um item para +12 ou mais -> aparece o anuncio global para todos.

## 2026-06-07 - Anuncio global de Ancient e de +10

Sintoma:

- Faltavam anuncios globais para criacao de item Ancient e para item chegar a +10.

Causa / pontos encontrados:

- Ancient: criado em `_MSG_CombineItem.cpp` (chama `GetMatchCombine`, que exige
  item nUnique 41-49, nao-ARCH, com pedras +7/+8/+9 e chance `g_pAnctChance`). No
  sucesso seta o sIndex Ancient + sanc 7 e manda "Sucesso na Composicao Anct".
- +10: `_MSG_CombineItemAilyn.cpp` cria gear +10 com alma/joia (no sucesso faz
  `BASE_SetItemSanc(..., 10, joia)` e "Processing Complete").
- ATENCAO: o refino normal de gear (+1..+9) e o "purificado" do Ehre (combine==5,
  que na verdade sobe o nivel do PROPRIO item Refinacao Abencoada 3338) NAO foram
  usados. O +10 anunciado eh o do combine do Ailyn (gear + alma/joia -> +10).

Solucao:

- `SendNotice` no sucesso de cada um:
  - Ancient: `> > > %s criou um item Ancient! < < <` (em _MSG_CombineItem.cpp,
    apos o SendItem do item Anct).
  - +10: `> > > %s compos %s +10! < < <` (em _MSG_CombineItemAilyn.cpp, apos o
    SetItemSanc 10).
- Recompilado o TMSrv e reiniciado.

Onde conferir:

- `Source/Code/TMSrv/_MSG_CombineItem.cpp` (Ancient)
- `Source/Code/TMSrv/_MSG_CombineItemAilyn.cpp` (+10 com alma/joia)
- `Source/Code/TMSrv/GetFunc.cpp` (GetMatchCombine = regra do Ancient)
- Backup do exe: `Server/TMSrv/run/TMSrv.exe.bak_anct10_*`

Validacao:

- Criar um Ancient -> anuncio global. Fazer um +10 com alma/joia -> anuncio global.

Observacao:

- Se existir OUTRO caminho de +10 (ex.: refino normal +9->+10) que tambem deva
  anunciar, e so apontar o NPC/combine usado que adiciono o mesmo SendNotice la.

## 2026-06-07 - Rotulos da receita Ref +12+ (NPC de composicao)

Sintoma:

- Na janela "Sistema de Composicao" (receita Ref +12+), os slots mostravam textos
  genericos/enganosos: "Lactolerium / Pingente" (na verdade aceita Poeira de
  Lactolerium x10 ou Emblema da Protecao, NAO pingente) e
  "Pedra Secreta/ Refinacao Abencoada". O texto longo ainda estourava para a coluna
  ao lado.

Causa:

- A regra real da receita esta no servidor: `GetFunc.cpp` (GetMatchCombineOdin,
  linha ~615, retorno 2 = Ref+12+): slots 0/1 = 413 Poeira de Lactolerium (>=10)
  ou 4043 Emblema da Protecao; slot 2 = item alvo +11..+14; slots 3-6 = Pedra
  Secreta (5334-5337) ou Refinacao Abencoada (3338).
- Os ROTULOS exibidos sao client-side, em `Cliente/UI/strdef.bin` (tabela de
  strings de 128 bytes por campo, null-padded). Offsets: 60672 = label dos slots
  0/1; 60800 = label dos slots 3-6.

Solucao:

- Editado `strdef.bin` in-place (mesmo tamanho de arquivo, campos null-padded):
  - 60672: "Lactolerium / Pingente" -> "10 Lac / Emblema P."
  - 60800: "Pedra Secreta/ Refinacao Abencoada" -> "Secreta / Refi +0 ate +9"
  - "Item +11 ou superior" (slot 2) mantido.
- Edicao byte-safe (Latin-1 / cp1252), sem mexer no resto do .bin.

Onde conferir:

- `Cliente/UI/strdef.bin` (backups `strdef.bin.bak_*`)
- `Source/Code/TMSrv/GetFunc.cpp` (regra real da receita, ~linha 615)

Validacao:

- Abrir o NPC de composicao, receita Ref +12+: os rotulos devem aparecer curtos e
  encaixados, refletindo os itens reais aceitos.

## 2026-06-07 - Helriohdon (item 346): remover todos os drops

Sintoma:

- Item 346 (Helriohdon) invisivel no inventario, equipava como montaria quebrada
  e estava dropando "na lan".

Causa:

- Todo drop de mob passa por ControleDropItem (DropControl.cpp), chamada unica em
  MobKilled.cpp. Quem cria o item no chao e o PutItem dentro dessa funcao.

Solucao:

- Filtro global no topo de ControleDropItem: se ItemDrop->sIndex == 346, return
  antes do PutItem (nao dropa). Cobre qualquer mob, mapa ou fonte.

Onde conferir:

- Source/Code/TMSrv/DropControl.cpp (~linha 24)
- Backup TMSrv.exe.bak_nodrop346_*

Validacao:

- Matar mobs e confirmar que o 346 nao cai mais.

## 2026-06-08 - Refino +12 de acessorio: item sumindo no NPC Odin

Sintoma:

- Refinar Brinco de Tita (e pingente/anel) do +11 pro +12 fazia o item SUMIR do
  inventario, sem aviso de falha, tanto com pack de lac quanto com emblema.

Causa:

- A receita do +12 (GetFunc.cpp, GetMatchCombineOdin, combine==2) so aceitava itens
  com nPos {2,4,8,16,32,64,128,192}. Acessorios sao nPos 256 -> nao casavam ->
  GetMatchCombineOdin retornava 0.
- E os itens eram ZERADOS (memset) ANTES de validar a receita; com combine==0 o
  handler dava return sem restaurar -> o item era apagado.

Solucao:

- GetFunc.cpp: adicionado nPos 256/512/1024 na receita (acessorios podem +12-15).
- _MSG_CombineItemOdin.cpp: guard "if (combine == 0)" ANTES do wipe -> receita
  invalida devolve os itens com aviso, NUNCA apaga.
- Acessorios isentos da trava "Items de Rank B proibidos de +12" (Extra<=4).

Onde conferir:

- Source/Code/TMSrv/GetFunc.cpp (~615) e _MSG_CombineItemOdin.cpp (~122 e ~490)
- Backup TMSrv.exe.bak_refino_acessorio_*

Validacao:

- Refinar acessorio +11->+12 no Odin: sucesso sobe, falha mantem/cai 1 nivel,
  nunca some.

## 2026-06-08 - Poeira de Fada Celestial (item 5600): level-up de celestial

Sintoma:

- A Poeira de Fada (414) sobe level de mortal/arch mas nao funcionava em
  celestial/sub-celestial. Depois, o item novo aparecia SEM icone e sumia da loja.

Causa:

- O item 414 (EF_VOLATILE 7) e um consumivel de level-up com trava
  "if (ClassMaster >= CELESTIAL) return" (so mortal/arch). A logica ja suportava
  celestial (tabela g_pNextLevel_2), so a trava impedia.
- Icone: itemicon.bin[5600] estava 0 (sem icone). E um efeito EF_UNKNOW6=1 no
  registro client fazia o cliente ESCONDER o item quando ele ganhava icone ativo.

Solucao:

- Reaproveitado o item 5600 (estava livre) como "Poeira de Fada Celestial". No bloco
  Vol==7: se sIndex==5600 exige celestial; senao mantem mortal/arch (414).
- Adicionado a loja do mercador BETA_A (a loja = Carry do mob npc/BETA_A).
- Cliente: ItemList.bin e so XOR 0x5A; trocado EF_UNKNOW6->EF_GRID e icone via
  itemicon.bin[5600]=6 (4 bytes/ID, nao cifrado). Precisa fechar/reabrir o WYD.exe.

Onde conferir:

- Source/Code/TMSrv/_MSG_UseItem.cpp (bloco Vol==7, ~1273)
- Server/TMSrv/run/npc/BETA_A (Carry) e ItemList.csv (nome 5600)
- Cliente/ItemList.bin (XOR 0x5A) e Cliente/itemicon.bin; backups varios

Validacao:

- Criar/comprar 5600, usar num celestial -> sobe level. Na loja BETA_A aparece com
  icone de poeira.

## 2026-06-08 - Hidra Dourada: brilho dourado de verdade

Sintoma:

- A Hidra Dourada farmavel parecia identica a Hidra normal.

Causa:

- A aparencia do mob vem do struct binario em npc/ (816 bytes). O "dourado" e um
  efeito de brilho EF_SANC no Equip[0] (offset 143 do arquivo). A Hidra_Dourada
  farmavel estava com valor 6 (fraco); Dragao Dourado e a variante boss usam 9.

Solucao:

- Trocado o byte do Equip[0].stEffect[0].cValue de 6->9 em npc/Hidra_Dourada
  (offset 143). Server-side; o cliente desenha a aura a partir do dado do mob.

Onde conferir:

- Server/TMSrv/run/npc/Hidra_Dourada; backup .bak_dourado_*

Validacao:

- Area das Hidras Douradas (respawn apos restart): aura dourada forte.

## 2026-06-08 - Destrave celestial (Lv40/Lv90) e XP travado

Sintoma:

- Celestial level 90 nao recebia XP matando mob e nem subia com a poeira.

Causa:

- As flags QuestInfo.Celestial.Lv40/Lv90 = 0 travam XP (GetExpApply) e level-up
  (CheckGetLevel). A poeira leva ate o portao mas nao marca a flag. O destrave do
  Lv40 so funciona em level exatamente 39, entao nao da pra refazer no 90.

Solucao:

- Criado comando GM "/gm +set destravecel" (_MSG_Imple.cpp, bloco set) que marca
  todas as flags celestiais (Lv40/90/240/280/320/360).
- Confirmado que os itens do destrave (Pedra da Furia 3020 e runas) ja sao
  obteniveis por player comum (drops + mercadores Runas_Joias e BETA_B), entao
  player normal nao trava de vez (faz a quest do portao).

Onde conferir:

- Source/Code/TMSrv/_MSG_Imple.cpp (set "destravecel"), _MSG_UseItem.cpp (Lv90 via
  item 3020), GetFunc.cpp/GetExpApply e CMob.cpp/CheckGetLevel
- Backup TMSrv.exe.bak_destravecel_*

Validacao:

- Logado no celestial: /wydkingdom on e /gm +set destravecel; matar mob -> XP volta.

## 2026-06-08/09 - Lojas dos reinos e dos BETAs (ajustes de itens)

Sintoma/Pedido:

- Ajustar os itens vendidos em NPCs de reino e nos mercadores BETA.

Como funciona:

- A loja de mercador = Carry do mob (npc/<Nome>), lida por SendShopList. Layout:
  Carry em offset 268, 8 bytes por item (sIndex + 3 stEffect), slots EXIBIDOS
  {0-8, 27-35, 54-62}. Preco vem do ItemList.csv.

Solucao:

- Redmiron e Bardes (dois reinos): adicionadas as 5 Barras de Mithril (3027-3031,
  do "adicional"); Barra de Prata (1Bi)=4011 mantida com preco 1.100.000.000
  (editado na ItemList.csv).
- BETA_A: Refinacao Abencoada normal (3338) e +9 (3338 com EF_SANC 9) e Poeira de
  Fada Celestial (5600).
- BETA_C: Emblema da Protecao (4043), junto dos Cristais de refino.
- Obs: 3338 e 4043 tem preco 0 na ItemList (saem de graca na loja) - decisao do
  dono manter assim por ser fase de teste.

Onde conferir:

- Server/TMSrv/run/npc/Redmiron, Bardes, BETA_A, BETA_C; ItemList.csv (preco 4011)
- Backups *.bak_mithril_*, *.bak_voltabarra_*, *.bak_precobarra_*, *.bak_emblema_*

Validacao:

- Abrir cada NPC e conferir os itens.

## 2026-06-09 - Pocao Revigorante: montaria morrendo (dano e fome)

Sintoma:

- A montaria morria mesmo com a Pocao Revigorante "ativa"; tambem nao dava pra
  consumir outra (constava efeito ainda ativo).

Causa:

- A invulnerabilidade depende do Affect Type 51. (1) So o item 5548 setava esse
  affect; as pocoes 3210/3211/3212 (VOLATILE 242) caiam no handler das Joias de PvP
  (Affect Type 8), nunca aplicando a invulnerabilidade. (2) A morte por FOME (timer
  em Server.cpp ~5758, "Sua montaria morreu de fome") nao checava o Affect 51.

Solucao:

- _MSG_UseItem.cpp: 3210/3211/3212 tambem aplicam o Affect 51, com duracao por item
  (24h / 3 dias / 7 dias).
- Server.cpp: enquanto o Affect 51 estiver ativo, a comida da montaria e mantida
  cheia (Feed=100) em vez de decrementar. Agora o revigorante protege das DUAS
  mortes (dano de combate e fome).

Onde conferir:

- Source/Code/TMSrv/_MSG_UseItem.cpp (~4997) e Server.cpp (~5566 ProcessAdultMount,
  ~5758 timer de fome)
- Backups TMSrv.exe.bak_revigorante_* e .bak_montfome_*

Validacao:

- Limpar buff antigo com /gm +nobuff, usar pocao revigorante nova: a montaria nao
  morre nem de dano nem de fome durante o efeito.

## 2026-06-09/11 - Balanceamento das classes, telemetria e auditoria V3

Sintoma/Pedido:

- Saber se as 4 classes estao equilibradas (dano, defesa, ataque magico, absorcao)
  e corrigir outliers sem aplicar multiplicadores globais sem medicao.

Solucao:

- O handoff inicial virou implementacao completa no TMSrv, com regressao numerica,
  telemetria opt-in e preparacao automatica de contas e cenarios.
- Ajustados os modificadores estruturais de TK, FM e BM para Arch/Celestial, a
  defesa elemental extra de FM, a Explosao Eterea, o `ForceDamage` em PvP e a
  validacao do custo total de mana.
- Criados os comandos `+balreset`, `+balnext`, `+balhit`, `+balsweep` e os fluxos
  de auditoria de skills e pets.
- A auditoria V3 percorreu 96 skills, 3 evolucoes e 4 classes defensoras:
  1152 cenarios.
- `Julgamento Divino` (skill 30) recebeu cap PvP localizado: media de
  `500000` caiu para `5149.92`, mantendo o custo de HP e o comportamento PvE.
- `Exterminar` (skill 22) recebeu formula PvP localizada: dano de `375` passou
  para `910`, mantendo o custo de zerar o MP.
- Os pets BM das skills 61-63 foram calibrados sem multiplicador global:
  Gorila `1226.37`, Dragao Negro `1305.03` e Succubus `1414.28` por rodada.

Onde conferir:

- `LAUDO-BALANCEAMENTO-PROMPT-CODEX.md`
- `IMPLEMENTACAO-BALANCEAMENTO.md`
- `RELATORIO-BALANCEAMENTO-CIRURGICO-CLASSES-V3.md`
- `RELATORIO-CALCULO-BALANCEAMENTO-V2.md`
- `PROTOCOLO-TESTE-BALANCEAMENTO.md`
- `ROTEIRO-TESTADORES-BALANCEAMENTO.md`
- `runtime/balance-sessions/`

Validacao:

- Regressao automatizada inicial: 8 verificacoes estruturais, 25 cenarios
  numericos e zero falhas.
- Sessao limpa V3: 934 eventos de combate, 3456 eventos de mana, 4608 eventos
  de auditoria e zero eventos de combate/mana rejeitados.
- Reteste final BM 61-63: 36/36 cenarios, 1080 eventos validos e zero rejeitados.
- A matriz completa de 1152 cenarios foi restaurada depois dos retestes locais.

## 2026-06-09 - Sistema de Drop: fidelidade do Painel D e todas as fontes

Pergunta:

- Os itens que aparecem no painel de Droplist (tecla D) sao 100% fieis ao que o mob
  realmente dropa? Tem risco de um mob dropar algo fora da lista?

Resposta (resumo):

- Drop NORMAL de mob comum: o painel e FIEL. Painel le o Carry do mob e o "Drop comum"
  sorteia do MESMO Carry. Nenhum drop aleatorio fora da lista. O painel pode mostrar de
  leve a MAIS (itens sIndex<=390 e 454 aparecem mas sao filtrados na hora do drop).
- Fontes FORA do painel (todas hardcoded/intencionais, por mob/area especifica): ouro;
  areas-lan (LanDrop, 10 itens); drops de boss por GenerateID (Agua, Boss R/Lac/Ori,
  Cristais, elmos do GenID 3); CartaDrop (poeiras/cristais); drop de evento global (GM).
- Reis (Harabard/Glantuar) NAO dropam item (so evento Kingdom Clear). Mobs de sala
  secreta NAO dropam (so progressao/teleporte).
- Cuidado: o painel vem de tabela no banco gerada do Carry; apos editar drops, rodar
  `/gm +makedroplist` (ou `+reloaddroplist`) pra regenerar, senao fica defasado.

Onde conferir:

- Relatorio detalhado: RELATORIO-SISTEMA-DROP.md (mapa completo de todas as fontes, IDs,
  coordenadas das areas-lan e linhas do codigo).

Validacao:

- Conferir um mob no painel D e comparar com o Carry dele (npc/<Nome>); regenerar a
  tabela com /gm +makedroplist apos qualquer mudanca.

## 2026-06-12 - Servidor publico na VPS (Battlehost)

Sintoma/Pedido:

- Tirar o servidor do PC (Radmin VPN) e colocar numa VPS publica de verdade.

Solucao:

- VPS Windows da Battlehost, IP publico `108.165.179.40`.
- Criados instalador e scripts: `install-newworld-vps.ps1`, `start-newworld-vps.ps1`,
  `import-wydsite.ps1`. Pacote em `runtime/deploy/`.
- MariaDB local na porta 3307 (root/123456) - NUNCA exposta na internet.
- Firewall publico libera so TCP 8281 (jogo) e 80 (status/site/API).
- Cliente conecta direto no IP `108.165.179.40:8281` (Cloudflare nao faz proxy do jogo).

Onde conferir:

- `GUIA-VPS-WYDKINGDOM.md`, `install-newworld-vps.ps1`, `start-newworld-vps.ps1`.

Validacao:

- Jogo acessivel em `108.165.179.40:8281`; conta matteo loga.

## 2026-06-12 - Admin por CONTA (nao mais por IP)

Sintoma/Pedido:

- Remover o admin por IP do primo; ser admin SO pela conta do dono (independente de
  WiFi) e garantir que ninguem que acesse a VPS vire admin.

Solucao (codex):

- Controle de staff por conta: `admin-accounts.txt` (1 login por linha, lido AO VIVO
  por `IsAdminAccount`). `/wydkingdom on` e o auto-admin de level 999 passam a exigir
  conta listada. Conta `matteo` = admin.

Onde conferir:

- `Source/Code/TMSrv/Functions.h` (IsAdminAccount/GetAccountStaffRole),
  `_MSG_MessageWhisper.cpp`, `ProcessDBMessage.cpp`.
- `Server/TMSrv/run/admin-accounts.txt`.

Validacao:

- So a conta matteo ativa `/wydkingdom on`; o IP nao importa mais.

## 2026-06-12 - Site WYD Kingdom no ar + cadastro/login/painel reais

Sintoma/Pedido:

- Site profissional com criar conta, login, painel (trocar senha/e-mail) e donate.

Solucao:

- Site Next.js no Cloudflare Pages (projeto `wydkingdom-site`); dominio
  `wydkingdom.com.br` aponta pra ele.
- Cadastro e login REAIS via backend na VPS (`api.wydkingdom.com.br`).
- Troca de senha pelo site reflete no jogo (grava banco + arquivo da conta).
- Donate ainda em implementacao (Fase 2 - Mercado Pago).

ATENCAO (duas contas Cloudflare):

- O dominio + o projeto Pages oficial estao na conta `mundorickoficial@gmail.com`.
- Existia um projeto duplicado `wyd-kingdom` na conta `bortolinfotografo` (descartavel).
- Atualizar o site = commit/push no repo `github.com/bortolinfotografo-alt/wydkingdom-site`
  -> a Cloudflare rebuilda sozinha.

Onde conferir:

- Repo `Site/` (separado do NewWorld); memoria `wydkingdom-site`.

Validacao:

- Criar conta no site -> logar no jogo; login no site -> painel `/conta`.

## 2026-06-12 - Backend de contas na VPS (api-backend, Node)

Solucao:

- Backend Node/Express em `NewWorld/api-backend`, rodando na porta 80 da VPS (no lugar
  do antigo webstatus PowerShell), exposto pela Cloudflare em `api.wydkingdom.com.br`
  (SSL Flexible).
- Rotas: `/api/register`, `/api/login` (token JWT), `/api/account/me`,
  `/api/account/password`, `/api/account/email`; serve tambem `/serv00.htm` e `/health`.
- Usa um usuario MySQL dedicado `wydapi` (mysql_native_password), porque o driver Node
  nao fala o auth gssapi do root. Adiciona a coluna `email` na tabela `accounts`.
- A troca de senha grava no banco E no arquivo de conta (8424 bytes, senha ASCII no
  offset 16) - mesmo formato do `New-WydAccount`.

Onde conferir:

- `NewWorld/api-backend/` (README-DEPLOY.md, install-api.ps1, src/), `GUIA-API-BACKEND.md`.

Validacao:

- `https://api.wydkingdom.com.br/health` = OK; `/api/login` retorna token.

## 2026-06-12 - DBSrv reconhece senha nova sem reiniciar

Sintoma:

- Trocar a senha pelo site grava certo no banco (`accounts.password`) E no arquivo da
  conta (offset 16) - confirmado por SELECT e leitura do arquivo -, mas o JOGO so aceita
  a senha ANTIGA ate reiniciar o DBSrv; a nova falha.

Causa:

- O fluxo antigo misturava a senha do arquivo, o estado de `pAccountList` e um
  `SELECT *` dependente da ordem das colunas.
- A sincronizacao do arquivo acontecia no momento errado e havia chamada invertida de
  `UpdateAccount`, permitindo comparar ou persistir uma credencial antiga.
- Os saves normais gravavam a estrutura mantida em memoria, com risco de restaurar no
  arquivo uma senha anterior depois de uma troca externa feita pelo site.
- O `cSQL` ja abria uma conexao nova por consulta; nao existia uma unica transacao
  persistente desde o boot. Mesmo assim, autocommit e `READ COMMITTED` nao estavam
  declarados explicitamente.

Solucao aplicada:

- O login agora executa `SELECT password ... LIMIT 1` em uma conexao MySQL nova a
  cada tentativa e valida a senha recebida contra esse valor atual.
- A conta/arquivo ausente so e criado depois de a senha do banco ser validada.
- Quando banco e arquivo divergem, a senha atual do banco e sincronizada para o
  arquivo de conta.
- `DBWriteAccount` rele a senha atual do MySQL antes de qualquer save/logout. Se o
  banco estiver indisponivel, preserva a senha ja existente no arquivo em vez de
  substitui-la pela copia antiga mantida em memoria.
- `UpdateAccount` ficou como caminho explicito para uma alteracao intencional de senha.
- As conexoes do DBSrv agora ativam autocommit e configuram a sessao como
  `READ COMMITTED`.
- Compilado em `Release|Win32` e publicado na VPS pelo hotfix
  `WYD-DBSrv-PasswordRefresh-20260612.zip`.
- SHA-256 do `DBSrv.exe` implantado:
  `6F0216A760120080F2802B9A3001EA44960CF27353B51832A694C14E7FD7CB3A`.
- Backup criado na VPS antes da instalacao:
  `C:\WYDKingdom\runtime\hotfix-backups\DBSrv-password-refresh-20260612_202847`.

Onde conferir:

- `Source/Code/DBSrv/CFileDB.cpp`
- `Source/Code/DBSrv/CFileDB.h`
- `Source/Code/DBSrv/dbMySQL.cpp`
- `Server/DBSrv/run/DBSrv.exe`
- `runtime/deploy/WYD-DBSrv-PasswordRefresh-20260612.zip`
- O lado do site continua em `api-backend` + `New-WydAccount`, gravando banco e
  arquivo no offset correto.

Validacao:

- Teste tecnico: conta criada depois do boot recebeu confirmacao de login sem
  reiniciar o DBSrv.
- Teste tecnico: senha antiga foi recusada e senha nova aceita sem trocar o PID do
  DBSrv.
- Teste tecnico de save: arquivo com senha antiga e banco com senha nova terminou
  preservando a senha nova depois de `DBWriteAccount`.
- Confirmacao real na VPS em 2026-06-12: uma conta antiga conectou normalmente;
  depois, a senha foi trocada pelo site e a nova senha entrou no jogo sem reiniciar
  DBSrv ou TMSrv.

## 2026-06-12 - Site: Discord -> grupo de comercio no WhatsApp

Solucao:

- Todas as referencias a Discord no site trocadas pelo grupo de comercio no WhatsApp
  (Header, Hero, Footer e secao Comunidade refeita como "Grupo de comercio", so com itens
  de comercio + aviso para ler as regras do grupo).
- Link do grupo: `https://chat.whatsapp.com/E1tnLOthzvoHAh6mlWs0a5`.

Onde conferir:

- `Site/components/` (Header.jsx, HeroSection.jsx, Footer.jsx, CommunitySection.jsx,
  WhatsappIcon.jsx).

Validacao:

- `wydkingdom.com.br` sem nenhum Discord; botoes levam ao grupo do WhatsApp.

## 2026-06-13 - Crash do TMSrv ao matar mob (Mortal/Arch Lv256+): divisao por zero

Sintoma:

- TMSrv caia (0xc0000094 = divisao por zero, offset 0x1e561) sempre que um char
  MORTAL ou ARCH acima do level 255 matava um mob. So apareceu quando o primeiro
  char chegou nessa faixa (quest Coracao de Kaizen / deserto). Celestial NAO caia.

Causa:

- Em `Server.cpp`, os divisores de XP `ExpBaseN[12]` (Mortal) e `ExpBaseM[12]` (Arch)
  estavam `= {}` (todos ZERO) e NUNCA eram carregados por nenhum codigo.
- `GetExpApply` (GetFunc.cpp ~1519-1615) faz `ExpTotal = (exp*100)/ExpBaseN[i]` para
  Mortal/Arch nos niveis 256-400 -> divisao por zero -> crash. Celestial usa divisores
  fixos no codigo (8500-18000), por isso nao caia.
- A tabela `expbase` no MySQL (que o codex preencheu) NAO alimenta esses arrays - o
  codigo nao le essa tabela. Por isso o fix no banco nao resolveu.

Solucao:

- Inicializei os arrays direto no `Server.cpp` com valores validos (nao-zero):
  - `ExpBaseN[12] = {200,220,250,300,350,400,500,600,700,800,900,1000}` (Mortal)
  - `ExpBaseM[12] = {200,400,800,1200,1600,2000,2400,2800,7600,9000,10000,12000}` (Arch)
- Recompilei o TMSrv (MSBuild VS18, Release|Win32, PlatformToolset v145) -> saida em
  `Source/Code/TMSrv/_production/TMSrv.exe`. Build sem `#ifdef` de staff -> mantem o
  admin por conta (IsAdminAccount). Deployado na VPS substituindo o TMSrv.exe.

Onde conferir:

- `Source/Code/TMSrv/Server.cpp` (def. ExpBaseN/M ~linha 87), `GetFunc.cpp` (GetExpApply)
- Backup na VPS: `Server/TMSrv/run/TMSrv.exe.bak_pre_expfix_*`
- Build: `C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe`
  `TMSrv.vcxproj /p:Configuration=Release /p:Platform=Win32 /p:PlatformToolset=v145`

Validacao:

- Char Mortal/Arch acima do 255 mata mob -> ganha XP, servidor NAO cai (confirmado).

Obs: o "Arco Guardiao" 3556 era suspeito mas NAO era a causa (o crash continuava sem
ele). O hotfix "itemlevel-crash-v1" do codex quebrou o TMSrv na inicializacao
(0xc0000409) e foi descartado.

## 2026-06-16 - Quest Amuleto em Azran: Cap.Explor. respondia apenas "Sol"

Sintoma:

- Na quest do amuleto em Azran, o NPC `Cap.Explor.` aparecia e respondia somente
  "Sol", sem entregar o amuleto e sem explicar por que o personagem nao podia
  concluir a quest.
- Depois do primeiro ajuste, o NPC passou a mostrar a aba `Quest Amuleto`, mas a
  interacao ainda podia cair em fluxo de fala/ataque em vez da rotina de quest.
- Teste real confirmou que a quest funcionava com personagem Mortal; o problema
  restante era a falta de mensagem clara para outros tipos de personagem.

Causa:

- A rotina da quest usa `TerraMistica` dentro de `QuestInfo.Mortal` e exige
  `ClassMaster == MORTAL`. Nao ha limite de level explicito nessa parte da entrega;
  o requisito relevante e ser Mortal.
- O NPC `Cap.Explor.` estava sem tratamento visual/click adequado em alguns fluxos:
  - `GetFunc.cpp` so rotulava `Cap.Mercenario` como `Quest Amuleto`;
  - `_MSG_REQShopList.cpp` nao redirecionava merchant `11` para `_MSG_Quest`;
  - `_MSG_Attack.cpp` podia tratar o NPC como alvo de ataque/interacao comum.

Solucao aplicada:

- `GetFunc.cpp`: `Cap.Explor.` tambem recebe o texto de aba `Quest Amuleto`.
- `_MSG_REQShopList.cpp`: merchant `11` redireciona para `Exec_MSG_Quest`.
- `_MSG_Attack.cpp`: se o alvo for `Cap.Explor`/`Cap.Explor.` ou merchant `11`,
  cancela o ataque e chama `Exec_MSG_Quest`.
- `_MSG_Quest.cpp`:
  - no inicio da quest (`AMU_MISTICO`), personagem nao-Mortal recebe:
    `Quest Amuleto: apenas personagens Mortais podem fazer esta quest.`;
  - na entrega (`EXPLOIT_LEADER`), personagem nao-Mortal recebe:
    `Quest Amuleto: apenas personagens Mortais podem receber este amuleto.`.

Onde conferir:

- `Source/Code/TMSrv/GetFunc.cpp`
- `Source/Code/TMSrv/_MSG_REQShopList.cpp`
- `Source/Code/TMSrv/_MSG_Attack.cpp`
- `Source/Code/TMSrv/_MSG_Quest.cpp`
- Pacotes gerados: `deploy-tmsrv-amuleto-fix-v2.zip`,
  `deploy-tmsrv-amuleto-fix-v3.zip`, `deploy-tmsrv-amuleto-fix-v4.zip`.
- Na VPS, os scripts criam backups `TMSrv.exe.bak_amuleto_azran_v*_YYYYMMDD_HHMMSS`
  antes de substituir o executavel.

Validacao:

- Personagem Mortal conclui a quest e recebe o amuleto.
- Personagem nao-Mortal deixa de receber apenas "Sol" e passa a receber aviso claro
  de que a quest do amuleto e exclusiva para Mortais.
- Hotfix v4 aplicado pelo pacote zipado com extracao automatica no PowerShell.

## Como atualizar este relatorio

Ao resolver um novo problema, adicionar uma entrada com:

- data;
- sintoma;
- causa encontrada;
- solucao aplicada;
- arquivos alterados;
- backups criados;
- como validar.

Se ja existir um relatorio especifico grande, manter aqui apenas o resumo e o
caminho do arquivo detalhado.

## Criar Arch: "Falha ao criar personagem" (DBSrv x MariaDB 12.3)

- Sintoma: criar o Arch (quest do Rei, mortal Lv380+) dava "Falha ao criar personagem. Tente outro nome ou avise o suporte." na VPS. Funcionava antes de migrar pra VPS. Imortalidade/traje sumiam visualmente mas voltavam ao relogar (dessync do cliente, sem perda real).
- Causa: o handler do arch no DBSrv (_MSG_DBCreateArchCharacter, CFileDB.cpp ~2161) faz um SELECT em accounts que o createchar normal nao faz. O DBSrv nao conectava no MySQL: a LIBMYSQL.dll do DBSrv/run era a MySQL velha (1,4 MB), incompativel com o MariaDB 12.3 da VPS -> mysql_real_connect falhava -> "arch fail: accounts query NULL". O TMSrv funcionava por usar libmariadb.dll.
- Solucao: copiar o libmariadb.dll do TMSrv COMO LIBMYSQL.dll no DBSrv/run (renomeado) e reiniciar DBSrv+TMSrv. NAO usar o forwarder de 2048 bytes do TMSrv (falta mysql_autocommit que o DBSrv usa -> "entry point not found"). Sem recompilar. Tambem conserta o UPDATE accounts SET online do login.
- Onde conferir: Source/Code/DBSrv/CFileDB.cpp (handler ~2161, logs "arch fail: ..." adicionados); Source/Code/DBSrv/dbMySQL.cpp (wStart/wRes). Memoria: newworld-arch-dbsrv-libmysql.
- Validacao: criar Arch com mortal Lv400 -> cria normal. DBSrv sobe sem janela "entry point not found" e a 7514 abre.
- Pendencia: atualizar o pacote de deploy local (Server/DBSrv/run/libmysql.dll) pro libmariadb, senao um redeploy do DBSrv quebra de novo.

## 2026-06-16 - Falha ao apagar personagem da conta

- Sintoma: ao tentar apagar personagem na tela da conta, o cliente retornava apenas
  falha generica.
- Causa: o DBSrv ainda tinha travas antigas de exclusao por classe/level e dependia
  do estado `SecurePass`; alem disso, o TMSrv apagava a linha do MySQL
  `characteres` antes do DBSrv confirmar a exclusao real do arquivo da conta.
- Solucao: DBSrv passa a permitir apagar qualquer personagem da propria conta quando
  a senha da conta estiver correta; falhas agora retornam motivo para o TMSrv
  informar slot invalido, conta nao encontrada, senha incorreta ou slot vazio. O
  TMSrv so sincroniza/remover o registro em `characteres` depois da confirmacao
  `_MSG_DBCNFDeleteCharacter`.
- Arquivos alterados:
  `Source/Code/DBSrv/CFileDB.cpp`,
  `Source/Code/TMSrv/_MSG_DeleteCharacter.cpp`,
  `Source/Code/TMSrv/ProcessDBMessage.cpp`.
- Build: `DBSrv.exe` e `TMSrv.exe` Release Win32 compilados com MSBuild; 0 erros,
  apenas warnings antigos do projeto.
- Pacote de entrega: `runtime/ENTREGAR-VPS/APAGAR-PERSONAGEM-FIX-20260616/`.
- Validacao esperada: apagar personagem com senha correta remove o slot; com senha
  incorreta, o jogo informa `Falha ao apagar personagem: senha incorreta.`.

Atualizacao v2:

- O teste real ainda retornou falha. Confirmado no protocolo que o pacote de apagar
  usa `Password[12]`, enquanto a senha numerica usa outro campo (`NumericToken[6]`).
  Como o cliente pode exibir/truncar esse campo de forma ambigua, o DBSrv v2 deixou
  de bloquear a exclusao por senha e passou a confiar na sessao ja autenticada,
  mantendo validacao de slot/personagem.
- Novo pacote: `runtime/ENTREGAR-VPS/APAGAR-PERSONAGEM-FIX-V2-20260616/`.
- VPS: pacote v2 aplicado em 2026-06-16, backup criado em
  `C:\WYDKingdom\backups\delete-character-fix-v2_20260616_171919`.
- Validacao real: personagem apagou corretamente no jogo apos instalar o v2.

## 2026-06-17 - Balanceamento da honra e loja Victor_Honra

- Decisao de balanceamento: ganho passivo da lojinha/autotrade reduzido de
  `1 honra / 10 minutos` para `1 honra / 30 minutos`, caindo de 144 honra/dia
  para 48 honra/dia em uso continuo.
- Causa do ajuste: a loja de honra tinha varios itens de progressao e consumo
  com preco baixo demais para uma moeda gerada passivamente.
- Codigo alterado:
  `Source/Code/TMSrv/ProcessSecMinTimer.cpp` e
  `Source/Code/TMSrv/_MSG_SendAutoTrade.cpp`.
- NPC alterado: `Server/TMSrv/run/npc/Victor_Honra`.
- Loja ajustada:
  precos principais triplicados; Classe E corrigida para 20 unidades e 6 honra;
  Medalha/Moeda de Honra mantida em 50 honra; joias removidas; ovos de Andaluz
  removidos; adicionados Ovo Dente de Sabre, Ovo Pantera Negra, Amago Pantera
  Negra x5 e Amago Dente de Sabre x5; ovos de Cavalo Equip N/B ajustados para
  100 honra.
- Build: `TMSrv.exe` Release Win32 compilado em 2026-06-17 com MSBuild; 0 erros,
  apenas warnings antigos do projeto.
- Pacote de entrega:
  `runtime/ENTREGAR-VPS/BALANCEAMENTO-HONRA-LOJA-V1-20260617/`.
- ZIP:
  `WYD-Kingdom-Balanceamento-Honra-Loja-V1-20260617.zip`.
- Validacao local: ZIP expandido em pasta temporaria; `INSTALAR-NA-VPS.ps1`
  parseado sem erro; `npc/Victor_Honra` conferido slot a slot com os novos
  IDs, quantidades e custos de honra.

Atualizacao v2:

- Ajuste pontual solicitado em 2026-06-17: `Ovo_de_Cavalo_Equip_N` e
  `Ovo_de_Cavalo_Equip_B` passaram de 100 honra para 150 honra.
- Novo pacote:
  `runtime/ENTREGAR-VPS/BALANCEAMENTO-HONRA-LOJA-V2-20260617/`.
- Novo ZIP:
  `WYD-Kingdom-Balanceamento-Honra-Loja-V2-20260617.zip`.

## 2026-06-17 - Drop List: Lich_Crunt ausente em Kefra

- Sintoma: `Lich_Crunt` existia nos arquivos de mob/drop, mas nao aparecia na
  lista de mobs da regiao Kefra no painel `Drop List`.
- Causa: o painel le a tabela SQL `droplist` por regiao. O mob estava com linha
  local em dump antigo usando regiao `12`, mas no fluxo do cliente Kefra e a
  regiao `8`; alem disso, a consulta de detalhe era sensivel a diferenca entre
  `Lich_Crunt` e `Lich Crunt`.
- Solucao aplicada no `Source/Code/TMSrv/DropList.cpp`:
  - a listagem por regiao passou a usar `ORDER BY nome ASC LIMIT 30`;
  - `Lich_Crunt` e forçado na lista quando a regiao solicitada e `8` (Kefra);
  - a consulta de detalhe aceita nome com underscore ou espaco;
  - foi adicionado fallback dos itens conhecidos do `Lich_Crunt` caso a linha
    do banco esteja ausente/defasada.
- Build: `TMSrv.exe` Release Win32 compilado em 2026-06-17 com MSBuild; 0 erros,
  apenas warnings antigos do projeto.
- Pacote que confirmou a correcao no jogo:
  `runtime/ENTREGAR-VPS/DROPLIST-KEFRA-UI-V3-20260617/`.
- Validacao real: `Lich_Crunt` apareceu no `Drop List` de Kefra.

## 2026-06-17 - Drop List: textos fora dos botoes azuis

- Sintoma: os nomes das regioes/mobs do `Drop List` apareciam fora dos botoes
  azuis. Em uma tentativa anterior, os textos apenas foram empurrados para outra
  posicao, sem ficarem presos ao botao correto.
- Causa: o painel `Drop List`, o painel de comandos e o fallback de ranking
  reaproveitam os mesmos handles (`179501..179520`). A correcao anterior usava
  coordenadas absolutas nos textos `179511..179520`, entao o texto nao seguia o
  retangulo azul real carregado pela UI.
- Solucao aplicada no `Source/Code/ClientPatch_v759/ButtonControl.cpp` e em
  `Source/Code/ClientPatch_v759/CClientInfo.cpp`:
  - cada texto `179511 + linha` agora e ancorado no botao `179501 + linha`;
  - `Left`, `Width`, `ViewLeft`, `ViewWidth` e `PosX` do texto sao calculados
    a partir do proprio botao da linha;
  - nao foram alterados `Top`, `Height`, `ViewTop` nem `ViewHeight`, porque essa
    foi a causa do bug visual dos blocos brancos em tentativa anterior.
- Build: `ClientPatch.dll` Release Win32 compilado com MSBuild usando toolset
  `v145`; 0 erros, apenas warnings antigos do projeto.
- Pacote confirmado no jogo:
  `runtime/ENTREGAR-VPS/DROPLIST-TEXTO-BOTOES-V4-20260617/`.
- Validacao real: nomes das regioes/mobs passaram a aparecer dentro dos botoes
  azuis.

Atualizacao v5:

- Sintoma restante: o numero da pagina (`179521`) continuou fora do retangulo
  entre as setas de pagina.
- Causa: o handle da pagina ainda usava coordenada fixa antiga (`315px`) em vez
  de seguir a posicao real dos botoes de seta.
- Solucao preparada: o `ClientPatch` passa a calcular a area da pagina usando as
  setas `179530` e `179531`, posicionando `179521` exatamente entre elas.
- Build: `ClientPatch.dll` Release Win32 recompilado em 2026-06-17; 0 erros,
  apenas warnings antigos do projeto.
- Pacote de entrega:
  `runtime/ENTREGAR-VPS/DROPLIST-PAGINA-V5-20260617/`.
- ZIP:
  `WYD-Kingdom-DropList-Pagina-V5-20260617.zip`.

## 2026-06-17 - Remocao do drop de Ovo/Amago de Fenrir

- Pedido: remover `Ovo_de_Fenrir` e `Amago_de_Fenrir` dos Taurons e de outros
  pontos antigos onde ainda existiam como drop/recompensa.
- IDs removidos:
  - `2316` = `Ovo_de_Fenrir`.
  - `2406` = `Amago_de_Fenrir`.
- Fontes originais registradas para restauracao futura:
  - `Ladrao_Tauron`: slots `16=2316`, `59=2316`, `60=2406` no arquivo
    `Server/TMSrv/run/npc/Ladrao_Tauron`; no SQL tambem estavam nos slots
    `16`, `59` e `60`. No arquivo gerado
    `Server/TMSrv/run/Drop/Zona_Desconhecida.txt`, a linha antiga era
    `2316,2406,`.
  - `Adamant_Tauron`: slots `58=2406` e `61=2316` no arquivo
    `Server/TMSrv/run/npc/Adamant_Tauron`; no SQL tambem estavam nos slots
    `58` e `61`. No arquivo gerado `Zona_Desconhecida.txt`, os IDs apareciam
    no trecho `...,1664,2406,663,2443,2316,413,`.
  - `Taron_Assassino` (nome visto pelo jogador como Tauron Assassino): slots
    `58=2406` e `60=2316` no arquivo
    `Server/TMSrv/run/npc/Taron_Assassino`; no SQL tambem estavam nos slots
    `58` e `60`. No arquivo gerado `Zona_Desconhecida.txt`, os IDs apareciam
    no trecho `...,419,2406,661,2316,`.
  - `BETA_G`: slot `7=2406` no arquivo `Server/TMSrv/run/npc/BETA_G`.
  - `GO!_Honra/Event.txt`: a terceira troca tinha a linha `2406 1` como
    recompensa de `Amago_de_Fenrir`; foi substituida por item `0 0`.
- Solucao aplicada:
  - zerados os slots dos itens `2316` e `2406` nos arquivos binarios dos mobs;
  - removidos os IDs dos dumps `Server/banco.sql` e `Server/wydsite-dump.sql`;
  - ajustado `Server/TMSrv/run/Drop/Zona_Desconhecida.txt`;
  - limpas as listas estaticas `MobDropList.txt` e `ItemDropList1.txt` para nao
    mostrarem mais esses drops antigos;
  - preparado instalador para copiar os arquivos e aplicar `UPDATE` na tabela
    `wydsite.droplist` da VPS.
- Como voltar no futuro: recolocar os IDs nos mesmos slots acima, restaurar a
  linha `2406 1` no `Event.txt` se quiser reativar a recompensa do GO!_Honra,
  copiar os arquivos para a VPS e atualizar a tabela `droplist` com os mesmos
  slots.

## 2026-06-17 - Remocao de ovos/amagos Andaluz, Unicornio, Pegasus e Unisus

- Pedido: remover drops de ovo e amago de Andaluz, Unicornio, Unisus e Pegasus,
  incluindo `Lich_Crunt` e qualquer outro mob/fonte antiga.
- IDs tratados:
  - `2310` = `Ovo_de_Andaluz_N`.
  - `2315` = `Ovo_de_Andaluz_B`.
  - `2321` = `Ovo_de_Unicornio`.
  - `2322` = `Ovo_de_Pegasus`.
  - `2323` = `Ovo_de_Unisus`.
  - `2400` = `Amago_de_Andaluz_N`.
  - `2405` = `Amago_de_Andaluz_B`.
  - `2411` = `Amago_de_Unicornio`.
  - `2412` = `Amago_de_Pegasus`.
  - `2413` = `Amago_de_Unisus`.
- Observacao importante: os ovos de Andaluz (`2310`, `2315`) nao estavam mais
  em NPC ativo; ainda apareciam como residuo na lista estatica
  `ItemDropList1.txt` apontando para `Homem_Kalintz` e `Mulher_Kalintz`. A lista
  foi limpa para nao mostrar drop inexistente.
- Fontes originais registradas para restauracao futura:
  - `Aranha_Venenosa`: slots `42=2400`, `45=2400`.
  - `BETA_G`: slots `5=2400`, `6=2405`, `54=2411`.
  - `Cria_de_Aranha`: slot `47=2405`.
  - `Demonio_do_Vale`: slots `42=2400`, `43=2405`, `44=2405`, `45=2400`.
  - `EntAmald`: slots `34=2400`, `35=2405`.
  - `KalintzAmaldM`: slots `35=2400`, `36=2405`.
  - `KalintzAmaldW`: slots `35=2400`, `36=2405`.
  - `Lich_Batama`: slots `16=2400`, `17=2405`.
  - `Lich_Crunt`: slots `16=2400`, `17=2405`.
  - `Lich_Infernal`: slots `42=2400`, `43=2405`, `44=2405`, `45=2400`.
  - `LoboAmald`: slots `34=2400`, `35=2405`.
  - `PerBerserker_`: slots `18=2411`, `19=2411`, `25=2411`.
  - `PerValkyrie_`: slots `18=2412`, `19=2412`, `25=2412`.
  - `PreKalintz_H_`: slots `18=2413`, `19=2413`, `25=2413`.
  - `PreKalintz_M`: slots `18=2413`, `19=2413`, `25=2413`.
  - `Test`: slots `27=2411`, `28=2412`, `29=2413`.
- Fontes de evento/banco tambem limpas:
  - tabela `evento`, NPC `GUARDA_REAL_II`: amagos de Andaluz em linhas antigas.
  - tabela `evento`, NPC `GUARDA_REAL_III`: amagos de Unisus, Pegasus e
    Unicornio em linhas antigas.
- Solucao aplicada:
  - zerados os slots dos itens nos arquivos binarios dos NPCs ativos;
  - atualizados `Server/banco.sql` e `Server/wydsite-dump.sql`;
  - atualizada a tabela `evento` nos dumps e no SQL de aplicacao;
  - removidos os IDs do arquivo gerado `Server/TMSrv/run/Drop/Zona_Desconhecida.txt`;
  - limpas as listas estaticas `MobDropList.txt` e `ItemDropList1.txt`.
- Como voltar no futuro: recolocar os IDs nos mesmos slots acima nos arquivos
  `Server/TMSrv/run/npc/<mob>`, atualizar os mesmos slots na tabela `droplist`,
  restaurar as linhas da tabela `evento` se quiser que os guardas reais voltem a
  entregar esses amagos e regenerar/ajustar `MobDropList.txt` e
  `ItemDropList1.txt`.
- Pacote de entrega:
  `runtime/ENTREGAR-VPS/REMOVER-DROP-ANDALUZ-UNICORNIO-V1-20260617/`.
- ZIP:
  `WYD-Kingdom-Remover-Drop-Andaluz-Unicornio-V1-20260617.zip`.

## 2026-06-17 - Lojinha (auto trade) presa apos trocar de personagem / relogar

- Sintoma: ao abrir uma lojinha e depois "dar personagem" (voltar para a
  selecao) ou fechar o cliente, ao voltar a lojinha ficava bugada: nao abria
  mais a vitrine para visualizar/comprar os itens, mas continuava contando o
  tempo e entregando honra normalmente.
- Causa: no logout/troca de personagem o servidor nao encerrava o estado da
  loja. `CloseUser` so removia o proxy (mob) da loja, sem limpar a estrutura
  `AutoTrade`/`TradeMode` do personagem; `CharLogOut` nem chamava a limpeza da
  loja antes de salvar e voltar para a selecao. O personagem voltava com estado
  "fantasma" de loja (sem o mob da vitrine), mas o timer de honra (que so checa
  `IsAutoTradeActive`) continuava rodando.
- Solucao: criada a funcao `ClearAutoTradeStoreState(conn)` em
  `Source/Code/TMSrv/Server.cpp`, que zera a estrutura `AutoTrade`, reseta os
  slots (`CarryPos = -1`), remove o proxy, zera `CheckLojinha` e `TradeMode`
  (no logout apenas reseta o estado, sem enviar sinal ao cliente). Passou a ser
  chamada em `CloseUser`, `CharLogOut` e `RemoveTrade`. A troca normal entre
  players NAO foi afetada (a limpeza so mexe no estado de auto trade/lojinha).
- Fechar a loja manualmente: comando de chat `/closeloja` (tambem `/fecharloja`
  e `/lojafechar`) -> chama `RemoveTrade` -> "Lojinha fechada.". Permite abrir
  outra loja com itens diferentes. Tratado em
  `Source/Code/TMSrv/_MSG_MessageChat.cpp` e `_MSG_MessageWhisper.cpp`.
- Build: TMSrv.exe Release Win32 (0 erros).
- Pacote de entrega:
  `runtime/ENTREGAR-VPS/LOJINHA-FIX-RELOG-V1-20260617/`.
- ZIP:
  `WYD-Kingdom-Lojinha-Fix-Relog-V1-20260617.zip`.
- Pasta para copiar p/ VPS:
  `runtime/ENTREGAR-VPS/COPIAR-VPS-LOJINHA-FIX-RELOG-V1-20260617/`.

## 2026-06-17 - Ranking: centralizar lista e tirar admins (Ranking Fix V1)

- Pedido: no painel de Ranking a lista estava em cima dos botoes azuis (devia
  ficar centralizada na area cinza, igual ao /comandos); e os personagens de
  admin/GM apareciam no ranking (devia mostrar so players normais).
- Cliente (ClientPatch 1.1.19): `Source/Code/ClientPatch_v759/ButtonControl.cpp`.
  A funcao `ApplyCommandsTextCenteredLayout(float ColWidth)` centra os rotulos
  179511-179520 na area cinza (grid 179600). Chamada no /comandos (ColWidth 230)
  e agora tambem no `RenderRankingFallbackPage` (ColWidth 250). O Drop List
  continua com os textos sobre os botoes azuis. Ranking usa o painel fallback
  179500 (mesmo do /comandos), nao o 1919400.
- Servidor (TMSrv): `Source/Code/TMSrv/Ranking.cpp`. As duas queries de
  `sendRanking` (state 0 = level/evolucao, state 1 = PvP/frags) ganharam
  `WHERE nick NOT LIKE` para `-ADM-%`, `ADM-%`, `[ADM]%`, `-GM-%`, `GM-%`,
  `[GM]%`. Staff sempre tem esse prefixo no nome (aplicado por
  `ApplyAccountStaffPrefix`; player normal e bloqueado de usar prefixo ao criar
  o char), entao o filtro exclui so staff e mantem todos os players normais.
- Este TMSrv.exe tambem inclui o fix da lojinha (relog) ja descrito acima.
- Build: ClientPatch.dll (v145) e TMSrv.exe (Release Win32), 0 erros.
- Como voltar: tirar as chamadas `ApplyCommandsTextCenteredLayout` (cliente) e
  remover o `WHERE nick NOT LIKE ...` das queries (servidor). ColWidth ajustavel
  para mexer na largura/posicao do bloco.
- Pacote unico (faz servidor + cliente):
  `runtime/ENTREGAR-VPS/RANKING-FIX-V1-20260617/`.
- ZIP: `WYD-Kingdom-Ranking-Fix-V1-20260617.zip`.

## REFERENCIA - Como cada painel DEVE ficar (estado final 2026-06-17)

Documentacao do layout/comportamento correto dos paineis, para nao quebrar de
novo. Tudo no cliente fica em `Source/Code/ClientPatch_v759`; recompilar a DLL
(toolset v145) e publicar como patch (publicar-patch.ps1 -> ClientPatch X.Y.Z).

- **Painel D (tecla D, handle 981300):** painel de BOTOES. Handler de clique
  `HKD_ControlEventClick` em `ButtonControl.cpp`. Botoes: 981301-981305 cidades
  (Sendteleport), 981306 Droplist, 981307 Filtro, 981308 Donate Store, 981309
  Painel Pix, 981310 Modo Foto (toggle igual tecla U), 981311 Modo Stream (sem
  funcao por enquanto). Ver [[newworld-painel-d-handles]].

- **Painel compartilhado 179500** (Droplist / Ranking / Comandos usam o MESMO
  painel: 10 botoes azuis 179501-510, 10 rotulos 179511-520, titulo/pagina
  179521, setas 179530/531, grid 179600 na area cinza). Cada modo posiciona os
  rotulos:
  - **DROPLIST**: os nomes dos mobs ficam EM CIMA dos botoes azuis (a esquerda).
    A pagina "X/Y" fica no retangulo entre as setas. Render do nome dos mobs:
    `CClientInfo.cpp` `_MSG_SendDListNames` -> `ApplyRankingFallbackLayout()`
    (ancora rotulo no botao: `Button->Left + 7`). NAO centralizar.
  - **COMANDOS** (/comandos, painel de comandos): os 10 textos ficam
    CENTRALIZADOS na area cinza. `ButtonControl.cpp` `OpenCommandsPanel` ->
    `ApplyCommandsTextCenteredLayout(230)`.
  - **RANKING**: os 10 nomes + o titulo "Ranking Level/PvP X/5" (179521) ficam
    CENTRALIZADOS na area cinza. `CClientInfo.cpp` `OpenRankingFallbackPanel` ->
    `ApplyRankingCenteredLayout()` (centra 179511-520 e 179521, ColWidth 250).
    Admins/GM (nick com prefixo -ADM-/-GM-/etc) NAO aparecem (filtro no
    servidor `Ranking.cpp` `sendRanking`, `WHERE nick NOT LIKE '-ADM-%%'...` —
    o `%` tem que ser `%%` por causa do sprintf).
  - ⚠️ ARMADILHA: Droplist e Ranking compartilhavam `ApplyRankingFallbackLayout`
    em `CClientInfo.cpp`. Centralizar essa funcao quebra o Droplist. Por isso
    existem DUAS: `ApplyRankingFallbackLayout` (ancora nos botoes = Droplist) e
    `ApplyRankingCenteredLayout` (centraliza = Ranking). Ver [[newworld-ranking]].
  - LIMITE: o painel tem so 1 controle de texto (179521) para titulo E pagina;
    nao da pra ter o titulo alinhado com os nomes E a pagina isolada no
    retangulo ao mesmo tempo sem criar um 2o controle (helpers addTitulo/addButton
    do Gui.h existem mas nunca foram testados -> risco de crash).

## 2026-06-18 - Relatorio de Eventos e Drops da Pista

- Levantados do codigo (TMSrv) os horarios de todos os eventos/guerras e os
  drops dos mobs da Pista (RuneQuest). Documento completo:
  **`RELATORIO-EVENTOS-E-DROPS-PISTA.md`** (raiz do NewWorld).
- Resumo dos horarios (hora do servidor):
  - **Guerra de Cidades** (guild war): DOMINGO 20h (Erion 20:05, Nipplehein
    20:20, Arzan 20:35, Armia 20:50; cada uma ~10 min). `GuildHour=20` fixo.
  - **Guerra de Noatum** (castelo/torres): DOMINGO 14h (abre ~14:05, fecha ~14:55).
  - **Torre de Guerra** (GTorre): dias uteis, hora `GTorreHour` (config no banco;
    historico 21h), :00 aviso / :05 inicia / :35 fim (+100 fama). So com
    NewbieEventServer=1 e GTorreStatus=1.
  - **Coliseu**: dias uteis, M 02h / A 03h / N 04h (fixos ColoHour*).
  - **Batalha Real**: Seg-Sab, registro a cada hora :00 (se BatalhaRealStatus=1).
  - **Bosses**: Kefra Ter 19h; Barao/Ent Obscuro Seg-Sex 20h; Sombra Negra
    Seg-Sex 0/4/8/12/16/20h; Mirror Queen Seg-Sex 19:40.
  - **Double XP**: fim de semana (sab 00:00 liga / seg 00:00 desliga).
- Drops da Pista (Lugefer_Inf, Amon_Inf, etc.): tabela no relatorio dedicado;
  saem do Carry do npc (offset 460+). Pedra do Lugefer = item 1758.

## 2026-06-18 - Kefra: spawn corrigido + penalidade XP/2 removida

- Sintoma: Kefra nao nascia na terca. Causa: o spawn (terca 19h, `ProcessSecMinTimer.cpp`)
  tinha gate `&& KefraLive`, que falha quando `KefraLive==0` (zera em todo restart do
  TMSrv) -> nao spawnava. Corrigido: guard por dia (`g_KefraLastSpawnYday != tm_yday`),
  nasce toda terca 19h independente de restart, com `SendNotice`.
- Mecanica de XP do Kefra: enquanto `KefraLive==0` (nao morto) o servidor tinha TODO o
  XP de mob pela METADE (`exp /= 2` em 3 pontos do `MobKilled.cpp`); matar dava +10 XP
  (`SendFunc.cpp`) e +100 fama p/ a guild. A pedido do usuario: **removida a penalidade
  XP/2** (XP normal sempre; matar Kefra so adiciona o +10). 
- Drops do Kefra: 413, 1740, 1741, 1760-1763, 3172, 3140 (ver RELATORIO-EVENTOS-E-DROPS-PISTA.md).
- Pacote: `runtime/ENTREGAR-VPS/KEFRA-FIX-20260618/` (TMSrv + npc; inclui tudo acumulado).
