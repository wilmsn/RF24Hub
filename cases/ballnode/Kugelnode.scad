// Aktuell ist ein Laderegler mit USB-C Buchse eingebaut

use <nw_toolbox.scad>
$fn = $preview ? 25 : 200;
//$fn=100;  
durchmesser=100;
wandstaerke=1;
displayversatz=25;
displayLochX=40.1;
displayLochY=34.1;
displayPlatineX=47;
displayPlatineY=47;
displayPlatinendicke=20;
usbX=10;
usbY=4;
fussversatz=10;
fusswinkel=-25;
heckversatz=5;
schraube_Kopf=7.5;
schraube_Kopflaenge=20;
schraube_Schaft=3; 
schraube_Schaftlaenge=10;
schraube_Bohrung=2;
schraube_position=durchmesser/2/1.8;
resetschalterX=6.7;  //+0.4
resetschalterY=6.7;
resetschalterZ=4.2;  //+0.2
resetschalterLoch=4.5;
usbPlatineX=17.5;
usbPlatineY=27;
usbPlatineZ=2;
//HauptPlatine: 6x4
HP_LochX=55.5; HP_LochY=35.9; HP_X=60; HP_Y=40;
//HauptPlatine: 7x3
//HP_LochX=64; HP_LochY=24; HP_X=70; HP_Y=30;
AkkuX=25;
AkkuY=40;
AkkuZ=12;
Abstand18650=23;

show_innenleben=false;

//------------------------------------
radius=durchmesser/2;

//druck_blende();

druck_vorderseite();

//druck_rueckseite();

//schnittbild();

//rotate(25,[0,1,0]) translate([0,0,45]) gehaeuse();

//vorderseite();

//rueckseite();

//kugelgeh();

//kugel_aussen();

//innenleben();

//platinenhalterung();

//alle_bolzen();

//bolzen();

//stege();

//bolzen_aussparung();

//alle_schraube_aussparungen();

//display_aussparung();

//platinenhalterung();

module innenleben() {
  if ( $preview ) {
    union() { 
    //Mainboard    
      color("#110088") translate([-radius+displayversatz+10+11,0,0]) cube([20,HP_X,HP_Y], center=true);
        //Akku    
      color("#110088") translate([-radius+displayversatz+AkkuX/2+10,0,-radius+fussversatz+11]) cube([AkkuX,AkkuY,AkkuZ], center=true);
        //Akku 18650
      color("#110088") translate([Abstand18650,32.5,13]) rotate(90,[1,0,0]) Batt18650(); 
        //Display 5110    
      translate([-22,23,-24]) rotate(90,[1,0,0]) rotate(-90,[0,1,0]) Display5110();     
    }
  }
}

module schnittbild() {
  difference() {
  union() {  
    gehaeuse();
    zentrierstege();  
  }
    color("black") translate([-radius,-radius,-radius]) cube([2*radius,radius,2*radius]);
  }
}

module druck_blende() {
  difference() {
    translate([0,0,displayversatz-radius])    
    rotate(90,[0,1,0])
    difference() {    
      kugelroh(radius,displayversatz-wandstaerke/2,fussversatz,fusswinkel,heckversatz);
      display_aussparung();
    }    
    rotate(180,[1,0,0]) translate([-radius,-radius,0])
      cube([2*radius,2*radius,2*radius]);
  }    
}

module druck_vorderseite() {
  translate([radius+3,0,displayversatz]) rotate(a=-90, v=[0,1,0]) 
    vorderseite();
}

module vorderseite() {
  union() {  
  difference() {
    gehaeuse();  
    // Ein Cube blendet die Rückseite aus!!
    translate([0,-2*radius,-2*radius]) cube([2*radius,4*radius,4*radius]);
  }
  zentrierstege();
  }
}

module druck_rueckseite() {
  rotate(a=180, v=[0,1,0])
    translate([radius+3,0,heckversatz-radius]) rotate(a=-90, v=[0,1,0]) 
      rueckseite();
}

module rueckseite() {
  difference() {
    gehaeuse();  
    // Ein Cube blendet die Vorderseite aus!!
    translate([-2*radius,-2*radius,-2*radius]) cube([2*radius,4*radius,4*radius]);
  }
}

module gehaeuse(){
  union() {
    difference() {
      kugelgeh();
      alle_schraube_aussparungen();
      aussparungen();
    }
    intersection() {
      union() {  
        alle_bolzen();
        platinenhalterung();  
        stege();
      }       
      kugel_aussen();
    }    
    if (show_innenleben) {
      innenleben();
    }
  }
}

module kugelgeh() {
  difference() {
      kugel_aussen();
      kugel_innen();
  }
}

module kugel_aussen() {
    kugelroh(radius,displayversatz,fussversatz,fusswinkel,heckversatz);
}

module kugel_innen() {
    kugelroh(radius-wandstaerke,displayversatz,fussversatz,fusswinkel,heckversatz);
}

