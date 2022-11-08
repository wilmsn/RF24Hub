-- Nummernplan:
-- Aussenthermometer1: 21...30
-- Aussenthermometer2: 31...40
-- Solarnode1:         41...50
-- Solarnode2:         51...60
-- Node 100:           101...110
--     ...
-- Node 110            201...210
-- Testnodes
-- Node 240:           1001...1010
-- ESP Node 1          9001...
-- Virtuelle Sensoren  10001...

-- Tabellen sichern
-- create table sensor_v20 as select * from sensor;
-- create table sensordata_v20 as select * from sensordata;

-- Laufzeit ab hier ca. 50 Minuten !!
-- Sensoren der normalen Zimmernodes löschen
drop table if exists sensor_delete;
create table sensor_delete as select sensor_id from sensor where node_id in (100,101,102,103,104,105,106,107,108,109,110,160,161,165,169,170 );
delete from sensordata where sensor_id in (select sensor_id from sensor_delete);
delete from sensor where sensor_id in (select sensor_id from sensor_delete);

-- Aussenthermometer1 geht auf 21...30
update sensor set sensor_id = 30 where sensor_id = 29;
update sensor set sensor_id = 23 where sensor_id = 22;
update sensor set sensor_id = 22 where sensor_id = 21;
update sensor set sensor_id = 21 where sensor_id = 20;

update sensordata set sensor_id = 30 where sensor_id = 29;
update sensordata set sensor_id = 23 where sensor_id = 22;
update sensordata set sensor_id = 22 where sensor_id = 21;
update sensordata set sensor_id = 21 where sensor_id = 20;

-- Aussenthermometer2 geht auf 31...40
update sensor set sensor_id = 31 where sensor_id = 120;
update sensor set sensor_id = 32 where sensor_id = 121;
update sensor set sensor_id = 33 where sensor_id = 122;
update sensor set sensor_id = 40 where sensor_id = 129;

update sensordata set sensor_id = 31 where sensor_id = 120;
update sensordata set sensor_id = 32 where sensor_id = 121;
update sensordata set sensor_id = 33 where sensor_id = 122;
update sensordata set sensor_id = 40 where sensor_id = 129;

-- Solarnode1 => Sensor 42 und 43 tauschen
update sensor set sensor_id = 99 where sensor_id = 42;
update sensor set sensor_id = 42 where sensor_id = 43;
update sensor set sensor_id = 43 where sensor_id = 99;
update sensor set sensor_id = 50 where sensor_id = 49;
update sensordata set sensor_id = 99 where sensor_id = 42;
update sensordata set sensor_id = 42 where sensor_id = 43;
update sensordata set sensor_id = 43 where sensor_id = 99;
update sensordata set sensor_id = 50 where sensor_id = 49;




-- ESP Node: Node 1
update sensor set sensor_id = 9001 where sensor_id = 1001;
update sensor set sensor_id = 9002 where sensor_id = 1002;
update sensor set sensor_id = 9003 where sensor_id = 1003;
update sensor set sensor_id = 9004 where sensor_id = 1006;
update sensor set sensor_id = 9005 where sensor_id = 1011;
update sensor set sensor_id = 9006 where sensor_id = 1012;
update sensor set sensor_id = 9007 where sensor_id = 1016;
update sensor set sensor_id = 9008 where sensor_id = 1017;

update sensordata set sensor_id = 9001 where sensor_id = 1001;
update sensordata set sensor_id = 9002 where sensor_id = 1002;
update sensordata set sensor_id = 9003 where sensor_id = 1003;
update sensordata set sensor_id = 9004 where sensor_id = 1006;
update sensordata set sensor_id = 9005 where sensor_id = 1011;
update sensordata set sensor_id = 9006 where sensor_id = 1012;
update sensordata set sensor_id = 9007 where sensor_id = 1016;
update sensordata set sensor_id = 9008 where sensor_id = 1017;


-- Node Solarnode2, 100 bis 110 neu anlegen
INSERT INTO sensor (sensor_id,sensor_name,add_info,node_id,channel,store_days,fhem_dev,html_show,html_order,datatype) 
VALUES
  (21,'Aussen1_Temp','Sensor alt 20',200,1,-1,'Aussen2_Temp','y',1,0),
  (22,'Aussen1_Pres','Sensor alt 21',200,2,-1,'Aussen2_Pres','y',2,0),
  (23,'Aussen1_Humi','Sensor alt 22',200,3,-1,'Aussen2_Humi','y',3,0),
  (30,'Aussen1_Ubatt','Sensor alt 29',200,79,-1,'Aussen2_Ubatt','y',9,0);
  
