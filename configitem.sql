INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 102,  'Spannungsfaktor',  0.1, 10.0, 'n', 'y', 2 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 103,  'Spannungsoffset',  -10.0,  10.0,  'n',  'y',  3 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 104,  'Kritischer Spannungswert',  1.0,  5.0,  'n',  'y', 4 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 106,  'Justierung Schlafzeit (regelmäßig in ms)', -1000.0, 1000.0, 'n', 'y', 5 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 107,  'Verschiebung Schlafzeit (einmalig in Sek.)', -1000.0, 1000.0, 'n', 'y', 6 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 111,  'Register auslesen (beliebiger Wert startet)', 0.0, 10.0, 'n', 'y', 22 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 112,  'Monitormode (0=aus; 1=ein)',  0.0,  1.0,  'n',  'y',  8 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 113,  'Display (Bit 15..8 => Helligkeit; Bit 7..0 Kontrast)', 0.0, 65536.0, 'n', 'y', 9 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 114,  'Schlafzeit in Sekunden', 10.0,  32400.0, 'n', 'y', 10 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 115,  'Schleifen ohne Sendung',  0.0,  20.0,  'n',  'y',  11 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 116,  'Wartezeit zwischen 2 Sendungen in ms',  50.0,  1000.0,  'n',  'y',  12 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 117,  'Maximale Anzahl für normale Sendungen', 1.0, 20.0,  'n',  'y',  13 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 118,  'Maximale Anzahl für Stopp Sendungen',  1.0,  20.0,  'n',  'y',  14 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 119,  'Sendeinterval bei kritischer Spannung (in Min.)', 60.0, 1440.0, 'n', 'y', 15 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 124, 'Gemessene Sendestärke (0=Min 1=Low 2=High 4=Max 9=Messung starten)', 0.0, 9.0, 'n', 'y', 16 );

INSERT INTO node_configitem( channel, itemname, MIN, MAX, readonly, html_show, html_order )
    VALUES ( 125, 'Softwareversion', 0.0, 0.0, 'y', 'y', 21 );

INSERT INTO gateway(gw_name, gw_no, gw_ip, isActive)
VALUES ('test', 99, 'xxxx', false );


COMMIT;
