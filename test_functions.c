#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <stddef.h>
#include <sys/utsname.h>

char LS = '1';
char FILES_NUMBER = '2';
char OS_INFO = '3';

char message[] = "1Non exisnant folder";
char message2[] = "2/home/travis";
char message3[] = "3";

int sock;
char buf[1024];


/*------------------------------------------------------------------------
-------------------------------MOCK FUNCTIONS---------------------------*/

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
	printf("%s\n%s\n", "System name:", sysinfo.sysname);


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

/*----------------------------------------------------------------------
-----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
-------------------------------TESTS------------------------------------*/

void should_throw_error_msg (CuTest* testContext) {
  memset(buf, 0, sizeof buf);
  
buf = manager(message);

  CuAssertStrEquals(testContext, "There is no file or directory:\nNon exisnant folder", buf);
}


void should_get_files_number (CuTest* testContext) {
  memset(buf, 0, sizeof buf);
  
buf = manager(message2);

  CuAssertStrEquals(testContext, "Files in directory:\n/home/travis\n18", buf);
}


void should_get_system_info (CuTest* testContext) {
  memset(buf, 0, sizeof buf);
  

buf = manager(message3);

  CuAssertStrEquals(testContext, "System name:\nLinux", buf);
}

CuSuite* test_suite() {
  CuSuite* suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, should_throw_error_msg);
    SUITE_ADD_TEST(suite, should_get_files_number);
    SUITE_ADD_TEST(suite, should_get_system_info);
  return suite;
}


void all_tests()
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, test_suite());
	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}

/*----------------------------------------------------------------------
------------------------------------------------------------------------*/



int main()
{
	all_tests();
    return 0;
}
