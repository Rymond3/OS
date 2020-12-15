#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>



int scall(char* command){
	pid_t child = fork();
	int status;
	
	if(child == 0)
		execl("/bin/bash", "/bin/bash", "-c", command, NULL);
	else if(child == -1) {
		printf("An error has occurred: Child process %u could  not fork.\n", child);
		exit(-1);
	}
	else
		while(child != wait(&status));
	
	return 0;
}

int main(int argc, char* argv[]) {
	if (argc!=2){
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}
	return system(argv[1]);
}

