<#
.SYNOPSIS
    Script de build, teste e empacotamento do zx81ide.

.DESCRIPTION
    Este script orquestra a detecção de ferramentas, compilação em C++17,
    população do diretório dist/, geração do pacote ZIP zx81ide-v-X.Y.Z.zip
    e opcionalmente a criação do Release no GitHub via gh CLI.

.PARAMETER Clean
    Limpa os diretórios build/ e dist/ antes de iniciar.

.PARAMETER Release
    Compila com otimizações em modo Release.

.PARAMETER CreateGitHubRelease
    Cria automaticamente a tag git e a Release no GitHub com as notas do RELEASE.md.

.EXAMPLE
    .\build.ps1
    .\build.ps1 -Clean -Release
    .\build.ps1 -CreateGitHubRelease
#>

[CmdletBinding()]
param(
    [switch]$Clean,
    [switch]$Release,
    [switch]$CreateGitHubRelease
)

$ErrorActionPreference = "Stop"

function Write-Step {
    param([string]$Message)
    Write-Host "`n[zx81ide-build] ===> $Message" -ForegroundColor Cyan
}

function Write-Success {
    param([string]$Message)
    Write-Host "[zx81ide-build] [OK] $Message" -ForegroundColor Green
}

function Write-Warn {
    param([string]$Message)
    Write-Host "[zx81ide-build] [WARN] $Message" -ForegroundColor Yellow
}

function Write-Failure {
    param([string]$Message)
    Write-Host "[zx81ide-build] [ERR] $Message" -ForegroundColor Red
}

$RootDir = $PSScriptRoot
if (-not $RootDir) { $RootDir = (Get-Location).Path }

Set-Location $RootDir

Write-Host "================================================================" -ForegroundColor DarkCyan
Write-Host "             zx81ide - Sistema de Build & Release              " -ForegroundColor White
Write-Host "================================================================" -ForegroundColor DarkCyan

# ---------------------------------------------------------------------------
# 1. Extração de Versão do Cabeçalho C++ (include/core/version.h)
# ---------------------------------------------------------------------------
Write-Step "Lendo configuracoes de versao de include/core/version.h..."

$VersionHeader = Join-Path $RootDir "include/core/version.h"
if (-not (Test-Path $VersionHeader)) {
    throw "Arquivo $VersionHeader nao encontrado!"
}

$HeaderContent = Get-Content $VersionHeader -Raw

$Major = 0
$Minor = 1
$Patch = 0
$MajorName = "Sinclair ZX81"
$GameCodename = "3D Monster Maze"
$BuildSubtitle = "Build"

if ($HeaderContent -match 'constexpr\s+int\s+VERSION_MAJOR\s*=\s*(\d+);') { $Major = $matches[1] }
if ($HeaderContent -match 'constexpr\s+int\s+VERSION_MINOR\s*=\s*(\d+);') { $Minor = $matches[1] }
if ($HeaderContent -match 'constexpr\s+int\s+VERSION_PATCH\s*=\s*(\d+);') { $Patch = $matches[1] }
if ($HeaderContent -match 'VERSION_MAJOR_NAME\s*=\s*"([^"]+)";') { $MajorName = $matches[1] }
if ($HeaderContent -match 'VERSION_GAME_CODENAME\s*=\s*"([^"]+)";') { $GameCodename = $matches[1] }
if ($HeaderContent -match 'VERSION_BUILD_SUBTITLE\s*=\s*"([^"]+)";') { $BuildSubtitle = $matches[1] }

$Version = "$Major.$Minor.$Patch"
$TagName = "v$Version"
$ZipName = "zx81ide-v-$Version.zip"

Write-Host "Versao detectada : $Version (Tag: $TagName)" -ForegroundColor Yellow
Write-Host "Modelo Base (X)  : $MajorName" -ForegroundColor Yellow
Write-Host "Jogo (Y)         : $GameCodename" -ForegroundColor Yellow
Write-Host "Subtitulo (Z)    : $BuildSubtitle" -ForegroundColor Yellow
Write-Host "Arquivo Zip      : $ZipName" -ForegroundColor Yellow

# ---------------------------------------------------------------------------
# 2. Limpeza (opcional)
# ---------------------------------------------------------------------------
$BuildDir = Join-Path $RootDir "build"
$DistDir = Join-Path $RootDir "dist"

