//this code injects faults at 52 locations independently and recovers the whole state before the chi layer. 

#include<stdio.h>
#include<stdint.h>
#include<string.h>

void  print_state(uint8_t s[257])
{
  for(int i=0;i<257;i++)
   printf("%u",s[i]&1);
}

void bit_shuffle(uint8_t s[257],uint8_t s_out[257])
{ 
  for(int i=0;i<257;i++)
  { 
    s_out[i]=s[(121*i)%257];
  }

}
  
void mixing(uint8_t s[257],uint8_t s_out[257])
{ 
  for(int i=0;i<257;i++)
  { 
    s_out[i]=s[i]^s[(i+3)%257]^s[(i+10)%257];
  }
  
}

void add_const(uint8_t s[257])
{
   s[0]^=1;

}

void non_linear(uint8_t s[257],uint8_t s_out[257])
{
  for(int i=0;i<257;i++)
  {
	  
    s_out[i]=s[i]^s[(i+2)%257]^(s[(i+1)%257]&s[(i+2)%257]);
  }
  
}

void Koala_P(uint8_t s[257],uint8_t s_out[257],int rounds)
{  
   uint8_t s_bit[257]={0}, s_mix[257]={0}, s_temp[257]={0};

   for(int i=0;i<257;i++)
      s_temp[i]=s[i];
   
   for(int i=0;i<rounds;i++)
   {  
	bit_shuffle(s_temp,s_bit);     
        mixing(s_bit,s_mix);
        if(i!=1&&i!=4&&i!=5)
        add_const(s_mix);
	printf("\nthe value of the fault free state before non-linear layer:\n");
        print_state(s_mix);
        non_linear(s_mix,s_temp);
   }
	for(int i=0;i<257;i++)
	   s_out[i]=s_temp[i];
}


void Koala_P_faulty(uint8_t s[257],uint8_t s_out[257],int rounds,int fault_location)
{  
   uint8_t s_bit[257]={0}, s_mix[257]={0}, s_temp[257]={0};

	for(int i=0;i<257;i++)
        s_temp[i]=s[i];

   	for(int i=0;i<rounds;i++)
	{	
		bit_shuffle(s_temp,s_bit);
		s_bit[fault_location]^=1;     
        	mixing(s_bit,s_mix);
        	if(i!=1&&i!=4&&i!=5)
        	add_const(s_mix);
        	non_linear(s_mix,s_temp);
         }
	for(int i=0;i<257;i++)
	   s_out[i]=s_temp[i];
}



void main()
{
   uint8_t s_init[257]={0},s_out[257]={0},s_out_faulty[257]={0},s_xor[257]={0};
   uint8_t extract_s[257];// extract the bits from the diff

   char bits[] = "10111100110101100000101100011110001110101111010010101001000111010101110101010010100000110100001000011000111010001001110101111111111011010110010111000101010101111000101000001011111001101101011111001101001110000000111001010101001100100011100100110000101110010";

   int  fault_loc[52]={245, 249, 3, 7, 18, 22, 33, 37, 48, 52, 63, 67, 78, 82, 93, 97, 108, 112, 123, 127, 138, 142, 153, 157, 168, 172, 183, 187, 198, 202, 213, 217, 228, 232, 236, 253, 11, 26, 41, 56, 71, 86, 101, 116, 131, 146, 161, 176, 191, 206, 221, 248}; //the initial 52 bits for fault

  for(int i = 0; i < 257; i++)
  {
     s_init[i] = (bits[i] == '1') ? 1 : 0;
  }

  Koala_P(s_init,s_out,1);

  for (int fault_location=0;fault_location<52;fault_location++)
  {
	Koala_P_faulty(s_init,s_out_faulty,1,fault_loc[fault_location]);

	for(int i=0;i<257;i++)
		s_xor[i]=s_out[i]^s_out_faulty[i];

	extract_s[(257+(fault_loc[fault_location]-11))%257]=s_xor[(257+(fault_loc[fault_location]-12))%257]^1;
	extract_s[(257+(fault_loc[fault_location]-9))%257]=s_xor[(257+(fault_loc[fault_location]-11))%257];
	extract_s[(257+(fault_loc[fault_location]-4))%257]=s_xor[(257+(fault_loc[fault_location]-5))%257]^1;
	extract_s[(257+(fault_loc[fault_location]-2))%257]=s_xor[(257+(fault_loc[fault_location]-4))%257];
	extract_s[(257+(fault_loc[fault_location]-1))%257]=s_xor[(257+(fault_loc[fault_location]-2))%257]^1;
	extract_s[(257+(fault_loc[fault_location]+1))%257]=s_xor[(257+(fault_loc[fault_location]-1))%257];

   }

   printf("\nthe value of the extracted state:\n");
   print_state(extract_s);
    	
   printf("\n----------------------------------------------------------------\n");
  

}
