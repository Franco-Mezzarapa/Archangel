#include "ssh_login.h"
#include <iostream>

//ssh_login dependant functions.
void getAddresses(ssh_session_data* sessionData){
   std::string file_location = sessionData->IP_ADDRESS_FILE;
    
    
    std::ifstream ip_file(file_location); // Open the file
    if (!ip_file.is_open()) {
        std::cerr << "Error: Unable to open file." << std::endl;
        return;
    } 
    
    //get ipaddresses from file and store them in a vector.
    std::string line;
    while (std::getline(ip_file, line)) {
        sessionData->numberOfAddresses ++;
        sessionData -> ipAddresses.push_back(line);
    }
    
    ip_file.close();
}
void getExploitName(ssh_session_data* sessionData){
        std::string EXPLOIT_FILE_PATH = sessionData->EXPLOIT_FILE;

        // Convert the full path string to a std::filesystem::path object
            std::filesystem::path fsPath(EXPLOIT_FILE_PATH);

            // Extract the filename (including extension) from the path
            std::string exploitFileName = fsPath.filename().string();
            sessionData->EXPLOIT_NAME = exploitFileName;
}


int main(int argc, char** argv){

 if (argc < 2) {
        showERR();
        return 1;
    }
    
    ssh_session_data sessionData;

    //look for wizard argument.
    if (strcmp(argv[1], "--wizard") == 0) {
        //wizard_logic();
        return 0;
    }else if (strcmp(argv[1], SCP_PROTO) == 0) {
        
        if (argc < 7) {
            std::cerr << "Usage: ./archangel scp <exploit> <dropoff> <ip_addresses_file> <username> <password> <Optional - Specify port>" << std::endl;
            return 1;
        }
        
        // Call getAddresses() function to populate ipAddresses before iterating over it
        sessionData.protocol = SCP_PROTO; //set protocol.
        sessionData.EXPLOIT_FILE = argv[2]; //set filelocation
        sessionData.DROPOFF = argv[3]; //dropoff point
        sessionData.IP_ADDRESS_FILE = argv[4]; //ip file
        sessionData.username = argv[5]; //username 
        sessionData.password = argv[6]; //passwd

        if(argc > 7)
            sessionData.port = std::stoi(argv[7]); //Portdata
        else
        sessionData.port = 22; //portdata


        getAddresses(&sessionData);
        getExploitName(&sessionData);

        std::cout << "struct data: " << std::endl <<
                        sessionData.EXPLOIT_FILE << std::endl <<
                        sessionData.EXPLOIT_NAME << std::endl <<
                        sessionData.DROPOFF << std::endl <<
                        sessionData.IP_ADDRESS_FILE << std::endl <<
                        sessionData.username << std::endl <<
                        sessionData.password << std::endl;

            // Iterate over ipAddresses if it's populated
        for (const auto& ipAddress : sessionData.ipAddresses) {
                std::cout << ipAddress << std::endl;
        }

        SSH_Handler(sessionData);

    }else if (strcmp(argv[1], SSH_PROTO) == 0) {
        if (argc < 6 || argc > 7) {
            std::cerr << "Usage: ./archangel ssh <ip_addresses_file> <username> <password> <commands_to_execute>" << std::endl;
            return 1;
        }
        //gather SSH information.
        sessionData.protocol = SSH_PROTO;
        sessionData.IP_ADDRESS_FILE = argv[2];
        sessionData.username = argv[3];
        sessionData.password = argv[4];
        sessionData.ssh_command_to_send[5];

        getAddresses(&sessionData);
        
        std::cout << "struct data: " << std::endl <<
            sessionData.IP_ADDRESS_FILE << std::endl <<
            sessionData.username << std::endl <<
            sessionData.password << std::endl <<
            sessionData.ssh_command_to_send << std::endl;
        
        for (const auto& ipAddress : sessionData.ipAddresses) {
                std::cout << ipAddress << std::endl;
        }

    }else{
        showERR();
    }
         
    return 0;
}

//non ssh_login.h dependant functions.
void showERR(){
        //usage: ./archangel --wizard
        //usage: ./archangel scp exploit dropoff ip_addresses.txt username password
        //usage: ./archangel ssh ip_addresses.txt username password commands_to_execute
        std::cerr << "Usage: ./archangel --wizard" << std::endl;
        std::cerr << "Usage: ./archangel scp exploit dropoff ip_addresses.txt username password" << std::endl;
        std::cerr << "Usage: ./archangel sh ip_addresses.txt username password commands_to_execute" << std::endl;
}
void printIntro(){


        printf(
            "  AAAAA   RRRRR   CCC  HH  HH   AAAA   NNN   NN  GGGGG  EEEEE  LL   \n"
            " AA   AA  RR  RR CC  C HH  HH  AA  AA  NN NN NN GG     EE      LL   \n"
            "AAAAAAAA  RRRRRR CC    HHHHHH AAAAAAA  NN NN NN GG  GG EEEEE   LL   \n"
            "AA     AA RR  RR CC  C HH  HH AA    AA NN   NNN GG   G EE      LL   \n"
            "AA     AA RR  RR  CCC  HH  HH AA    AA NN   NNN  GGGGG  EEEEE  LLLLL\n\n"
    );
}
