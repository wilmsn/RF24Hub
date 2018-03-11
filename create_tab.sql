DROP TABLE IF EXISTS battery;

CREATE TABLE battery
(
   battery_id       INT           NOT NULL,
   battery_name     VARCHAR(20),
   u_empty          FLOAT,
   u_nominal        FLOAT,
   battery_sel_txt  VARCHAR(20),
   PRIMARY KEY (battery_id)
)
ENGINE=InnoDB;

DROP TABLE IF EXISTS jobbuffer;

CREATE TABLE jobbuffer
(
   orderno    INT,
   flags      INT UNSIGNED   DEFAULT 0,
   node_id    VARCHAR(10),
   channel    INT UNSIGNED,
   value      FLOAT,
   sensor_id  INT,
   priority   INT,
   utime      INT
)
ENGINE=InnoDB;

DROP TABLE IF EXISTS node;

CREATE TABLE node
(
   node_id            VARCHAR(10)    NOT NULL,
   node_name          VARCHAR(50),
   add_info           VARCHAR(500),
   u_batt             FLOAT,
   sleeptime1         INT,
   sleeptime2         INT,
   sleeptime3         INT,
   sleeptime4         INT,
   radiomode          INT,
   battery_id         INT            NOT NULL,
   is_online          INT,
   signal_quality     VARCHAR(10),
   last_contact       INT,
   voltagefactor      FLOAT,
   PRIMARY KEY (node_id)
)
ENGINE=InnoDB;

ALTER TABLE node
  ADD CONSTRAINT battery_id FOREIGN KEY (battery_id)
  REFERENCES battery (battery_id)
  ON UPDATE RESTRICT
  ON DELETE RESTRICT;



DROP TABLE IF EXISTS sensor;

CREATE TABLE sensor
(
   sensor_id    INT            NOT NULL,
   sensor_name  VARCHAR(30),
   add_info     VARCHAR(100),
   node_id      VARCHAR(10)    NOT NULL,
   channel      INT            NOT NULL,
   value        FLOAT,
   utime        INT,
   store_days   INT,
   fhem_dev     VARCHAR(50),
   type         CHAR(1),
   PRIMARY KEY (sensor_id)
)
ENGINE=InnoDB;


DROP TABLE IF EXISTS sensordata;

CREATE TABLE sensordata
(
   sensor_id  INT     NOT NULL,
   utime      INT     NOT NULL,
   value      FLOAT,
   PRIMARY KEY (sensor_id, utime)
)
ENGINE=InnoDB;

CREATE INDEX sensordata_utime_idx
   ON sensordata (utime ASC);

CREATE INDEX sensordata_id_utime_idx
   ON sensordata (sensor_id ASC, utime ASC);

