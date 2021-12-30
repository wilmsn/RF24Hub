// Bibliothek nw_toolbox
//
// Diverse Bauteile für Gehäuse und Co.
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
//translate([0,-1,0])
//Platinenhalter2(20,10,10,1);
//Schienenfuehrung(4,3,20,1.5,1.5);
//Schraubbolzen(10,6,2);

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