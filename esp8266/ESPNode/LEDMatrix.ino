#if defined(LEDMATRIX)
/******************************************
 * Definition der Zeilenbeschriftung (line)
 * In den ersten 6 Zeichen werden Formatierungsangaben übertragen:
 * zeile[0]:              die Zeilennummer
 * zeile[1] und zeile[2]: offset in Pixel, gezählt von der Formatierungsseite. Wertebereich "00" bis "99"  
 * zeile[3]:              verwendeter font: "1" = variable x-size; "2" = fixed x-size 
 * zeile[4]:              Überblendmodus: "0" = just wipe; "9" = wipe with sprite
 * 
 * zeile[5]:              Ausrichtung: "R" = rechtsbündig; "L" = linksbündig
 * 
 * Definition der Graphikübertragung (graph)
 * zeile[N*5+0] und zeile[N*5+1]: X-Koordinaten; Wertebereich "00" bis "99" 
 * zeile[N*5+2] und zeile[N*5+3]: Y-Koordinaten; Wertebereich "00" bis "99" 
 * zeile[N*5+4]:                  "0" = Pixel löschen; "1" = Pixel setzen
 * ==> mit N zwischen 0 ... 3
 * Achtung: Wenn die Zeile länger wird stürzt der MC ab!!!!
 ******************************************/


byte sprite[][8]={ 
           {B00011000,
            B00100100,
            B00100100,
            B00011000,
            B01111110,
            B00011000,
            B00100100,
            B01000010},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B01111100,
            B00011000,
            B01100100,
            B00000010},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B00111100,
            B01011000,
            B00110100,
            B00000100},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B00111100,
            B01011000,
            B00011000,
            B00011000},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B00111100,
            B01011000,
            B00010100,
            B00010000},

          { B00011000,
            B00100100,
            B00100100,
            B00011000,
            B00111110,
            B01011000,
            B00010100,
            B00010100}
};


void drawSprite( byte* sprite, int x, int y, int width, int height )
{
  byte mask = B10000000;
  for( int iy = 0; iy < height; iy++ )
  {
    for( int ix = 0; ix < width; ix++ )
    {
      matrix.setPixel(x + ix, y + iy, (bool)(sprite[iy] & mask ));
      mask = mask >> 1;
    }
    mask = B10000000;
  }
}

void print_line(char* rohtext ) {
  unsigned int cursor_x, cursor_y;
  char linetext[LINE_SIZE];
  uint8_t line = rohtext[0]-'0';
  uint8_t offset = (rohtext[1]-'0')*10 + (rohtext[2]-'0');
  uint8_t font = (rohtext[3]-'0');
  unsigned char changemode = rohtext[4]-'0';
  uint8_t textallign = rohtext[5];
  uint8_t textlen = strlen(rohtext)-6;
  if (textlen > 0) {
    memcpy( linetext, &rohtext[6], textlen );
    linetext[textlen]='\0';
  } else {
    linetext[0]='\0';
  }
  switch (changemode) {
    case 9:      // Sprite
      {
        for (int x1 = 0; x1 < 6; x1++) {
          for (int x2 = 0; x2 < 6; x2++) {
            drawSprite( (byte*)&sprite[x2], x1 * 6 + x2, (line-1)*8, 8, 8 );
            matrix.display();
            delay(LED_MATRIX_ANIM_DELAY);
          }
        }            
      }
    break;
    case 0:      // just wipe
    default:
      {
        matrix.clear(0,(line-1)*8,31,line*8-1);            
      }
  }
  matrix.setFont(font);
  cursor_y = (line-1)*8;
  cursor_x = offset;
  if (textallign == 'L') {
    matrix.setCursor(cursor_x, cursor_y);
  }
  if (textallign == 'R') {
    cursor_x = matrix.getNumDevicesX()*8 - matrix.getSize(linetext) - offset;
    matrix.setCursor(cursor_x,cursor_y);
  }
//char mytext[100];
//sprintf(mytext,"Line:%u TA:%c X:%u Y:%u OS: %u Text:%s  len:%u",line,textallign,cursor_x,cursor_y,offset,linetext,strlen(linetext));
//write2log(mytext);
//sprintf(mytext,"NumDev: %u  SizeText: %u Offset: %u",  matrix.getNumDevicesX(), matrix.getSize(rohtext), offset);
//write2log(mytext);
  matrix.print(linetext); 
  matrix.display();
}

void getMatrixFB(char* mymsg) {
  unsigned int dotx = matrix.getNumDevicesX() * 8;
  unsigned int doty = matrix.getNumDevicesY() * 8;  
  memset(mymsg,0,sizeof mymsg);
  for (unsigned int y=0; y<doty; y++) {
    for (unsigned int x=0; x<dotx; x++) {
      if (matrix.getPixel(x,y)) {
        mymsg[x+y*dotx]='1';
      } else {
        mymsg[x+y*dotx]='0';  
      }
    }
  }
}

#endif
