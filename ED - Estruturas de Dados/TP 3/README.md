# TP 3

**TP 3: Search engine**  
Course: [ED - Estruturas de Dados](../README.md) (Data Structures), 2021/2

Search engine over a document collection: builds an inverted index with hashing, removes stopwords and ranks documents by TF-IDF term weights (vector space model).

## Files

- [`Documentacao.pdf`](Documentacao.pdf): Report (Portuguese).
- [`TP/`](TP): Source code (`src/`), headers (`include/`) and Makefile.
- [`tp3_maquinabusca-v1.pdf`](tp3_maquinabusca-v1.pdf): Assignment statement (Portuguese).

## How to run

```bash
cd TP
mkdir -p obj bin
make
./bin/buscar -i queries.txt -o ranking.txt -c corpus_folder -s stopwords.txt
```

Comments in the code and reports are in Portuguese unless noted otherwise.
