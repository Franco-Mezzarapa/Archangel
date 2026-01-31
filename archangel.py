#!/usr/bin/env python3
"""
Archangel - Remote SSH Command Executor and File Transfer Tool
================================================================
A powerful SSH automation tool for executing commands and transferring files
across multiple hosts with support for password and key-based authentication.

Features:
- Execute commands on single or multiple hosts
- Upload/download files via SFTP
- Execute scripts remotely
- Multi-threaded for concurrent connections
- Password and SSH key authentication
- Support for bulk operations from files

Usage Examples:
    # Execute command on single host
    python3 archangel.py --hosts 192.168.1.100 --users root --passwords toor --command "uname -a"
    
    # Execute on multiple hosts from file
    python3 archangel.py --ips hosts.txt --users users.txt --passwords passwords.txt --command "whoami"
    
    # Upload file with SSH key
    python3 archangel.py --hosts 192.168.1.100 --users admin --keys ~/.ssh/id_rsa --upload /tmp/file.txt:/tmp/file.txt
    
    # Execute script
    python3 archangel.py --hosts 192.168.1.100 --users root --passwords toor --script commands.sh

Author: Security Research Team
"""

import paramiko
import argparse
import os
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
import time
from datetime import datetime
import logging
import warnings
import socket

# Suppress paramiko warnings and verbose logging
warnings.filterwarnings('ignore', category=DeprecationWarning)
logging.getLogger('paramiko').setLevel(logging.CRITICAL)
logging.getLogger('paramiko.transport').setLevel(logging.CRITICAL)

# Colors for terminal output
class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def print_banner():
    banner = f"""{Colors.OKCYAN}
    ╔═══════════════════════════════════════════════════════════╗
    ║              ARCHANGEL - SSH Automation Tool              ║
    ║         Remote Command Execution & File Transfer          ║
    ╚═══════════════════════════════════════════════════════════╝
    {Colors.ENDC}"""
    print(banner)

# Global quiet mode flag
QUIET_MODE = False
VERBOSE_MODE = False

def log_success(message):
    print(f"{Colors.OKGREEN}[+]{Colors.ENDC} {message}")

def log_error(message):
    print(f"{Colors.FAIL}[!]{Colors.ENDC} {message}")

def log_info(message):
    if not QUIET_MODE:
        print(f"{Colors.OKBLUE}[*]{Colors.ENDC} {message}")

def log_warning(message):
    if not QUIET_MODE:
        print(f"{Colors.WARNING}[~]{Colors.ENDC} {message}")

def log_verbose(message):
    if VERBOSE_MODE:
        print(f"{Colors.OKBLUE}[*]{Colors.ENDC} {message}")

def parse_file(filepath):
    """
    Parse a file and return a list of items (IPs, users, passwords, or keys).
    
    Args:
        filepath: Path to the file containing items (one per line)
    
    Returns:
        List of items from the file
    """
    if not filepath:
        return []
    
    if not os.path.exists(filepath):
        log_error(f"File not found: {filepath}")
        return []
    
    try:
        with open(filepath, 'r') as f:
            items = [line.strip() for line in f if line.strip() and not line.startswith('#')]
        log_info(f"Loaded {len(items)} item(s) from {filepath}")
        return items
    except Exception as e:
        log_error(f"Error reading file {filepath}: {e}")
        return []

