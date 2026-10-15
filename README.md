# UFMG Computer Science Coursework

Code, notebooks and reports I wrote during my **B.Sc. in Computer Science** at the [Universidade Federal de Minas Gerais (UFMG)](https://ufmg.br), Belo Horizonte, Brazil.

Each folder is one course. Folder names keep the original course name in Portuguese (`ABBREVIATION - Course name`), and every folder has its own README in English describing the assignments.

## Featured projects

| Project | What it does | Course | Tech |
|---|---|---|---|
| **[Simplex Method](PO%20-%20Pesquisa%20Operacional/TP%202%20-%20Simplex/)** | Solves general linear programs with the tableau Simplex method. It prints every pivot step and reports whether the problem is optimal, infeasible or unbounded, with the primal and dual solutions. | Operations Research | Python, NumPy |
| **[Compiler for a small ML-like language](Compiladores%20I/)** | Built in 15 incremental steps: lexer, parser, type checking and inference, functions, RISC-V code generation and register allocation. | Compilers I | Python |
| **[Search engine indexer](RI%20-%20Recupera%C3%A7%C3%A3o%20de%20Informa%C3%A7%C3%A3o/TP%202%20-%20Indexador%20e%20Processador%20de%20Consultas/)** | Builds an inverted index under a memory limit and answers queries with TF-IDF and BM25. | Information Retrieval | Python |
| **[Entity Search](RI%20-%20Recupera%C3%A7%C3%A3o%20de%20Informa%C3%A7%C3%A3o/RC%20-%20Entity%20Search/)** | Ranks 4.6M entities for natural language queries with BM25, query expansion, dense retrieval, cross-encoder re-ranking and learning to rank. | Information Retrieval | Python |
| **[Web crawler](RI%20-%20Recupera%C3%A7%C3%A3o%20de%20Informa%C3%A7%C3%A3o/TP%201%20-%20Web%20Crawler/)** | Multithreaded crawler that respects robots.txt, balances requests between hosts and stores pages in WARC files. | Information Retrieval | Python |
| **[Federated learning with differential privacy](PPAM%20-%20Prote%C3%A7%C3%A3o%20da%20Privacidade%20em%20Aprendizado%20de%20M%C3%A1quina/Projeto%20-%20Aprendizado%20Federado/)** | Predictive maintenance model trained across simulated factories, with a membership inference attack to measure privacy leakage. | Privacy Protection in ML | Python, Flower, Docker |
| **[DQN for Lunar Lander](AR%20-%20Aprendizado%20por%20Refor%C3%A7o/TP%20-%20DQN%20Lunar%20Lander/)** | Deep Q-Network written from scratch, hyperparameter search and comparison with Stable Baselines3. | Reinforcement Learning | Python, PyTorch |
| **[Serverless runtime](CN%20-%20Computa%C3%A7%C3%A3o%20em%20Nuvem/TP%203%20-%20Serverless/)** | Serverless functions on Kubernetes, a metrics dashboard and a custom serverless runtime backed by Redis. | Cloud Computing | Python, Redis, Kubernetes |
| **[Recommendation API on Kubernetes](CN%20-%20Computa%C3%A7%C3%A3o%20em%20Nuvem/TP%202%20-%20DevOps/)** | Playlist recommendation service deployed on Kubernetes with continuous delivery through Argo CD. | Cloud Computing | Python, Docker, Kubernetes |
| **[Contra-Diction](DJD%20-%20Desenvolvimento%20de%20Jogos%20Digitais/TP%20Final%20-%20Contra-Diction/)** | Team game: 2D action platformer about an astronaut lost on an alien planet. | Digital Game Development | C++, SDL2, OpenGL |
| **[Art Gallery Problem](Alg%20II%20-%20Algoritmos%20II/TP%201/)** | Polygon triangulation and vertex 3-coloring to place guards in an art gallery, with an interactive visualization. | Algorithms II | Python |
| **[Mobile robot with perception](IR%20-%20Introdu%C3%A7%C3%A3o%20%C3%A0%20Rob%C3%B3tica/TP%203%20-%20Rob%C3%B4%20M%C3%B3vel%20com%20Percep%C3%A7%C3%A3o/)** | Arduino robot with sensors and closed-loop control, built by a team of four. | Introduction to Robotics | C++, PlatformIO |
| **[Unix shell](SO%20-%20Sistemas%20Operacionais/TP%201/)** | Shell in C that runs commands, pipes and redirections using Linux system calls. | Operating Systems | C |

## Courses

| Folder | Course (English) | Semester | Main tech |
|---|---|---|---|
| [PDS I - Programação e Desenvolvimento de Software I](PDS%20I%20-%20Programa%C3%A7%C3%A3o%20e%20Desenvolvimento%20de%20Software%20I/) | Programming and Software Development I | 2020/1 | C, Allegro 5 |
| [ALC - Álgebra Linear Computacional](ALC%20-%20%C3%81lgebra%20Linear%20Computacional/) | Computational Linear Algebra | 2020/2 and 2021/1 | Python, NumPy, Jupyter |
| [MD - Matemática Discreta](MD%20-%20Matem%C3%A1tica%20Discreta/) | Discrete Mathematics | 2020/2 and 2021/1 | C |
| [PDS II - Programação e Desenvolvimento de Software II](PDS%20II%20-%20Programa%C3%A7%C3%A3o%20e%20Desenvolvimento%20de%20Software%20II/) | Programming and Software Development II | 2020/2 | C++ |
| [ED - Estruturas de Dados](ED%20-%20Estruturas%20de%20Dados/) | Data Structures | 2021/2 | C, C++, Make |
| [IBD - Introdução a Banco de Dados](IBD%20-%20Introdu%C3%A7%C3%A3o%20a%20Banco%20de%20Dados/) | Introduction to Databases | 2021/2 | SQL, SQLite, Python |
| [ISL - Introdução aos Sistemas Logicos](ISL%20-%20Introdu%C3%A7%C3%A3o%20aos%20Sistemas%20Logicos/) | Introduction to Logic Systems | 2021/2 | Python |
| [Alg I - Algoritmos I](Alg%20I%20-%20Algoritmos%20I/) | Algorithms I | 2023/1 and 2023/2 | C++, Make |
| [Probabilidade](Probabilidade/) | Probability | 2023/1 and 2023/2 | Python |
| [OC I - Organização de Computadores I](OC%20I%20-%20Organiza%C3%A7%C3%A3o%20de%20Computadores%20I/) | Computer Organization I | 2023/2 | RISC-V, Verilog, C++ |
| [Alg II - Algoritmos II](Alg%20II%20-%20Algoritmos%20II/) | Algorithms II | 2024/1 | Python |
| [ICD - Introdução à Ciência de Dados](ICD%20-%20Introdu%C3%A7%C3%A3o%20%C3%A0%20Ci%C3%AAncia%20de%20Dados/) | Introduction to Data Science | 2024/1 | Python, pandas, Jupyter |
| [LP - Linguagens de Programação](LP%20-%20Linguagens%20de%20Programa%C3%A7%C3%A3o/) | Programming Languages | 2024/1 | Prolog |
| [PO - Pesquisa Operacional](PO%20-%20Pesquisa%20Operacional/) | Operations Research | 2024/1 and 2024/2 | Julia, Python |
| [Álgebra A](%C3%81lgebra%20A/) | Algebra A (number theory) | 2024/1 | Python |
| [Compiladores I](Compiladores%20I/) | Compilers I | 2024/2 | Python |
| [ES - Engenharia de Software](ES%20-%20Engenharia%20de%20Software/) | Software Engineering I | 2024/2 | TypeScript, JavaScript, Java |
| [IFEC - Introdução à Física Estatística Computacional](IFEC%20-%20Introdu%C3%A7%C3%A3o%20%C3%A0%20F%C3%ADsica%20Estat%C3%ADstica%20Computacional/) | Introduction to Computational Statistical Physics | 2024/2 | Python, NumPy, Matplotlib, Jupyter |
| [SO - Sistemas Operacionais](SO%20-%20Sistemas%20Operacionais/) | Operating Systems | 2024/2 | C, Linux |
| [ES II - Engenharia de Software II](ES%20II%20-%20Engenharia%20de%20Software%20II/) | Software Engineering II | 2025/1 | Python, Flask, pytest, GitHub Actions |
| [FSPD - Fundamentos de Sistemas Paralelos e Distribuídos](FSPD%20-%20Fundamentos%20de%20Sistemas%20Paralelos%20e%20Distribu%C3%ADdos/) | Fundamentals of Parallel and Distributed Systems | 2025/1 | C, Pthreads, Python, gRPC |
| [IIA - Introdução à Inteligência Artificial](IIA%20-%20Introdu%C3%A7%C3%A3o%20%C3%A0%20Intelig%C3%AAncia%20Artificial/) | Introduction to Artificial Intelligence | 2025/1 | Python, C++, scikit-learn |
| [RC - Redes de Computadores](RC%20-%20Redes%20de%20Computadores/) | Computer Networks | 2025/1 | C, sockets |
| [AR - Aprendizado por Reforço](AR%20-%20Aprendizado%20por%20Refor%C3%A7o/) | Reinforcement Learning | 2025/2 | Python, Gymnasium, PyTorch |
| [CN - Computação em Nuvem](CN%20-%20Computa%C3%A7%C3%A3o%20em%20Nuvem/) | Cloud Computing | 2025/2 | Python, Spark, Docker, Kubernetes |
| [Cibersegurança](Ciberseguran%C3%A7a/) | Cybersecurity | 2025/2 | Python, C |
| [DJD - Desenvolvimento de Jogos Digitais](DJD%20-%20Desenvolvimento%20de%20Jogos%20Digitais/) | Digital Game Development | 2025/2 | C++, SDL2, OpenGL, CMake |
| [Nanocomputação](Nanocomputa%C3%A7%C3%A3o/) | Nanocomputing | 2025/2 | Python, PyTorch |
| [IR - Introdução à Robótica](IR%20-%20Introdu%C3%A7%C3%A3o%20%C3%A0%20Rob%C3%B3tica/) | Introduction to Robotics | 2026/1 | Arduino, C++, PlatformIO |
| [PPAM - Proteção da Privacidade em Aprendizado de Máquina](PPAM%20-%20Prote%C3%A7%C3%A3o%20da%20Privacidade%20em%20Aprendizado%20de%20M%C3%A1quina/) | Privacy Protection in Machine Learning | 2026/1 | Python, Flower, scikit-learn |
| [RI - Recuperação de Informação](RI%20-%20Recupera%C3%A7%C3%A3o%20de%20Informa%C3%A7%C3%A3o/) | Information Retrieval | 2026/1 | Python |
| [RSF - Redes Sem Fio](RSF%20-%20Redes%20Sem%20Fio/) | Wireless Networks | 2026/1 | C++, ns-3, Python |

## Notes

- Most code comments, notebooks and reports are in Portuguese.
- Assignment statements and starter code belong to the course instructors. Group projects list their team members in their own READMEs.
- Large datasets, build outputs and model weights are not included. When a project has its own public repository, the course README links to it.
- Some projects are private for now and are not published here. Their course READMEs mark them as private.
- If you are taking one of these courses, please use this repository only as a reference and follow your course's academic integrity rules.

## Author

Vinicius Trindade ([@Viniciustda](https://github.com/Viniciustda))
