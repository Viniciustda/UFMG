create schema escall;

create table escall.support_technician (
    id int primary key not null,
    name varchar (100) not null unique
);

create table escall.call (
    id int primary key not null,
    support_technician_id int,
    name varchar (100) not null,
    phone_number varchar (100) not null,
    description varchar (200) not null,
    status varchar (100) not null,
    created_at timestamp not null,
    technician_added_date timestamp,
    finished_at timestamp,
    priority varchar (100),
    constraint call_support_technician_fk foreign key (support_technician_id) references escall.support_technician(id)    
);

