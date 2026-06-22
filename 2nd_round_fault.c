//in this code the fault is injected in the penultimate round and then the fault location is identified


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void print_state(uint8_t s[257]) {
    for (int i = 0; i < 257; i++)
        printf("%u", s[i] & 1);
}

void bit_shuffle(uint8_t s[257], uint8_t s_out[257]) {
    for (int i = 0; i < 257; i++) {
        s_out[i] = s[(121 * i) % 257];
    }
}

void mixing(uint8_t s[257], uint8_t s_out[257]) {
    for (int i = 0; i < 257; i++) {
        s_out[i] = s[i] ^ s[(i + 3) % 257] ^ s[(i + 10) % 257];
    }
}

void add_const(uint8_t s[257]) {
    s[0] ^= 1;
}

void non_linear(uint8_t s[257], uint8_t s_out[257]) {
    for (int i = 0; i < 257; i++) {
        s_out[i] = s[i] ^ s[(i + 2) % 257] ^ (s[(i + 1) % 257] & s[(i + 2) % 257]);
    }
}

void Koala_P(uint8_t s[257], uint8_t s_out[257], int rounds) {
    uint8_t s_bit[257] = {0}, s_mix[257] = {0}, s_temp[257] = {0};

    for (int i = 0; i < 257; i++)
        s_temp[i] = s[i];

    for (int i = 0; i < rounds; i++) {
        bit_shuffle(s_temp, s_bit);
        mixing(s_bit, s_mix);
        if (i != 1 && i != 4 && i != 5)
            add_const(s_mix);
        non_linear(s_mix, s_temp);
    }
    for (int i = 0; i < 257; i++)
        s_out[i] = s_temp[i];
}

void Koala_P_faulty_2round(uint8_t s[257], uint8_t s_out[257], int rounds, int fault_location) {
    uint8_t s_bit[257] = {0}, s_mix[257] = {0}, s_temp[257] = {0};

    for (int i = 0; i < 257; i++)
        s_temp[i] = s[i];

    for (int i = 0; i < rounds; i++) {
        bit_shuffle(s_temp, s_bit);
        if (i == 0) { // Inject fault in the penultimate round (round j-1)
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

int in_D2(int candidate, int idx) {
    int offsets[9] = {0, 1, 2, 3, 4, 5, 10, 11, 12};
    for (int k = 0; k < 9; k++) {
        int l = (candidate - offsets[k] + 257) % 257;
        int l17 = (17 * l) % 257;
        for (int j = 0; j < 9; j++) {
            int pos = (l17 - offsets[j] + 257) % 257;
            if (idx == pos) return 1;
        }
    }
    return 0;
}

int check_B(int candidate, uint8_t s_xor[257]) {
    int x_offsets[3] = {0, 3, 10};
    int b_offsets[3] = {0, 3, 10};
    for (int i = 0; i < 3; i++) {
        int x = (candidate - x_offsets[i] + 257) % 257;
        int x17 = (17 * x) % 257;
        for (int j = 0; j < 3; j++) {
            int pos = (x17 - b_offsets[j] + 257) % 257;
            if (s_xor[pos] == 0) return 0; 
        }
    }
    return 1;
}

void main() {
    uint8_t s_init[257] = {0}, s_out[257] = {0}, s_out_faulty[257] = {0}, s_xor[257] = {0};

    char bits[] = "10111100110101100000101100011110001110101111010010101001000111010101110101010010100000110100001000011000111010001001110101111111111011010110010111000101010101111000101000001011111001101101011111001101001110000000111001010101001100100011100100110000101110010";

    for (int i = 0; i < 257; i++)
    {
        s_init[i] = (bits[i] == '1') ? 1 : 0;
    }

    int NUM_SIMULATIONS = 300000;
    int success_count = 0;

    srand(time(NULL));

    clock_t start_time = clock();

    printf("Starting 2-round fault identification simulation for %d iterations...\n", NUM_SIMULATIONS);

    Koala_P(s_init,s_out,2);

    for (int sim = 1; sim <= NUM_SIMULATIONS; sim++) {
        int r_loc = rand() % 257;

       // Koala_P(s_init, s_out, 2);
        Koala_P_faulty_2round(s_init, s_out_faulty, 2, r_loc);

        for (int i = 0; i < 257; i++) {
            s_xor[i] = s_out[i] ^ s_out_faulty[i];
        }

        // Fault Identification 
        int identified_loc = -1;
        int C[257]={0};
        int c_count = 0;

        // Step 1: Find candidate set C = {i | O subset D2(i)}
        for (int candidate = 0; candidate < 257; candidate++) {
            int is_subset = 1;
            for (int idx = 0; idx < 257; idx++) {
                if (s_xor[idx] == 1) {
                    if (!in_D2(candidate, idx)) {
                        is_subset = 0;
                        break;
                    }
                }
            }
            if (is_subset) {
                C[c_count++] = candidate;
            }
        }

        // Step 2: For each m in C, check if B(m) subset O
        for (int i = 0; i < c_count; i++) {
            int m = C[i];
            if (check_B(m, s_xor)) {
                identified_loc = m;
                break;
            }
        }

        if (identified_loc == r_loc) {
            success_count++;
        }

        if (sim % 5000 == 0) {
            printf("\n--- Debug Info for Simulation %d ---\n",sim);
            printf("1. Actual Fault Location (k): %d\n", r_loc);
            //printf("2. Recovered Fault Location: %d\n", identified_loc);
            
            printf("2. The 81 probable locations D^2(k):\n   ");
            int d2_count = 0;
            for (int p = 0; p < 257; p++) {
                if (in_D2(r_loc, p)) {
                    printf("%d ", p);
                    d2_count++;
                }
            }
            printf("\n   (Total %d locations)\n", d2_count);

            printf("3. Shortlisted Candidates (C):\n   ");
            for(int i=0; i < c_count; i++) {
                printf("%d ", C[i]);
            }
	    printf("\n");
            printf("4. Recovered Fault Location: %d\n", identified_loc);
	    printf("\n-----------------------------------\n\n");
        }

        if (sim % 1000 == 0) {
            printf("Progress: %d / %d simulations completed (Success Rate: %.2f%%)\n", 
                   sim, NUM_SIMULATIONS, (double)success_count / sim * 100.0);
        }
    }

    clock_t end_time = clock();
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("\n=== %d SIMULATIONS COMPLETE ===\n", NUM_SIMULATIONS);
    printf("Successful Identifications: %d\n", success_count);
    printf("Success Rate: %.2f%%\n", (double)success_count / NUM_SIMULATIONS * 100.0);
    printf("Total Execution Time: %f seconds\n", cpu_time_used);
    printf("----------------------------------------------------------------\n");

    //return 0;
}
