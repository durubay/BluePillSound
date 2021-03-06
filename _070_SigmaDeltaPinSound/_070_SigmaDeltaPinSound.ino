/*

  sigma delta modulator experiments

  Generate a wavetable song sound.

  The wave is recorded 'A' ( 440Hz )

  Uses the melodies of  "Weihnachtsklingel" from M.Baudisch ( take a look at the comments of the file lied.h )


  ************************************************************************

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  ********************* list of outhors **********************************

  v0.1   2.6.2013  C. -H-A-B-E-R-E-R-  initial version
  v0.2  22.3.2017  C. -H-A-B-E-R-E-R-  addapted and improved for STM32F103 ( Blue Pill )

  It is mandatory to keep the list of authors in this code.
  Please add your name if you improve/extend something


*/

#include "lied.h"
#include "wavetables.h"
#include "speakerpin.h"



void setup() {
  // initialize the digital pin as an output.
  //pinMode(LOUDSPEAKER, OUTPUT);
  FASTSOUNDPIN.pinMode(OUTPUT);

}

//uint8_t n;

// pitch in Hz
// duration in ms
void sound(uint16_t pitch, uint16_t duration)
{
  uint32_t phaseDelta = 1070UL * 10UL;
  int16_t  integrator = 0;
  uint8_t  sollwert = 64;
  uint8_t  oldValue = 0;
  uint32_t phase = 0;
  uint16_t timer;
  uint16_t index = 0;
  uint8_t n;

  timer = duration * 25 * 4;
  //timer=duration*50;
  //phaseDelta = 42UL * pitch;
  phaseDelta = 36UL * pitch;
  const uint8_t *p;
  if(pitch>300)  p=sabine;
  else p=piano;
  FASTSOUNDPIN.pinMode(OUTPUT);
  while (timer != 0)
  {
    // DDS ( direct digital synthesis )
    index = phase >> 16;
    if(index<SONGLEN) sollwert=p[index];
    else sollwert = 128;
    phase += phaseDelta;
    timer--;

    // sigma delta DAC, hold the DAC value for n-steps constant
    //FASTSOUNDPIN.pinMode(OUTPUT);
    for (n = 0; n < 40; n++)
    {
      integrator += sollwert - oldValue;
      if (integrator > 0)
      {
        oldValue = MAXVALUE;
        //digitalWrite(LOUDSPEAKER,1);
        fastWrite(FASTSOUNDPIN, 1);
      }
      else
      {
        oldValue = 0;
        //digitalWrite(LOUDSPEAKER,0);
        fastWrite(FASTSOUNDPIN, 0);
      }
    }
    //pinMode(LOUDSPEAKER,INPUT);
    //FASTSOUNDPIN.pinMode(INPUT); // turn off pull up
  }
  FASTSOUNDPIN.pinMode(INPUT); // turn off pull up
}

const uint16_t tonhoehe[] =
{
  262,//262 c    Timerwerte fuer die enspr. Tonhoehen in Hz
  277,//277 cis
  294,//294 d
  311,//311 dis
  330,//330 e
  349,//349 f
  370,//370 fis
  392,//392 g
  416,//416 gis
  440,//440 a
  466,//466 b
  495,//495 h
  524,//524 c'
  554,//554 cis'
  588,//588 d'
  622,//622 dis'
  660,//660 e'
  698,//698 f'
  740,//740 fis'
  784,//784 g'
  831,//831 gis'
  880,//880 a'
  929,//929 b'
  992,//992 h'
  1050,//1050 c''
  1106,//1106 cis''
  1179,//1179 d''
  1244,//1244 dis''
  1316,//1316 e'
  1397,//1397 f''
  1562, //1562 g'' auf position von fis'' verschoben
  0
};

void playLied(uint8_t liedNr)
{
  uint16_t liedIndex, endIndex;
  uint32_t duration;
  uint8_t dauer, playSpeed;
  uint16_t frequency;
  uint16_t n;

  liedIndex = liedofs[liedNr];
  playSpeed = lied[liedIndex];
  endIndex = liedofs[liedNr + 1];
  for (n = liedIndex; liedIndex < endIndex; n++)
  {
    dauer = pgm_read_byte(&lied[n]) >> 5;
    duration = (uint32_t) zeit[dauer] * 30 * playSpeed; // duration in ms
    frequency = tonhoehe[(pgm_read_byte(&lied[n]) & 0x1F)]; // frequenz in Hz
    sound(frequency, duration);

    liedIndex++;
  }
  sound(0, 1000); // delay 1000ms
}

void loop() 
{
  uint8_t n;
  /*

    for(n=0;n<0x20;n++)
    {

      sound(440,200);
    }
  */
  for (n = 0; n < ANZ; n++) playLied(n);
}

