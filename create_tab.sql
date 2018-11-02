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

CREATE TABLE sensor
(
   sensor_id       INT            NOT NULL,
   sensor_name     VARCHAR(50),
   add_info        VARCHAR(100),
   node_id         VARCHAR(10)    NOT NULL,
   channel         INT            NOT NULL,
   value           FLOAT,
   utime           INT,
   store_days      INT,
   fhem_dev        VARCHAR(50),
   signal_quality  VARCHAR(10),
   s_type          CHAR(1),
   PRIMARY KEY (sensor_id)
)
ENGINE=InnoDB;

CREATE TABLE node
(
   node_id         VARCHAR(10)    NOT NULL,
   node_name       VARCHAR(50),
   add_info        VARCHAR(500),
   u_batt          FLOAT,
   sleeptime1      FLOAT,
   sleeptime2      FLOAT,
   sleeptime3      FLOAT,
   sleeptime4      FLOAT,
   radiomode       FLOAT,
   battery_id      INT            NOT NULL,
   is_online       INT,
   signal_quality  VARCHAR(10),
   last_contact    INT,
   voltagefactor   FLOAT,
   html_sort       INT,
   html_show       CHAR(1),
   PRIMARY KEY (node_id)
)
ENGINE=InnoDB;

ALTER TABLE node
  ADD CONSTRAINT battery_id FOREIGN KEY (battery_id)
  REFERENCES battery (battery_id)
  ON UPDATE RESTRICT
  ON DELETE RESTRICT;

CREATE TABLE node_init
(
   node_id  VARCHAR(10)   NOT NULL,
   channel  INT,
   prio     INT,
   value    FLOAT,
   PRIMARY KEY (node_id, channel)
)
ENGINE=InnoDB;

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
