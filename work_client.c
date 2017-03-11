//��ǻ���������к� 12121484 ���α�
//��ǻ�� ��Ʈ��ũ 2016 2�б� 1�� ������
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
	int sock; //Ŭ���̾�Ʈ�� fd�� ������ ����
	char message[BUF_SIZE]; //�޼����� ������ ����
	int str_len; //�޼����� ���̸� ������ ����
	struct sockaddr_in serv_adr; //���� ��û�� ������ �ּҸ� ������ ����ü

	if (argc != 3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0); //Ŭ���̾�Ʈ ���� ����, ���Ͽ� ������, ���۹�� ����, IPv4, TCP, default
	if (sock == -1)
		error_handling("socket() error");
	//Ŭ���̾�Ʈ�� ��� bind�� �������� �ʴ´�.
	//OS, Ŀ�ο��� �˾Ƽ� �����ǿ� ��Ʈ��ȣ�� connect() ȣ��� �Ҵ��ϱ� �����̴�.
	memset(&serv_adr, 0, sizeof(serv_adr)); //���� �ּҸ� ������ ����ü�� �ʱ�ȭ
	serv_adr.sin_family = AF_INET; //������ ����
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]); //ip���� *�Է°�����
	serv_adr.sin_port = htons(atoi(argv[2])); //port���� *�Է°�����

	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) //�ش��ϴ� �������� ������ ��û�Ѵ�
		error_handling("connet() error");
	else //���� ����
		puts("Connected..................");

	while (1)
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin); //������ ������ �޼����� �Է��Ѵ�

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) //���� ������ ���� Ż��
			break;
		
		write(sock, message, strlen(message)); //write�� ���� ������ �����͸� �����Ѵ�.
		str_len = read(sock, message, BUF_SIZE - 1); //read�� ���� �����κ��� �����͸� ���޹ް�, �� ��ȯ��(�������� ����)�� �����Ѵ�

		//TCP�� ��谡 �������� �ʴ´�
		//���� ����� ���� ��� ������ �߻��� ���ɼ��� �����Ѵ�
		

		
		/* ���� �ذ�
		int recv_len, recv_cnt;
		while (recv_len < str_len) //���� �ռ� ���� �������� ���̸�ŭ �����͸� �ޱ� ������ read�� �ݺ�
		{//���� ���´� �����͸� ��� �ޱ� ������ read�� �ݺ������� �ٸ� ���� ������� ���ϵ��� ���´�.
			recv_cnt = read(sock, &message[recv_len], BUF_SIZE - 1);
			if (recv_cnt == -1)
				error_handling("read() error");
			recv_len += recv_cnt;
		}
		*/
		message[str_len] = 0; 
		printf("Message from server %s", message);
	}
	

	
	

	
	close(sock);//Ŭ���̾�Ʈ ������ ����
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}