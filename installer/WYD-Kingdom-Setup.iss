; ============================================================
; Instalador oficial do WYD Kingdom
; Compilar: ISCC.exe WYD-Kingdom-Setup.iss
; - Instala o cliente completo (pasta Cliente)
; - Atalhos (Desktop + Menu Iniciar) apontam pro Launcher
; - Instala o .NET 8 Desktop Runtime se faltar
; - Pasta com permissao de escrita p/ o auto-update do launcher
; ============================================================

#define AppName "WYD Kingdom"
#define AppVersion "1.1.22"
#define AppPublisher "WYD Kingdom"
#define AppURL "https://wydkingdom.com.br"
#define LauncherExe "Launcher WYD Kingdom.exe"
#define ClientDir "..\Cliente"

[Setup]
AppId={{B7E25A41-9C1D-4F6B-9C73-3A8D54E0F127}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
DefaultDirName={sd}\WYD Kingdom
DisableProgramGroupPage=yes
DirExistsWarning=no
PrivilegesRequired=admin
OutputDir=..\runtime\deploy
OutputBaseFilename=WYD-Kingdom-Setup
SetupIconFile=..\Launcher\app-icon.ico
UninstallDisplayIcon={app}\{#LauncherExe}
UninstallDisplayName={#AppName}
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
ShowLanguageDialog=no

[Languages]
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"

[Files]
; Cliente completo (sem logs/backups/lixo)
Source: "{#ClientDir}\*"; DestDir: "{app}"; \
    Excludes: "*.bak*,*.log,*.old,*.download,*.tmp,*.dmp,*.pdb,unins*.exe,unins*.dat,launcher-user.txt,_snlog.txt,WYD.log,createchar-send.log,autotrade-click.log,Logs\*,Log\*,Screenshots\*"; \
    Flags: recursesubdirs createallsubdirs ignoreversion

; .NET 8 Desktop Runtime (instala so se faltar)
Source: "redist\windowsdesktop-runtime-8-win-x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall

[Dirs]
; O launcher atualiza o jogo sozinho -> a pasta precisa ser gravavel sem admin
Name: "{app}"; Permissions: users-modify

[Icons]
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\{#LauncherExe}"; WorkingDir: "{app}"
Name: "{autoprograms}\{#AppName}"; Filename: "{app}\{#LauncherExe}"; WorkingDir: "{app}"

[Run]
Filename: "{tmp}\windowsdesktop-runtime-8-win-x64.exe"; \
    Parameters: "/install /quiet /norestart"; \
    StatusMsg: "Instalando o .NET Desktop Runtime 8 (necessario para o launcher)..."; \
    Check: not IsDotNet8DesktopInstalled
Filename: "{app}\{#LauncherExe}"; Description: "Abrir o {#AppName} Launcher"; Flags: nowait postinstall skipifsilent

[Code]
function IsDotNet8DesktopInstalled: Boolean;
var
  FindRec: TFindRec;
begin
  Result := FindFirst(ExpandConstant('{commonpf64}\dotnet\shared\Microsoft.WindowsDesktop.App\8.*'), FindRec);
  if Result then
    FindClose(FindRec);
end;
