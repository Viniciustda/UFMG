from ucimlrepo import fetch_ucirepo
import numpy as np
import pandas as pd
import random
import math
import matplotlib.pyplot as plt
import csv
import time
from sklearn import datasets
from sklearn.cluster import KMeans
from sklearn.impute import SimpleImputer
from sklearn.preprocessing import StandardScaler, LabelEncoder
from sklearn.metrics import silhouette_score, adjusted_rand_score

raisin = fetch_ucirepo(id=850)
blood_transfusion_service_center = fetch_ucirepo(id=176)
rice_cammeo_and_osmancik = fetch_ucirepo(id=545)
hepatitis_c_virus_hcv_for_egyptian_patients = fetch_ucirepo(id=503)
ozone_level_detection = fetch_ucirepo(id=172)
statlog_image_segmentation = fetch_ucirepo(id=147)
statlog_vehicle_silhouettes = fetch_ucirepo(id=149)
website_phishing = fetch_ucirepo(id=379)
absenteeism_at_work = fetch_ucirepo(id=445)
myocardial_infarction_complications = fetch_ucirepo(id=579)

def minkowski(x,y,p):
    x = np.asarray(x, dtype=np.float64)
    y = np.asarray(y, dtype=np.float64)
    return np.power(np.sum(np.power(np.abs(x - y), p)), 1/p)

def matrizDistancia(amostras, X, p):
    matriz = np.zeros((amostras, amostras))
    for i in range(amostras):
        for j in range(i + 1, amostras):
            distancia = minkowski(X[i], X[j], p)
            matriz[i, j] = distancia
            matriz[j, i] = distancia 
    return matriz

def algoritmoMaioresDistancias(N, k, dist):
    k_centros = []
    k_centros.append(random.randrange(N))
    for i in range(k-1):
        melhor = -math.inf
        centro = None
        for p in range(N):
            val = math.inf
            for q in k_centros:
                val = min(val, dist[p][q])
            if val > melhor:
                centro = p
                melhor = val
        k_centros.append(centro)

    
    r = -math.inf
    for p in range(N):
        melhor = math.inf
        for q in k_centros:
            melhor = min(melhor, dist[p][q])
        r = max(r, melhor)

    return r, k_centros 

def prepara_dados(dataset, label):
    X = dataset.data.features 
    imputer = SimpleImputer(strategy='mean')
    X_imputed = imputer.fit_transform(X)
    y_true = dataset.data.targets 

    if hasattr(y_true, 'values'):
        y_true = y_true.values.flatten()

    
    label_encoder = LabelEncoder()
    y_true_encoded = label_encoder.fit_transform(y_true)

    return X_imputed, y_true_encoded, label 

real_data = [prepara_dados(hepatitis_c_virus_hcv_for_egyptian_patients, 'hepatitis_c_virus_hcv_for_egyptian_patients'),prepara_dados(ozone_level_detection, 'ozone_level_detection'),prepara_dados(statlog_image_segmentation, 'statlog_image_segmentation'),prepara_dados(website_phishing, 'website_phishing'),prepara_dados(myocardial_infarction_complications, 'myocardial_infarction_complications'),prepara_dados(statlog_vehicle_silhouettes, 'statlog_vehicle_silhouettes'),prepara_dados(raisin, 'raisin'),prepara_dados(absenteeism_at_work, 'abstenteeism_at_work'),prepara_dados(rice_cammeo_and_osmancik, 'rice_cammeo_and_osmancik'),prepara_dados(blood_transfusion_service_center, 'blood_transfusion_service_center'),]

k_values = [len(np.unique(data[1])) for data in real_data]

X = real_data[0][0]
amostras = X.shape[0]
X_arr = X.to_numpy() if hasattr(X, 'to_numpy') else np.array(X)

euclid_d = matrizDistancia(amostras, X_arr, p=2)


