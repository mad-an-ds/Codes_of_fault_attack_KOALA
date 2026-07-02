//in this code, for 100 states faults are injected randomly and the state before non-linear layer (chi layer) is recovered.
//for each state 1000 times the faults are injected in random and we observe the number of faults required to recover the state  every time (out of the 1000 times)
//then we take an average of how many faults are needed for each state.
//Finally, we do a GRAND Average of the average no. of fauts reqired to recover each state.
//here the faults are injected in the penultimate round 

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void bit_shuffle(uint8_t s[257], uint8_t s_out[257]) 
{
    for (int i = 0; i < 257; i++) 
    {
        s_out[i] = s[(121 * i) % 257];
    }
}

void mixing(uint8_t s[257], uint8_t s_out[257]) 
{
    for (int i = 0; i < 257; i++) 
    {
        s_out[i] = s[i] ^ s[(i + 3) % 257] ^ s[(i + 10) % 257];
    }
}

void add_const(uint8_t s[257]) 
{
    s[0] ^= 1;
}

void non_linear(uint8_t s[257], uint8_t s_out[257]) 
{
    for (int i = 0; i < 257; i++) 
    {
        s_out[i] = s[i] ^ s[(i + 2) % 257] ^ (s[(i + 1) % 257] & s[(i + 2) % 257]);
    }
}

void Koala_P_faulty_2round(uint8_t s[257], uint8_t s_out[257], int fault_location) 
{
    uint8_t s_bit[257] = {0}, s_mix[257] = {0}, s_temp[257] = {0};

    for (int i = 0; i < 257; i++)
        s_temp[i] = s[i];

    for (int i = 0; i < 2; i++) 
    {
        bit_shuffle(s_temp, s_bit);
        if (i == 0) { 
	// injecting fault in the first round
            s_bit[fault_location] ^= 1;
        }
        mixing(s_bit, s_mix);
        if (i != 1 && i != 4 && i != 5)
            add_const(s_mix);
        non_linear(s_mix, s_temp);
    }
    for (int i = 0; i < 257; i++)
        s_out[i] = s_temp[i];
}

void Koala_P(uint8_t s[257], uint8_t s_out[257]) 
{
    uint8_t s_bit[257] = {0}, s_mix[257] = {0}, s_temp[257] = {0};

    for (int i = 0; i < 257; i++)
        s_temp[i] = s[i];

    for (int i = 0; i < 2; i++) 
    {
        bit_shuffle(s_temp, s_bit);
        mixing(s_bit, s_mix);
        if (i != 1 && i != 4 && i != 5)
            add_const(s_mix);
        non_linear(s_mix, s_temp);
    }
    for (int i = 0; i < 257; i++)
        s_out[i] = s_temp[i];
}

