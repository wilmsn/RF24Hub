use rf24hub;

-- Tabelle "sensordata_d" komplett neu erstellen
create table sensordata_max engine = MEMORY as select UNIX_TIMESTAMP(FROM_UNIXTIME(max(utime),'%Y%m%d')) max_ts from sensordata_d;

delete from sensordata_d where utime > (select max_ts from sensordata_max);

insert into sensordata_d(sensor_id, value, utime) 
select sensor_id as s_id, min(value) as min_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+21600 as ts 
from sensordata where utime > (select max_ts from sensordata_max) group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'));

insert into sensordata_d(sensor_id, value, utime) 
select sensor_id as s_id, max(value) as max_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+64800 as ts 
from sensordata where utime > (select max_ts from sensordata_max) group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'));

drop table sensordata_max;
-- ENDE: Tabelle "sensordata_d" komplett neu erstellen
-- -----------------------------------------------------------------
-- Solarernte als täglichen Wert neu berechnen
delete from sensor_im where sensor_id = 10002;
delete from sensordata where sensor_id = 10002;

insert into sensordata(sensor_id, utime, value) 
select 10002, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d')), sum(value*value/4*200)/(count(value)/24)
from sensordata 
where sensor_id in (51, 41) group by UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))
order by UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d')) asc;

delete from sensordata where sensor_id = 10002 and utime = (select max(utime) from sensordata where sensor_id = 10002 );
-- ENDE: Solarernte als täglichen Wert neu berechnen