def buscaBinaria(N, k, dist, raio_maximo = None, precisao = 0.0001, tentativas = 100):
    if raio_maximo == None:  
        raio_maximo = 0
        for i in range(amostras):
            for j in range(amostras):
                raio_maximo = max(raio_maximo, euclid_d[i][j])
    lower_bound = 0
    upper_bound = raio_maximo
    ans = []
    while upper_bound - lower_bound >= precisao * raio_maximo:
        mid = (upper_bound + lower_bound)/2
        ok = 0
        for _ in range(tentativas):
            centro = [False] * N
            coberto = [False] * N
            sobra = N
            for i in range(k):
                c = random.randrange(N)
                while centro[c] or (sobra > 0 and coberto[c]):
                    c = random.randrange(N)
                centro[c] = True
                for p in range(N):
                    if not coberto[p] and dist[p][c] <= mid:
                        coberto[p] = True
                        sobra -= 1
            if sobra == 0:
                ok = 1
                upper_bound = mid
                ans = []
                for i in range(N):
                    if centro[i]:
                        ans.append(i)
                break
        if not ok:
            lower_bound = mid
    return upper_bound, ans


semente = 111000

noisy_circles = datasets.make_circles(
    n_samples=500, factor=0.5, noise=0.05, random_state=semente
)

noisy_moons = datasets.make_moons(n_samples=500, noise=0.05, random_state=semente)

blobs = datasets.make_blobs(n_samples=500, random_state=semente)

rng = np.random.RandomState(semente)

no_structure = rng.rand(500, 2), np.zeros(500)

X, y = datasets.make_blobs(n_samples=500, random_state=semente)
transformation = [[0.6, -0.6], [-0.4, 0.8]]
X_aniso = np.dot(X, transformation)
aniso = (X_aniso, y)

varied = datasets.make_blobs(n_samples=500, cluster_std=[1.0, 2.5, 0.5], random_state=semente)
                             
data = [
    noisy_circles,
    noisy_moons,
    varied,
    aniso,
    blobs,
    no_structure,
]

labels = [
    "noisy_circles",
    "noisy_moons",
    "varied_blobs",
    "aniso_blobs",
    "blobs",
    "no_structure"
]

labeled_data = [(X, y, label) for (X, y), label in zip(data, labels)]

colors = ['purple', 'black', 'pink', 'yellow']


def calculatePlotClusters(X, k, ax, title, p):
    
    X = StandardScaler().fit_transform(X)
    n_amostras = X.shape[0]
    dist_matriz = matrizDistancia(n_amostras, X, p)

    
    r_maiores, centros_maiores = algoritmoMaioresDistancias(n_amostras, k, dist_matriz)
    labels_maiores = np.zeros(n_amostras, dtype=int)
    for p in range(n_amostras):
        distances_to_centros = [dist_matriz[p][centro] for centro in centros_maiores]
        labels_maiores[p] = np.argmin(distances_to_centros)

    
    r_busca, centros_busca = buscaBinaria(n_amostras, k, dist_matriz)
    labels_busca = np.zeros(n_amostras, dtype=int)
    for p in range(n_amostras):
        distances_to_centros = [dist_matriz[p][centro] for centro in centros_busca]
        labels_busca[p] = np.argmin(distances_to_centros)

    
    ax[0].scatter(X[:, 0], X[:, 1], c=[colors[label % len(colors)] for label in labels_maiores], s=10)
    ax[0].set_title(f"{title} - maiores_distancias")
    ax[0].set_xlim(-2.5, 2.5)
    ax[0].set_ylim(-2.5, 2.5)
    ax[0].set_xticks(())
    ax[0].set_yticks(())

    
    ax[1].scatter(X[:, 0], X[:, 1], c=[colors[label % len(colors)] for label in labels_busca], s=10)
    ax[1].set_title(f"{title} - busca_binaria")
    ax[1].set_xlim(-2.5, 2.5)
    ax[1].set_ylim(-2.5, 2.5)
    ax[1].set_xticks(())
    ax[1].set_yticks(())

plt.figure(figsize=(8, 18))

k=3
for i, (X, y) in enumerate(data):
    if X is not None:
        ax = plt.subplot(6, 2, 2 * i + 1)
        calculatePlotClusters(X, k, [ax, plt.subplot(6, 2, 2 * i + 2)], f"Dataset {i+1}", p=2)

