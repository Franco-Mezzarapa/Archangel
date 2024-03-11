#include "archangel.h"

session_data current_session = current_session;

int main(int argc, char** argv){

 if (argc < 2) {
        //usage: ./archangel --wizard
        //usage: ./archangel scp exploit dropoff ip_addresses.txt username password
        //usage: ./archangel ssh ip_addresses.txt username password commands_to_execute
        std::cerr << "Usage: ./archangel --wizard" << std::endl;
        std::cerr << "Usage: ./archangel scp exploit dropoff ip_addresses.txt username password" << std::endl;
        std::cerr << "Usage: ./archangel sh ip_addresses.txt username password commands_to_execute" << std::endl;
        return 1;
    }

    if (strcmp(argv[1], "--wizard") == 0) {
        wizard_logic();
    }
    else if (strcmp(argv[1], "scp") == 0) {
         
	if (argc < 7) {
            std::cerr << "Usage: ./archangel scp <exploit> <dropoff> <ip_addresses_file> <username> <password>" << std::endl;
            return 1;
        }
        printIntro_CLI();
        current_session.set_exploitFile(argv[2], argv[3]);
        current_session.set_ipfile(argv[4]);
        current_session.ssh_credential_set(argv[5],argv[6]);
        
        current_session.SCP_sender();
    }
    else if (strcmp(argv[1], "ssh") == 0) {
        printIntro_CLI();
        if (argc < 6) {
            std::cerr << "Usage: ./archangel ssh <ip_addresses_file> <username> <password> <commands_to_execute>" << std::endl;
            return 1;
        }
        current_session.set_ipfile(argv[2]);
        current_session.ssh_credential_set(argv[3],argv[4]);
        current_session.half_command_CLI = argv[5];
        
        current_session.SSH_sender(current_session.half_command_CLI);
    }
    
    return 0;
}

void wizard_logic(){
        int number;
        std::string input;
        std::string EXPLOIT_FILE;
        std::string DROPOFF;
        std::string username;
        std::string password;
        std::string IP_ADDRESS_FILE;

    do {
        std::string ssh_command;
        printIntro();
        std::cout << "Archangel> ";
        std::getline(std::cin, input);
        std::istringstream isString(input);

        if (!(isString >> number)) 
        std::cout << "Invalid input: not an integer" << std::endl;

        switch (number) {
            case 1:
                std::cout << "Enter SSH command: ";
                std::getline(std::cin >> std::ws, ssh_command); // Read the entire line including whitespace
                // Debugging output
                std::cout << "SSH command entered: " << ssh_command << std::endl;

                current_session.SSH_sender(ssh_command);
                break;
            
            case 2:
                current_session.SCP_sender();
                break;

            case 3:
                std::cout << "Enter IP file path: ";
                std::cin >> IP_ADDRESS_FILE;
                current_session.set_ipfile(IP_ADDRESS_FILE);
                break;

            case 4:
                //set creds
                std::cout << "SSH Username: ";
                std::cin >> username;
                std::cout << "SSH Password: ";
                std::cin >> password;
                current_session.ssh_credential_set(username,password);
                break;

            case 5:
                std::cout << "Enter Exploit file path: ";
                std::cin >> EXPLOIT_FILE;

                std::cout << "Enter Dropoff file path: ";
                std::cin >> DROPOFF;
                current_session.set_exploitFile(EXPLOIT_FILE,DROPOFF);
                break;
                
            default:
                // Code for handling invalid input
                break;
        }
            
        std::cin.clear(); // Clear error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input


    } while (number != 6);

}
