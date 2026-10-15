FROM python:3.9-slim
WORKDIR /app
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
COPY dashboard.py .
EXPOSE 50131
CMD ["streamlit", "run", "dashboard.py", "--server.port", "50131", "--server.address", "0.0.0.0"]