module kugelroh(radius,displayversatz,fussversatz,fusswinkel,heckversatz) {
  difference() {
    sphere(r=radius); 
    translate([-1.5*radius+displayversatz,0,0])
        cube([radius,2*radius,3*radius], center=true);
    rotate(a=fusswinkel, v=[0,1,0]) 
      translate([0,0,(1.5*radius-fussversatz)*-1]) 
        cube([2*radius,2*radius,radius], center=true);
    rotate(a=90, v=[0,1,0]) 
      translate([0,0,radius/2+radius-heckversatz])
        cube([2*radius,2*radius,radius], center=true);
  }
}

module platinenhalterung() {
color( "#FF0011" ) 
  union() {  
      //Halterung Hauptplatine vor Display
    rotate(270,[0,1,0])
      translate([HP_LochY/2,HP_LochX/2,radius-displayversatz-wandstaerke-10])
        Schraubbolzen(10,6,2);
    rotate(270,[0,1,0])
      translate([HP_LochY/2,-HP_LochX/2,radius-displayversatz-wandstaerke-10])
        Schraubbolzen(10,6,2);
    rotate(270,[0,1,0])
      translate([-HP_LochY/2,HP_LochX/2,radius-displayversatz-wandstaerke-10])
        Schraubbolzen(10,6,2);
    rotate(270,[0,1,0])
      translate([-HP_LochY/2,-HP_LochX/2,radius-displayversatz-wandstaerke-10])
        Schraubbolzen(10,6,2);
      //Halterung Reset Schalter
      translate([radius-heckversatz,0,10])
      union() {
        translate([0,-resetschalterX/2,0])
          rotate(270,[0,1,0])
          Halter_Platine2(resetschalterX,1,resetschalterZ,0.5);
        translate([0,resetschalterX/2,0])    
          mirror([0,1,0])
          rotate(270,[0,1,0])
            Halter_Platine2(resetschalterX,1,resetschalterZ,0.5);
        translate([-2,-1,resetschalterY/2])    
          cube([2,2,2]);
        translate([-2,-1,-resetschalterY/2-2])    
          cube([2,2,2]);
      }
     //Halter für die USB-Platine
      translate([radius-heckversatz,0,-8])
      mirror([0,0,0])
      union() {
          translate([0,-usbPlatineX/2,0])
          rotate(270,[0,1,0])
          Schienenfuehrung(usbPlatineZ+3,usbPlatineZ+2,usbPlatineY-5,usbPlatineZ,usbPlatineZ);
          translate([0,usbPlatineX/2,0])
          mirror([0,1,0])
          rotate(270,[0,1,0])
          Schienenfuehrung(usbPlatineZ+3,usbPlatineZ+2,usbPlatineY-5,usbPlatineZ,usbPlatineZ);
          translate([0,0,-1.5])
          rotate(90,[1,0,0])
          rotate(270,[0,1,0])
          Halter_Platine2(5,2,usbPlatineY,2);
    
      }
    // Halterung für den Temperatursensor
    rotate(270,[0,1,0]) 
      translate([36,5,radius-displayversatz-wandstaerke-7])
        Schraubbolzen(7,6,2);
    rotate(270,[0,1,0]) 
      translate([36,-5,radius-displayversatz-wandstaerke-7])
        Schraubbolzen(7,6,2);
  }
}

module zentrierstege() {
  pos = radius-5; 
  stegbreite = 10;  
  steghoehe = radius; 
  difference() {  
    intersection() {
      union() {  
        //oben
        translate([0,0,pos])
          rotate(a=90, v=[1,0,0])
            cube([steghoehe,stegbreite,2], center=true);
        //links  
        translate([0,-pos,0])
          cube([steghoehe,stegbreite,2], center=true);
        //rechts        
        translate([0,pos,0])
          cube([steghoehe,stegbreite,2], center=true);     
        //unten
        difference() {  
        //Stege am Fuss
          union() {
            translate([0,-10,-pos+5])
              rotate(a=90, v=[1,0,0])
                cube([steghoehe,stegbreite,2], center=true);
            translate([0,10,-pos+5])
              rotate(a=90, v=[1,0,0])
                cube([steghoehe,stegbreite,2], center=true);
          }
          union() {
            translate([-15,0,-pos+6])
              rotate(a=90, v=[1,0,0])
                cylinder(h=25,d=4,center=true);    
            translate([-5,0,-pos+6])
              rotate(a=90, v=[1,0,0])
                cylinder(h=25,d=4,center=true);    
          }
        }
      }
      kugel_innen();
    }  
    translate([5,-radius,-radius]) cube([2*radius,2*radius,2*radius]);
  }
}