plt.tight_layout()
plt.savefig('img1.png')


def geraDados(dts=10, cts=3, points_centro=100, desv=None):
    if desv is None:
        desv = [0.1, 0.5, 1.0]  
    datasets = []

    ind = 1
    for desvio in desv:
        for _ in range(dts):
            centros = np.random.rand(cts, 2) * 4 - 2
            X = []
            y = []
            for i, centro in enumerate(centros):
                
                points = np.random.multivariate_normal(centro, np.eye(2) * desvio, points_centro)
                X.append(points)
                y.append(np.full(points_centro, i))
            X = np.vstack(X)
            y = np.hstack(y)
            datasets.append((X, y, f"{ind}: {desvio}"))
            ind += 1

    return datasets


data_2 = geraDados()


def plotaDadosGerados(datasets):
    plt.figure(figsize=(10, 15))
    for i, (X, y, title) in enumerate(datasets):
        ax = plt.subplot(6, 5, i + 1)
        cluster_colors = [colors[label % len(colors)] for label in y]
        ax.scatter(X[:, 0], X[:, 1], c=cluster_colors, s=10)
        ax.set_title("Database "+title)
        ax.set_xticks(())
        ax.set_yticks(())
    plt.tight_layout()
    plt.savefig('img2.png')
    
plotaDadosGerados(data_2)

n_datasets = len(data_2)
n_rows = (n_datasets * 2 + 1) // 6
plt.figure(figsize=(15, n_rows*2))

for i, (X, y, title) in enumerate(data_2):
    ax = plt.subplot(n_rows, 6, 2 * i + 1)
    calculatePlotClusters(X, k=3, ax=[ax, plt.subplot(n_rows, 6, 2 * i + 2)], title="Base"+title, p=2)

plt.tight_layout()
plt.savefig('img3.png')


def silhouette_score_custom(X, labels, distance_matriz):
    unique_labels = np.unique(labels)
    n_amostras = len(labels)
    if len(unique_labels) == 1:
        return 0.0

    a = np.zeros(n_amostras)
    for label in unique_labels:
        mask = (labels == label)
        cluster_points = distance_matriz[np.ix_(mask, mask)]
        a[mask] = np.mean(cluster_points, axis=1)

    b = np.full(n_amostras, np.inf)
    for label in unique_labels:
        mask = (labels == label)
        other_labels = unique_labels[unique_labels != label]
        for other_label in other_labels:
            other_mask = (labels == other_label)
            other_cluster_points = distance_matriz[np.ix_(mask, other_mask)]
            mean_dist = np.mean(other_cluster_points, axis=1)
            b[mask] = np.minimum(b[mask], mean_dist)

    silhouette_scores = (b - a) / np.maximum(a, b)
    return np.mean(silhouette_scores)


def evaluate(X, y_true, labels, metric="euclidean", matriz=None):
    if(metric == "euclidean"):
        silhouette = silhouette_score(X, labels, metric=metric)
        rand_index = adjusted_rand_score(y_true, labels)
    else:
        silhouette = silhouette_score_custom(X, labels, matriz)
        rand_index = adjusted_rand_score(y_true, labels)

    return silhouette, rand_index


