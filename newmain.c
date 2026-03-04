#include <xc.h>

#pragma config FOSC = INTOSCIO 
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = ON
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000

#define LCD_RS RB1
#define LCD_EN RB3
#define BTN_MOD  RA0  
#define BTN_OYUN RA1  
#define DISP1_EN RA2  
#define DISP2_EN RA3  

const unsigned char seg_map[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void sistem_temizle() {
    DISP1_EN = 0; 
    DISP2_EN = 0; 
    PORTB = 0x00;
}

void lcd_nibble_yaz(unsigned char veri) {
    PORTB = (PORTB & 0x0F) | (veri & 0xF0); 
    LCD_EN = 1; __delay_us(20); LCD_EN = 0; __delay_us(20);
}

void lcd_veri_gonder(unsigned char veri, unsigned char tip) {
    LCD_RS = tip; lcd_nibble_yaz(veri); lcd_nibble_yaz(veri << 4); __delay_ms(2); 
}

void lcd_temizle(void) { lcd_veri_gonder(0x01, 0); __delay_ms(2); }

void lcd_baslat(void) {
    sistem_temizle();
    LCD_RS = 0; LCD_EN = 0; __delay_ms(20);
    lcd_nibble_yaz(0x30); __delay_ms(5); lcd_nibble_yaz(0x30); __delay_ms(1); lcd_nibble_yaz(0x30);
    lcd_nibble_yaz(0x20); lcd_veri_gonder(0x28, 0); lcd_veri_gonder(0x0C, 0); lcd_temizle();
}

void lcd_kapat(void) { lcd_veri_gonder(0x08, 0); }

void lcd_git(unsigned char satir, unsigned char sutun) {
    unsigned char adres = (satir == 1) ? (0x80 + sutun) : (0xC0 + sutun);
    lcd_veri_gonder(adres, 0);
}

void lcd_yaz(const char *s) { while(*s) lcd_veri_gonder(*s++, 1); }

void lcd_sayi_yaz(unsigned char deger) { lcd_veri_gonder(deger + '0', 1); }

void mod_refleks_oyunu() {
    sistem_temizle();
    unsigned char sayac = 0, hedef = 0, yakalanan, ilk_acilis = 1;
    lcd_baslat(); 
    while(1) {
        lcd_temizle();
        if (ilk_acilis) { lcd_git(1, 0); lcd_yaz("BASLAMAK ICIN"); lcd_git(2, 0); lcd_yaz("BUTONA BAS"); ilk_acilis = 0; }
        else { lcd_git(1, 0); lcd_yaz("YENI OYUN ICIN"); lcd_git(2, 0); lcd_yaz("BUTONA BAS"); }
        while(BTN_OYUN == 0) {
            hedef++; if(hedef > 9) hedef = 0; __delay_ms(5);
            if(BTN_MOD == 1) { while(BTN_MOD == 1); return; }
        }
        while(BTN_OYUN == 1); __delay_ms(200);
        lcd_temizle(); lcd_git(1, 0); lcd_yaz("HEDEF: "); lcd_sayi_yaz(hedef);
        lcd_git(2, 0); lcd_yaz("HAZIRLAN..."); __delay_ms(1000); 
        lcd_temizle(); lcd_git(1, 0); lcd_yaz("HEDEF: "); lcd_sayi_yaz(hedef);
        sayac = 0; 
        while(BTN_OYUN == 0) {
            lcd_git(2, 7); lcd_sayi_yaz(sayac); __delay_ms(90); 
            if(BTN_MOD == 1) { while(BTN_MOD == 1); return; }
            if(BTN_OYUN == 1) break;
            sayac++; if(sayac > 9) sayac = 0;
        }
        yakalanan = sayac; lcd_temizle();
        if (yakalanan == hedef) { lcd_git(1, 3); lcd_yaz("BRAVO!"); }
        else { lcd_git(1, 0); lcd_yaz("OLMADI :("); lcd_git(2, 0); lcd_yaz("GELEN:"); lcd_sayi_yaz(yakalanan); lcd_yaz(" HDF:"); lcd_sayi_yaz(hedef); }
        for(unsigned char k=0; k<20; k++) { __delay_ms(100); if(BTN_MOD == 1) { while(BTN_MOD == 1); return; } }
        while(BTN_OYUN == 1); 
    }
}

void mod_kara_simsek() {
    sistem_temizle();
    unsigned char i, k, limit = 30;
    lcd_kapat(); 
    while(1) {
        for(i=0; i<7; i++) {
            PORTB = (1 << i);
            for(k=0; k < limit; k++) {
                __delay_ms(5);
                if (BTN_OYUN == 1) { __delay_ms(20); if (BTN_OYUN == 1) { if (limit > 5) limit -= 5; else limit = 30; while(BTN_OYUN == 1); } }
                if (BTN_MOD == 1) { sistem_temizle(); while(BTN_MOD == 1); return; }
            }
        }
        for(i=7; i>0; i--) {
            PORTB = (1 << i);
            for(k=0; k < limit; k++) {
                __delay_ms(5);
                if (BTN_OYUN == 1) { __delay_ms(20); if (BTN_OYUN == 1) { if (limit > 5) limit -= 5; else limit = 30; while(BTN_OYUN == 1); } }
                if (BTN_MOD == 1) { sistem_temizle(); while(BTN_MOD == 1); return; }
            }
        }
    }
}

void mod_display_sayac() {
    sistem_temizle();
    signed char sayi = 50;
    unsigned int oto_sayac = 0;
    lcd_kapat(); 
    while(1) {
        DISP2_EN = 0; PORTB = seg_map[sayi / 10]; DISP1_EN = 1; __delay_ms(5);
        DISP1_EN = 0; PORTB = seg_map[sayi % 10]; DISP2_EN = 1; __delay_ms(5);
        oto_sayac++;
        if(oto_sayac > 40) { oto_sayac = 0; sayi--; }
        if(BTN_OYUN == 1) {
            __delay_ms(50); 
            if(BTN_OYUN == 1) {
                sayi--; oto_sayac = 0;
                while(BTN_OYUN == 1) {
                   DISP2_EN = 0; PORTB = seg_map[sayi / 10]; DISP1_EN = 1; __delay_ms(5);
                   DISP1_EN = 0; PORTB = seg_map[sayi % 10]; DISP2_EN = 1; __delay_ms(5);
                }
            }
        }
        if(sayi < 0) sayi = 50;
        if(BTN_MOD == 1) { sistem_temizle(); while(BTN_MOD == 1); return; }
    }
}

void mod_genisleyen_led() {
    sistem_temizle();
    lcd_kapat();
    unsigned char asama = 0;
    PORTB = 0x18; 
    while(1) {
        if(BTN_OYUN == 1) {
            __delay_ms(20); 
            if(BTN_OYUN == 1) {
                asama++;
                if(asama > 4) asama = 0; 
                switch(asama) {
                    case 0: PORTB = 0x18; break; 
                    case 1: PORTB = 0x3C; break; 
                    case 2: PORTB = 0x7E; break; 
                    case 3: PORTB = 0xFF; break; 
                    case 4: PORTB = 0x00; break; 
                }
                while(BTN_OYUN == 1); 
            }
        }
        if(BTN_MOD == 1) {
            sistem_temizle();
            while(BTN_MOD == 1);
            return;
        }
    }
}

void main(void) {
    CMCON = 0x07; TRISA = 0xF3; TRISB = 0x00; PORTB = 0x00;
    while(1) {
        mod_refleks_oyunu();
        mod_kara_simsek();
        mod_display_sayac();
        mod_genisleyen_led(); 
    }
}