create database file_inspection;

-- drop table check_type (simulated files, experimental files, etc.)
create table check_type
(
	type_id smallserial primary key,
	type_name varchar (30) not null unique
);

-- drop table storage_
create table storage_
(
	storage_id smallserial primary key,
	storage_name varchar (20) not null unique
);

-- drop table integrity_error
create table integrity_error
(
	error_id smallserial primary key,
	error_name varchar (30) not null unique
);

-- drop table integrity_check
-- alter table integrity_check add column complete_date timestamp null
create table integrity_check
(
	check_id serial primary key,
	type_id smallint not null references check_type(type_id) on update cascade,
	storage_id smallint not null references storage_(storage_id) on update cascade,
	check_date timestamp not null default now()::timestamp,
	complete_date timestamp null,
	file_count int not null check (file_count >= 0)
);

-- drop table failed_record
create table failed_record
(
	record_id serial primary key,
	check_id int not null references integrity_check(check_id),
	error_id smallint not null references integrity_error(error_id) on update cascade,
	file_path varchar(255) not null,
	error_details varchar(100)
);

INSERT INTO check_type(type_name)
VALUES ('simulation data')
INSERT INTO check_type(type_name)
VALUES ('experimental data')


INSERT INTO storage_(storage_name)
VALUES ('NCX')
INSERT INTO storage_(storage_name)
VALUES ('MICC')

INSERT INTO integrity_error(error_name)
VALUES ('file not found')
INSERT INTO integrity_error(error_name)
VALUES ('file read error')
INSERT INTO integrity_error(error_name)
VALUES ('blank file checksum')
INSERT INTO integrity_error(error_name)
VALUES ('different file checksum')