INSERT INTO sensor (sensor_id,sensor_name,add_info,node_id,channel,store_days,fhem_dev,html_show,html_order,datatype) 
VALUES
  (31,'Aussen2_Temp','Sensor alt 120',201,1,-1,'Aussen2_Temp','y',1,0),
  (32,'Aussen2_Pres','Sensor alt 121',201,2,-1,'Aussen2_Pres','y',2,0),
  (33,'Aussen2_Humi','Sensor alt 122',201,3,-1,'Aussen2_Humi','y',3,0),
  (40,'Aussen2_Ubatt','Sensor alt 129',201,79,-1,'Aussen2_Ubatt','y',9,0);

INSERT INTO sensor (sensor_id,sensor_name,add_info,node_id,channel,store_days,fhem_dev,html_show,html_order,datatype) 
VALUES
  (41,'Solarzelle1','',202,5,-1,'Solarnode_Solarspannung','y',1,0),
  (42,'Solarzelle2','Spannung der speisenden Solarzelle',202,6,-1,'Solarnode_Zelle2','y',2,0),
  (43,'Solarnode_U_Batt1','Spannung der ersten Batterie',202,7,-1,'Solarnode_Ubatt1','y',4,0),
  (46,'Batterie_Status','Statusübersicht:
1 : No DS
2 : LED on
4 : Discharge
4 : Dis Batt1
8 : Dis Batt2',202,8,-1,'Solarnode_batterie_status','y',6,0),
  (50,'Solarnode_UBatt','',202,79,-1,'Solarnode_UBatt','y',9,0);
  
INSERT INTO sensor (sensor_id,sensor_name,add_info,node_id,channel,store_days,fhem_dev,html_show,html_order,datatype) 
VALUES
  (51,'Solarzelle1','Sensor_alt 41',203,5,-1,'Solarnode_Solarspannung','y',1,0),
  (52,'Solarzelle2','Spannung der speisenden Solarzelle (alt43)',203,6,-1,'Solarnode_Zelle2','y',2,0),
  (53,'Solarnode_U_Batt1','Spannung der ersten Batterie (alt42)',203,7,-1,'Solarnode_Ubatt1','y',4,0),
  (56,'Batterie_Status','Statusübersicht:
1 : No DS
2 : LED on
4 : Discharge
4 : Dis Batt1
8 : Dis Batt2 (alt 46)',203,8,-1,'Solarnode_batterie_status','y',6,0),
  (60,'Solarnode_UBatt','alt 49',203,79,-1,'Solarnode_UBatt','y',9,0);
  
INSERT INTO sensor (sensor_id,sensor_name,add_info,node_id,channel,store_days,fhem_dev,html_show,html_order,datatype) 
VALUES
  (101,'Bastelzimmer_Temp',NULL,100,1,-1,'Bastelzimmer_Temp','y',1,0),
  (102,'Bastelzimmer_Feld1','',100,21,-1,'Bastelzimmer_Feld1','n',2,0),
  (103,'Bastelzimmer_Feld2',NULL,100,22,-1,'Bastelzimmer_Feld2','n',3,0),
  (104,'Bastelzimmer_Feld3',NULL,100,23,-1,'Bastelzimmer_Feld3','n',4,0),
  (105,'Bastelzimmer_Feld4','',100,24,-1,'Bastelzimmer_Feld4','y',5,0),
  (106,'Bastelzimmer_Display',NULL,100,52,-1,'Bastelzimmer_Display','n',6,0),
  (107,'Bastelzimmer_Licht',NULL,100,51,-1,'Bastelzimmer_Licht','n',7,0),
  (110,'Bastelzimmer_Ubatt',NULL,100,79,-1,'Bastelzimmer_Ubatt','y',9,0),
  (111,'N101_Temp','',101,1,-1,'N101_Temp','y',1,0),
  (112,'N101_Pres','',101,2,-1,'N101_Pres','y',2,0),
  (113,'N101_Humi','',101,3,-1,'N101_Humi','y',3,0),
  (114,'N101_Feld1','',101,21,-1,'N101_Feld1','n',4,0),
  (115,'N101_Feld2','',101,22,-1,'N101_Feld2','n',5,0),
  (116,'N101_Feld3','',101,23,-1,'N101_Feld3','n',6,0),
  (117,'N101_Feld4','',101,24,-1,'N101_Feld4','y',7,0),
  (118,'N101_Licht','',101,51,-1,'N101_Licht','n',8,0),
  (119,'N101_Display','',101,52,-1,'N101_Display','y',9,0),
  (120,'N101_Ubatt','',101,79,-1,'N101_Ubatt','y',10,0),
  (121,'Schlafzimmer_Temp','',102,1,-1,'Schlafzimmer_Temp','y',1,0),
  (122,'Schlafzimmer_Pres','',102,2,-1,'Schlafzimmer_Pres','y',2,0),
  (123,'Schlafzimmer_Humi','',102,3,-1,'Schlafzimmer_Humi','y',3,0),
  (124,'Schlafzimmer_Feld1','',102,21,-1,'Schlafzimmer_Feld1','n',4,0),
  (125,'Schlafzimmer_Feld2','',102,22,-1,'Schlafzimmer_Feld2','n',5,0),
  (126,'Schlafzimmer_Feld3','',102,23,-1,'Schlafzimmer_Feld3','n',6,0),
  (127,'Schlafzimmer_Feld4','',102,24,-1,'Schlafzimmer_Feld4','y',7,0),
  (128,'Schlafzimmer_Licht','',102,51,-1,'Schlafzimmer_Licht','n',8,0),
  (129,'Schlafzimmer_Display','',102,52,-1,'Schlafzimmer_Display','y',9,0),
  (130,'Schlafzimmer_Ubatt','',102,79,-1,'Schlafzimmer_Ubatt','y',10,0),
  (131,'Kueche_Temp',NULL,103,1,-1,'Kueche_Temp','y',1,0),
  (134,'Kueche_Feld1','',103,21,-1,'Kueche_Feld1','y',2,0),
  (135,'Kueche_Feld2',NULL,103,22,-1,'Kueche_Feld2','n',3,0),
  (136,'Kueche_Feld3',NULL,103,23,-1,'Kueche_Feld3','n',4,0),
  (137,'Kueche_Feld4','',103,24,-1,'Kueche_Feld4','y',5,0),
  (138,'Kueche_Licht',NULL,103,51,-1,'Kueche_Licht','n',6,0),
  (139,'Kueche_Display',NULL,103,52,-1,'Kueche_Display','n',7,0),
  (140,'Kueche_Ubatt',NULL,103,79,-1,'Kueche_Ubatt','y',9,0),
  (141,'Gaestezimmer_Temp',NULL,104,1,-1,'Gaestezimmer_Temp','y',1,0),
  (150,'Gaestezimmer_Ubatt',NULL,104,79,-1,'Gaestezimmer_Ubatt','y',9,0),
  (151,'Bastelzimmer_sw_Temp','Dallas 18B20',105,1,-1,'Bastelzimmer_sw_Temp','y',1,0),
  (154,'Bastelzimmer_sw_Feld1','',105,21,-1,'Bastelzimmer_sw_Feld1','n',4,0),
  (155,'Bastelzimmer_sw_Feld2','Dallas 18B20',105,22,-1,'Bastelzimmer_sw_Feld2','n',5,0),
  (156,'Bastelzimmer_sw_Feld3','Dallas 18B20',105,23,-1,'Bastelzimmer_sw_Feld3','n',6,0),
  (157,'Bastelzimmer_sw_Feld4','Dallas 18B20',105,24,-1,'Bastelzimmer_sw_Feld4','y',7,0),
  (160,'Bastelzimmer_sw_Ubatt','Dallas 18B20',105,79,-1,'Bastelzimmer_sw_Ubatt','y',9,0),
  (161,'Kugelnode1_Temp','',106,1,-1,'Kugelnode1_Temp','y',1,0),
  (162,'Kugelnode1_Pres','',106,2,-1,'Kugelnode1_Pres','y',2,0),
  (164,'Kugelnode1_Feld1','',106,21,-1,'Kugelnode1_Feld1','y',5,0),
  (165,'Kugelnode1_Feld2','',106,22,-1,'Kugelnode1_Feld2','y',6,0),
  (166,'Kugelnode1_Feld3','',106,23,-1,'Kugelnode1_Feld3','y',7,0),
  (167,'Kugelnode1_Feld4','',106,24,-1,'Kugelnode1_Feld4','y',8,0),
  (168,'Kugelnode1_Display','',106,52,-1,'Kugelnode1_Display','y',99,0),
  (170,'Kugelnode1_Ubatt','',106,79,-1,'Kugelnode1_Ubatt','y',101,0),
  (171,'Kugelnode2_Temp','',107,1,-1,'Kugelnode2_Temp','y',1,0),
  (172,'Kugelnode2_Pres','',107,2,-1,'Kugelnode2_Pres','y',2,0),
  (174,'Kugelnode2_Feld1','',107,21,-1,'Kugelnode2_Feld1','y',5,0),
  (175,'Kugelnode2_Feld2','',107,22,-1,'Kugelnode2_Feld2','y',6,0),
  (176,'Kugelnode2_Feld3','',107,23,-1,'Kugelnode2_Feld3','y',7,0),
  (177,'Kugelnode2_Feld4','',107,24,-1,'Kugelnode2_Feld4','y',8,0),
  (178,'Kugelnode2_Display','',107,52,-1,'Kugelnode2_Display','y',99,0),
  (180,'Kugelnode2_Ubatt','',107,79,-1,'Kugelnode2_Ubatt','y',9,0),
  (181,'N108_Temp','',108,1,-1,'N108_Temp','y',1,0),
  (182,'N108_Pres','',108,2,-1,'N108_Pres','y',2,0),
  (183,'N108_Humi','',108,3,-1,'N108_Humi','y',3,0),
  (184,'N108_Feld1','',108,21,-1,'N108_Feld1','n',4,0),
  (185,'N108_Feld2','',108,22,-1,'N108_Feld2','n',5,0),
  (186,'N108_Feld3','',108,23,-1,'N108_Feld3','n',6,0),
  (187,'N108_Feld4','',108,24,-1,'N108_Feld4','y',7,0),
  (189,'N108_Display','',108,52,-1,'N108_Display','y',9,0),
  (190,'N108_Ubatt','',108,79,-1,'N108_Ubatt','y',10,0),
  (191,'N109_Temp','',109,1,-1,'N109_Temp','y',1,0),
  (192,'N109_Pres','',109,2,-1,'N109_Pres','y',2,0),
  (193,'N109_Humi','',109,3,-1,'N109_Humi','y',3,0),
  (194,'N109_Feld1','',109,21,-1,'N109_Feld1','n',4,0),
  (195,'N109_Feld2','',109,22,-1,'N109_Feld2','n',5,0),
  (196,'N109_Feld3','',109,23,-1,'N109_Feld3','n',6,0),
  (197,'N109_Feld4','',109,24,-1,'N109_Feld4','y',7,0),
  (198,'N109_Licht','',109,51,-1,'N109_Licht','n',8,0),
  (199,'N109_Display','',109,52,-1,'N109_Display','y',9,0),
  (200,'N109_Ubatt','',109,79,-1,'N109_Ubatt','y',10,0),  
  (201,'Ankleidezimmer_Temp',NULL,110,1,-1,'Ankleidezimmer_Temp','y',1,0),
  (204,'Ankleidezimmer_Feld1','',110,21,-1,'Ankleidezimmer_Feld1','n',2,0),
  (205,'Ankleidezimmer_Feld2',NULL,110,22,-1,'Ankleidezimmer_Feld2','n',3,0),
  (206,'Ankleidezimmer_Feld3',NULL,110,23,-1,'Ankleidezimmer_Feld3','n',4,0),
  (207,'Ankleidezimmer_Feld4','',110,24,-1,'Ankleidezimmer_Feld4','y',5,0),
  (208,'Ankleidezimmer_Display',NULL,110,52,-1,'Ankleidezimmer_Display','n',7,0),
  (210,'Ankleidezimmer_Ubatt',NULL,110,79,-1,'Ankleidezimmer_Ubatt','y',9,0);

-- Korrektur der Batteriedaten (auf rpi1 durchgeführt: 1.9.22)
-- update sensordata set sensor_id = 30 where sensor_id = 40 and utime < (select min(utime) from sensordata where sensor_id = 30 );

/*
select a.sensor_id, b.sensor_name, from_unixtime(min(a.utime)), from_unixtime(max(a.utime)), count(*) as anzahl
from sensordata a left join sensor as b 
on a.sensor_id = a.sensor_id
group by a.sensor_id;
*/
select sensor_id, from_unixtime(min(utime)), from_unixtime(max(utime)), count(*) from sensordata
group by sensor_id;

select a.sensor_id, b.sensor_name, from_unixtime(min(a.utime)), from_unixtime(max(a.utime)), count(*) from sensordata a, sensor b 
where a.sensor_id = b.sensor_id 
group by a.sensor_id;

select utime, value from sensordata where sensor_id = 22 order by utime desc limit 20;
delete from sensordata where sensor_id = 22 and utime in ( 1662057051,  1662057038 );




update sensor set channel = 101 where channel = 79;
delete from sensordata where sensor_id > 100 and sensor_id < 9000;
-- Sensoren ändern 
-- alt -> neu
--  29    30
--  22    23
--  21    22
--  20    21

-- Ergebnis sieht gut aus !!!!!

commit;
