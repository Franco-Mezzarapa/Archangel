// session_data.h
#ifndef SESSION_DATA_H
#define SESSION_DATA_H
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>

class session_data {
    public:

        std::string IP_ADDRESS_FILE;
        std::string EXPLOIT_FILE;
        std::string EXPLOIT_NAME;
        std::vector<std::string> IP_ADDRESS_VECTOR;
        std::string username;
        std::string password;
        std::string half_command_CLI;
        int number_of_addresses = 0;
        std::string DROPOFF;


        // Function to set SSH credentials (public)
        void ssh_credential_set(std::string username_passed, std::string password_passed) {
            username = username_passed;
            password = password_passed;
        }

            // Function to set SSH credentials (public)
         void set_ipfile(std::string IP_ADDRESS_FILE_PASSED) {
            IP_ADDRESS_FILE = IP_ADDRESS_FILE_PASSED;
            extract_IP(IP_ADDRESS_FILE); //grab ip and put them in the object
        }

        void set_exploitFile(std::string EXPLOIT_FILE_PASSED, std::string DROPOFF_PASSED) {

            // Convert the full path string to a std::filesystem::path object
            std::filesystem::path fsPath(EXPLOIT_FILE_PASSED);
            EXPLOIT_FILE = EXPLOIT_FILE_PASSED;

            // Extract the filename (including extension) from the path
            std::string exploitFileName = fsPath.filename().string();

             std::cout << "Exploit file name: " << exploitFileName << std::endl;
             std::cout << "Dropoff location name: " << DROPOFF_PASSED << std::endl;

             EXPLOIT_NAME = exploitFileName;
             DROPOFF = DROPOFF_PASSED;

        }

        void SCP_sender() {

            for (const auto& ipAddress : IP_ADDRESS_VECTOR) {
                // Construct the SCP command
                std::string scpCommand = "echo \"" + password + "\" | sshpass -p " + password +
                                        " scp " + EXPLOIT_FILE + " " + username + "@" +
                                        ipAddress + ":" + DROPOFF;

                // Execute the command
                std::cout << scpCommand << std::endl;
                int status = std::system(scpCommand.c_str());

                // Check if the command executed successfully
                if (status == 0) {
                    std::cout << "Command sent successfully to IP aSddress: " << ipAddress << std::endl;
                } else {
                    std::cerr << "Error sending command to IP address: " << ipAddress << std::endl;
                }

                std::string sshCommand = "ssh -t " + username + "@" + ipAddress + " \""
                         "cd " + DROPOFF + " && " // Change directory
                         "echo " + password + " && " // Echo the password
                         "sudo ./" + EXPLOIT_NAME + "\""; // Execute the exploit
                
                std::system(sshCommand.c_str());
            }
        }

        void SSH_sender(std::string half_ssh_command){
            
            for (const auto& ipAddress : IP_ADDRESS_VECTOR) {
                std::string ssh_command = "ssh " + username + "@" + ipAddress + " \"" + half_ssh_command + "\"";
                std::system(ssh_command.c_str());
            }
        }
        
    private:
        void extract_IP(std::string IP_FILE_LOCATION){
            //Open file
            std::ifstream ip_file(IP_ADDRESS_FILE);
                if (!ip_file.is_open()) {
                std::cerr << "Error: Unable to open file." << std::endl;
            } 
            //get lines and set in object.
            std::string line;
            while (std::getline(ip_file, line)) {
                number_of_addresses++;
                IP_ADDRESS_VECTOR.push_back(line);
            }
            ip_file.close();
        }

};


#endif // SESSION_DATA_H