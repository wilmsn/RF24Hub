// Bibliothek nw_toolbox
// Version: 221003
//
// Diverse Bauteile für Gehäuse und Co.
// Bei der Ausrichtung der Bauteile gilt folgendes
// Die X (von links nach rechts) und Y (von vorne nach hinten) Achse bilden die Grundfläche; Z-Achse geht in die Höhe
// Bauteile werden nach ihrer Haupteinbaurichtung mit Blick auf die Anschlüsse/Buchsen angelegt.
//
// Halterungen sind grün eingefärbt
// Parameter: typ=1
//
// Bauteile / Bestückung sind BLAU eingefärbt
// Parameter: typ=2
//
// Aussparungen nutzen den gleichen Ursprung wie Bauteile
// Aussparungen sind ROT eingefärbt.
// Parameter: typ=3
//
// Eingebaut sind derzeit:
// 
//Mikroschalter();
//
//Wippschalter();
//
//Platine4x6();
//
//USB_C();
//
// ========================================
// Alt: zu überarbeiten
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
//Platinenhalter2(20,1,10,1);
//Schienenfuehrung(4,3,20,1.5,1.5);
//Schraubbolzen(10,6,2);
//Batt18650();
//Aussparung_Display5110();
//Display5110();
//Aussparung_micro_USB_Buchse();
//Aussparung_USB_A_Buchse();
//USB_A_Buchse();
//Aussparung_DisplayOLED096();
//DisplayOLED096();

//Teil_PlatMicroUSB(false);
//Loch_PlatMicroUSB(false);

//Teil_PlatMiniUSB(false);
//Loch_PlatMiniUSB(false);
//translate([0,0,10]) Teil_Platine4x6(20);

$fn = $preview ? 25 : 100;

module Platine4x6(
// typ: 1=Halterung; 2=Bauteil
  typ=1,
// hoehe: Hoehe der Halterung
  hoehe=5,
// bt_hoehe: Hoehe der Platine
  bt_hoehe=20,
// durchmesser: Durchmesser der Schraubbolzen
  durchmesser=6,
// loch: Bohrung fuer die Schrauben
  loch=2) {
  DimX=40;
  DimY=60;
  LochX=35.9;
  LochY=55.5;
  dx=(DimX-LochX)/2;
  dy=(DimY-LochY)/2;
// Halterung    
  if (typ==1) {
    color("green")
      union() {
        translate([dx,dy,0]) 
          Schraubbolzen(hoehe, durchmesser, loch);
        translate([dx,LochY+dy,0]) 
          Schraubbolzen(hoehe, durchmesser, loch);
        translate([LochX+dx,dy,0]) 
          Schraubbolzen(hoehe, durchmesser, loch);
        translate([LochX+dx,LochY+dy,0]) 
          Schraubbolzen(hoehe, durchmesser, loch);
      }
  }
// Bauteil
  if (typ==2) {
    color("blue")
      translate([0,0,hoehe])
    difference() {
      cube([DimX,DimY,bt_hoehe]);
      union() {  
        translate([-1,-1,1]) cube([7,7,bt_hoehe]);
        translate([DimX-6,-1,1]) cube([7,7,bt_hoehe]);
        translate([-1,DimY-6,1]) cube([7,7,bt_hoehe]);
        translate([DimX-6,DimY-6,1]) cube([7,7,bt_hoehe]);
        translate([dx,dy,-0.1]) cylinder(2,d=3);
        translate([DimX-dx,dy,-0.1]) cylinder(2,d=3);
        translate([dx,DimY-dy,-0.1]) cylinder(2,d=3);
        translate([DimX-dx,DimY-dy,-0.1]) cylinder(2,d=3);
      }
    }
  }  
// Aussparung
  if (typ==3) {
    color("red");

  }    
}


