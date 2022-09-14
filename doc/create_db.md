



show databases;

create database www;

create user 'wwwuser'@'%' identified by '**********';

grant all on www.* to 'wwwuser'@'%' identified by '***********';

flush privileges;

use mysql;

select user, host from user;

user rf24hub;


