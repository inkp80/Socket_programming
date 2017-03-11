#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h> //멀티플렉싱 -> select

#define BUF_SIZE 100
void error_handling(char *buf);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout; //타이머를 위한 구조체
    //timeout -> tv_usec, sec 두 종류의 시간 세팅 가능
    fd_set reads, cpy_reads; //파일 디스크립터의 상태를 나타낼 구조체 reads와, 이 것을 임시로 저장할 cp_reads
    //cpy_reads는 해당 fd를 갖는 클라이언트 소켓이 실제로 연결을 유지하는지를 나타내는 용도로 사용된다.
    
    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];
    if(argc!=2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    //serv_sock에 정보 할당
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)))
        error_handling("bind() error");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    FD_ZERO(&reads); //reads를 초기화한다.
    FD_SET(serv_sock, &reads); //reads 구조체에 serv_sock의 fd에 해당하는 비트를 set한다.
    fd_max=serv_sock; //select를 통해 관찰할 fd의 번호 범위를 설정한다, default fd들을 제외하고 여기서 
                        //세트된 최초의 fd가 serv_sock을 통한 fd이므로 serv_sock를 max val로 설정한다.
    while(1)
    {
        cpy_reads=reads;
        //어떤 소켓이 현재 연결되어 있는 상태인지 따로 저장할 필요가 있다
        //select를 통해 변화가 생긴 fd만 set되고 나머지는 0으로 masking되면서, 손실되기 때문에 
        //따로 reads를 뽑아내어 어떤 fd(client)가 연결되었는지 나타낼 필요가 있다.
        timeout.tv_sec=5;
        timeout.tv_usec=5000;
        //timer를 설정해 select함수가 무한 bloking 됨을 막는다.
        //타이머가 다 되면 반환값을 0으로
        if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)
            break; //select 오류시 -1을 반환한다 => 종료한다
        //select호출 시 fd 참조 범위는 fd_max에 비례한다.
        //select는 관심대상으로 등록된 fd에 관련된 변화가 발생했음을 뜻한다.
        //2번째 인자=>데이터 수신여부의 관찰 대상에 포함된 fd로 수신된 데이터가 존재하는 경우 = 변화가 발생하였음.

        if(fd_num == 0)
            continue;//타임 아웃 => 다시 select를 호출하고 변화를 기다린다.

        for(i=0; i<fd_max+1; i++)
        {
            if(FD_ISSET(i, &cpy_reads)) //fd = i가 cpy_reads에 존재하면서(즉, 연결이 유지되고 있으면서)
            {
                if(i==serv_sock) //i==serv_sock이면, server로 client의 연결 요청이 들어온 것으로,
                {
                    adr_sz=sizeof(clnt_adr);
                    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                    FD_SET(clnt_sock, &reads);//accept을 통해 소켓을 생성하고 이를 fd_set을 통해 reads에 등록한다.

                    if(fd_max<clnt_sock)
                        fd_max=clnt_sock;//관찰 범위의 수정이 필요하면 수정을 한다

                    printf("connected client: %d\n", clnt_sock);
                }
                else
                {//서버 fd 이외의 것에 변화가 생겼을 경우, 이는 해당 클라이언트 소켓으로 데이터의 전송이 발생한 것이다
                    str_len=read(i, buf, BUF_SIZE);//read를 통해 데이터들을 읽는다
                    if(str_len==0)//EOF의 경우 소켓을 닫으며, reads에 설정된 해당 fd를 초기화 한다
                    {
                        FD_CLR(i, &reads);
                        close(i);
                        printf("close client:%d\n", i);
                    }
                    else//EOF가 아닐 경우 echo
                        write(i, buf, str_len);
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}


void error_handling(char *buf){
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}

                       




