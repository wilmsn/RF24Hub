// ToDo:


// Masse der Box
size_x = 45;
size_y = 90;
size_z = 40;
steg_x = 2;
steg_y = 2;
steg_z = 2;
wallsize = 1.8;
//HauptPlatine: 6x4
//Einbauposition
HP_dx=-20;
HP_dy=-26;
HP_dz=wallsize;
HP_hoehe=10;
// Wippschalter
// Einbauposition
WS_dx=-10;
WS_dy=-45;
WS_dz=15;
// USB-Ladebuchse
// Einbauposition
LB_dx=0;
LB_dy=-45+wallsize;
LB_dz=wallsize;
LB_hoehe=5;
//Gehäuseschrauben
schraube_Kopf=5;
schraube_Schaft=3;
schraube_Loch=2;
show_innenleben=true;

//schnittX();
//schnittY();
unterteil();
//deckel();
//deckelsteg();
//innenleben();
//aussparungen();

use <nw_toolbox.scad>
$fn = $preview ? 25 : 200;

//Halter_Platine4x6(HP_dz,6,2); 
//Teil_Wippschalter();
//Teil_PlatMiniUSB();
//Loch_PlatMiniUSB();
//translate([0,0,2]) union() {
//Teil_PlatUSB_C();
//Loch_PlatUSB_C();
//}
//Halter_PlatUSB_C(2);

x=size_x;
y=size_y;
z=size_z;
w=wallsize;
sx=steg_x;
sy=steg_y;
h=4; // Deckelhöhe;
spalt=0.5; // Spalt wischen Deckel und Gehäuse
r=2*w+spalt+0.1; // Radius der Randkrümmung (r >= 2*w+gap)

module schnittY() {
  difference() {
    unterteil();
    translate([-size_x/2-15,-size_y/2-10,0]) cube([size_x+20,size_y+20,size_z+20]);
  }
}

module deckel() {
  translate([-x/2-5,0,0])  
  union() {  
  translate([0,0,z-0.3]) rotate(180,[1,0,0])
  difference() {
    gehaeuse();
    // Ein Cube blendet die Rückseite aus!!
    translate([0,0,-h]) cube([2*x,2*y,2*z], center=true);
  }
  translate([0,0,sy+h/2]) deckelsteg();
  }
}

module unterteil() {
  translate([x/2+5,0,0])  
  union() {  
    difference() {
      gehaeuse();
      // Ein Cube blendet den Deckel aus!!
      translate([0,0,2*z-h]) cube([2*x,2*y,2*z], center=true);
      translate([WS_dx,WS_dy,WS_dz]) Wippschalter(typ=3);
      translate([LB_dx,LB_dy,LB_dz])
       // Loch_PlatUSB_C();
        USB_C(typ=3,hoehe=LB_hoehe);
    }
    translate([HP_dx,HP_dy,wallsize])
      Platine4x6(typ=1,hoehe=HP_hoehe);
    translate([LB_dx,LB_dy,LB_dz])
      USB_C(typ=1,hoehe=LB_hoehe);
      //Halter_PlatUSB_C(LB_dz);
    if (show_innenleben) innenleben();  
  }
};

module innenleben() {
  if ( $preview ) {
    union() { 
      translate([HP_dx,HP_dy,HP_dz])
        Platine4x6(typ=2, hoehe=HP_hoehe);
      translate([WS_dx,WS_dy,WS_dz]) Wippschalter(typ=2);
      translate([LB_dx,LB_dy,LB_dz])
        //Teil_PlatUSB_C();
        USB_C(typ=2,hoehe=LB_hoehe);
    }
  }
}

module gehaeuse() {
  bolzendurchmesser=8; 
  bolzenhoehe=z-h;
  dx=wallsize+5;
  dy=wallsize+5;
  difference() {  
    union() {  
      difference() {
        geh_aussen();
        geh_innen();
      }
      intersection() {
        geh_innen();  
        union() {
          //Bolzen unten
          geh_bolzen(x/2-dx,y/2-dy,bolzendurchmesser,bolzenhoehe);  
          mirror([0,1,0])
          geh_bolzen(x/2-dx,y/2-dy,bolzendurchmesser,bolzenhoehe);  
          mirror([1,0,0])
          geh_bolzen(x/2-dx,y/2-dy,bolzendurchmesser,bolzenhoehe);  
          mirror([0,1,0]) mirror([1,0,0])
          geh_bolzen(x/2-dx,y/2-dy,bolzendurchmesser,bolzenhoehe);  
          //Ende Bolzen unten  
              
          // Verstaerkung Deckel
          translate([x/2-dx,y/2-dy,z-r+0.25])
            Schraubbolzen(3,11,schraube_Loch);
          translate([-x/2+dx,y/2-dy,z-r+0.25])
            Schraubbolzen(3,11,schraube_Loch);
          translate([x/2-dx,-y/2+dy,z-r+0.25])
            Schraubbolzen(3,11,schraube_Loch);
          translate([-x/2+dx,-y/2+dy,z-r+0.25])
            Schraubbolzen(3,11,schraube_Loch);
          // Ende Verstaerkung Deckel
        }
      }
    }
    union() {
      // Alle Aussparungen  
      translate([x/2-dx,y/2-dy,z])
        cylinder(h=5, d1=schraube_Kopf, d2=schraube_Schaft, center=true);
      translate([-x/2+dx,y/2-dy,z])
        cylinder(h=5, d1=schraube_Kopf, d2=schraube_Schaft, center=true);
      translate([x/2-dx,-y/2+dy,z])
        cylinder(h=5, d1=schraube_Kopf, d2=schraube_Schaft, center=true);
      translate([-x/2+dx,-y/2+dy,z])
        cylinder(h=5, d1=schraube_Kopf, d2=schraube_Schaft, center=true);
      // Ende Alle Aussparungen
    }
  }
} 

module geh_aussen() {
  translate([0,0,z/2-0.15]) 
    minkowski() {
      cube([x-2*r,y-2*r,z-2*r], center=true);
      sphere(r=r);
  };   
}

module geh_innen() {
  translate([0,0,z/2-0.15]) 
    minkowski() {
      cube([x-2*r,y-2*r,z-2*r], center=true);
      sphere(r=r-w);
    }
}

module geh_bolzen(bx,by,bolzendurchmesser,bolzenhoehe,pos) {
  intersection() {
    geh_innen();  
    union() {
      translate([bx,by,0])
        Schraubbolzen(bolzenhoehe,bolzendurchmesser,schraube_Loch);
      translate([bx-2,by+bolzendurchmesser/2-2,0])
        cube([5,5,bolzenhoehe]);
      translate([bx+bolzendurchmesser/2-2,by-2,0])
        cube([5,5,bolzenhoehe]);
    }
  }
}

module deckelsteg() {
  dx=x/2-sx/2-w-spalt;  
  dy=y/2-sy/2-w-spalt;
  dz=sy+h/2;  
  lx=x-2*w-2*spalt-2*11;
  ly=y-2*w-2*spalt-2*11;  
//  translate([0,0,sy+h/2])
    union() {
    translate([0,dy,0])
      cube([lx,sy,dz], center=true);
    translate([dx,0,0])
      cube([sx,ly,dz], center=true);
    translate([0,-dy,0])
      cube([lx,sy,dz], center=true);
    translate([-dx,0,0])
      cube([sx,ly,dz], center=true);
    }
}
