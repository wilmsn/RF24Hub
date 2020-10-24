DROP TABLE IF EXISTS battery;

CREATE TABLE battery
(
   battery_id       INT           NOT NULL,
   battery_name     VARCHAR(20)   DEFAULT NULL,
   u_empty          FLOAT         DEFAULT NULL,
   u_nominal        FLOAT         DEFAULT NULL,
   battery_sel_txt  VARCHAR(20)   DEFAULT NULL,
   CONSTRAINT `PRIMARY` PRIMARY KEY (battery_id)
)
ENGINE=InnoDB;

DROP TABLE IF EXISTS node;

CREATE TABLE node
(
   node_id     INT            NOT NULL,
   node_name   VARCHAR(50)    DEFAULT NULL,
   add_info    VARCHAR(500)   DEFAULT NULL,
   battery_id  INT            NOT NULL,
   html_show   CHAR(1)        DEFAULT NULL,
   html_order  INT            DEFAULT NULL,
   heartbeat   CHAR(1)        DEFAULT NULL,
   pa_utime    INT            DEFAULT NULL,
   pa_level    INT            DEFAULT NULL,
   CONSTRAINT `PRIMARY` PRIMARY KEY (node_id)
)
ENGINE=InnoDB;

ALTER TABLE node
  ADD CONSTRAINT battery_id FOREIGN KEY (battery_id)
  REFERENCES battery (battery_id)
  ON UPDATE RESTRICT
  ON DELETE RESTRICT;

DROP TABLE IF EXISTS node_configdata;

CREATE TABLE node_configdata
(
   node_id  INT     NOT NULL,
   channel  INT     NOT NULL,
   value    FLOAT   DEFAULT NULL,
   utime    INT     DEFAULT NULL,
   CONSTRAINT `PRIMARY` PRIMARY KEY (node_id, channel)
)
ENGINE=InnoDB;

DROP TABLE IF EXISTS node_configdata_im;

CREATE TABLE node_configdata_im
(
   node_id  INT     NOT NULL,
   channel  INT     NOT NULL,
   value    FLOAT   DEFAULT NULL,
   utime    INT     DEFAULT NULL,
   CONSTRAINT `PRIMARY` PRIMARY KEY (node_id, channel)
)
ENGINE=MEMORY;

DROP TABLE IF EXISTS node_configitem;

CREATE TABLE node_configitem
(
   channel     INT            NOT NULL,
   itemname    VARCHAR(200)   NOT NULL,
   min         FLOAT          NOT NULL,
   max         FLOAT          NOT NULL,
   readonly    CHAR(1)        DEFAULT 'n',
   html_show   CHAR(1)        DEFAULT NULL,
   html_order  INT            DEFAULT 1,
   CONSTRAINT `PRIMARY` PRIMARY KEY (channel)
)
ENGINE=InnoDB;

DROP TABLE IF EXISTS sensor;

CREATE TABLE sensor
(
   sensor_id    INT            NOT NULL,
   sensor_name  VARCHAR(50)    DEFAULT NULL,
   add_info     VARCHAR(100)   DEFAULT NULL,
   node_id      INT            NOT NULL,
   channel      INT            NOT NULL,
   store_days   INT            DEFAULT NULL,
   fhem_dev     VARCHAR(50)    DEFAULT NULL,
   html_show    CHAR(1)        DEFAULT NULL,
   html_order   INT            DEFAULT 1,
   value        FLOAT          DEFAULT NULL,
   utime        INT            DEFAULT NULL,
   CONSTRAINT `PRIMARY` PRIMARY KEY (sensor_id)
)
ENGINE=InnoDB;

DROP TABLE IF EXISTS sensor_im;

CREATE TABLE sensor_im
(
   sensor_id    INT            NOT NULL,
   sensor_name  VARCHAR(50)    DEFAULT NULL,
   add_info     VARCHAR(100)   DEFAULT NULL,
   node_id      INT            NOT NULL,
   channel      INT            NOT NULL,
   store_days   INT            DEFAULT NULL,
   fhem_dev     VARCHAR(50)    DEFAULT NULL,
   html_show    CHAR(1)        DEFAULT NULL,
   html_order   INT            DEFAULT 1,
   value        FLOAT          DEFAULT NULL,
   utime        INT            DEFAULT NULL,
   CONSTRAINT `PRIMARY` PRIMARY KEY (sensor_id)
)
ENGINE=MEMORY;

DROP TABLE IF EXISTS sensordata;

CREATE TABLE sensordata
(
   sensor_id  INT     NOT NULL,
   utime      INT     NOT NULL,
   value      FLOAT   DEFAULT NULL,
   CONSTRAINT `PRIMARY` PRIMARY KEY (sensor_id, utime)
)
ENGINE=InnoDB;

CREATE INDEX sensordata_utime_idx
   ON sensordata (utime ASC);

DROP TABLE IF EXISTS sensordata_im;

CREATE TABLE sensordata_im
(
   sensor_id  INT     NOT NULL,
   utime      INT     NOT NULL,
   value      FLOAT   DEFAULT NULL,
   CONSTRAINT `PRIMARY` PRIMARY KEY (sensor_id, utime)
)
ENGINE=MEMORY;

DROP TABLE IF EXISTS sensordata_d;

CREATE TABLE sensordata_d
(
   Sensor_ID  INT     DEFAULT NULL,
   Utime      INT     DEFAULT NULL,
   Value      FLOAT   DEFAULT NULL
)
ENGINE=MEMORY;

CREATE INDEX sensordata_d_utime
   ON sensordata_d (Utime);

DROP TABLE IF EXISTS gateway;

CREATE TABLE gateway
(
   gw_name   VARCHAR(40)   NOT NULL,
   gw_no     INT           DEFAULT NULL,
   gw_ip     VARCHAR(40)   NOT NULL,
   isActive  BIT           DEFAULT 0,
   CONSTRAINT `PRIMARY` PRIMARY KEY (gw_ip)
)
ENGINE=InnoDB;

DROP TABLE IF EXISTS gateway_im;

CREATE TABLE gateway_im
(
   gw_name   VARCHAR(40)   NOT NULL,
   gw_no     INT           DEFAULT NULL,
   gw_ip     VARCHAR(40)   NOT NULL,
   isActive  BIT           DEFAULT 0,
   CONSTRAINT `PRIMARY` PRIMARY KEY (gw_ip)
)
ENGINE=MEMORY;