def parse_credentials(ips=None, users=None, passwords=None, keys=None):
    """
    Parse IP addresses, usernames, passwords, and keys from files or single values.
    Creates all possible combinations for authentication attempts.
    
    Args:
        ips: Single IP or path to file with IPs
        users: Single username or path to file with usernames
        passwords: Single password or path to file with passwords
        keys: Single key path or path to folder with keys
    
    Returns:
        List of dictionaries containing connection credentials
    """
    # Parse IPs
    ip_list = []
    if ips:
        if os.path.isfile(ips):
            ip_list = parse_file(ips)
        else:
            ip_list = [ips]
    
    # Parse users
    user_list = []
    if users:
        if os.path.isfile(users):
            user_list = parse_file(users)
        else:
            user_list = [users]
    
    # Parse passwords
    password_list = []
    if passwords:
        if os.path.isfile(passwords):
            password_list = parse_file(passwords)
        else:
            password_list = [passwords]
    
    # Parse SSH keys
    key_list = []
    if keys:
        if os.path.isdir(keys):
            # Load all keys from directory
            key_dir = Path(keys)
            for key_file in key_dir.iterdir():
                if key_file.is_file():
                    key_list.append(str(key_file))
            log_info(f"Found {len(key_list)} SSH key(s) in directory")
        elif os.path.isfile(keys):
            key_list = [keys]
        else:
            log_warning(f"Key path not found: {keys}")
    
    # Create credential combinations
    credentials = []
    for ip in ip_list:
        for user in user_list:
            # Add password-based auth
            if password_list:
                for password in password_list:
                    credentials.append({
                        'host': ip,
                        'username': user,
                        'password': password,
                        'key': None
                    })
            # Add key-based auth
            if key_list:
                for key in key_list:
                    credentials.append({
                        'host': ip,
                        'username': user,
                        'password': None,
                        'key': key
                    })
            # If no password or key, add user anyway (for interactive)
            if not password_list and not key_list:
                credentials.append({
                    'host': ip,
                    'username': user,
                    'password': None,
                    'key': None
                })
    
    log_info(f"Generated {len(credentials)} credential combination(s)")
    return credentials

def create_ssh_client(host, port=22, username=None, password=None, key=None, timeout=10):
    """
    Create and return an SSH client connection.
    
    Args:
        host: Target host IP/hostname
        port: SSH port (default 22)
        username: SSH username
        password: SSH password (if using password auth)
        key: Path to SSH private key (if using key auth)
        timeout: Connection timeout in seconds
    
    Returns:
        Connected paramiko.SSHClient object or None on failure
    """
    client = paramiko.SSHClient()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    
    # Suppress paramiko exceptions from printing to console
    paramiko.util.log_to_file(os.devnull)
    
    try:
        # Prepare connection parameters
        connect_params = {
            'hostname': host,
            'port': port,
            'username': username,
            'timeout': timeout,
            'allow_agent': False,
            'look_for_keys': False,
            'banner_timeout': 30,  # Increase banner timeout
            'auth_timeout': 30     # Increase auth timeout
        }
        
        # Add authentication method
        if key:
            # Key-based authentication
            try:
                # Try RSA key first
                try:
                    pkey = paramiko.RSAKey.from_private_key_file(key)
                    connect_params['pkey'] = pkey
                    auth_method = f"key: {os.path.basename(key)}"
                except paramiko.SSHException:
                    # Try Ed25519 key
                    try:
                        pkey = paramiko.Ed25519Key.from_private_key_file(key)
                        connect_params['pkey'] = pkey
                        auth_method = f"key: {os.path.basename(key)}"
                    except paramiko.SSHException:
                        # Try DSS key
                        try:
                            pkey = paramiko.DSSKey.from_private_key_file(key)
                            connect_params['pkey'] = pkey
                            auth_method = f"key: {os.path.basename(key)}"
                        except paramiko.SSHException:
                            # Try ECDSA key
                            try:
                                pkey = paramiko.ECDSAKey.from_private_key_file(key)
                                connect_params['pkey'] = pkey
                                auth_method = f"key: {os.path.basename(key)}"
                            except Exception:
                                return None
            except Exception:
                return None
        elif password:
            # Password-based authentication
            connect_params['password'] = password
            auth_method = "password"
        else:
            return None
        
        # Attempt connection with retries
        max_retries = 2
        for attempt in range(max_retries):
            try:
                client.connect(**connect_params)
                log_success(f"Connected to {host}:{port} as {username} ({auth_method})")
                return client
            except (paramiko.SSHException, EOFError) as e:
                # On SSH protocol errors, retry once
                if attempt < max_retries - 1:
                    time.sleep(0.5)  # Brief delay before retry
                    continue
                else:
                    # Final failure - return None silently
                    return None
            except Exception:
                return None
        
        return None
        
    except paramiko.AuthenticationException:
        # Silently fail on auth errors
        return None
    except paramiko.SSHException:
        # Silently fail on SSH errors
        return None
    except socket.timeout:
        # Silently fail on timeout
        return None
    except socket.error:
        # Silently fail on socket errors
        return None
    except Exception:
        # Catch-all for any other exceptions
        return None