module USB_C(typ=1, hoehe=5) {


// Halterung    
  if (typ==1) {
    color("green")
      union() {
        translate([-5,26.8,0]) cube([10,3,hoehe+3]);  
        translate([-10,2,0]) cube([20,2,hoehe]);  
        translate([-1,2,0]) cube([2,27,hoehe]);  
        translate([-10.8,2,0]) cube([2,2,hoehe+3]);  
        translate([8.65,2,0]) cube([2,2,hoehe+3]);  
        translate([8.7,22,0])
        rotate([0,0,-90])
          Halter_Platine2(8,1,hoehe+1, nasenbreite=1);
        translate([-8.7,22,0])
          rotate([0,0,90])
            Halter_Platine2(8,1,hoehe+1, nasenbreite=1);
      }
  }  
// Bauteil
  if (typ==2) {
    color("blue")  
      translate([-8.8,0,hoehe]) 
        union() {
          translate([4.4,-1,1.5]) cube([8.8,7.2,3]);    
          cube([17.3,26.7,1.5]);
          translate([0,26.7,0]) cube([2.5,1,1.5]);
          translate([14.8,26.7,0]) cube([2.5,1,1.5]);
        }
  }
// Aussparung
  if (typ==3) {
    color("red")  
      translate([-5,-5,hoehe+1])
        cube([10,10,4]);
  }    
}

module Wippschalter(
// typ: 2=Bauteil; 3=Aussparung
  typ=1) {

// Bauteil
  if (typ==2) {
    color("blue")  
      union() {  
        translate([0,-2,0]) 
          cube([21,2,15]);
        translate([0.8,0,1])
          cube([19.4,16,13]);
        translate([4,-5,2.5])
          rotate(20,[0,0,1])
            cube([7,4,10]);  
      }
  }  
// Aussparung
  if (typ==3) {
    color("red")
      translate([0.8,-10,1]) 
        cube([19.4,26,13]);
  }
}


//================================================

module Teil_PlatUSB_C(center=true) {
  // Maße: 17.3 breit, 27.7 tief, 1.5 dick
    module PlatUSB_C_() {  
      union() {
        translate([4.4,-1,1.5]) cube([8.8,7.2,3]);    
        cube([17.3,26.7,1.5]);
        translate([0,26.7,0]) cube([2.5,1,1.5]);
        translate([14.8,26.7,0]) cube([2.5,1,1.5]);
      }
    }
  color("blue")  
  if (center) translate([-8.8,0,0]) PlatUSB_C_();
  else PlatUSB_C_();
}

module Loch_PlatUSB_C(center=true) {
  // Maße: 3.5 * 9  
  module Loch_PlatUSB_C_() {
    cube([10,10,4]);
  }  
  color("red")  
    if (center) translate([-5,-5,1]) Loch_PlatUSB_C_();  
    else translate([5,-5,1]) Loch_PlatUSB_C_();
}

module Halter_PlatUSB_C(hoehe, center=true) {
  translate([-5,26.8,0]) cube([10,3,hoehe+3]);  
  translate([-10,2,0]) cube([20,2,hoehe]);  
  translate([-1,2,0]) cube([2,27,hoehe]);  
  translate([-10.8,2,0]) cube([2,2,hoehe+3]);  
  translate([8.65,2,0]) cube([2,2,hoehe+3]);  
  translate([8.7,22,0])
    rotate([0,0,-90])
      Halter_Platine2(8,1,hoehe+1, nasenbreite=1);
  translate([-8.7,22,0])
    rotate([0,0,90])
      Halter_Platine2(8,1,hoehe+1, nasenbreite=1);
}

module Teil_PlatMiniUSB(center=true) {
  // Maße: 17 breit, 22 tief  
    module PlatMiniUSB_() {  
      union() {
        translate([4.5,-0.5,1]) cube([8,10,4]);    
        cube([17,22,1.5]);    
      }
    }
  color("blue")  
  if (center) translate([-8.5,0,0]) PlatMiniUSB_();
  else PlatMiniUSB_();    
}

module Loch_PlatMiniUSB(center=true) {
  color("red")  
    if (center) translate([-5,-5,0]) cube([10,10,6]);    
    else translate([3.5,-5,0]) cube([10,10,6]);    
}

module Teil_PlatMicroUSB(center=true) {
  module PlatMicroUSB_() {
    difference() {
      union() {
        translate([3.1,-1,1]) cube([8,6,3.3]);    
        cube([14.2,15,1.5]);    
      }
      translate([2.7,10,-1]) cylinder(h=3,d=3.4); 
      translate([11.5,10,-1]) cylinder(h=3,d=3.4); 
    }
  } 

