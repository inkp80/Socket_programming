//��ǻ���������к� 12121484 ���α�
//��ǻ�� ��Ʈ��ũ 2016 2�б� 1�� ������
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
	int serv_sock, clnt_sock; //����, Ŭ���̾�Ʈ�� ���� ��ũ���͸� ������ ����
	char message[BUF_SIZE]; //������ �޼����� ������ ����
	int str_len, i; //�ش� ������ ���̸� ������ ����

	struct sockaddr_in serv_adr, clnt_adr; //������ Ŭ���̾�Ʈ ������ �ּ������� ���� ����ü
	socklen_t clnt_adr_sz; //Ŭ���̾�Ʈ �ּ��� ũ�⸦ ������ ����

	if (argc != 2) {
		printf("Usage : %s, <port>\n", argv[0]);
		exit(1);
	}
	
	puts("server is on");

	serv_sock = socket(PF_INET, SOCK_STREAM, 0); //���� ����, ���Ͽ� ������, ���۹�� ����, IPv4, TCP, default
	//���ϵ� �����̴�(file decriptor), ������ ����µ� �� ������ �ɼ��� ������ ��.
	if (serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr)); //����ü�� ������ �ֱ� ���� �켱 0���� �ʱ�ȭ
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); //IP�ּҸ� ����, INADDR_ANY�� (�����ϴ� ��ǻ���� IP) 
	//htonl�Լ��� �̿��� ��Ʈ��ũ ����Ʈ(�򿣵��) ����� ���Ѵ�.
	serv_adr.sin_port = htons(atoi(argv[1])); //port ��ȣ�� �����Ѵ�, string�� int������ ��ȯ�Ͽ� ���� ��Ʈ��ũ ����Ʈ ������� ��ȯ�Ѵ�.


	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) //bind()�� �̿��� �ش� FD:serv_sock�� �ռ� ����ü�� �Է��� �ּ� ������ ���̸� �����Ѵ�.
		error_handling("bind() error");

	if (listen(serv_sock, 5) == -1) //listen()�� �̿��� �ش� ������ ����� �� ������ �˸���. ���� �����û ��� ť�� ������ 5�� �����Ͽ���.
		error_handling("listen error"); //�� �ִ� 5���� �����û�� ���������� ó���� �� �ִ�.

	clnt_adr_sz = sizeof(clnt_adr); //���� ��û�� ���� Ŭ���̾�Ʈ�� �ּ� ���̸� �����Ѵ�.

	for (i = 0; i < 5; i++) //���� ��û�� �� 5���� Ŭ���̾�Ʈ�� ó���Ѵ�.
	{
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		//accept�� ����� �ڵ������� ������ �����ȴ�.
		//ť���� ������̴� ���� ��� Ŭ���̾�Ʈ ������ ������ clnt_sock�� �����Ѵ�.
		
		if (clnt_sock == -1)
			error_handling("accept error");
		else
		{//���ῡ ����
			char *IP_Pointer;
			IP_Pointer = inet_ntoa(clnt_adr.sin_addr);
			//string������ ip�� x.x.x.x �������� ��ȯ�Ѵ�
			printf("Conneted client : %s %d\n", IP_Pointer, ntohs(clnt_adr.sin_port), i + 1);
			//��ȯ�� ip�� ����ϰ� port�� network ����Ʈ ������ host����Ʈ ������ �ٲپ� ����Ѵ�
			printf("Conneted client %d \n", i + 1);
			//���� �䱸����
		}

		while ((str_len = read(clnt_sock, message, BUF_SIZE)) != 0) //close, EOF�� ���� ������ ����ؼ� Ŭ���̾�Ʈ�κ��� ����
			write(clnt_sock, message, str_len);						//�����͸� Ŭ���̾�Ʈ�� �����Ѵ�.

		printf("connetion %d is closed\n", i);
		close(clnt_sock); //����Ǿ��ִ� Ŭ���̾�Ʈ ������ close�Ѵ�, ��������
	}
	close(serv_sock); //5���� Ŭ���̾�Ʈ�� ó���� �� ������ �����Ѵ�.
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}