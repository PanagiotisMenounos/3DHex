void signals_buf1(){
  switch (pin_config){
    case 0:
      PORTF |= (bitRead(buffer1.byte_1[j],0)<<PF1); //XDIR
      PORTF |= (bitRead(buffer1.byte_1[j],2)<<PF7); //YDIR
      PORTL |= (bitRead(buffer1.byte_1[j],4)<<PL1); //ZDIR
      PORTA |= (bitRead(buffer1.byte_1[j],6)<<PA6); //EDIR
      PORTF |= (bitRead(buffer1.byte_1[j],1)<<PF0); //XSTEP
      PORTF |= (bitRead(buffer1.byte_1[j],3)<<PF6); //YSTEP
      PORTL |= (bitRead(buffer1.byte_1[j],5)<<PL3); //ZSTEP      
      PORTA |= (bitRead(buffer1.byte_1[j],7)<<PA4); //ESTEP
      break;
    }
}

void signals_buf2(){
  switch (pin_config){
    case 0:
      PORTF |= (bitRead(buffer2.byte_2[j],0)<<PF1);
      PORTF |= (bitRead(buffer2.byte_2[j],2)<<PF7);
      PORTL |= (bitRead(buffer2.byte_2[j],4)<<PL1);
      PORTA |= (bitRead(buffer2.byte_2[j],6)<<PA6);
      PORTF |= (bitRead(buffer2.byte_2[j],1)<<PF0);
      PORTF |= (bitRead(buffer2.byte_2[j],3)<<PF6);
      PORTL |= (bitRead(buffer2.byte_2[j],5)<<PL3);
      PORTA |= (bitRead(buffer2.byte_2[j],7)<<PA4);  
      break;
  }
}

void signal_zero(){
  switch (pin_config){
    case 0:
      PORTF &= (0<<PF0);
      PORTF &= (0<<PF6);
      PORTL &= (0<<PL3);
      PORTA &= (0<<PA4);
      break;
  }
}
