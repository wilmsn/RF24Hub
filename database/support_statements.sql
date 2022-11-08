use rf24hub;

-- Tabelle sensordata_d komplett neu aufbauen
truncate table sensordata_d;
insert into sensordata_d(sensor_id, value, utime) 
select sensor_id, min(value) as min_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+21600 
from sensordata group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'));
insert into sensordata_d(sensor_id, value, utime) 
select sensor_id, max(value) as max_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+64800 
from sensordata group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'));
commit;

-- Tabelle sensordata_d inkrementell neu aufbauen
create table sensordata_max engine = MEMORY as select UNIX_TIMESTAMP(FROM_UNIXTIME(max(utime),'%Y%m%d')) max_ts from sensordata_d;
delete from sensordata_d where utime > (select max_ts from sensordata_max);
insert into sensordata_d(sensor_id, value, utime) 
select sensor_id as s_id, min(value) as min_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+21600 as ts 
from sensordata where utime > (select max_ts from sensordata_max) group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'));
insert into sensordata_d(sensor_id, value, utime) 
select sensor_id as s_id, max(value) as max_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+64800 as ts 
from sensordata where utime > (select max_ts from sensordata_max) group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'));
drop table sensordata_max;

insert into sensordata_sum (sensor_id, anzahl)
select sensor_id, count(*) from sensordata group by sensor_id;

--Erweiterung der Partitionen nach Jahreswechsel
ALTER TABLE sensordata
    REORGANIZE PARTITION p_now INTO (
        PARTITION p2023 VALUES LESS THAN (UNIX_TIMESTAMP('24/01/01')),
        PARTITION p_now     VALUES LESS THAN MAXVALUE);

--Erweiterung der Partitionen nach Jahreswechsel
ALTER TABLE sensordata_d
    REORGANIZE PARTITION p_now INTO (
        PARTITION p2023 VALUES LESS THAN (UNIX_TIMESTAMP('24/01/01')),
        PARTITION p_now     VALUES LESS THAN MAXVALUE);

--Anzeigen des create Statements
SHOW CREATE TABLE sensordata;


ALTER TABLE sensordata_d
    REORGANIZE PARTITION p2022 INTO (
        PARTITION p_now     VALUES LESS THAN MAXVALUE);

create table sensordata2 as select * from sensordata;

DROP TABLE IF EXISTS sensordata;

CREATE TABLE `sensordata` (
  `sensor_id` int(10) unsigned NOT NULL,
  `utime` int(10) unsigned NOT NULL,
  `value` float DEFAULT NULL,
  PRIMARY KEY (`sensor_id`,`utime`),
  KEY `sensordata_utime_idx` (`utime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
 PARTITION BY RANGE (`utime`)
(PARTITION `p2014` VALUES LESS THAN (UNIX_TIMESTAMP('15/01/01')) ENGINE = InnoDB,
 PARTITION `p2015` VALUES LESS THAN (UNIX_TIMESTAMP('16/01/01')) ENGINE = InnoDB,
 PARTITION `p2016` VALUES LESS THAN (UNIX_TIMESTAMP('17/01/01')) ENGINE = InnoDB,
 PARTITION `p2017` VALUES LESS THAN (UNIX_TIMESTAMP('18/01/01')) ENGINE = InnoDB,
 PARTITION `p2018` VALUES LESS THAN (UNIX_TIMESTAMP('19/01/01')) ENGINE = InnoDB,
 PARTITION `p2019` VALUES LESS THAN (UNIX_TIMESTAMP('20/01/01')) ENGINE = InnoDB,
 PARTITION `p2020` VALUES LESS THAN (UNIX_TIMESTAMP('21/01/01')) ENGINE = InnoDB,
 PARTITION `p2021` VALUES LESS THAN (UNIX_TIMESTAMP('22/01/01')) ENGINE = InnoDB,
 PARTITION `pnow` VALUES LESS THAN MAXVALUE ENGINE = InnoDB);

insert into sensordata (sensor_id, utime,value)
select sensor_id, utime,value from sensordata2;


create table sensordata_d2 as select * from sensordata_d;

DROP TABLE IF EXISTS sensordata_d;

CREATE TABLE `sensordata_d` (
  `sensor_id` int(10) unsigned NOT NULL,
  `utime` int(10) unsigned NOT NULL,
  `value` float DEFAULT NULL,
  PRIMARY KEY (`sensor_id`,`utime`),
  KEY `sensordata_d_utime_idx` (`utime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
 PARTITION BY RANGE (`utime`)
(PARTITION `p2014` VALUES LESS THAN (UNIX_TIMESTAMP('15/01/01')) ENGINE = InnoDB,
 PARTITION `p2015` VALUES LESS THAN (UNIX_TIMESTAMP('16/01/01')) ENGINE = InnoDB,
 PARTITION `p2016` VALUES LESS THAN (UNIX_TIMESTAMP('17/01/01')) ENGINE = InnoDB,
 PARTITION `p2017` VALUES LESS THAN (UNIX_TIMESTAMP('18/01/01')) ENGINE = InnoDB,
 PARTITION `p2018` VALUES LESS THAN (UNIX_TIMESTAMP('19/01/01')) ENGINE = InnoDB,
 PARTITION `p2019` VALUES LESS THAN (UNIX_TIMESTAMP('20/01/01')) ENGINE = InnoDB,
 PARTITION `p2020` VALUES LESS THAN (UNIX_TIMESTAMP('21/01/01')) ENGINE = InnoDB,
 PARTITION `p2021` VALUES LESS THAN (UNIX_TIMESTAMP('22/01/01')) ENGINE = InnoDB,
 PARTITION `pnow` VALUES LESS THAN MAXVALUE ENGINE = InnoDB);

insert into sensordata_d (sensor_id, utime,value)
select sensor_id, utime,value from sensordata_d2;
