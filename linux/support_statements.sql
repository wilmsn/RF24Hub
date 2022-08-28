use rf24hub;

create table sensordata_max engine = MEMORY as select UNIX_TIMESTAMP(FROM_UNIXTIME(max(utime),'%Y%m%d')) max_ts from sensordata_d;

delete from sensordata_d where utime > (select max_ts from sensordata_max);

insert into sensordata_d(sensor_id, value, utime) 
select sensor_id as s_id, min(value) as min_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+21600 as ts 
from sensordata where utime > (select max_ts from sensordata_max) group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'));

insert into sensordata_d(sensor_id, value, utime) 
select sensor_id as s_id, max(value) as max_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+64800 as ts 
from sensordata where utime > (select max_ts from sensordata_max) group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'));

drop table sensordata_max;


select * from sensor;
select sensor_id, from_unixtime(max(utime)) from sensordata group by sensor_id;