def experimentosMaioresDistancias(datasets, k=3, exec=30, k_list=None):
    results = []

    if(k_list==None):
        
        for X, y_true, label in datasets:
            X = StandardScaler().fit_transform(X)
            N = len(X)

            for p in [1, 2]:
                dist_matriz = matrizDistancia(N, X, p)
                distance_type = "manhattan" if p == 1 else "euclidean"

                for _ in range(exec):
                    start_time = time.time()
                    r, centros = algoritmoMaioresDistancias(N, k, dist_matriz)
                    exec_time = time.time() - start_time

                    labels = np.argmin(dist_matriz[:, centros], axis=1)

                    silhouette, rand_index = evaluate(X, y_true, labels, distance_type, dist_matriz)

                    results.append([label, distance_type, r, 'Maiores Distancias', silhouette, rand_index, exec_time])
    else:
        
        for (X, y_true, label), kx in zip(datasets, k_list):
            X = StandardScaler().fit_transform(X)
            N = len(X)

            for p in [1, 2]:
                dist_matriz = matrizDistancia(N, X, p)
                distance_type = "manhattan" if p == 1 else "euclidean"

                for _ in range(exec):
                    start_time = time.time()
                    r, centros = algoritmoMaioresDistancias(N, kx, dist_matriz)
                    exec_time = time.time() - start_time

                    labels = np.argmin(dist_matriz[:, centros], axis=1)

                    silhouette, rand_index = evaluate(X, y_true, labels, distance_type, dist_matriz)

                    results.append([label, distance_type, r, 'Maiores Distancias', silhouette, rand_index, exec_time])

    return np.array(results)


def experimentosVariacaoRaio(datasets, exec, k_list):
    results = []
    for (X, y_true, label), kx in zip(datasets, k_list):
        X = StandardScaler().fit_transform(X)
        N = len(X)

        for p in [1, 2]:
            dist_matriz = matrizDistancia(N, X, p)
            distance_type = "manhattan" if p == 1 else "euclidean"
            raio_maximo = 0
            for i in range(N):
                for j in range(N):
                    raio_maximo = max(raio_maximo, dist_matriz[i][j])

            for it in [2, 3, 4, 5, 6, 20]:
                prec = 2**(-it)

                
                for _ in range(exec):
                    start_time = time.process_time()
                    r, centros = buscaBinaria(N, kx, dist_matriz, raio_maximo = raio_maximo, precisao = prec, tentativas = 1)
                    exec_time = time.process_time() - start_time

                    labels = np.argmin(dist_matriz[:, centros], axis=1)
                    silhouette, rand_index = evaluate(X, y_true, labels, distance_type, dist_matriz)
                    results.append([label, distance_type, r, 'Busca Binaria 1', it, silhouette, rand_index, exec_time])

                
                start_time = time.process_time()
                r, centros = buscaBinaria(N, kx, dist_matriz, raio_maximo = raio_maximo, precisao = prec, tentativas = 30)
                exec_time = time.process_time() - start_time

                labels = np.argmin(dist_matriz[:, centros], axis=1)
                silhouette, rand_index = evaluate(X, y_true, labels, distance_type, dist_matriz)
                results.append([label, distance_type, r, 'Busca Binaria 2', it, silhouette, rand_index, exec_time])
    return np.array(results)


