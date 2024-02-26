//
// Created by vorri on 2/26/2024.
//
#ifndef ARCHANGEL_MAIN_H
#define ARCHANGEL_MAIN_H
#endif //ARCHANGEL_MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct userdata{
    char* ipAddressFileLocation;
    char* exploitLocation;
    char** ipAddresses;
    char* dropOff;
    char* userName;
    char* password;
    int numIps;
};


void printIntro(){
    printf(
            "  AAAAA   RRRRR   CCC  HH  HH   AAAA   NNN   NN  GGGGG  EEEEE  LL   \n"
            " AA   AA  RR  RR CC  C HH  HH  AA  AA  NN NN NN GG     EE      LL   \n"
            "AAAAAAAA  RRRRRR CC    HHHHHH AAAAAAA  NN NN NN GG  GG EEEEE   LL   \n"
            "AA     AA RR  RR CC  C HH  HH AA    AA NN   NNN GG   G EE      LL   \n"
            "AA     AA RR  RR  CCC  HH  HH AA    AA NN   NNN  GGGGG  EEEEE  LLLLL\n\n"
    );
}
int menuDisplay();
int logicHandler(int userChoice);
void ipAddressExtractor();
void credentialGather();
void setExploitLocation();
void SCP_sender();