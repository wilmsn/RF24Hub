
alter TABLE node add column rec_level   INT UNSIGNED;
alter TABLE node add column volt_lv     FLOAT;
alter TABLE node add column lv_flag     CHAR(1);
   
alter TABLE node drop column pa_utime;

alter TABLE node drop column low_voltage;

DROP TABLE IF EXISTS node_configdata_im;

DROP TABLE IF EXISTS node_configitem;

CREATE TABLE node_configitem
(
   channel     INT UNSIGNED   NOT NULL,
   itemname    VARCHAR(200)   NOT NULL,
   min         FLOAT          NOT NULL,
   max         FLOAT          NOT NULL,
   readonly    CHAR(1)        DEFAULT 'n' NOT NULL,
   html_show   CHAR(1)        DEFAULT 'y' NOT NULL,
   html_order  INT UNSIGNED   DEFAULT NULL,
   CONSTRAINT `PRIMARY` PRIMARY KEY (channel)
)
ENGINE=InnoDB;

INSERT INTO node_configitem (channel,itemname,min,max,readonly,html_show,html_order) 
VALUES
  (91,'Aktive Entladung Level 1',1.0,20.0,'n','y',21),
  (92,'Aktive Entladung Level 2',1.0,20.0,'n','y',22),
  (93,'Aktive Entladung Level 3',1.0,20.0,'n','y',23),
  (94,'Load Ballancer Differenzspannung',0.01,1.0,'n','y',24),
  (95,'Display: Kontrast',0.0,100.0,'n','y',30),
  (96,'Display: Helligkeit',0.0,100.0,'n','y',31),
  (110,'Spannungsfaktor',0.1,10.0,'n','y',2),
  (111,'Spannungsoffset',-10.0,10.0,'n','y',3),
  (112,'Kritischer Spannungswert',1.0,5.0,'n','y',4),
  (113,'Schlafzeit in Sekunden (kritische Spannung)',10.0,32400.0,'n','y',5),
  (114,'Schlafzeit in Sekunden',10.0,32400.0,'n','y',10),
  (115,'Sekundenjustierung (aprox. millisec)',500.0,2000.0,'n','y',16),
  (116,'Wartezeit zwischen 2 Sendungen in ms',50.0,1000.0,'n','y',12),
  (117,'Maximale Anzahl für normale Sendungen',1.0,20.0,'n','y',13),
  (118,'Maximale Anzahl für Stopp Sendungen',1.0,2.0,'n','y',14),
  (119,'Schleifen ohne Sendung',0.0,20.0,'n','y',11),
  (120,'Sendestärke (0=Min 1=Low 2=High 3=Max)',0.0,9.0,'n','y',91),
  (121,'Empfangsstärke gemessen (0=Min 1=Low 2=High 3=Max)',0.0,9.0,'y','y',90),
  (125,'Softwareversion',0.0,0.0,'y','y',91);

DROP TABLE IF EXISTS node_configdata;

CREATE TABLE node_configdata
(
   node_id  INT UNSIGNED   NOT NULL,
   channel  INT UNSIGNED   NOT NULL,
   value    FLOAT          DEFAULT NULL,
   utime    INT UNSIGNED   DEFAULT NULL,
   CONSTRAINT `PRIMARY` PRIMARY KEY (node_id, channel)
)
ENGINE=InnoDB;

DROP TABLE IF EXISTS sensor_im;

CREATE TABLE sensor_im
(
   sensor_id   INT UNSIGNED   NOT NULL,
   value       VARCHAR(10)    DEFAULT NULL,
   last_utime  INT UNSIGNED   DEFAULT NULL,
   records     INT UNSIGNED   DEFAULT NULL,
   CONSTRAINT `PRIMARY` PRIMARY KEY (sensor_id)
)
ENGINE=MEMORY;

commit;




