const bd = require('../bd/bd_utils.js');
const modelo = require('../modelo.js');

beforeEach(() => {
  bd.reconfig('./bd/esmforum-teste.db');
  // limpa dados de todas as tabelas
  bd.exec('delete from perguntas', []);
  bd.exec('delete from respostas', []);
});

test('Testando banco de dados vazio', () => {
  expect(modelo.listar_perguntas().length).toBe(0);
});

test('Testando cadastro de três perguntas', () => {
  modelo.cadastrar_pergunta('1 + 1 = ?');
  modelo.cadastrar_pergunta('2 + 2 = ?');
  modelo.cadastrar_pergunta('3 + 3 = ?');
  const perguntas = modelo.listar_perguntas(); 
  expect(perguntas.length).toBe(3);
  expect(perguntas[0].texto).toBe('1 + 1 = ?');
  expect(perguntas[1].texto).toBe('2 + 2 = ?');
  expect(perguntas[2].num_respostas).toBe(0);
  expect(perguntas[1].id_pergunta).toBe(perguntas[2].id_pergunta-1);
});

test('Testando cadastro de três respostas', () => {
  modelo.cadastrar_pergunta('6 * 6 = ?');
  modelo.cadastrar_pergunta('7 * 8 = ?');
  modelo.cadastrar_pergunta('9 * 9 = ?');

  const perguntas = modelo.listar_perguntas();

  const id_pergunta_0 = perguntas[0].id_pergunta;
  const id_pergunta_1 = perguntas[1].id_pergunta;
  const id_pergunta_2 = perguntas[2].id_pergunta;

  modelo.cadastrar_resposta(id_pergunta_0, '36');
  modelo.cadastrar_resposta(id_pergunta_1, '56');
  modelo.cadastrar_resposta(id_pergunta_2, '81');
  
  const resposta_0 = modelo.get_respostas(id_pergunta_0);
  const resposta_1 = modelo.get_respostas(id_pergunta_1);
  const resposta_2 = modelo.get_respostas(id_pergunta_2); 
  
  expect(resposta_0[0].texto).toBe('36');
  expect(resposta_1[0].texto).toBe('56');
  expect(resposta_2[0].texto).toBe('81');
});

test('Testando selecao de pergunta', () => {
  modelo.cadastrar_pergunta('2 * 2 = ?');
  modelo.cadastrar_pergunta('3 * 3 = ?');
  modelo.cadastrar_pergunta('4 * 4 = ?');

  const perguntas = modelo.listar_perguntas();

  const id_pergunta_0 = perguntas[0].id_pergunta;
  const id_pergunta_1 = perguntas[1].id_pergunta;
  const id_pergunta_2 = perguntas[2].id_pergunta;
  
  const pergunta_0 = modelo.get_pergunta(id_pergunta_0);
  const pergunta_1 = modelo.get_pergunta(id_pergunta_1);
  const pergunta_2 = modelo.get_pergunta(id_pergunta_2);

  expect(pergunta_0.texto).toBe('2 * 2 = ?');
  expect(pergunta_1.texto).toBe('3 * 3 = ?');
  expect(pergunta_2.texto).toBe('4 * 4 = ?');
});

test('Testando contagem de três respostas', () => {
  modelo.cadastrar_pergunta('a * b = ?');

  const perguntas = modelo.listar_perguntas();

  const id_pergunta = perguntas[0].id_pergunta;

  modelo.cadastrar_resposta(id_pergunta, 'c');
  modelo.cadastrar_resposta(id_pergunta, 'd');
  modelo.cadastrar_resposta(id_pergunta, 'e');

  const num_respostas = modelo.get_num_respostas(id_pergunta);

  expect(num_respostas).toBe(3);
});