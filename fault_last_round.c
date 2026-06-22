//in this code the fault location is identified and the full state is recovered.

#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

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
	printf("\n");
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
   uint8_t extract_s[257] = {0};// extract the bits from the diff

   char bits[] = "10111100110101100000101100011110001110101111010010101001000111010101110101010010100000110100001000011000111010001001110101111111111011010110010111000101010101111000101000001011111001101101011111001101001110000000111001010101001100100011100100110000101110010";

   for(int i = 0; i < 257; i++)
   {
      s_init[i] = (bits[i] == '1') ? 1 : 0;
   }

   Koala_P(s_init,s_out,1);

   clock_t start_time, end_time;
   double cpu_time_used;

   long total_accumulated_faults = 0;
   double total_accumulated_time = 0.0;
   int NUM_SIMULATIONS = 30000;

   srand(time(NULL)); 

   for (int sim = 1; sim <= NUM_SIMULATIONS; sim++) {
       int recovered_flags[257] = {0}; 
       int recovered_count = 0;        
       int total_faults = 0;           
       int recorded_locations[2000];   
       
       // Clear extract_s for each simulation
       for(int i = 0; i < 257; i++) {
           extract_s[i] = 0;
       }

       start_time = clock();

       while(recovered_count < 257) 
       {
           int r_loc = rand() % 257;//random faults
           
           recorded_locations[total_faults] = r_loc;
           total_faults++;

           Koala_P_faulty(s_init, s_out_faulty, 1, r_loc);

           for(int i = 0; i < 257; i++)
               s_xor[i] = s_out[i] ^ s_out_faulty[i];

           // Fault Identification 
           int identified_loc = -1;

           for (int candidate = 0; candidate < 257; candidate++) {
               int is_subset = 1;
               for (int idx = 0; idx < 257; idx++) {
                   if (s_xor[idx] == 1) {
                       int in_D = 0;
                       int offsets[9] = {0, 1, 2, 3, 4, 5, 10, 11, 12};
                       for(int k = 0; k < 9; k++) {
                           if (idx == (257 + candidate - offsets[k]) % 257) {
                               in_D = 1;
                               break;
                           }
                       }
                       if (in_D == 0) {
                           is_subset = 0;
                           break;
                       }
                   }
               }
               
               if (is_subset == 1) {
                   int b1 = candidate;
                   int b2 = (257 + candidate - 3) % 257;
                   int b3 = (257 + candidate - 10) % 257;
                   
                   if (s_xor[b1] == 1 && s_xor[b2] == 1 && s_xor[b3] == 1) {
                       identified_loc = candidate;
                       break; 
                   }
               }
           }

            //printf("Attempt %d: Real Fault Loc = %d, Identified Loc = %d\n", total_faults, r_loc, identified_loc);

           // Extraction
           if (identified_loc != -1) {
               int indices[6] = {
                   (257 + (identified_loc - 11)) % 257,
                   (257 + (identified_loc - 9))  % 257,
                   (257 + (identified_loc - 4))  % 257,
                   (257 + (identified_loc - 2))  % 257,
                   (257 + (identified_loc - 1))  % 257,
                   (257 + (identified_loc + 1))  % 257
               };

               uint8_t values[6] = {
                   s_xor[(257 + (identified_loc - 12)) % 257] ^ 1,
                   s_xor[(257 + (identified_loc - 11)) % 257],
                   s_xor[(257 + (identified_loc - 5))  % 257] ^ 1,
                   s_xor[(257 + (identified_loc - 4))  % 257],
                   s_xor[(257 + (identified_loc - 2))  % 257] ^ 1,
                   s_xor[(257 + (identified_loc - 1))  % 257]
               };

               for (int k = 0; k < 6; k++) {
                   if (recovered_flags[indices[k]] == 0) {
                       extract_s[indices[k]] = values[k];
                       recovered_flags[indices[k]] = 1; 
                       recovered_count++;               
                   }
               }
           }
       }

       end_time = clock();
       cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

       // Muted per-simulation output to optimize performance
       // printf("Simulation %d: %d random faults required for full recovery.\n", sim, total_faults);

       // Heartbeat progress logger
       if (sim % 5000 == 0) {
           printf("Progress: Completed %d / %d simulations (still running...)\n", sim, NUM_SIMULATIONS);
       }

       total_accumulated_faults += total_faults;
       total_accumulated_time += cpu_time_used;
   }

   printf("\nthe value of the extracted state for the last simulation:\n");
   print_state(extract_s);

   double average_faults = (double)total_accumulated_faults / NUM_SIMULATIONS;
   double average_time = total_accumulated_time / NUM_SIMULATIONS;

   printf("\n\n=== %d SIMULATIONS COMPLETE ===\n", NUM_SIMULATIONS);
   printf("Average faults required: %.2f\n", average_faults);
   printf("Average time per recovery: %f seconds\n", average_time);
   printf("Total time taken: %f seconds\n", total_accumulated_time);
   printf("----------------------------------------------------------------\n");
  

}
