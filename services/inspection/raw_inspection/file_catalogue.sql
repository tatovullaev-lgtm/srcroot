create database file_catalogue;

create table storage_
(
   storage_name varchar(20) primary key,
   xrootd_url varchar(50) null
);

create table run_
(
   run_number int primary key
);

create table run_storage
(
   run_storage_id serial primary key,
   run_number int not null references run_(run_number),
   storage_name varchar(20) not null references storage_(storage_name),
   storage_path varchar(255) not null
);

drop table file_catalogue
create table file_catalogue
(
   file_guid serial primary key,
   run_storage_id int not null references run_storage(run_storage_id),
   file_path varchar(255) not null,
   file_size bigint null check (file_size >= 0),
   file_checksum varchar(32) null
);

insert into storage_(storage_name)
values 
      ('ddc'),
      ('ncx'),
      ('cicc');
insert into run_(run_number)
values
      (7),
      (8);
insert into run_storage(run_number, storage_name, storage_path)
values
      (8, 'ddc', '/ceph/bmn/run/run8/beam/BMN_2022/'),
      (8, 'ncx', '/eos/nica/bmn/exp/raw/run8/beam/BMN_2022/'),
      (8, 'cicc', '/eos/nica/bmn/exp/raw/run8/beam/BMN_2022/');


select *
from file_catalogue fc
where run_storage_id = 3

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
