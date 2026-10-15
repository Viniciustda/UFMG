# TP 2 - DevOps

**TP 2: DevOps**  
Course: [CN - Computação em Nuvem](../README.md) (Cloud Computing), 2025/2

Playlist recommendation service. A Kubernetes Job trains association rules from the Spotify dataset and saves the model on a persistent volume. A Flask API reads the model and recommends songs. Argo CD keeps the cluster in sync with the repository (continuous delivery).

## Files

- [`.gitignore`](.gitignore)
- [`Dockerfile`](Dockerfile): Image of the API.
- [`Dockerfile.ml`](Dockerfile.ml): Image of the training job.
- [`TESTES.md`](TESTES.md): How to test the API with curl.
- [`TP2 - DevOps - Vinicius Trindade.pdf`](TP2%20-%20DevOps%20-%20Vinicius%20Trindade.pdf): Report (Portuguese).
- [`app.py`](app.py): Flask API that loads the model and answers `/api/recommend`.
- [`argocd-manifest.yaml`](argocd-manifest.yaml): Argo CD application.
- [`deployment.yaml`](deployment.yaml): API deployment.
- [`generate_rules.py`](generate_rules.py): Trains the association rules.
- [`job.yaml`](job.yaml): Training job.
- [`pvc.yaml`](pvc.yaml): Persistent volume claim for the model.
- [`requirements.txt`](requirements.txt): Python dependencies.
- [`service.yaml`](service.yaml): API service.

## How to run

```bash
docker build -t tp2-api -f Dockerfile .
docker build -t tp2-ml -f Dockerfile.ml .
kubectl apply -f pvc.yaml -f job.yaml -f deployment.yaml -f service.yaml
# or let Argo CD deploy it:
kubectl apply -f argocd-manifest.yaml
```

Comments in the code and reports are in Portuguese unless noted otherwise.