int main() 
{
    uint8_t s_init[257] = {0}, s_out[257] = {0}, s_out_faulty[257] = {0}, s_xor[257] = {0};
    uint8_t extract_s[257] = {0}; //this will be used to extract the bits of the state
    
    int NUM_STATES = 100;
    int RECOVERIES_PER_STATE = 1000;
    
    srand(time(NULL));

    FILE *fp = fopen("state_recovery_results.txt", "w");
    if (fp == NULL) 
    {
        printf("Error opening file!\n");
        return 1;
    }

    fprintf(fp, "Number of Unique States: %d\n", NUM_STATES);
    fprintf(fp, "Recoveries per State: %d\n\n", RECOVERIES_PER_STATE);
    
    clock_t start_time = clock();
    double grand_total_average = 0.0;

    for (int state_idx = 1; state_idx <= NUM_STATES; state_idx++) 
    {
       //this loop will determine the unique states 
        // generates and fixes a random state for this iteration
        for (int i = 0; i < 257; i++) 
	{
            s_init[i] = rand() % 2;
        }

        long total_faults_for_this_state = 0;

        // performing 1000 recoveries on this EXACT same state
        for (int sim = 1; sim <= RECOVERIES_PER_STATE; sim++) 
	{
            int recovered_flags[257] = {0};
            int recovered_count = 0;
            int faults_this_sim = 0;

            for (int i = 0; i < 257; i++) 
		extract_s[i] = 0;

            Koala_P(s_init, s_out);

            while (recovered_count < 257) //keep injecting fault till all the locations are recovered
	    { 
                int k = rand() % 257; // Random fault location
                faults_this_sim++;

                Koala_P_faulty_2round(s_init, s_out_faulty, k);

                for (int i = 0; i < 257; i++) {
		
                    s_xor[i] = s_out[i] ^ s_out_faulty[i];
                }
                // identify fault location
                int identified_loc = -1;
                for (int candidate = 0; candidate < 257; candidate++) 
		{
                    int is_subset = 1;
                    for (int idx = 0; idx < 257; idx++) 
		    {
                        if (s_xor[idx] == 1) 
			{
                            int in_D = 0;
                            int offsets[9] = {0, 1, 2, 3, 4, 5, 10, 11, 12};
                            for(int f = 0; f < 9; f++) 
			    {
                                int l = (candidate - offsets[f] + 257) % 257;
                                int l17 = (17 * l) % 257;
                                for (int j = 0; j < 9; j++) 
				{
                                    int pos = (l17 - offsets[j] + 257) % 257;
                                    if (idx == pos) 
				    { 
					in_D = 1; 
					break; 
				    }
                                }
                                if(in_D) 
					break;
                            }
                            if (!in_D) 
			    {
				 is_subset = 0; break; 
			    }
                        }
                    }
                    
                    if (is_subset) 
		    {
                        int b_offsets[9] = {
                            (17 * candidate) % 257,
                            (17 * candidate - 3 + 257) % 257,
                            (17 * candidate - 10 + 257) % 257,
                            (17 * (candidate - 3 + 257)) % 257,
                            (17 * (candidate - 3 + 257) - 3 + 257) % 257,
                            (17 * (candidate - 3 + 257) - 10 + 257) % 257,
                            (17 * (candidate - 10 + 257)) % 257,
                            (17 * (candidate - 10 + 257) - 3 + 257) % 257,
                            (17 * (candidate - 10 + 257) - 10 + 257) % 257
                        };
                        
                        int all_b = 1;
                        for(int i=0; i<9; i++) 
			{
                            if (s_xor[b_offsets[i]] == 0) 
				{
					 all_b = 0; 
					 break; 
				}
                        }
                        if (all_b) 
			{ 	identified_loc = candidate; 
				break; 
			}
                    }
                }

                // extracting the Bit
                if (identified_loc != -1) 
		{
                    int P_det[3] = {
                        (17 * identified_loc) % 257, 
                        (17 * (identified_loc - 3 + 257)) % 257, 
                        (17 * (identified_loc - 10 + 257)) % 257
                    };
                    
                    int P_dep[6] = {
                        (17 * (identified_loc - 1 + 257)) % 257, 
                        (17 * (identified_loc - 2 + 257)) % 257, 
                        (17 * (identified_loc - 4 + 257)) % 257,
                        (17 * (identified_loc - 5 + 257)) % 257,
                        (17 * (identified_loc - 11 + 257)) % 257,
                        (17 * (identified_loc - 12 + 257)) % 257
                    };

                    int active_positions[9];
                    int active_count = 0;

                    for(int i=0; i<3; i++) 
		    {
                        active_positions[active_count++] = P_det[i];
                    }

                    for(int i=0; i<6; i++) 
		    {
                        int q = P_dep[i];
                        if (s_xor[q] == 1 && s_xor[(q - 3 + 257) % 257] == 1 && s_xor[(q - 10 + 257) % 257] == 1) 
			{
                            active_positions[active_count++] = q;
                        }
                    }

                    for(int a = 0; a < active_count; a++) 
		    {
                        int q = active_positions[a];
                        int indices[6] = {
                           (q - 11 + 257) % 257, (q - 9 + 257)  % 257, (q - 4 + 257)  % 257,
                           (q - 2 + 257)  % 257, (q - 1 + 257)  % 257, (q + 1)        % 257
                        };
                        uint8_t values[6] = {
                           s_xor[(q - 12 + 257) % 257] ^ 1, s_xor[(q - 11 + 257) % 257],
                           s_xor[(q - 5 + 257)  % 257] ^ 1, s_xor[(q - 4 + 257)  % 257],
                           s_xor[(q - 2 + 257)  % 257] ^ 1, s_xor[(q - 1 + 257)  % 257]
                        };

                        for (int i = 0; i < 6; i++) 
			{
                            if (recovered_flags[indices[i]] == 0) 
			    {
                                extract_s[indices[i]] = values[i];
                                recovered_flags[indices[i]] = 1; 
                                recovered_count++;               
                            }
                        }
                    }
                }
            }
            
            total_faults_for_this_state += faults_this_sim;
        }

        // calculatingaverage for this state
        double avg_for_state = (double)total_faults_for_this_state / RECOVERIES_PER_STATE;
        grand_total_average += avg_for_state;
        
        fprintf(fp, "State %d Average Faults: %.4f\n", state_idx, avg_for_state);
        fflush(fp);
        
    }

    clock_t end_time = clock();
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // calculating Grand Average
    grand_total_average /= NUM_STATES;

    fprintf(fp, "\n=== FINAL RESULTS ===\n");
    fprintf(fp, "Grand Average over 100 states: %.4f faults\n", grand_total_average);
    fprintf(fp, "Total Execution Time: %.2f seconds\n", cpu_time_used);
    fclose(fp);
     
    return 0;

}
