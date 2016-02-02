#include <iostream>
#include <string>
#include <stdio.h>
#include <libssh/libssh.h>
#include "examples_common.h"

struct sshStruct
{
	int rc;
	ssh_session session;
	ssh_channel channel;
};

int interactive_shell_session(ssh_channel);
int interactive_shell_session_send(ssh_channel, std::string);
int interactive_shell_session_receive(ssh_channel);
sshStruct open_session (std::string);
void close_session (sshStruct);
int open_interactive_shell_session(ssh_channel);