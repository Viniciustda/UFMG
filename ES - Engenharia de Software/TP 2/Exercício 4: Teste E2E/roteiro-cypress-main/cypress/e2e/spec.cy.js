describe('TODOMvc App', () => {
  it('Verifica se app está abrindo', () => {
    cy.visit('')
  })

  it('Insere uma tarefa', () => {
    cy.visit(''); 

    cy.get('[data-cy=todo-input]')
      .type('TP2 de Engenharia de Software{enter}');

    cy.get('[data-cy=todos-list]')
      .children()
      .should('have.length', 1) 
      .first()
      .should('have.text', 'TP2 de Engenharia de Software'); 
  });

  it('Insere e deleta uma tarefa', () => {
    cy.visit('');

    cy.get('[data-cy=todo-input]')
      .type('TP2 de Engenharia de Software{enter}');

    cy.get('[data-cy=todos-list]')
      .children()
      .should('have.length', 1);

    cy.get('[data-cy=todos-list] > li [data-cy=remove-todo-btn]')
      .invoke('show')
      .click();

    cy.get('[data-cy=todos-list]')
      .children()
      .should('have.length', 0);
  });

  it('Filtra tarefas completas e ativas', () => {
    cy.visit(''); 

    cy.get('[data-cy=todo-input]')
      .type('TP2 de ES{enter}')
      .type('Prova de ES{enter}');

    cy.get('[data-cy=todos-list] > li [data-cy=toggle-todo-checkbox]')
      .first()
      .click();

    cy.get('[data-cy=filter-active-link')
      .click();
    cy.get('[data-cy=todos-list]')
      .children()
      .should('have.length', 1)
      .first()
      .should('have.text', 'Prova de ES');

    cy.get('[data-cy=filter-completed-link')
      .click();
    cy.get('[data-cy=todos-list]')
      .children()
      .should('have.length', 1)
      .first()
      .should('have.text', 'TP2 de ES');

    cy.get('[data-cy=filter-all-link')
      .click();
    cy.get('[data-cy=todos-list]')
      .children()
      .should('have.length', 2);
  });

  it('Insere uma tarefa, marca como completa e volta para ativa', () => {
    cy.visit('');
  
    // Adiciona uma tarefa
    cy.get('[data-cy=todo-input]')
      .type('TP2 de Engenharia de Software{enter}');
  
    cy.get('[data-cy=todos-list]')
      .children()
      .should('have.length', 1)
      .first()
      .should('have.text', 'TP2 de Engenharia de Software');
  
    // Marca como completa
    cy.get('[data-cy=todos-list] > li [data-cy=toggle-todo-checkbox]')
      .click();
    cy.get('[data-cy=todos-list] > li')
      .first()
      .should('have.class', 'completed');
  
    // Volta para ativa
    cy.get('[data-cy=todos-list] > li [data-cy=toggle-todo-checkbox]')
      .click();
    cy.get('[data-cy=todos-list] > li')
      .first()
      .should('not.have.class', 'completed');
  });
  
  it('Cria uma tarefa e edita a tarefa', () => {
    cy.visit('');
  
    // Adiciona uma tarefa
    cy.get('[data-cy=todo-input]')
      .type('TP2 de ES{enter}');
  
    // Ativa o modo de edição
    cy.get('[data-cy=todos-list] > li')
      .dblclick();
  
    // Edita tarefa
    cy.get('input.edit') 
      .clear()
      .type('Prova de ES atualizada{enter}');
  
    cy.get('[data-cy=todos-list]')
      .first()
      .should('have.text', 'Prova de ES atualizada');
  });  
  
  it('Limpa todas as tarefas completas', () => {
    cy.visit('');
  
    // Adiciona duas tarefas
    cy.get('[data-cy=todo-input]')
      .type('TP2 de ES{enter}')
      .type('Prova de ES{enter}');
  
    // Marca uma tarefa como completa
    cy.get('[data-cy=todos-list] > li [data-cy=toggle-todo-checkbox]')
      .first()
      .click();
  
    // Limpa tarefas completas
    cy.get('.clear-completed') 
      .click();
  
    // Verifica que apenas tarefas ativas permanecem
    cy.get('[data-cy=todos-list]')
      .children()
      .should('have.length', 1)
      .first()
      .should('have.text', 'Prova de ES');
  });  
  
});