  color("blue")  
  if (center) translate([-7.1,0,0]) PlatMicroUSB_();
  else PlatMicroUSB_();    
}

module Loch_PlatMicroUSB(center=true) {
  color("red")  
    if (center) translate([-4.5,-5,0]) cube([9,10,5]);    
    else translate([2.6,-5,0]) cube([9,10,5]);    
}

module Loch_USB_A_Buchse() {
        cube([16,9,20],center=true);    
}

module Teil_USB_A_Buchse() {
  translate([-7.3,0,-6]) 
      union() {
        translate([0,-3.1,0]) cube([14.5,10,6]);    
        cube([14.6,7.5,14.5]);    
      }
}

module Loch_DisplayOLED096() {
   translate([2.8,7.25,-1]) cube([21.8,11.2,10]); 
}

module Teil_DisplayOLED096() {
    difference() {
      union() {
        color("gray") translate([2.8,7.25,1]) cube([21.8,11.2,4]);    
        color("darkred") cube([27,27,1]);  
        color("#000000") translate([5,14,5]) text("OLED", size=4);  
        color("#000000") translate([6,8,5]) text("0.96", size=5);
      }
      translate([3,2.5,-1]) cylinder(h=3,d=3); 
      translate([24.6,2.5,-1]) cylinder(h=3,d=3); 
      translate([3,24,-1]) cylinder(h=3,d=3); 
      translate([24.6,24,-1]) cylinder(h=3,d=3); 
    } 
}

module Loch_Display5110() {
   translate([2.8,7.25,0]) cube([40,34,10]); 
}