def salvaTabela(results, filename="Agrupamento.csv"):
    header = ["Dataset", "Distance", "Radius", "Algorithm", "Silhouette Score", "Adjusted Rand Index", "Execution Time"]

    with open(filename, mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(header)
        writer.writerows(results)

def get_radius(N, centros, labels, dist_matriz):
    max_radii = []

    for i in range(len(centros)):
        cluster_indices = np.where(labels == i)[0]
        max_distance = -np.inf
        for idx in cluster_indices:
            for centro_idx in range(len(centros)):
                if labels[idx] == i:
                    max_distance = max(max_distance, dist_matriz[idx][centro_idx])
        max_radii.append(max_distance)

    return max(max_radii)


def experimentosKmeans(datasets, k=3, exec=30, k_list=None):
    results = []

    if(k_list==None):
        
        for X, y_true, label in datasets:
            X = StandardScaler().fit_transform(X)
            N = len(X)

            for p in [1, 2]:
                dist_matriz = matrizDistancia(N, X, p)
                distance_type = "manhattan" if p == 1 else "euclidean"

                for _ in range(exec):
                    start_time = time.time()

                    kmeans = KMeans(n_clusters=k, random_state=0).fit(X)
                    labels = kmeans.labels_
                    exec_time = time.time() - start_time
                    centros = kmeans.cluster_centers_
                    radius = get_radius(N, centros, labels, dist_matriz)


                    silhouette, rand_index = evaluate(X, y_true, labels, distance_type, dist_matriz)

                    results.append([label, distance_type, radius, 'KMeans', silhouette, rand_index, exec_time])
    else:
        
        for (X, y_true, label), kx in zip(datasets, k_list):
            X = StandardScaler().fit_transform(X)
            N = len(X)

            for p in [1, 2]:
                dist_matriz = matrizDistancia(N, X, p)
                distance_type = "manhattan" if p == 1 else "euclidean"

                for _ in range(exec):
                    start_time = time.time()

                    kmeans = KMeans(n_clusters=kx, random_state=0).fit(X)
                    labels = kmeans.labels_
                    exec_time = time.time() - start_time
                    centros = kmeans.cluster_centers_
                    radius = get_radius(N, centros, labels, dist_matriz)
                    silhouette, rand_index = evaluate(X, y_true, labels, distance_type, dist_matriz)

                    results.append([label, distance_type, radius, 'KMeans', silhouette, rand_index, exec_time])

    return np.array(results)

def resultadosExperimentos(array1, array2, csv=None):
    combined_array = np.concatenate([array1, array2])

    df = pd.DataFrame(combined_array, columns=['label', 'distance_type', 'radius', 'algorithm', 'silhouette', 'rand_index', 'exec_time'])

    df[['silhouette', 'rand_index', 'exec_time']] = df[['silhouette', 'rand_index', 'exec_time']].astype(float)

    summary_df = df.groupby(['label', 'distance_type', 'algorithm']).agg(
        silhouette_mean=('silhouette', 'mean'),
        silhouette_std=('silhouette', 'std'),
        rand_index_mean=('rand_index', 'mean'),
        rand_index_std=('rand_index', 'std'),
        exec_time_mean=('exec_time', 'mean'),
        exec_time_std=('exec_time', 'std')
    ).reset_index()

    if csv:
        summary_df.to_csv(csv, index=True)

    return summary_df

results_md_1 = experimentosMaioresDistancias(labeled_data)
results_kmeans_1 = experimentosKmeans(labeled_data)
results_md_2 = experimentosMaioresDistancias(data_2)
results_kmeans_2 = experimentosKmeans(data_2)

def resultadosBsearch(results):
    df = pd.DataFrame(results, columns=['label', 'distance_type', 'radius', 'algorithm', 'precisao', 'silhouette', 'rand_index', 'exec_time'])

    df[['radius', 'silhouette', 'rand_index', 'exec_time']] = df[['radius', 'silhouette', 'rand_index', 'exec_time']].astype(float)

    summary_df = df.groupby(['label', 'distance_type', 'algorithm', 'precisao']).agg(
        radius_min=('radius', 'min'),
        silhouette_mean=('silhouette', 'mean'),
        silhouette_max=('silhouette', 'max'),
        rand_index_mean=('rand_index', 'mean'),
        rand_index_max=('rand_index', 'max'),
        exec_time_mean=('exec_time', 'mean'),
    ).reset_index()

    return summary_df

# resultadosBsearch(experimentosVariacaoRaio(labeled_data, 30, [3] * len(labeled_data))).to_csv('BinarySearchSint1.csv')
# resultadosBsearch(experimentosVariacaoRaio(data_2, 30, [3] * len(data_2))).to_csv('BinarySearchSint2.csv')

summary_2 = resultadosExperimentos(results_md_2, results_kmeans_2)
summary_2['label'] = summary_2['label'].astype(str)
summary_2['label_first_number'] = summary_2['label'].str.extract(r'(\d+)').astype(int)
summary_table_2 = summary_2.sort_values(by='label_first_number').drop(columns=['label_first_number'])
summary_table_2.to_csv('CompKmeansSint.csv', index=False)

results_kmeans_real = experimentosKmeans(real_data, 3, 30, k_values)

results_bsearch_real = experimentosVariacaoRaio(real_data, 30, k_values)

results_real = experimentosMaioresDistancias(real_data, 3, 30, k_values)
summary_real = resultadosExperimentos(results_real, results_kmeans_real, csv='KMeansKCenterDadosReais.csv')

resultadosBsearch(results_bsearch_real).to_csv('resultadoBinSearchDadosReais.csv')