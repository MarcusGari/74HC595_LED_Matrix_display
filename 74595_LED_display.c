///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////  AUTOR : Marcus Vinícius Marques Costa                                ///////////////
/////////////  FUNÇÃO: Exibir placar em LEDs usando 8 CIs 74HC595 em série          ///////////////
/////////////  DATA  : 13/mai/2015(v0.1 - PROTÓTIPO)                                ///////////////
/////////////  OBS.  : Inclui o uso de 4 pontos                                     ///////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//*******************************definições*********************************
#define dado        portA.f0            // Porta de saída p/ envio de dados ao display
#define atualiza    portB.f2            // Porta de saída p/ atualizar os dados do display
#define clock       portC.f7            // Porta de saída p/ controle de ciclos do display (CLOCK)
#define bit0(param) ((short *)&param)[0] & 0b00000001       // Ler o 1º bite de um registro

unsigned short int  brilho;             // Indica o nível de brilho ajustado

//***************Inicialização das variáveis do display**********************
void Inicia_display(){
// Configurações para leitura AD da corrente do display
   ADC_Init();
   TRISA.f2 = 1;                        //configura o pino de corrente do display como entrada
   ANSELA.f2 = 1;                       //configura o pino de corrente do display como analógico
   WPUA.f2 = 0;                         //desliga pull-up no pino de corrente do display
   FVRCON.f7 = 1;                       //ativa FVR (fixed voltage reference)
   FVRCON.f1 = 0;                       //ajusta FVR para 1,024 V
   FVRCON.f0 = 1;                       //ajusta FVR para 1,024 V
   ADCON0.f7 = 0;                       //ajusta resolução do ADC para 12 bits
   ADCON0.f0 = 1;                       //ativa conversor AD
   ADCON1.f7 = 1;                       //ajusta formato da conversão para complemento de 2
   ADCON1.f6 = 1;                       //ajusta clock da conversão AD para oscilador interno dedicado
   ADCON1.f5 = 1;                       //ajusta clock da conversão AD para oscilador interno dedicado
   ADCON1.f4 = 1;                       //ajusta clock da conversão AD para oscilador interno dedicado
   ADCON1.f2 = 0;                       //ajusta referência negativa para GND
   ADCON1.f1 = 1;                       //ajusta referência positiva para FVR (Fixed Voltage Reference)
   ADCON1.f0 = 1;                       //ajusta referência positiva para FVR (Fixed Voltage Reference)
// Configurações dos pinos de comunicação com o display
   TRISA.f0=0;                          //ajusta I/O do pino DADO como saída
   TRISB.f2=0;                          //ajusta I/O do pino ATUALIZA como saída
   TRISC.f7=0;                          //ajusta I/O do pino CLOCK como saída
   dado = 0;
   atualiza = 0;
   clock = 0;
// Configurações de brilho
   brilho = 1;
   PWM1_init(30000);
   PWM1_Set_Duty(254);
   PWM1_start();
}

//***************Função p/ Converter valores em caracteres*******************
int converte(unsigned char convertido){
// Números
//----------------------------abcdefg.
   if(convertido==0) return(0b11111100);
   if(convertido==1) return(0b01100000);
   if(convertido==2) return(0b11011010);
   if(convertido==3) return(0b11110010);
   if(convertido==4) return(0b01100110);
   if(convertido==5) return(0b10110110);
   if(convertido==6) return(0b10111110);
   if(convertido==7) return(0b11100000);
   if(convertido==8) return(0b11111110);
   if(convertido==9) return(0b11110110);
// Letras hexadecimais
//------------------------------abcdefg.
   if(convertido==10) return(0b11101110);      // A
   if(convertido==11) return(0b00111110);      // b
   if(convertido==12) return(0b10011100);      // C
   if(convertido==13) return(0b01111010);      // d
   if(convertido==14) return(0b10011110);      // E
   if(convertido==15) return(0b10001110);      // F
// Letras e símbolos
//------------------------------abcdefg.
   if(convertido==100) return(0b00000000);      // [espaço]
   if(convertido==101) return(0b11101110);      // A
   if(convertido==102) return(0b01111010);      // d
   if(convertido==103) return(0b00000010);      // -
   if(convertido==104) return(0b10011100);      // C
   if(convertido==105) return(0b11000110);      // ° (bolinha em cima)
   if(convertido==106) return(0b00111010);      // % (bolinha embaixo)
   if(convertido==107) return(0b11101100);      // n
   if(convertido==108) return(0b00010000);      // _
   if(convertido==109) return(0b00010010);      // =
   if(convertido==110) return(0b10010010);      // = (3 traços)
   if(convertido==111) return(0b10000000);      // ¨ (linha superior)
                       return(0);
}

//********************Função p/ enviar dados ao 74HC595**********************
void envia(unsigned char dados){
   unsigned char y = 0;
   while(y<8){
      dado = bit0(dados);
//	  if(bit0(dados)) qtd_seguimentos++;
      clock = 1;
      y++;
      dados >>= 1;
      clock = 0;
   }
}

//********************Função p/ Preencher Display****************************
void fill_display(unsigned char DIG11, unsigned char DIG12, unsigned char PONTO1, unsigned char PONTO2, unsigned char DIG13, unsigned char DIG14, unsigned char DIG21, unsigned char DIG22, unsigned char PONTO3, unsigned char PONTO4, unsigned char DIG23, unsigned char DIG24){
   unsigned char saida;               
//   qtd_seguimentos = 0;
   saida = converte(DIG21);
   envia(saida);
   saida = converte(DIG22);
   if(PONTO3) saida++;
   envia(saida);
   saida = converte(DIG23);
   if(PONTO4) saida++;
   envia(saida);
   saida = converte(DIG24);
   envia(saida);
   saida = converte(DIG14);
   envia(saida);
   saida = converte(DIG13);
   if(PONTO2) saida++;
   envia(saida);
   saida = converte(DIG12);
   if(PONTO1) saida++;
   envia(saida);
   saida = converte(DIG11);
   envia(saida);
   atualiza = 1;
   atualiza = 0;
}

void ajusta_brilho(){
   if(brilho==  1) PWM1_Set_Duty(254);
   if(brilho==  2) PWM1_Set_Duty(245);
   if(brilho==  3) PWM1_Set_Duty(210);
   if(brilho==  4) PWM1_Set_Duty(180);
   if(brilho==  5) PWM1_Set_Duty(150);
   if(brilho==  6) PWM1_Set_Duty(120);
   if(brilho==  7) PWM1_Set_Duty( 90);
   if(brilho==  8) PWM1_Set_Duty( 60);
   if(brilho==  9) PWM1_Set_Duty( 30);
   if(brilho== 10) PWM1_Set_Duty(  0);
}