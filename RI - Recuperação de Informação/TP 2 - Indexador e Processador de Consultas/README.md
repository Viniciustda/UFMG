# TP2 — Indexador e Processador de Consultas

Vinicius Trindade (2020007112) — Recuperação de Informação 

## Como rodar

Indexar:

```bash
python3 indexer.py -m <MEMORY> -c <CORPUS> -i <INDEX>
```

Buscar:

```bash
python3 processor.py -i <INDEX> -q <QUERIES> -r <TFIDF|BM25>
```

## Sobre o `nltk_data/`

A pasta `nltk_data/` empacotada já traz as stopwords inglesas (o Porter
stemmer não precisa de download, vem com o `nltk` do `requirements.txt`).
Em `tp2/text.py` o `nltk_data/` local é inserido no `nltk.data.path`
antes do default, então fora-da-caixa não é preciso fazer nada.

Se preferir baixar você mesmo em vez de confiar no empacotado:

```bash
rm -rf nltk_data
python3 -c "import nltk; nltk.download('stopwords', download_dir='./nltk_data')"
```

Ou, alternativamente, deletar `nltk_data/` e deixar o NLTK baixar no
caminho default dele (`~/nltk_data`) na primeira execução. O código tem
fallback para `nltk.download('stopwords', quiet=True)` se a carga local
falhar.

## Estrutura


```
.
├── README.md              este arquivo
├── indexer.py             CLI: parse de args → tp2.indexer.run
├── processor.py           CLI: parse de args → tp2.processor.run
├── requirements.txt       nltk, numpy, psutil
├── nltk_data/             stopwords + Porter (offline-safe)
├── Documentation.pdf      relatório (PDF)
├── link_drive.txt         link do Drive com o índice produzido
└── tp2/
    ├── config.py          constantes + Posting, LexEntry, DocEntry
    ├── text.py            tokenize + stopword + Porter (mesma função no indexer e processor)
    ├── codec.py           BitReader/BitWriter, γ-codes e unário
    ├── memory.py          monitor de RSS do processo (psutil)
    ├── indexer/
    │   ├── runner.py      orquestra: ler → pré-processar → SPIMI → merge
    │   ├── reader.py      streaming do JSONL
    │   ├── workers.py     pré-processa batches em ThreadPoolExecutor
    │   ├── builder.py     acumulador SPIMI + flush em run files
    │   ├── merge.py       k-way merge dos runs → inverted_index + lexicon
    │   └── stats.py       IndexerMetrics + DfHistogram
    └── processor/
        ├── runner.py      orquestra: abrir índice → ranquear → imprimir
        ├── store.py       abre o índice (mmap de inverted_index e doc_index, lexicon em RAM)
        ├── postings.py    iterador lazy sobre uma posting list comprimida
        ├── daat.py        DAAT conjuntivo + top-k heap
        ├── ranker.py      TFIDF (Pivoted Length) e BM25 (Okapi)
        ├── parallel.py    ThreadPoolExecutor pra paralelizar consultas
        └── stats.py       métricas por consulta
```

## Pontos de entrada

- `indexer.py` → `tp2.indexer.runner.run()`
- `processor.py` → `tp2.processor.runner.run()`

Toda a lógica vive nos dois `runner.py`. Os shims na raiz só fazem parse
de argumentos.

## Arquivos de saída

`indexer.py` grava em `<INDEX>`:

- `inverted_index.bin`, `lexicon.bin`, `doc_index.bin` — os três do enunciado
- `corpus_stats.json` — N, avdl, k1, b (lidos pelo processor)
- `runtime_metrics.json`, `df_histogram.json` — alimentam o relatório

`processor.py` imprime um JSON por linha no stdout (no formato pedido) e
grava `query_metrics_<ranker>.json` no `<INDEX>`.

## Variáveis de ambiente opcionais

- `TP2_INDEXER_WORKERS` — número de threads de pré-processamento do indexer.
  Default: `cpu_count()-1`.
- `TP2_PROCESSOR_THREADS` — número de threads do processor.
  Default: `cpu_count()-1`.

## Índice produzido (corpus completo)

Indexação completa do corpus de 4.641.784 entidades da Wikipédia rodada
com `-m 1024`. Os três arquivos binários estão no Drive:

https://drive.google.com/drive/folders/1vOW-ttQv3QS7MC0yFM8DxcWFLJRi2bA5