if ($Clean) {
    Write-Step "Limpando artefatos anteriores (-Clean)..."
    if (Test-Path $BuildDir) { Remove-Item $BuildDir -Recurse -Force | Out-Null }
    if (Test-Path $DistDir) { Remove-Item $DistDir -Recurse -Force | Out-Null }
    $ExistingZip = Join-Path $RootDir $ZipName
    if (Test-Path $ExistingZip) { Remove-Item $ExistingZip -Force | Out-Null }
    Write-Success "Diretorios limpos com sucesso."
}

# ---------------------------------------------------------------------------
# 3. Verificação de Compiladores e Ferramentas
# ---------------------------------------------------------------------------
Write-Step "Verificando ferramentas de compilacao..."

$HasCMake = (Get-Command "cmake" -ErrorAction SilentlyContinue) -ne $null
$HasGpp = (Get-Command "g++" -ErrorAction SilentlyContinue) -ne $null
$HasClang = (Get-Command "clang++" -ErrorAction SilentlyContinue) -ne $null
$HasGit = (Get-Command "git" -ErrorAction SilentlyContinue) -ne $null
$HasGh = (Get-Command "gh" -ErrorAction SilentlyContinue) -ne $null

Write-Host "CMake     : $(if ($HasCMake) { '[OK]' } else { '[NAO ENCONTRADO]' })" -ForegroundColor $(if ($HasCMake) { 'Green' } else { 'Yellow' })
Write-Host "GCC (g++) : $(if ($HasGpp) { '[OK]' } else { '[NAO ENCONTRADO]' })" -ForegroundColor $(if ($HasGpp) { 'Green' } else { 'Yellow' })
Write-Host "Clang++   : $(if ($HasClang) { '[OK]' } else { '[NAO ENCONTRADO]' })" -ForegroundColor $(if ($HasClang) { 'Green' } else { 'Yellow' })
Write-Host "Git       : $(if ($HasGit) { '[OK]' } else { '[NAO ENCONTRADO]' })" -ForegroundColor $(if ($HasGit) { 'Green' } else { 'Yellow' })
Write-Host "GitHub CLI: $(if ($HasGh) { '[OK]' } else { '[NAO ENCONTRADO]' })" -ForegroundColor $(if ($HasGh) { 'Green' } else { 'Yellow' })

if (-not $HasCMake -and -not $HasGpp -and -not $HasClang) {
    throw "Nenhum compilador C++ ou CMake encontrado no PATH. Instale GCC (MSYS2) ou Clang."
}

# ---------------------------------------------------------------------------
# 4. Compilação
# ---------------------------------------------------------------------------
Write-Step "Iniciando compilacao do zx81ide..."

if (-not (Test-Path $DistDir)) { New-Item -ItemType Directory -Path $DistDir -Force | Out-Null }

$TargetExe = Join-Path $DistDir "zx81ide.exe"

if ($HasCMake) {
    Write-Host "Compilando via CMake..." -ForegroundColor Gray
    if (-not (Test-Path $BuildDir)) { New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null }
    
    $BuildType = "Release"
    & cmake -B $BuildDir -S $RootDir "-DCMAKE_BUILD_TYPE=$BuildType"
    if ($LASTEXITCODE -ne 0) { throw "Falha na configuracao do CMake!" }
    
    & cmake --build $BuildDir --config $BuildType
    if ($LASTEXITCODE -ne 0) { throw "Falha na compilacao do CMake!" }
    
    # Localizar o executavel gerado pelo CMake
    $FoundExe = $null
    $PossiblePaths = @(
        (Join-Path $BuildDir "zx81ide.exe"),
        (Join-Path $BuildDir "Release/zx81ide.exe"),
        (Join-Path $BuildDir "Debug/zx81ide.exe"),
        (Join-Path $BuildDir "bin/zx81ide.exe")
    )
    foreach ($p in $PossiblePaths) {
        if (Test-Path $p) {
            $FoundExe = $p
            break
        }
    }
    
    if (-not $FoundExe) {
        $Candidates = Get-ChildItem -Path $BuildDir -Filter "zx81ide.exe" -Recurse
        if ($Candidates.Count -gt 0) { $FoundExe = $Candidates[0].FullName }
    }
    
    if ($FoundExe) {
        Copy-Item -Path $FoundExe -Destination $TargetExe -Force
    } else {
        throw "Executavel zx81ide.exe nao foi localizado apos build do CMake!"
    }
} else {
    Write-Host "CMake nao disponivel. Compilando diretamente com g++..." -ForegroundColor Gray
    $SourceFiles = (Join-Path $RootDir "src/main.cpp")
    $IncludeDir = (Join-Path $RootDir "include")
    
    & g++ -std=c++17 -O2 -Wall -Wextra -I$IncludeDir $SourceFiles -o $TargetExe
    if ($LASTEXITCODE -ne 0) { throw "Falha na compilacao com g++!" }
}

