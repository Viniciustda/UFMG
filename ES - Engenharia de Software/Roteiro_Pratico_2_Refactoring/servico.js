// Classe ServicoCalculoFatura
module.exports = class ServicoCalculoFatura {

    // Construtor
    constructor(repo) {
        this.repo = repo;
    }

    // Função para calcular créditos
    calcularCredito(apre) {
      let creditos = 0;
      creditos += Math.max(apre.audiencia - 30, 0);
      if (this.repo.getPeca(apre).tipo === "comedia") 
          creditos += Math.floor(apre.audiencia / 5);
      return creditos;
    }

    // Função para calcular o total de créditos
    calcularTotalCreditos(apresentacoes) {
      return apresentacoes.reduce((totalCreditos, apre) => 
          totalCreditos + this.calcularCredito(apre), 0);
    }

    // Função para calcular o total de uma apresentação
    calcularTotalApresentacao(apre) {
      let total = 0;
      const peca = this.repo.getPeca(apre);
      switch (peca.tipo) {
          case "tragedia":
              total = 40000;
              if (apre.audiencia > 30) {
                  total += 1000 * (apre.audiencia - 30);
              }
              break;
          case "comedia":
              total = 30000;
              if (apre.audiencia > 20) {
                  total += 10000 + 500 * (apre.audiencia - 20);
              }
              total += 300 * apre.audiencia;
              break;
          default:
              throw new Error(`Peça desconhecida: ${peca.tipo}`);
      }
      return total;
    }

    // Função para calcular o total da fatura
    calcularTotalFatura(apresentacoes) {
      return apresentacoes.reduce((total, apre) => 
          total + this.calcularTotalApresentacao(apre), 0);
    }

}