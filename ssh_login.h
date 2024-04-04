#include "archangel.h"
#include <libssh/libssh.h>
#include <vector>

struct ssh_session_data{ 
        std::string username;
        std::string password;
        std::vector<std::string> ipAddresses;
        std::string IP_ADDRESS_FILE;
        std::string EXPLOIT_FILE;
        std::string EXPLOIT_NAME;
        std::string DROPOFF;
        std::string login_key;
        std::string ssh_command_to_send;
        std::string protocol;
        int numberOfAddresses;
        int port;
};

void SSH_Handler(ssh_session_data sessionData);
void session_connect(ssh_session ssh_session, ssh_session_data sessionData);
int SCP_Handler(ssh_session ssh_session, ssh_session_data sessionData);
int SSH_command(ssh_session ssh_session, ssh_session_data sessionData);