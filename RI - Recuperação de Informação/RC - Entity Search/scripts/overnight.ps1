# Corrente noturna (12->13/06): roda na GPU em sequência, com checkpoints.
# 1. espera o scoring v2-m3 terminar (libera a GPU)
# 2. re-treino gentil do cross-encoder (anti-overfit, salva os 5 modelos)
# 3. bge-base: codifica o corpus inteiro (sufixo _base)
# 4. busca densa com bge-base
$env:PYTHONIOENCODING = 'utf-8'
$root = Split-Path $PSScriptRoot -Parent
Set-Location $root

$deadline = (Get-Date).AddMinutes(100)
while (-not (Test-Path "cache\ce_v2_scores.csv.gz") -and (Get-Date) -lt $deadline) {
    Start-Sleep -Seconds 60
}
Write-Output ("=== v2 liberado em " + (Get-Date))

venv\Scripts\python src\train_ce.py --pool cache\pool_fused.csv.gz --depth 1000 `
    --epochs 1 --lr 1e-5 --batch 8 --accum 2 --grad-ckpt --freeze-emb `
    --neg-skip 100 --neg-top 1000 --pairs-per-pos 4 --save-models `
    --out cache\ce_ft2_scores.csv.gz
Write-Output ("=== ft2 done em " + (Get-Date))

venv\Scripts\python src\dense_encode.py --model BAAI/bge-base-en-v1.5 --batch 128 `
    --maxlen 192 --suffix _base
Write-Output ("=== encode base done em " + (Get-Date))

venv\Scripts\python src\dense_search.py --k 1000 --suffix _base
Write-Output ("=== TUDO PRONTO em " + (Get-Date))
