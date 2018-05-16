#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include "CuTest.h"

char message[] = "1Non exisnant folder";
char message2[] = "2/home/travis";
char message3[] = "3";
char buf[1024];
int sock;


void should_throw_error_msg (CuTest* testContext) {
  memset(buf, 0, sizeof buf);
  
send(sock, message, sizeof(message), 0);
    recv(sock, buf, sizeof(buf), 0);

  CuAssertStrEquals(testContext, "There is no file or directory:\nNon exisnant folder", buf);
}


void should_get_files_number (CuTest* testContext) {
  memset(buf, 0, sizeof buf);
  
send(sock, message2, sizeof(message2), 0);
    recv(sock, buf, sizeof(buf), 0);

  CuAssertStrEquals(testContext, "Files in directory:\n0", buf);
}


void should_get_system_info (CuTest* testContext) {
  memset(buf, 0, sizeof buf);
  
send(sock, message3, sizeof(message3), 0);
    recv(sock, buf, sizeof(buf), 0);

  CuAssertStrEquals(testContext, "Linux", buf);
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


int main()
{
    
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425); // или любой другой порт...
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }

  //  send(sock, message, sizeof(message), 0);
    //recv(sock, buf, sizeof(message), 0);
    
  //  printf(buf);

	all_tests();

    close(sock);


	
    return 0;
}