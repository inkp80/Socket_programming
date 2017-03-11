//컴퓨터정보공학부 12121484 박인규
//컴퓨터 네트워크 2016 2학기 1차 과제물
//2016. 10. 10

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);


int main(int argc, char *argv[])
{
	int sock; //클라이언트의 fd를 저장할 변수
	char message[BUF_SIZE]; //메세지를 저장할 버퍼
	int str_len; //메세지의 길이를 저장할 변수
	struct sockaddr_in serv_adr; //연결 요청할 서버의 주소를 저장할 구조체

	if (argc != 3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0); //클라이언트 소켓 생성, 소켓에 도메인, 전송방식 설정, IPv4, TCP, default
	if (sock == -1)
		error_handling("socket() error");
	//클라이언트의 경우 bind가 존재하지 않는다.
	//OS, 커널에서 알아서 아이피와 포트번호를 connect() 호출시 할당하기 때문이다.
	memset(&serv_adr, 0, sizeof(serv_adr)); //서버 주소를 저장할 구조체의 초기화
	serv_adr.sin_family = AF_INET; //도메인 설정
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]); //ip설정 *입력값으로
	serv_adr.sin_port = htons(atoi(argv[2])); //port설정 *입력값으로

	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) //해당하는 서버로의 연결을 요청한다
		error_handling("connet() error");
	else //연결 성공
		puts("Connected..................");

	while (1)
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin); //서버에 전달할 메세지를 입력한다

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) //조건 만족시 루프 탈출
			break;
		
		write(sock, message, strlen(message)); //write를 통해 서버로 데이터를 전달한다.
		str_len = read(sock, message, BUF_SIZE - 1); //read를 통해 서버로부터 데이터를 전달받고, 그 반환값(데이터의 길이)를 저장한다

		//TCP는 경계가 존재하지 않는다
		//위의 방식을 취할 경우 문제가 발생할 가능성이 존재한다
		

		
		/* 문제 해결
		int recv_len, recv_cnt;
		while (recv_len < str_len) //내가 앞서 보낸 데이터의 길이만큼 데이터를 받기 전까지 read를 반복
		{//내가 보냈던 데이터를 모두 받기 전까지 read를 반복함으로 다른 것이 끼어들지 못하도록 막는다.
			recv_cnt = read(sock, &message[recv_len], BUF_SIZE - 1);
			if (recv_cnt == -1)
				error_handling("read() error");
			recv_len += recv_cnt;
		}
		*/
		message[str_len] = 0; 
		printf("Message from server %s", message);
	}
	

	
	

	
	close(sock);//클라이언트 소켓의 종료
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}