Write-Success "Compilacao concluida com sucesso -> $TargetExe"

# ---------------------------------------------------------------------------
# 5. Teste de Execução Rápido
# ---------------------------------------------------------------------------
Write-Step "Executando teste funcional preliminar do executavel gerado..."
$TestOutput = & $TargetExe
Write-Host $TestOutput -ForegroundColor DarkGray
Write-Success "Executavel respondeu com sucesso."

# ---------------------------------------------------------------------------
# 6. Povoamento da Pasta dist/
# ---------------------------------------------------------------------------
Write-Step "Montando pacote de distribuicao em dist/..."

$DocsToCopy = @("README.md", "MANUAL.md", "SPEC.md", "CHANGELOG.md", "RELEASE.md", "LICENSE")
foreach ($doc in $DocsToCopy) {
    $DocPath = Join-Path $RootDir $doc
    if (Test-Path $DocPath) {
        Copy-Item -Path $DocPath -Destination $DistDir -Force
        Write-Host "  + Incluido: $doc" -ForegroundColor DarkGray
    }
}

Write-Success "Diretorio dist/ preparado com executavel e documentacao."

# ---------------------------------------------------------------------------
# 7. Geração do Arquivo ZIP (zx81ide-v-X.Y.Z.zip)
# ---------------------------------------------------------------------------
Write-Step "Gerando pacote compactado para Release ($ZipName)..."

$ZipPathRoot = Join-Path $RootDir $ZipName
$ZipPathDist = Join-Path $DistDir $ZipName

if (Test-Path $ZipPathRoot) { Remove-Item $ZipPathRoot -Force }
if (Test-Path $ZipPathDist) { Remove-Item $ZipPathDist -Force }

Compress-Archive -Path "$DistDir/*" -DestinationPath $ZipPathRoot -Force
Copy-Item -Path $ZipPathRoot -Destination $ZipPathDist -Force

$ZipSizeKB = [math]::Round(((Get-Item $ZipPathRoot).Length / 1KB), 2)
Write-Success "Arquivo ZIP gerado com sucesso: $ZipName ($ZipSizeKB KB)"

# ---------------------------------------------------------------------------
# 8. GitHub Release (opcional via -CreateGitHubRelease)
# ---------------------------------------------------------------------------
if ($CreateGitHubRelease) {
    Write-Step "Criando Release oficial no GitHub via gh CLI..."
    
    if (-not $HasGh) {
        Write-Failure "GitHub CLI (gh) nao encontrado no sistema. Nao foi possivel publicar o release."
    } else {
        $ReleaseTitle = "zx81ide $TagName [$GameCodename] - $BuildSubtitle"
        $ReleaseNotes = Join-Path $RootDir "RELEASE.md"
        
        Write-Host "Criando release $TagName ($ReleaseTitle)..." -ForegroundColor Yellow
        
        & gh release create $TagName $ZipPathRoot `
            --title $ReleaseTitle `
            --notes-file $ReleaseNotes `
            --latest
            
        if ($LASTEXITCODE -eq 0) {
            Write-Success "Release $TagName publicado no GitHub com sucesso!"
        } else {
            Write-Warn "Comando 'gh release create' retornou codigo $LASTEXITCODE. Verifique se o release ja existe."
        }
    }
}

Write-Host "`n================================================================" -ForegroundColor DarkCyan
Write-Host "  Build Concluido com Sucesso!" -ForegroundColor Green
Write-Host "  Executavel : dist/zx81ide.exe" -ForegroundColor White
Write-Host "  Pacote ZIP : $ZipName" -ForegroundColor White
Write-Host "  Versao     : $Version [$GameCodename]" -ForegroundColor White
Write-Host "================================================================`n" -ForegroundColor DarkCyan
