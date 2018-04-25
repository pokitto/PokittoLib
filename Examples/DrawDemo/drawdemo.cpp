#include "Pokitto.h" // liittää Pokitto-kirjaston ohjelman osaksi

Pokitto::Core peli; // luodaan ohjelmaobjekti nimeltä "peli"

// ohjelmassa käytettävät muuttujat
int vari=1; // pensselin väri - ääkkösiä ei voi käyttää muuttujissa!
int koko=2; // pensselin koko
int x=55; // pensselin x-sijainti
int y=44; // pensselin y-sijainti

int main () {
    peli.begin(); // käynnistetään Pokitton näyttö ja muut toiminnot
    peli.display.persistence=1; // käännetään näytön "pyyhkiytyminen" pois päältä
    
    while (peli.isRunning()) { // while eli "kunnes" pyörii niin kauan kun peli.isRunning = 1 
        if (peli.update()) { // kun on tullut aika piirtää näyttö uudelleen           
            
            // tarkistetaan näppäimet
            if (peli.buttons.upBtn()) y--;
            if (peli.buttons.downBtn()) y++;
            if (peli.buttons.leftBtn()) x--;
            if (peli.buttons.rightBtn()) x++;
            if (peli.buttons.aBtn()) vari++;
            if (peli.buttons.bBtn()) koko+=2;
            if (peli.buttons.cBtn()) peli.display.clear();
                
            // rajoitetaan pensselin liike pysymään näytön sisäpuolella
            if (x<0) x=0;
            if (y<0) y=0;
            if (x>110) x=110;
            if (y>88) y=88;
            
            // rajoitetaan pensselin koko ja väri, huomaa "==" on vertailu C++ kielessä!!
            if (vari==16) vari=1;
            if (koko>8) koko=2;
            
            // piirretään pensseli
            peli.display.setColor(vari); // aseta pensselin väri
            peli.display.fillCircle(x,y,koko); // piirrä pensseli
            
            } 
        }    
}