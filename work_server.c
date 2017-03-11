//컴퓨터정보공학부 12121484 박인규
//컴퓨터 네트워크 2016 2학기 1차 과제물
//2016. 10. 10

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>

#define BUF_SIZE 1024

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock; //서버, 클라이언트의 파일 디스크립터를 저장할 변수
	char message[BUF_SIZE]; //들어오는 메세지를 저장할 버퍼
	int str_len, i; //해당 버퍼의 길이를 저장할 변수

	struct sockaddr_in serv_adr, clnt_adr; //서버와 클라이언트 소켓의 주소정보를 담을 구조체
	socklen_t clnt_adr_sz; //클라이언트 주소의 크기를 저장할 변수

	if (argc != 2) {
		printf("Usage : %s, <port>\n", argv[0]);
		exit(1);
	}
	
	puts("server is on");

	serv_sock = socket(PF_INET, SOCK_STREAM, 0); //소켓 생성, 소켓에 도메인, 전송방식 설정, IPv4, TCP, default
	//소켓도 파일이다(file decriptor), 파일을 만드는데 그 파일의 옵션을 선택한 것.
	if (serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr)); //구조체에 정보를 넣기 전에 우선 0으로 초기화
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); //IP주소를 대입, INADDR_ANY로 (동작하는 컴퓨터의 IP) 
	//htonl함수를 이용해 네트워크 바이트(빅엔디안) 방식을 취한다.
	serv_adr.sin_port = htons(atoi(argv[1])); //port 번호를 대입한다, string을 int형으로 변환하여 역시 네트워크 바이트 방식으로 변환한다.


	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) //bind()를 이용해 해당 FD:serv_sock에 앞서 구조체에 입력한 주소 정보와 길이를 전달한다.
		error_handling("bind() error");

	if (listen(serv_sock, 5) == -1) //listen()을 이용해 해당 서버가 연결될 수 있음을 알린다. 또한 연결요청 대기 큐를 사이즈 5로 선언하였다.
		error_handling("listen error"); //즉 최대 5개의 연결요청을 순차적으로 처리할 수 있다.

	clnt_adr_sz = sizeof(clnt_adr); //연결 요청이 들어온 클라이언트의 주소 길이를 저장한다.

	for (i = 0; i < 5; i++) //연결 요청해 온 5개의 클라이언트를 처리한다.
	{
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		//accept의 결과로 자동적으로 소켓이 생성된다.
		//큐에서 대기중이던 연결 대기 클라이언트 소켓의 정보를 clnt_sock에 저장한다.
		
		if (clnt_sock == -1)
			error_handling("accept error");
		else
		{//연결에 성공
			char *IP_Pointer;
			IP_Pointer = inet_ntoa(clnt_adr.sin_addr);
			//string형으로 ip를 x.x.x.x 포멧으로 변환한다
			printf("Conneted client : %s %d\n", IP_Pointer, ntohs(clnt_adr.sin_port), i + 1);
			//변환된 ip를 출력하고 port의 network 바이트 순서를 host바이트 순서로 바꾸어 출력한다
			printf("Conneted client %d \n", i + 1);
			//과제 요구사항
		}

		while ((str_len = read(clnt_sock, message, BUF_SIZE)) != 0) //close, EOF가 오기 전까지 계속해서 클라이언트로부터 읽은
			write(clnt_sock, message, str_len);						//데이터를 클라이언트로 전송한다.

		printf("connetion %d is closed\n", i);
		close(clnt_sock); //연결되어있는 클라이언트 소켓을 close한다, 연결종료
	}
	close(serv_sock); //5개의 클라이언트를 처리한 후 서버를 종료한다.
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}