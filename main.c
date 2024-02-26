#include "main.h"

struct userdata programData;
int main() {

        int userInput = 0;
        printIntro();

        //Main execution thread.
        while(userInput != 6) {
            userInput = menuDisplay();
            logicHandler(userInput);

            //Reset input var.
            userInput = 0;
        }
        free(programData.ipAddressFileLocation);
        free(programData.dropOff);
        free(programData.ipAddresses);
        free(programData.exploitLocation);

}

int menuDisplay(){
    printf(
            "\n1) Launch a command across multiple computers\n"
            "2) Upload a file across multiple computers.\n"
            "3) Set IP file location.\n"
            "4) Set SSH credentials\n"
            "5) Set Exploit file locations - Sets exploit location and Dropoff.\n"
            "6) Exit.\n"
    );

    int userInput = 0;
    printf("Archangel:  ");
    scanf(" %d", &userInput);
    return userInput;
}
int logicHandler(int userChoice){
    switch (userChoice) {
        case(1):
            break;
        case(2):
            SCP_sender();
            break;
        case(3):
            ipAddressExtractor();
            break;
        case(4):
            break;
        case(5):
            setExploitLocation();
            break;
    }

}
void SCP_sender(){
        for (int i = 0; i < programData.numIps; ++i) {
            // Calculate the length of the SCP command
            int commandLength = snprintf(NULL, 0, "echo \"%s\" | sshpass -p %s scp %s %s@%s:%s",
                                         programData.password, programData.password,
                                         programData.exploitLocation, programData.userName,
                                         programData.ipAddresses[i], programData.dropOff);
            // Allocate memory for the SCP command
            char* shpassCommand = malloc(commandLength + 1); // Add 1 for the null terminator
            if (shpassCommand == NULL) {
                printf("Memory allocation failed.\n");
                exit(1);
            }
            // Construct the SCP command
            snprintf(shpassCommand, commandLength + 1, "echo \"%s\" | sshpass -p %s scp %s %s@%s:%s",
                     programData.password, programData.password,
                     programData.exploitLocation, programData.userName,
                     programData.ipAddresses[i], programData.dropOff);

            // Execute the command
            int status = system(shpassCommand);

            // Check if the command executed successfully
            if (status == 0) {
                printf("Command sent successfully to IP address: %s\n", programData.ipAddresses[i]);
            } else {
                printf("Error sending command to IP address: %s\n", programData.ipAddresses[i]);
            }

            // Free dynamically allocated memory
            free(shpassCommand);
        }
}
void credentialGather(){
    //Allocate memory for exploitLocation and then grab user location. Prone to overflow if the user is inting.
    programData.userName = malloc(1024 * sizeof(char));
    printf("Please enter the SSH username: ");
    if (scanf("%s", programData.userName) != 1) {
        printf("Error reading input.\n");
        free(programData.userName); // Free memory if allocation failed
        exit(1);
    }

    programData.password = malloc(1024 * sizeof(char));
    printf("Please enter the SSH password: ");
    if (scanf("%s", programData.password) != 1) {
        printf("Error reading input.\n");
        free(programData.password); // Free memory if allocation failed
        exit(1);
    }

}
void ipAddressExtractor(){
    // Allocate memory for file location in struct
    programData.ipAddressFileLocation = malloc(1024 * sizeof(char));
    if (programData.ipAddressFileLocation == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    printf("Please enter the location of the ip-address-file: ");
    if (scanf("%s", programData.ipAddressFileLocation) != 1) {
        printf("Error reading input.\n");
        free(programData.ipAddressFileLocation); // Free memory if allocation failed
        exit(1);
    }

    // Open the IP address file
    FILE* ipAddressFile = fopen(programData.ipAddressFileLocation, "r");
    if (ipAddressFile == NULL) {
        printf("Error opening file %s.\n", programData.ipAddressFileLocation);
        free(programData.ipAddressFileLocation); // Free memory for file location
        exit(1);
    }

    // Read IP addresses from the file and store them in ipAddresses array
    char buffer[1024];
    int numIPs = 0;
    programData.ipAddresses = malloc(100 * sizeof(char*)); // Allocating memory for an array of 100 IP addresses
    if (programData.ipAddresses == NULL) {
        printf("Memory allocation failed.\n");
        fclose(ipAddressFile); // Close the file before exiting
        free(programData.ipAddressFileLocation); // Free memory for file location
        exit(1);
    }

    // Read IP addresses until end of file is reached
    while (fgets(buffer, sizeof(buffer), ipAddressFile) != NULL) {
        // Remove trailing newline character, if any
        char *newline = strchr(buffer, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }
        // Allocate memory for the IP address and copy it
        programData.ipAddresses[numIPs] = malloc(strlen(buffer) + 1);
        if (programData.ipAddresses[numIPs] == NULL) {
            printf("Memory allocation failed.\n");
            fclose(ipAddressFile); // Close the file before exiting
            free(programData.ipAddressFileLocation); // Free memory for file location
            // Free memory for previously allocated IP addresses
            for (int i = 0; i < numIPs; i++) {
                free(programData.ipAddresses[i]);
            }
            free(programData.ipAddresses); // Free memory for ipAddresses array
            exit(1);
        }
        strcpy(programData.ipAddresses[numIPs], buffer);
        numIPs++;
        programData.numIps = numIPs;
    }

    // Close the file
    fclose(ipAddressFile);
    printf("File Location: %s\n",programData.ipAddressFileLocation);
}
void setExploitLocation(){
    //Allocate memory for exploitLocation and then grab user location. Prone to overflow if the user is inting.
    programData.exploitLocation = malloc(1024 * sizeof(char));
    printf("Please enter the location of the exploit: ");
    if (scanf("%s", programData.exploitLocation) != 1) {
        printf("Error reading input.\n");
        free(programData.exploitLocation); // Free memory if allocation failed
        exit(1);
    }

    programData.dropOff = malloc(1024 * sizeof(char));
    printf("Please enter the DropoffLocation: ");
    if (scanf("%s", programData.dropOff) != 1) {
        printf("Error reading input.\n");
        free(programData.dropOff); // Free memory if allocation failed
        exit(1);
    }

}