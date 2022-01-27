// Bibliothek nw_toolbox
//
// Diverse Bauteile für Gehäuse und Co.
// Bei der Ausrichtung der Bauteile gilt folgendes
// Die X und Y Achse bilden die Grundfläche; Z-Achse geht in die Höhe
// Bauteile werden nach ihrer Haupteinbaurichtung angelegt.
//
// Eingebaut sind derzeit:
// 
// Platinenhalter(x,y,z,[nasenbreite])
// mit x=Breite; y=Tiefe; z=Höhe bis zum Haken
// Nullpunkt ist Mitte der Innenseite der Wand
//
// Platinenhalter2(x,y,z,[nasenbreite])
// Druckfreundlicher wenn aufrecht stehend !!!!
// mit x=Breite; y=Tiefe; z=Höhe bis Unterkante Haken
// Nullpunkt ist Mitte der Innenseite der Wand
//
// Schienenfuehrung(x,y,z, [schienenbreite, [schienentiefe]])
// mit x=Breite; y=Tiefe; z=Höhe; 
// Schinenbreite (default 1); Schinentiefe (default 1)
// Nullpunkt ist Mitte Hinten in der Führungsschine
//
// Schraubbolzen(hoehe,durchmesser,loch)
// Zentrum ist Mitte Loch
//
//Platinenhalter2(20,10,10,1);
//Schienenfuehrung(4,3,20,1.5,1.5);
//Schraubbolzen(10,6,2);
//Batt18650();
//Aussparung_Display5110();
//Display5110();
//Aussparung_micro_USB_Buchse();
//PlatMicroUSB();
//Aussparung_USB_A_Buchse();
//USB_A_Buchse();

module PlatMicroUSB() {
  translate([0,-7.1,0]) 
    difference() {
      union() {
        translate([-1,3.1,1]) cube([6,8,3.3]);    
        cube([15,14.2,1.5]);    
      }
      translate([10,2.7,-1]) cylinder(h=3,d=3.4); 
      translate([10,11.5,-1]) cylinder(h=3,d=3.4); 
    } 
}

module Aussparung_micro_USB_Buchse() {
    translate([-10,-4.5,0]) cube([20,9,5]);    
}

module Aussparung_USB_A_Buchse() {
        cube([16,9,20],center=true);    
}

module USB_A_Buchse() {
  translate([-7.3,0,-6]) 
      union() {
        translate([0,-3.1,0]) cube([14.5,10,6]);    
        cube([14.6,7.5,14.5]);    
      }
}

module Aussparung_Display5110() {
   translate([2.8,7.25,0]) cube([40,34,10]); 
}

module Display5110() {
    difference() {
      union() {
        color("gray") translate([2.8,7.25,1]) cube([40,34,4]);    
        color("darkgray") translate([2.8,34.25,2]) cube([40,7,4]);    
        color("darkred") cube([45.6,46.5,1]);  
        color("#000000") translate([7.8,17.25,5]) text("5110");  
      }
      translate([3,2.5,-1]) cylinder(h=3,d=3); 
      translate([42.6,2.5,-1]) cylinder(h=3,d=3); 
      translate([3,44,-1]) cylinder(h=3,d=3); 
      translate([42.6,44,-1]) cylinder(h=3,d=3); 
    } 
}

module Batt18650() {
    union() {
        translate([0,0,65]) cylinder(h=1,d=5);
        cylinder(h=65,d=19); 
    }
}

module Schienenfuehrung(x,y,z, schienenbreite=1, schienentiefe=1) {
    translate([x/2,schienentiefe,0])
    rotate(a=180, v=[0,0,1])
    difference() {
    cube([x,y,z]);
    translate([x/2-schienenbreite/2,-0.1,-1]) cube([schienenbreite,schienentiefe+0.1,z+2]);
    }
}

module Platinenhalter(x,y,z, nasenbreite=1) {
    naseY=y+nasenbreite;
    naseZ=nasenbreite;
    translate([-x/2,-y,0])
    difference() {
    union() {
    cube([x,y,z]);
    translate([0,0,z]) cube([x,naseY,naseZ]);
    }
    translate([-1,naseY,z])    
    rotate(a=45, v=[1,0,0])
    cube([x+2,2*nasenbreite,2*nasenbreite]);
    }
}

module Platinenhalter2(x,y,z, nasenbreite=1) {
    naseY=1.41*nasenbreite;
    naseZ=1.41*nasenbreite;
    translate([-x/2,-y,0])
    difference() {
    union() {
      cube([x,y,z+2*nasenbreite]);
      translate([x/2,y,z+nasenbreite])
      rotate(a=45, v=[1,0,0])
      cube([x,naseY,naseZ], center=true);
    }
      translate([-0.1,-naseY,0])
      cube([x+0.2,naseY,z+2*nasenbreite]);
    }
}

module Schraubbolzen(hoehe,durchmesser,loch) {
    difference() {
        cylinder(hoehe,d=durchmesser);
        translate([0,0,-0.1]) cylinder(hoehe+0.2,d=loch);
    }
}