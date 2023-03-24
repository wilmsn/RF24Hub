INSERT INTO gateway (gw_name,gw_no,isActive)
VALUES
  ('test.fritz.box',1,'j');
INSERT INTO node (node_id,node_name,add_info,battery_id,html_show,html_order,pa_level,mastered,rec_level,volt_lv,lv_flag)
VALUES
  (100,'TestNode','',3,'y',70,3,'y',0,3.6,'n');
INSERT INTO sensor (sensor_id,sensor_name,add_info,node_id,channel,store_days,fhem_dev,html_show,html_order,datatype)
VALUES
  (100,'TestNode_Temp',NULL,100,1,-1,'no_use','y',1,0);
