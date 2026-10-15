# Log de experimentos e submissões

Validação offline: nDCG@100 (ganho exponencial) nas 234 train queries.
Anotar o score do Kaggle ao lado quando submeter (máx. 20 uploads/dia).

| upload # | Arquivo | Config | nDCG train | Kaggle (público) |
|---|---------|--------|-----------:|-------:|
| #01 | sub1_bm25.csv | BM25 campos concatenados, k1=1.2, b=0.75 | 0.3985 | 0.40789 |
| #02 | sub2_bm25f.csv | BM25F k1=0.9, b=(0.9, 0.15, 0.2), w=(16, 2, 1) | 0.4678 | 0.45574 |
| #03 | sub3_rm3.csv | BM25F + RM3 (fb=10, 25 termos, λ=0.1, fusão de pools) | 0.4678 | 0.46408 |
| #04 | sub4_rrf.csv | híbrido RRF (k=60): pool lexical (BM25F+RM3) ∪ denso (bge-small) | 0.5254 | 0.50626 |
| #05 | sub4_ce.csv | + rerank CE zero-shot (bge-reranker-base) top-1000, α=0.5 c/ RRF | 0.5653 | 0.53228 |
| #06 | sub5_ltr.csv | LambdaMART: todos os sinais exceto CE fine-tuned (CV OOF honesta) | 0.5912 (CV) | 0.54772 |
| #07 | sub6_ceft_blend.csv | blend 0.3·RRF + 0.3·CE + 0.4·CE-ft (pesos no train) | 0.6046 | — |
| #08 | sub7_ltr_full.csv | LambdaMART completo (+ ceft como feature; CV OOF) | 0.6126 (CV) | — |

| #09 | sub9_ltr_v2.csv | LambdaMART + feature bge-reranker-v2-m3 zero-shot (CV OOF) | 0.6297 (CV) | 0.58375 |

| #10 | sub10_ltr_ft2.csv | LambdaMART + CE fine-tuned regularizado (ft2) como feature | 0.6312 (CV) | 0.58420 |

v2-m3 (12/06): zero-shot puro 0.5514 (base: 0.5052); como feature domina o gain
do LTR. Kaggle #07=0.56305, #08=0.56640 → gap CV→público estabilizado ≈ -0.045.

| #11 | sub11_ltr_3way.csv | LambdaMART no pool 3-way + v2-m3 completo + ft2 (CV OOF) | 0.6362 (CV) | 0.59059 |

#11 assumiu o 1º lugar (0.59059; 2º ex-líder 0.58907, parado há 20h). Calibração
cravou de novo: previsto ~0.589, obtido 0.591. Gap CV→público médio = 0.046.

ft2 — fine-tuning regularizado (13/06): 1 época, lr 1e-5, embeddings congelados,
negativos amostrados em [100,1000) p/ evitar falsos negativos. OOF puro 0.4348→
0.5625 (resolve o overfit). No LTR vira a feature #1 (gain 20135); CV 0.6312.
bge-base denso: recall@1000 0.786→0.812. Pool 3-way (lex+small+base): recall@2000
0.883→0.897 — re-scoring v2-m3 dos candidatos novos em andamento.

Fine-tuning do CE (12/06): RankNet, hard negatives do pool, 5 folds, ensemble.
Resultado: PURO piora (OOF 0.4348 vs 0.5052 zero-shot; loss→0 = overfit/falsos
negativos), mas como SINAL em blend/LTR é a feature mais valiosa (ceft_z #3 no
gain). Lição p/ relatório: modelo saturado ordena mal sozinho; ensemble recupera.

LTR (12/06): CV OOF 0.5912 sem o sinal fine-tuned — supera o blend α (0.5653,
train-tuned). Importância: rrf >> ce_z > dense_z; features de título/cobertura
marginais (sinal já capturado pelo BM25F dentro do rrf).

CE zero-shot (10/06): 1º lugar (0.53228; 2º = 0.51074). Gap train→público por
submissão: #02 -0.012, #03 +0.008, #04 -0.019, #05 -0.033 — quanto mais tuning
no train (α, params), maior o otimismo; reforça decidir por CV/OOF nos próximos.

Denso (10/06): bge-small-en-v1.5, recall@1000 = 0.786 puro (lexical = 0.776,
erros complementares). Fusão RRF: recall do pool @1000 0.776→0.852, @2000
0.820→0.883; nDCG train 0.4678→0.5254 (+0.058 — maior ganho unitário até aqui;
hipótese: mismatch de vocabulário resolvido pelo espaço denso).

Calibração (10/06): local↔Kaggle dentro de ±0.012 — validação offline confiável.
RM3 ganhou +0.008 no público apesar do empate no train (expansão generaliza;
tuning lexical levemente otimista no train). Posição: ~4º (líder 0.50096).

## Hipóteses (para o relatório)

1. **Baseline lexical**: BM25 sobre os 3 campos concatenados estabelece a referência
   (0.3985 com a nossa tokenização: Snowball + stopwords NL + fold de acentos).
2. **Estrutura de campos importa**: em entity search o título praticamente identifica a
   entidade; BM25F com pesos/normalizações por campo tunados no train (descida por
   coordenadas sobre cache de tf) dá o maior ganho lexical unitário.
   Resultado: +0.069 (0.3985→0.4678); peso ótimo do título = 16×, b_title=0.9 (quase
   normalização total de comprimento), b_text=0.2.
3. **Vocabulary mismatch**: queries NL curtas vs. descrições; RM3 (pseudo-relevância)
   deveria ajudar. Resultado: λ ótimo = 0 para precisão (sem ganho; títulos/keywords
   curtos diluem termos de expansão sob w_title=16), MAS a fusão de pools melhora o
   recall do conjunto de candidatos: 0.8005 → 0.8201 @2000 — eleva o teto dos estágios
   neurais (justifica manter RM3 no pipeline de candidatos).
4. **Matching semântico**: cross-encoder captura relevância além do overlap lexical;
   rerank do top-1000 do pool fundido (lexical ∪ denso) deve dar o maior ganho global.
5. **Adaptação ao domínio**: fine-tuning do cross-encoder nos 8.202 qrels (positivos
   graduados + hard negatives do pool), CV 5-fold por query sem vazamento + ensemble
   dos folds no test, deve superar o zero-shot.

## Decisões técnicas (material para o relatório)

- Índice invertido parcial (vocabulário das 467 queries) via 2 passadas streaming
  document-at-a-time com multiprocessing (62s + 119s nas 4,6M entidades); cache de
  tf por (candidato, termo, campo) → re-tuning de parâmetros em segundos.
- Pool de candidatos: top-2000/query (pré-score BM25F) ∪ pool RM3 ∪ top denso;
  docs julgados do train forçados no cache (avaliação não-enviesada).
- Recall do pool lexical (train): @100=0.579, @500=0.727, @1000=0.776, @2000=0.820.
- Denso: bge-small-en-v1.5 (384d, fp16, memmap 3,4GB) sobre o corpus inteiro;
  busca por matmul em blocos na GPU; fusão RRF com o pool lexical.
