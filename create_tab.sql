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
   node_id    VARCHAR(10),
   channel    INT,
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
   voltagecorrection  FLOAT,
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
   Sensor_ID    INT            NOT NULL,
   Sensor_Name  VARCHAR(30),
   Add_Info     VARCHAR(100),
   Node_ID      VARCHAR(10)    NOT NULL,
   Channel      INT            NOT NULL,
   Value        FLOAT,
   Utime        INT,
   store_days   INT,
   fhem_dev     VARCHAR(50),
   type         CHAR(1),
   PRIMARY KEY (Sensor_ID)
)
ENGINE=InnoDB;


DROP TABLE IF EXISTS sensordata;

CREATE TABLE sensordata
(
   Sensor_ID  INT     NOT NULL,
   Utime      INT     NOT NULL,
   Value      FLOAT,
   PRIMARY KEY (Sensor_ID, Utime)
)
ENGINE=InnoDB;

CREATE INDEX sensordata_utime_idx
   ON sensordata (Utime ASC);

CREATE INDEX sensordata_id_utime_idx
   ON sensordata (Sensor_ID ASC, Utime ASC);

