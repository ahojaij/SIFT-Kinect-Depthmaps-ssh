#include "ssh_Send.h"

/*************************************
open_session:
*************************************/
sshStruct open_session (std::string Host)
{
	int port = 22;
	int verbosity = SSH_LOG_NOLOG;// SSH_LOG_PROTOCOL;
	const char* HostIP;
	const char* username;
	char* password;

	if (Host == "lab")
	{
		HostIP= "129.97.172.145";
		username = "abdullah";
		password = "starbucks";
	}
	else if (Host == "WAM")
	{
		HostIP= "192.168.58.101";
		username = "robot";
		password = "WAM";
	}

	char buffer[512];
	sshStruct sshStruct_session;

	sshStruct_session.session = connect_ssh(HostIP, username, verbosity);

	if (sshStruct_session.session == NULL)
	{
		sshStruct_session.rc = 1;
		return sshStruct_session;
	}

	sshStruct_session.channel = ssh_channel_new(sshStruct_session.session);
	if (sshStruct_session.channel == NULL)
	{
		ssh_disconnect(sshStruct_session.session);
		return sshStruct_session;
	}
	return sshStruct_session;
}
/*************************************
close_session:
*************************************/
void close_session (sshStruct sshStruct_session)
{
	ssh_channel_send_eof(sshStruct_session.channel);
	ssh_channel_close(sshStruct_session.channel);
	ssh_disconnect(sshStruct_session.session);
}
/*************************************
open_interactive_shell_session:
*************************************/
int open_interactive_shell_session(ssh_channel channel)
{
	int rc;
	rc = ssh_channel_request_pty(channel);
	if (rc != SSH_OK) return rc;
	rc = ssh_channel_change_pty_size(channel, 80, 24);
	if (rc != SSH_OK) return rc;
	rc = ssh_channel_request_shell(channel);
	if (rc != SSH_OK) return rc;
}

int interactive_shell_session(ssh_channel channel)
{
	int rc = 0;
	char buffer[256];
	int nbytes = 256;

	while (ssh_channel_is_open(channel) &&
			!ssh_channel_is_eof(channel))
	{
		nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
		if (nbytes < 0)
		{
			break;
		}
		
		if (nbytes > 0)
		{
			fwrite(buffer, 1, nbytes, stdout);
		}
	}
	nbytes = -2;
	while (ssh_channel_is_open(channel) &&
			!ssh_channel_is_eof(channel))
	{
//		if (nbytes<0 && nbytes < sizeof(buffer))
		if (nbytes<0)
		{
			std::string cmd;
			std::cout << "Give characters: ";
			std::getline(std::cin, cmd);
			cmd+="\r";
			int length = cmd.size();

			if (cmd.substr(0,4) == "quit")
			{
				break;
			}
			else
			{
				rc = ssh_channel_write(channel, cmd.data(), length);
			
	
				if (rc < 0)
				{
					return SSH_ERROR;
				}
				if (rc > 0)
				{
		//			return SSH_ERROR;
				}
			}
		
		}

		nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
//		if (nbytes < 0)
//		{
//			return SSH_ERROR;
//		}
		
		if (nbytes > 0)
		{
			fwrite(buffer, 1, nbytes, stdout);
		}
		
	}
	return rc;
}


int interactive_shell_session_send(ssh_channel channel, std::string cmd)
{
	int rc;
	char buffer[512];
//	cmd+="\r";
	int length = cmd.size();
	rc = ssh_channel_write(channel, cmd.data(), length);
	if (rc < 0)
	{
		return SSH_ERROR;
	}
	return rc;
}


int interactive_shell_session_receive(ssh_channel channel)
{
	int rc = 0;
	char buffer[1024];
	int nbytes = 1024;

//	while (ssh_channel_is_open(channel) && !ssh_channel_is_eof(channel))
	{
		nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
		if (nbytes < 0)
		{
//			return SSH_ERROR;
//			break;
		}
		
		if (nbytes > 0)
		{
			fwrite(buffer, 1, nbytes, stdout);
		}
	}
	return rc;
}
