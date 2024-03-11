#include <stdio.h>
#include <sstream>
#include <limits>
#include "session_data.h"
#include <string.h>

void printIntro(){

        printf(
            "  AAAAA   RRRRR   CCC  HH  HH   AAAA   NNN   NN  GGGGG  EEEEE  LL   \n"
            " AA   AA  RR  RR CC  C HH  HH  AA  AA  NN NN NN GG     EE      LL   \n"
            "AAAAAAAA  RRRRRR CC    HHHHHH AAAAAAA  NN NN NN GG  GG EEEEE   LL   \n"
            "AA     AA RR  RR CC  C HH  HH AA    AA NN   NNN GG   G EE      LL   \n"
            "AA     AA RR  RR  CCC  HH  HH AA    AA NN   NNN  GGGGG  EEEEE  LLLLL\n\n"
    );


        printf(
            "\n1) Launch a command across multiple computers\n"
            "2) Upload a file across multiple computers.\n"
            "3) Set IP file location.\n"
            "4) Set SSH credentials\n"
            "5) Set Exploit file locations - Sets exploit location and Dropoff.\n"
            "6) Exit.\n"
    );
}

void printIntro_CLI(){
            printf(
            "  AAAAA   RRRRR   CCC  HH  HH   AAAA   NNN   NN  GGGGG  EEEEE  LL   \n"
            " AA   AA  RR  RR CC  C HH  HH  AA  AA  NN NN NN GG     EE      LL   \n"
            "AAAAAAAA  RRRRRR CC    HHHHHH AAAAAAA  NN NN NN GG  GG EEEEE   LL   \n"
            "AA     AA RR  RR CC  C HH  HH AA    AA NN   NNN GG   G EE      LL   \n"
            "AA     AA RR  RR  CCC  HH  HH AA    AA NN   NNN  GGGGG  EEEEE  LLLLL\n\n"
    );
}
void wizard_logic();