module stege() {
  pos = schraube_position + schraube_Kopf/2+2*wandstaerke; 
  stegbreite = 9;  
  translate([0,-pos,pos])
    rotate(a=90+45, v=[1,0,0])
      cube([radius*1.7,stegbreite,2], center=true);     
  translate([0,-pos,-pos])
    rotate(a=90-45, v=[1,0,0])
      cube([radius*1.7,stegbreite,2], center=true);     
  translate([0,pos,pos])
    rotate(a=90-45, v=[1,0,0])
      cube([radius*1.7,stegbreite,2], center=true);     
  translate([0,pos,-pos])
    rotate(a=90+45, v=[1,0,0])
      cube([radius*1.7,stegbreite,2], center=true);  
  //Halterung für den 18650 Akku
  difference() {
    union() {  
      translate([Abstand18650,22,0]) cube([radius,2,35]);  
      translate([Abstand18650,11,12.5]) cube([radius,13,2]);  
      translate([Abstand18650,-24,0]) cube([radius,2,35]);  
      translate([Abstand18650,-23,12.5]) cube([radius,13,2]);  
    }
    translate([41,17,0]) cylinder(h=20,d=8);   
    translate([41,-17,0]) cylinder(h=20,d=8);   
    translate([Abstand18650,35,13]) 
      rotate(90,[1,0,0]) cylinder(h=70,d=20);           
  }  
}

module aussparungen() {
  union() {
    //Displaylock  
    translate([-radius,-displayLochX/2,-displayLochY/2]) cube([radius,displayLochX,displayLochY]);
    //Platz für die Platine am Display
    translate([displayversatz-radius+wandstaerke+displayPlatinendicke/2,0,0]) 
        cube([displayPlatinendicke, displayPlatineX, displayPlatineY], center=true);
    //USB Buchse
    translate([radius-heckversatz,0,-5.5]) 
        cube([20, usbX, usbY], center=true);      
    //Reset Scalter
    translate([radius-heckversatz,0,10]) 
      rotate(a=90, v=[0,1,0])
        cylinder(h=20, d=resetschalterLoch, center=true);      
  }   
}

module schraube_aussparung() {
// bl: Länge des Bolzens
   bolzendurchmesser=schraube_Kopf+wandstaerke;
   cylinder(h=radius, d=bolzendurchmesser, center=true);
}

module schraube_aussparung1() {
  translate([radius/2,schraube_position,schraube_position])
    rotate(a=90, v=[0,1,0])
      schraube_aussparung();
}
module schraube_aussparung2() {
  translate([radius/2,-schraube_position,schraube_position])
    rotate(a=90, v=[0,1,0])
      schraube_aussparung();
}
module schraube_aussparung3() {
  translate([radius/2,schraube_position,-schraube_position])
    rotate(a=90, v=[0,1,0])
      schraube_aussparung();
}
module schraube_aussparung4() {
  translate([radius/2,-schraube_position,-schraube_position])
    rotate(a=90, v=[0,1,0])
      schraube_aussparung();
}
module alle_schraube_aussparungen() {
  union() {
    schraube_aussparung1();
    schraube_aussparung2();
    schraube_aussparung3();
    schraube_aussparung4();
  }
}

module bolzen_aussparung() {
  bolzenlaenge=radius;
    union() {
      translate([0,0,bolzenlaenge/2+schraube_Kopflaenge])
        cylinder(h=bolzenlaenge+0.1, d=schraube_Kopf, center=true);
// Bohrung Übergang von Kopf auf Schaft
      translate([0,0,schraube_Kopflaenge/2])
        cylinder(h=schraube_Kopflaenge+0.1, d2=schraube_Kopf, d1=schraube_Schaft, center=true);
// Bohrung Übergang von Schaft auf Gewinde        
    translate([0,0,-schraube_Schaftlaenge/2])        cylinder(h=schraube_Schaftlaenge+0.1, d1=schraube_Bohrung, d2=schraube_Schaft, center=true);
// Bohrung für den Gewindeteil 
    translate([0,0,-bolzenlaenge/2-schraube_Schaftlaenge])
        cylinder(h=bolzenlaenge+1, d=schraube_Bohrung, center=true);
    }    
}

module bolzen() {
  bolzenlaenge=radius;
  difference() {
    cylinder(h=2*bolzenlaenge, d=schraube_Kopf+2*wandstaerke, center=true);
    bolzen_aussparung();  
  }
}


module bolzen1() {
  translate([0,schraube_position,schraube_position])
    rotate(a=90, v=[0,1,0])
      bolzen();
}

module bolzen2() {
  translate([0,-schraube_position,schraube_position])
    rotate(a=90, v=[0,1,0])
      bolzen();
}

module bolzen3() {
  translate([0,schraube_position,-schraube_position])
    rotate(a=90, v=[0,1,0])
      bolzen();
}

module bolzen4() {
  translate([0,-schraube_position,-schraube_position])
    rotate(a=90, v=[0,1,0])
      bolzen();
}

module alle_bolzen() {
  union() {
      bolzen1();
      bolzen2();
      bolzen3();
      bolzen4();
  }
}