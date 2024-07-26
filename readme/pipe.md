# pipe()

## 정의

```C
#include <unistd.h>

int pipe(int pipefd[2]);
```

## 기능

Pipe() 를 만들어줍니다. 

> 부모-자식 프로세스는 기본적으로 복사된 이후 완벽하게 독립적으로 행동합니다.  
>
> 이 부모 프로세스와 자식 프로세스가 데이터를 주고받을 수 있도록 만들어진 것이 파이프입니다.

## 인자

- **`int piped[2]`** : 파이프의 읽기, 쓰기 부분의 fd 를 받아올 int[2] 배열입니다. 직접 만들어야 합니다.

>  ```C
>  # typedef WRITE 1
>  # typedef READ 0 //이런 식으로 관리하면 나중에 헷갈리지 않습니다. 0이나 1은 중요하지 않지만 저는 이렇게 썼습니다.
>  
>  int pipefd[2];
>  ```

## 반환

- Int 형태로 반환하며, 성공시 0을 반환. 실패하면 -1 반환.

> 과제를 하다보면 다양한 동적 할당을 실행 할 것입니다. -1 이 반환되었을 때 성공적으로 종료하기 위해서 free() 등을 잘 수행해야합니다.

## 예시

```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
 int    pipefd[2];
 char   buf;
 pid_t  cpid;

 if (argc != 2) {
     fprintf(stderr, "Usage: %s <string>\n", argv[0]);
     exit(EXIT_FAILURE);
 }

 if (pipe(pipefd) == -1) {
     perror("pipe");
     exit(EXIT_FAILURE);
 }

 cpid = fork();
 if (cpid == -1) {
     perror("fork");
     exit(EXIT_FAILURE);
 }

 if (cpid == 0) {    /* Child reads from pipe */
     close(pipefd[1]);          /* Close unused write end */

     while (read(pipefd[0], &buf, 1) > 0)
         write(STDOUT_FILENO, &buf, 1);

     write(STDOUT_FILENO, "\n", 1);
     close(pipefd[0]);
     _exit(EXIT_SUCCESS);

 } else {            /* Parent writes argv[1] to pipe */
     close(pipefd[0]);          /* Close unused read end */
     write(pipefd[1], argv[1], strlen(argv[1]));
     close(pipefd[1]);          /* Reader will see EOF */
     wait(NULL);                /* Wait for child */
     exit(EXIT_SUCCESS);
 }
}
```

> 해당 코드는 파이프로 read / write 를 실행하면서 소통을 합니다. 여기에서 pipe fd 를 읽는 것이 아니라 표준 출력과 표준 입력으로 꺾어주는 설계를 해야합니다.

## 구동 원리

 ![image](https://user-images.githubusercontent.com/79521972/169849580-00d96551-e817-464c-8564-216a23f2ef7c.png)

- pipe() 를 실행하면 임시 버퍼를 생성하고, 이 버퍼에 쓸 수 있는 fd와 읽을 수 있는 fd 를 하나씩 제공합니다. 파이프로 데이터가 들어가면 그림처럼 버퍼가 그만큼의 데이터를 임시로 가지고 있고, 반대쪽 읽기 fd 를 통해 read() 가 발생하면 그만큼 버퍼를 비우게 됩니다. 이런 과정을 통해서 데이터 통신을 할 수 있습니다.

> 위의 설명을 통해 유추할 수 있겠지만, 파이프는 한 방향으로만 소통할 수 있습니다. 양 방향으로 소통하고 싶다면?  파이프를 두개를 사용해야 합니다. (과제와는 크게 연관이없음)

- Pipe()는 unnamed FIFO 라고도 구분됩니다.

  > pipe()를 통한 데이터 통신은 자식 / 부모 프로세스 간에만 소통할 수 있습니다. 왜냐면, 이 명령을 실행할 때 원하는 경로에 파일을 만들지 않기 때문입니다.

  > 원하는 경로에 실제 파일을 만들면 완전 다른 pid 에서도 pathname 으로 접근해서 서로 소통할 수 있었을 텐데요, 실제로 이런 기능은 mkfifo() 계열의 시스템 콜로 구현할 수 있으며 이 기능은 실제 파일name 이 있다보니 named FIFO 라고 불립니다. 이런 시스템 콜을 사용하면 foo.fifo 와같은 fifo 파일이 생깁니다.

- 파이프의 버퍼는 일정 크기를 가집니다. 그 크기는 short_max 값인 65536 입니다. 

  > 만약 버퍼가 가득 찬 상태에서 write 를 한다면 시스템은 반대쪽에서 read 를 할 때까지 block 상태가 됩니다.
  >
  > 반대쪽도 마찬가지입니다. 버퍼가 비어있는데 read() 를 콜 하면 데이터가 들어올 때까지 block 상태가 되지요.
  >
  > 이런 특성을 이해하고 있으면 cat | cat | ls 와 같은 명령이 blocking  이 걸리는지 이해할 수 있습니다. 
  >
  > 저 상황에서 Cat 명령어가 어떻게 동작할 지 유추한 뒤, 대략 어떤 포인트에서 걸릴 지 생각해보면 도움이 많이 됩니다.

## 참고자료

https://speardragon.github.io/system/system%20programming/System-Programming-12장.-파이프/

https://www.man7.org/linux/man-pages/man2/pipe.2.html - 리눅스전용인 pipe2() 도 섞여있으니 구분해서 잘 읽어보거나, 맥 터미널에서 직접 man pipe 를 쳐서 확인하는 것을 추천합니다.



