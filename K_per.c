//this code has implementation of Koala Permutation using uint8_t which means every bit is represented using 8 bits.

#include<stdio.h>
#include<stdint.h>
#include <string.h>

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
        non_linear(s_mix,s_temp);
      
   }
	for(int i=0;i<257;i++)
	   s_out[i]=s_temp[i];
}

void main()
{
    uint8_t s0[257]={0},s0_p[257]={0};
    printf("\nthe value of the initial state s0\n");
    print_state(s0); 
    Koala_P(s0,s0_p,1);
    printf("\nthe final value after 1 round of koala P s0_p\n");
    print_state(s0_p); 
 printf("\n--------------------------------------------------------------------------\n");    
}



