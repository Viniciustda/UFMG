
-- add call

insert into postgres.escall."call" (id, support_technician_id, "name", phone_number, description, status, created_at, technician_added_date, finished_at, priority)
values (1, null, 'Barack Obama', '(31) 999988854', 'O computador não está acessando a internet', 'WAITING', '2024-10-28', null, null, null);

insert into postgres.escall."call" (id, support_technician_id, "name", phone_number, description, status, created_at, technician_added_date, finished_at, priority)
values (2, null, 'Nelson Mandela', '(31) 999983333', 'O computador não está ligando', 'WAITING', '2024-10-29', null, null, null);

insert into postgres.escall."call" (id, support_technician_id, "name", phone_number, description, status, created_at, technician_added_date, finished_at, priority)
values (3, 3, 'Eduardo Saverin', '(31) 977798333', 'O teclado parou de funcionar', 'ALLOCATED', '2024-10-30', null, null, null);
