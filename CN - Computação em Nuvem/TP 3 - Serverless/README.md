# TP 3 - Serverless

**TP 3: Serverless computing**  
Course: [CN - Computação em Nuvem](../README.md) (Cloud Computing), 2025/2

Three tasks on serverless computing in Kubernetes. Task 1 is a serverless function that computes resource usage metrics (for example the share of outgoing network traffic). Task 2 is a dashboard that shows these metrics. Task 3 is my own serverless runtime that loads the user function and reads and writes data in Redis.

## Files

- [`Task 1/`](Task%201): Serverless function (`myModule.py`), ConfigMaps and report.
- [`Task 2/`](Task%202): Monitoring dashboard (`dashboard.py`), Dockerfile, deployment and report.
- [`Task 3/`](Task%203): Custom runtime (`runtime.py`), Dockerfile, deployment and report.

## How to run

Each task has its Kubernetes manifests and a report. For example, Task 2:

```bash
cd "Task 2"
docker build -t dashboard .
kubectl apply -f dashboard-deployment.yaml
```

Comments in the code and reports are in Portuguese unless noted otherwise.
