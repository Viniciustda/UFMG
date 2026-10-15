# Entity Search — Research Challenge (RI 2026/1)

Competição: [ir-20261-rc no Kaggle](https://www.kaggle.com/c/ir-20261-rc) — ranking de entidades (4,6M no corpus) para 233 queries de teste, métrica nDCG@100.

## Setup

```powershell
python -m venv venv
venv\Scripts\pip install -r requirements.txt
```

Coloque o zip da competição na raiz e extraia para `data/` (corpus.jsonl, train/test_queries.csv, train_qrels.csv).

## Pipeline (estágios; cada um gera uma submissão)

1. **BM25 baseline** — `src/score_corpus.py` (passadas streaming) + `src/rank.py`
2. **BM25F tunado** — `src/tune.py` (grid sobre o cache de tf, validado por nDCG@100 no train)
3. **RM3** — `src/rm3.py` (expansão por pseudo-relevância, fusão de pools)
4. **Rerank cross-encoder** — `src/rerank_ce.py` (zero-shot) e `src/train_ce.py` (fine-tuned nos qrels, CV 5-fold)
5. **LTR** — `src/ltr.py` (LambdaMART sobre todos os sinais)

Recall denso: `src/dense_encode.py` + `src/dense_search.py` (bi-encoder sobre o corpus inteiro, GPU).

## Fluxo de trabalho

- Toda submissão é validada offline (`src/evaluate.py`, nDCG@100 nas train queries) e formatada/verificada (`src/check_submission.py`) antes do upload — limite de 20 submissões/dia.
- Cada submissão é registrada em `submissions/log.md` (config, score local, score Kaggle, hipótese) — base do relatório.
