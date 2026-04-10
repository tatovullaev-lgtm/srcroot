create database file_inspection;
--create extension dblink;

create table cluster_
(
   cluster_name varchar(20) primary key,
   temp_dir text not null default '/tmp',
   node_prefix varchar(10) null, -- from which symbols starts node names of the cluster (ncx..., ddc...)
   ssh_prefix text null
);

-- drop table storage_type (sim, exp, other...)
create table storage_type
(
   storage_type varchar(20) primary key
);

--drop table storage_
-- check_flags:   'd' means "delete absent files from the database"
-- replica_flags: 'e' means "replica enabled", 'a' means "add file if missing", 'd' means "delete_file if not exists on the source", 'u' means "recopy (update) file if different from the source"
create table storage_
(
   cluster_name varchar(20) not null references cluster_(cluster_name) on update cascade,
   storage_name varchar(30) not null,
   storage_path text not null,
   storage_type varchar(20) not null references storage_type(storage_type) on update cascade,
   xrootd_url text null,
   check_interval int null,
   full_check smallint null, -- if true then perform manual recalculation file checksum even if it exists in the File System 
   check_flags varchar(1),
   replica_cluster varchar(20) null,
   replica_storage varchar(30) null,
   replica_flags varchar(4) null,
   primary key(cluster_name, storage_name),
   foreign key (replica_cluster, replica_storage) references storage_(cluster_name, storage_name) on update cascade
);

--drop table storage_communication
create table storage_communication
(
   source_cluster varchar(20) not null references cluster_(cluster_name) on update cascade,
   destination_cluster varchar(20) not null,
   destination_storage varchar(30) not null,
   check_protocol smallint not null default 0,    -- 0 = local, 1 = ssh, 2 = xrootd, 3 = eos
   transfer_protocol smallint not null default 0, -- 0 = cp,    1 = scp, 2 = xrootd, 3 = eos
   primary key(source_cluster, destination_cluster, destination_storage),
   foreign key (destination_cluster, destination_storage) references storage_(cluster_name, storage_name) on update cascade
);

--drop table file_catalogue
create table file_catalogue
(
   file_guid serial primary key,
   cluster_name varchar(20) not null,
   storage_name varchar(30) not null,
   file_path text not null,
   file_size bigint null check (file_size >= 0),
   file_checksum varchar(32) null,
   replica_validity boolean null,
   foreign key (cluster_name, storage_name) references storage_(cluster_name, storage_name) on update cascade
);

--drop table storage_element_check
create table storage_check
(
   check_id serial primary key,
   cluster_name varchar(20) not null,
   storage_name varchar(30) not null,
   start_datetime timestamp not null default now()::timestamp,
   end_datetime timestamp null,
   file_count int not null check (file_count >= 0),
   full_check boolean not null default false,
   foreign key (cluster_name, storage_name) references storage_(cluster_name, storage_name)
);

create table error_type
(
   error_type_id smallserial primary key,
   error_type_name varchar (30) not null unique
);

--drop table failed_check
create table failed_check
(
   record_id serial primary key,
   check_id int not null references storage_check(check_id) on update cascade on delete cascade,
   file_guid int null references file_catalogue(file_guid) on update cascade,
   error_type_id smallint not null references error_type(error_type_id) on update cascade,
   error_details text
);

INSERT INTO cluster_(cluster_name, temp_dir, node_prefix, ssh_prefix)
VALUES
      ('ddc',  '/ceph/bmn/ssd/tmp', 'ddc',  'gertsen@ddc.jinr.ru'),
      ('ncx',  '/tmp',              'ncx',  'gertsen@ncx.jinr.ru'),
      ('cicc', '/tmp',              'lxui', 'gertsen@lxui.jinr.ru')

INSERT INTO storage_type(storage_type)
VALUES
      ('sim'),
      ('exp'),
      ('other')

INSERT INTO storage_(cluster_name, storage_name, storage_path, storage_type, xrootd_url, check_interval, full_check, check_flags, replica_cluster, replica_storage, replica_flags)
VALUES
      ('ddc',  'raw_data_run8', '/ceph/bmn/run/run8/beam/BMN_2022/', 'exp', null, 20, 4, '', null, null, null),  -- check flags: 'd' means "delete absent files from the database"
      ('ncx',  'exp_data', '/eos/nica/bmn/exp/', 'exp', 'root://ncm.jinr.ru/', 10, 3, '', 'ddc', 'raw data (run 8)', ''), -- replica_flags: 'e' means "replica enabled",
      ('ncx',  'sim_data', '/eos/nica/bmn/sim/', 'sim', 'root://ncm.jinr.ru/', 10, 3, '', null, null, null),              -- 'a' means "add file if missing",
      ('cicc', 'exp_data', '/eos/nica/bmn/exp/', 'exp', 'root://eos.jinr.ru/', 10, 3, '', 'ncx', 'exp data', ''),         -- 'd' means "delete_file if not exists on the source",
      ('cicc', 'sim_data', '/eos/nica/bmn/sim/', 'sim', 'root://eos.jinr.ru/', 10, 3, '', 'ncx', 'sim data', '')          -- 'u' means "recopy (update) file if different from the source"

-- check_protocol:    0 = local, 1 = ssh, 2 = xrootd, 3 = eos
-- transfer_protocol: 0 = cp,    1 = scp, 2 = xrootd, 3 = eos
-- protocol data format: "login1@host1:login2@host2" used by scp (ssh uses only the second part: ":login2@host2")
INSERT INTO storage_communication(source_cluster, destination_cluster, destination_storage, check_protocol, transfer_protocol)
VALUES
      ('ddc', 'ncx', 'exp data', 2, 2),
      ('ncx', 'ddc', 'raw data (run 8)', 1, 1),
      ('ncx', 'cicc', 'exp data', 2, 2),
      ('ncx', 'cicc', 'sim data', 2, 2),
      ('cicc', 'ncx', 'exp data', 2, 2),
      ('cicc', 'ncx', 'sim data', 2, 2)


--INSERT INTO file_catalogue(cluster_name, storage_name, file_path, file_size, file_checksum, replica_validity)
--SELECT 'cicc', 'exp data', file_path, file_size, file_checksum, replica_validity
--FROM dblink('host=bmn-elogdb.he.jinr.ru dbname=file_inspection_old user=mpd password=mpdsoft',
--            'select file_path,file_size,file_checksum,replica_validity from file_catalogue where se_id = 2')
--             as t1(file_path text, file_size bigint, file_checksum text, replica_validity boolean);

INSERT INTO error_type(error_type_name)
VALUES
      ('file not found'),
      ('file read error'),
      ('blank checksum'),
      ('different checksum')



select *
from file_catalogue fc
where file_path like '%8344_ev1_p5.data'


SELECT *
FROM operation_info oi full JOIN unnest(array['/eos','/eos2']) stor(file_path) ON oi.file_path = stor.file_path
where oi.file_path is null
limit 128

SELECT *
FROM unnest(array['/eos','/eos2']) stor(file_path)
where file_path not in (SELECT file_path FROM operation_info)
limit 128

create function array_diff(p_one int[], p_other int[])
  returns int[]
as
$$
  select array_agg(item)
  from (
     select *
     from unnest(p_one) item
     except
     select *
     from unnest(p_other)
  ) t
$$
language sql
stable;
select id, array_diff(arr1, arr2)
from the_table

with diff_data as (
    select id, unnest(arr1) as data
    from test_table
        except
    select id, unnest(arr2) as data
    from test_table
)
select id, array_agg(data order by data) as diff
from diff_data
group by id
