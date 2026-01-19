$outputFile = "saida.txt"
$nRegistros = 1000 # Default set to 1000, user can change or pass arg if I modified script, but let's stick to simple.

"Relatorio de Testes de Compressao" | Out-File -FilePath $outputFile -Encoding utf8
"Data: $(Get-Date)" | Add-Content $outputFile
"N Registros: $nRegistros" | Add-Content $outputFile
"--------------------------------------------------" | Add-Content $outputFile

$metodos = @(
    @{ Id=1; Name="Huffman" },
    @{ Id=2; Name="LZ77" },
    @{ Id=3; Name="LZW" }
)

foreach ($m in $metodos) {
    $totalTaxa = 0
    "--- Testando $($m.Name) ---" | Add-Content $outputFile
    Write-Host "Executando $($m.Name)..."

    for ($i = 1; $i -le 3; $i++) {
        # Run program: Option 2 (Compress), N records, Method Id, Option 3 (Exit)
        $cmdOutput = '2', "$nRegistros", "$($m.Id)", '3' | .\programa.exe dados
        
        # Filter output for the line containing "Taxa:"
        $line = $cmdOutput | Where-Object { $_ -match "Taxa:" }
        
        if ($line) {
            # Extract number using regex. Matches "Taxa: 12.34%"
            if ($line -match "Taxa:\s*([\d\.]+)") {
                # Need to handle potential locale issues (comma vs dot). 
                # C++ source uses default locale, usually dot. Powershell [double] might expect comma if PC is PT-BR.
                # Let's replace comma with dot or ensure culture invariant.
                $valStr = $matches[1].Replace(',', '.')
                $taxa = [double]$valStr
                $totalTaxa += $taxa
                "Execucao ${i}: $line" | Add-Content $outputFile
            } else {
                "Execucao ${i}: Falha ao ler taxa. Output: $line" | Add-Content $outputFile
            }
        } else {
            "Execucao ${i}: Nenhuma saida de taxa encontrada." | Add-Content $outputFile
        }
        Start-Sleep -Seconds 1 # Brief pause
    }

    $media = $totalTaxa / 3
    "MEDIA ($($m.Name)): $media %" | Add-Content $outputFile
    "--------------------------------------------------" | Add-Content $outputFile
}

Write-Host "Testes concluidos. Resultados salvos em $outputFile"
Get-Content $outputFile