def execute_command(host, port=22, username=None, password=None, key=None, command=None, timeout=30):
    """
    Execute a command on a remote host via SSH.
    
    Args:
        host: Target host IP/hostname
        port: SSH port
        username: SSH username
        password: SSH password
        key: Path to SSH private key
        command: Command to execute
        timeout: Command execution timeout
    
    Returns:
        Dictionary with results or None on failure
    """
    if not command:
        return None
    
    client = create_ssh_client(host, port, username, password, key)
    if not client:
        return None
    
    try:
        log_info(f"Executing command on {host}: {command}")
        stdin, stdout, stderr = client.exec_command(command, timeout=timeout)
        
        output = stdout.read().decode('utf-8', errors='ignore')
        error = stderr.read().decode('utf-8', errors='ignore')
        exit_code = stdout.channel.recv_exit_status()
        
        result = {
            'host': host,
            'command': command,
            'output': output,
            'error': error,
            'exit_code': exit_code,
            'success': exit_code == 0
        }
        
        if result['success']:
            log_success(f"Command completed on {host} (exit code: {exit_code})")
            if output:
                print(f"{Colors.OKCYAN}Output from {host}:{Colors.ENDC}\n{output}")
        else:
            log_warning(f"Command failed on {host} (exit code: {exit_code})")
            if error:
                print(f"{Colors.WARNING}Error from {host}:{Colors.ENDC}\n{error}")
        
        return result
        
    except socket.timeout:
        log_error(f"Command timeout on {host}")
        return None
    except Exception as e:
        # Silently catch other exceptions
        return None
    finally:
        try:
            client.close()
        except:
            pass

def upload_file(host, port=22, username=None, password=None, key=None, local_path=None, remote_path=None):
    """
    Upload a file to a remote host via SFTP.
    
    Args:
        host: Target host IP/hostname
        port: SSH port
        username: SSH username
        password: SSH password
        key: Path to SSH private key
        local_path: Local file path
        remote_path: Remote destination path
    
    Returns:
        True on success, False on failure
    """
    if not local_path or not remote_path:
        return False
    
    if not os.path.exists(local_path):
        log_error(f"Local file not found: {local_path}")
        return False
    
    client = create_ssh_client(host, port, username, password, key)
    if not client:
        return False
    
    try:
        sftp = client.open_sftp()
        log_info(f"Uploading {local_path} to {host}:{remote_path}")
        
        # Get file size for progress
        file_size = os.path.getsize(local_path)
        
        sftp.put(local_path, remote_path)
        log_success(f"Uploaded {local_path} to {host}:{remote_path} ({file_size} bytes)")
        
        sftp.close()
        return True
        
    except Exception:
        # Silently fail
        return False
    finally:
        try:
            client.close()
        except:
            pass

def download_file(host, port=22, username=None, password=None, key=None, remote_path=None, local_path=None):
    """
    Download a file from a remote host via SFTP.
    
    Args:
        host: Target host IP/hostname
        port: SSH port
        username: SSH username
        password: SSH password
        key: Path to SSH private key
        remote_path: Remote file path
        local_path: Local destination path
    
    Returns:
        True on success, False on failure
    """
    if not local_path or not remote_path:
        return False
    
    client = create_ssh_client(host, port, username, password, key)
    if not client:
        return False
    
    try:
        sftp = client.open_sftp()
        log_info(f"Downloading {host}:{remote_path} to {local_path}")
        
        sftp.get(remote_path, local_path)
        
        file_size = os.path.getsize(local_path)
        log_success(f"Downloaded {host}:{remote_path} to {local_path} ({file_size} bytes)")
        
        sftp.close()
        return True
        
    except Exception:
        # Silently fail
        return False
    finally:
        try:
            client.close()
        except:
            pass

