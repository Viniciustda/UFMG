const { readFileSync } = require('fs');

// Classe Repositorio
module.exports = class Repositorio {

    // Construtor
    constructor() {
        this.pecas = JSON.parse(readFileSync('./pecas.json'));
    }
  
    // Função query
    getPeca(apre) {
        return this.pecas[apre.id];
    }
}