module Teil_Display5110() {
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

module Teil_Batt18650() {
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

module Halter_Platine(x,y,z, nasenbreite=1) {
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

module Halter_Platine2(x,y,z, nasenbreite=1) {
    naseY=1.41*nasenbreite;
    naseZ=1.41*nasenbreite;
    translate([-x/2,0,0])
    union() {
      cube([x,y,z+2*nasenbreite]);
      translate([x/2,0,z+nasenbreite])
      rotate(a=45, v=[1,0,0])
      cube([x,naseY,naseZ], center=true);
    }
}

module Schraubbolzen(hoehe,durchmesser,loch) {
    difference() {
        cylinder(hoehe,d=durchmesser);
        translate([0,0,-0.1]) cylinder(hoehe+0.2,d=loch);
    }
}

module Halter_Platine3x7(hoehe,durchmesser,loch) {
    LochX=26;
    LochY=66;
    translate([2,2]) 
      Schraubbolzen(hoehe, durchmesser, loch);
    translate([2,LochY+2]) 
      Schraubbolzen(hoehe, durchmesser, loch);
    translate([LochX+2,2]) 
      Schraubbolzen(hoehe, durchmesser, loch);
    translate([LochX+2,LochY+2]) 
      Schraubbolzen(hoehe, durchmesser, loch);
}

module Teil_Platine3x7(hoehe) {
    HP_X=30;
    HP_Y=70;
    dx  =2;
    dy  =2;
    Loch=2.5;
    difference() {
      cube([HP_X,HP_Y,hoehe]);
      union() {  
        translate([-1,-1,1]) cube([7,7,hoehe]);
        translate([HP_X-6,-1,1]) cube([7,7,hoehe]);
        translate([-1,HP_Y-6,1]) cube([7,7,hoehe]);
        translate([HP_X-6,HP_Y-6,1]) cube([7,7,hoehe]);
        translate([dx,dy,-0.1]) cylinder(2,d=Loch);
        translate([HP_X-dx,dy,-0.1]) cylinder(2,d=Loch);
        translate([dx,HP_Y-dy,-0.1]) cylinder(2,d=Loch);
        translate([HP_X-dx,HP_Y-dy,-0.1]) cylinder(2,d=Loch);
      }
    }
}

module Halter_Platine4x6(hoehe,durchmesser,loch,center=true) {
  LochX=35.9;
  LochY=55.5;
  module Halter_() {
    union() {
      translate([3,3,0]) Schraubbolzen(hoehe, durchmesser, loch);
      translate([3,LochY+3,0]) Schraubbolzen(hoehe, durchmesser, loch);
      translate([LochX+3,3,0]) Schraubbolzen(hoehe, durchmesser, loch);
      translate([LochX+3,LochY+3,0]) Schraubbolzen(hoehe, durchmesser, loch);
    }
  }
  color("green")
  if (center) translate([-20,0,0]) Halter_();
  else Halter_();    
}

module Teil_Platine4x6(hoehe,center=true) {
  HP_X=40;
  HP_Y=60;
  // Lochabstände
  LochX=35.9;
  LochY=55.5;
  dx=(HP_X-LochX)/2;
  dy=(HP_Y-LochY)/2;
  module Platine_() {  
    difference() {
      cube([HP_X,HP_Y,hoehe]);
      union() {  
        translate([-1,-1,1]) cube([7,7,hoehe]);
        translate([HP_X-6,-1,1]) cube([7,7,hoehe]);
        translate([-1,HP_Y-6,1]) cube([7,7,hoehe]);
        translate([HP_X-6,HP_Y-6,1]) cube([7,7,hoehe]);
        translate([dx,dy,-0.1]) cylinder(2,d=3);
        translate([HP_X-dx,dy,-0.1]) cylinder(2,d=3);
        translate([dx,HP_Y-dy,-0.1]) cylinder(2,d=3);
        translate([HP_X-dx,HP_Y-dy,-0.1]) cylinder(2,d=3);
      }
    }
  }
  color("blue")
  if (center) translate([-20,0,0]) Platine_();
  else Platine_();    
}

module Teil_Wippschalter(center=true) {
  module Wippschalter_() {
    union() {  
      translate([0,-2,0]) cube([21,2,15]);
      translate([0.8,0,1]) cube([19.4,16,13]);
      translate([4,-5,2.5]) rotate(20,[0,0,1]) cube([7,4,10]);  
    }
  }  
  color("blue")
  if (center) translate([-10.5,0,0]) Wippschalter_();
  else Wippschalter_();    
}

module Loch_Wippschalter(center=true) {
  color("red")  
  if (center) translate([-9.7,-9,1]) cube([19.4,25,13]);
  else translate([0.8,-9,1]) cube([19.4,25,13]);
}

module Halter_Mikroschalter(center=true,X=6.3,Y=6.3,Z=4.2) {
  Toleranz=0.2;  
  module Mikroschalter_() {
      union() {
        translate([X+Toleranz,0,Y/2])
          rotate(270,[1,0,0])
          rotate(270,[0,0,1])
          Halter_Platine2(Y,1,Z,0.5);
        translate([-Toleranz,0,Y/2])
          rotate(270,[1,0,0])
          rotate(90,[0,0,1])
          Halter_Platine2(Y,1,Z,0.5);
        translate([X/2-1,0,-2-Toleranz])    
          cube([2,1,2]);
        translate([X/2-1,0,Y+Toleranz])    
          cube([2,1,2]);
      }
  }
  color("green")
  if (center) translate([-X/2,0,0]) Mikroschalter_();
  else translate([0,0,0]) Mikroschalter_();
}

module Teil_Mikroschalter(center=true,X=6.3,Y=6.3,Z=4.2) {
  module Mikroschalter_() {
      union() {
        color("blue") cube([X,Z,Y]);
        color("brown") translate([X/2,1,Y/2]) rotate(90,[1,0,0]) cylinder(2,d=3);  
      }
  }
  
  if (center) translate([-X/2,0,0]) Mikroschalter_();
  else translate([0,0,0]) Mikroschalter_();
}

module Loch_Mikroschalter(center=true,X=6.3,Y=6.3,Z=4.2) {
  module Mikroschalter_L() {
      union() {
        color("red") translate([X/2,5,Y/2]) rotate(90,[1,0,0]) cylinder(10,d=4.5);  
      }
  }
  color("red")  
  if (center) translate([-X/2,0,0]) Mikroschalter_L();
  else translate([0,0,0]) Mikroschalter_L();
}