def execute_script(host, port=22, username=None, password=None, key=None, script_path=None):
    """
    Execute a local script on a remote host.
    Uploads the script and executes it.
    
    Args:
        host: Target host IP/hostname
        port: SSH port
        username: SSH username
        password: SSH password
        key: Path to SSH private key
        script_path: Path to local script file
    
    Returns:
        Dictionary with results or None on failure
    """
    if not script_path or not os.path.exists(script_path):
        return None
    
    client = create_ssh_client(host, port, username, password, key)
    if not client:
        return None
    
    try:
        # Read script content
        with open(script_path, 'r') as f:
            script_content = f.read()
        
        log_info(f"Executing script on {host}: {script_path}")
        
        # Upload and execute script
        remote_script = f"/tmp/archangel_script_{int(time.time())}.sh"
        
        sftp = client.open_sftp()
        with sftp.open(remote_script, 'w') as f:
            f.write(script_content)
        sftp.chmod(remote_script, 0o755)
        sftp.close()
        
        # Execute the script
        stdin, stdout, stderr = client.exec_command(f"bash {remote_script}; rm {remote_script}")
        
        output = stdout.read().decode('utf-8', errors='ignore')
        error = stderr.read().decode('utf-8', errors='ignore')
        exit_code = stdout.channel.recv_exit_status()
        
        result = {
            'host': host,
            'script': script_path,
            'output': output,
            'error': error,
            'exit_code': exit_code,
            'success': exit_code == 0
        }
        
        if result['success']:
            log_success(f"Script completed on {host} (exit code: {exit_code})")
            if output:
                print(f"{Colors.OKCYAN}Output from {host}:{Colors.ENDC}\n{output}")
        else:
            log_warning(f"Script failed on {host} (exit code: {exit_code})")
            if error:
                print(f"{Colors.WARNING}Error from {host}:{Colors.ENDC}\n{error}")
        
        return result
        
    except Exception:
        # Silently fail
        return None
    finally:
        try:
            client.close()
        except:
            pass

def worker_task(cred, args):
    """
    Worker function for thread pool execution.
    
    Args:
        cred: Credential dictionary
        args: Parsed command-line arguments
    
    Returns:
        Result dictionary
    """
    host = cred['host']
    username = cred['username']
    password = cred['password']
    key = cred['key']
    port = args.port
    
    result = {
        'host': host,
        'username': username,
        'success': False
    }
    
    try:
        # Execute command
        if args.command:
            cmd_result = execute_command(host, port, username, password, key, args.command)
            if cmd_result and cmd_result['success']:
                result['success'] = True
                result['command_result'] = cmd_result
        
        # Upload file
        if args.upload:
            if ':' in args.upload:
                local_path, remote_path = args.upload.split(':', 1)
            else:
                local_path = args.upload
                remote_path = f"/tmp/{os.path.basename(local_path)}"
            
            if upload_file(host, port, username, password, key, local_path, remote_path):
                result['success'] = True
                result['upload'] = f"{local_path} -> {remote_path}"
        
        # Download file
        if args.download:
            if ':' in args.download:
                remote_path, local_path = args.download.split(':', 1)
            else:
                remote_path = args.download
                local_path = os.path.basename(remote_path)
            
            # Add host to local filename to avoid conflicts
            local_path = f"{host}_{local_path}"
            
            if download_file(host, port, username, password, key, remote_path, local_path):
                result['success'] = True
                result['download'] = f"{remote_path} -> {local_path}"
        
        # Execute script
        if args.script:
            script_result = execute_script(host, port, username, password, key, args.script)
            if script_result and script_result['success']:
                result['success'] = True
                result['script_result'] = script_result
        
    except KeyboardInterrupt:
        raise
    except Exception:
        # Silently catch all exceptions in worker
        pass
    
    return result

