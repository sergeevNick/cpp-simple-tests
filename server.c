#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <stddef.h>
#include <sys/utsname.h>

char LS = '1';
char FILES_NUMBER = '2';
char OS_INFO = '3';

int sock;
char buf[1024];

const char * getLS(char * path) {
	int length = 0;
	DIR * d;
	struct dirent * dir;
	char * res = (char *)malloc(1024);
	d = opendir(path);

	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (!strcmp(dir->d_name, "."))
				continue;
			if (!strcmp(dir->d_name, ".."))
				continue;
			if (!dir->d_type == DT_REG || !dir->d_type == DT_DIR)
				continue;
			strcat(res, dir->d_name);
			strcat(res, "\n");
		}
		printf("%s\n%s\n", "Sending folder info", path);
	}
	else {
		strcat(res, "There is no file or directory:\n");
		strcat(res, path);
		printf("%s\n%s\n", "There is no file or directory:", path);
	}

	send(sock, res, strlen(res), 0);
	closedir(d);
	return res;
}

void getFilesNumber(char * path) {
	int count = 0;
	char append[5];
	DIR * d;
	struct dirent * dir;
	char * res = (char *)malloc(256);

	d = opendir(path);

	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_type == DT_REG) {
				count++;
				printf("%s\n", dir->d_name);
			}
		}
		strcat(res, "Files in directory:\n");
		strcat(res, path);
		strcat(res, "\n");
		sprintf(append, "%d", count);
		strcat(res, append);
		printf("%s\n%s\n%d\n", "Files in directory:", path, count);
	}
	else {
		strcat(res, "There is no file or directory:\n");
		strcat(res, path);
		printf("%s\n%s\n", "There is no file or directory:", path);
	}

	send(sock, res, strlen(res), 0);
	closedir(d);
}

void getOSInfo() {
	char * res = (char *)malloc(256);
	struct utsname sysinfo;
	uname(&sysinfo);

	strcat(res, "System name:\n");
	strcat(res, sysinfo.sysname);
	strcat(res, "\nSystem version:\n");
	strcat(res, sysinfo.version);
	printf("%s\n%s\n", "System name:", sysinfo.sysname);
	printf("%s\n%s\n", "System version:", sysinfo.version);

	send(sock, res, strlen(res), 0);
}

void manager(char * buf) {
	char task = '0';
	task = buf[0];
	buf++;
	printf("%c\n", task);

	if (task == LS)
		getLS(buf);
	if (task == FILES_NUMBER)
		getFilesNumber(buf);
	if (task == OS_INFO)
		getOSInfo();
}

int main()
{
	int listener;
	struct sockaddr_in addr;
	int bytes_read;

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0)
	{
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3425);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		exit(2);
	}

	listen(listener, 1);

	while (1)
	{
		sock = accept(listener, 0, 0);
		if (sock < 0)
		{
			perror("accept");
			exit(3);
		}

		while (1)
		{
            memset(buf, 0, sizeof buf);
			//buf[0] = 0;
			bytes_read = recv(sock, buf, 1024, 0);
			if (bytes_read <= 0) break;
			manager(buf);
		}

		close(sock);
	}

	return 0;
}