def main(args):
    """
    Main execution function.
    
    Args:
        args: Parsed command-line arguments
    """
    # Parse credentials
    credentials = parse_credentials(
        ips=args.ips or args.hosts,
        users=args.users,
        passwords=args.passwords,
        keys=args.keys
    )
    
    if not credentials:
        log_error("No valid credentials found. Please specify hosts, users, and authentication methods.")
        return
    
    log_info(f"Starting operations with {args.threads} thread(s)")
    
    # Execute tasks using thread pool
    results = []
    with ThreadPoolExecutor(max_workers=args.threads) as executor:
        futures = {executor.submit(worker_task, cred, args): cred for cred in credentials}
        
        for future in as_completed(futures):
            cred = futures[future]
            try:
                result = future.result(timeout=120)  # 2 minute timeout per task
                results.append(result)
            except KeyboardInterrupt:
                log_warning("Interrupted by user, stopping...")
                executor.shutdown(wait=False, cancel_futures=True)
                raise
            except Exception:
                # Silently skip failed tasks
                pass
    
    # Print summary
    print(f"\n{Colors.HEADER}{'='*60}{Colors.ENDC}")
    print(f"{Colors.HEADER}EXECUTION SUMMARY{Colors.ENDC}")
    print(f"{Colors.HEADER}{'='*60}{Colors.ENDC}")
    
    successful = sum(1 for r in results if r.get('success'))
    failed = len(results) - successful
    
    log_info(f"Total tasks: {len(results)}")
    log_success(f"Successful: {successful}")
    if failed > 0:
        log_warning(f"Failed: {failed}")
    
    # Print successful connections
    if successful > 0:
        print(f"\n{Colors.OKGREEN}Successful Connections:{Colors.ENDC}")
        for r in results:
            if r.get('success'):
                print(f"  {Colors.OKGREEN}✓{Colors.ENDC} {r['host']} ({r['username']})")

if __name__ == "__main__":
    print_banner()
    
    parser = argparse.ArgumentParser(
        description="Archangel - Remote SSH Command Executor and File Transfer Tool",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Execute command on single host
  %(prog)s --hosts 192.168.1.100 --users root --passwords toor --command "uname -a"
  
  # Execute on multiple hosts from file
  %(prog)s --ips hosts.txt --users users.txt --passwords passwords.txt --command "whoami"
  
  # Upload file with SSH key
  %(prog)s --hosts 192.168.1.100 --users admin --keys ~/.ssh/id_rsa --upload /tmp/file.txt:/tmp/file.txt
  
  # Download file
  %(prog)s --hosts 192.168.1.100 --users root --passwords toor --download /etc/passwd:passwd.txt
  
  # Execute script
  %(prog)s --hosts 192.168.1.100 --users root --passwords toor --script commands.sh
  
  # Spray credentials across multiple hosts
  %(prog)s --ips hosts.txt --users users.txt --passwords passwords.txt --command "id"
        """
    )
    
    parser.add_argument("--hosts", help="Single target IP or hostname")
    parser.add_argument("--ips", help="File containing IP addresses (one per line)")
    parser.add_argument("--users", help="Username or file containing usernames")
    parser.add_argument("--passwords", help="Password or file containing passwords")
    parser.add_argument("--keys", help="Path to SSH private key or directory containing keys")
    parser.add_argument("--command", help="Command to execute on target machine(s)")
    parser.add_argument("--upload", help="Upload file (format: local_path:remote_path)")
    parser.add_argument("--download", help="Download file (format: remote_path:local_path)")
    parser.add_argument("--port", type=int, default=22, help="SSH port (default: 22)")
    parser.add_argument("--threads", type=int, default=4, help="Number of concurrent threads (default: 4)")
    parser.add_argument("--script", help="Path to script file to execute on remote host(s)")
    parser.add_argument("--quiet", "-q", action="store_true", help="Quiet mode - only show successes and errors")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose mode - show all connection attempts")
    
    args = parser.parse_args()
    
    # Set global flags FIRST before any function calls
    QUIET_MODE = args.quiet
    VERBOSE_MODE = args.verbose
    
    # Validate arguments
    if not (args.hosts or args.ips):
        log_error("Error: Must specify either --hosts or --ips")
        parser.print_help()
        sys.exit(1)
    
    if not args.users:
        log_error("Error: Must specify --users")
        parser.print_help()
        sys.exit(1)
    
    if not (args.passwords or args.keys):
        log_error("Error: Must specify either --passwords or --keys for authentication")
        parser.print_help()
        sys.exit(1)
    
    if not (args.command or args.upload or args.download or args.script):
        log_error("Error: Must specify an action (--command, --upload, --download, or --script)")
        parser.print_help()
        sys.exit(1)
    
    try:
        main(args)
    except KeyboardInterrupt:
        print(f"\n{Colors.WARNING}[!] Interrupted by user{Colors.ENDC}")
        sys.exit(0)
    except Exception as e:
        log_error(f"Fatal error: {e}")
        sys.exit(1)