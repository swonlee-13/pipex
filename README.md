**PIPEX**
=====

**과제 설명**
--------

해당 과제는 쉘 명령어의 pipe (|) 를 구현하기 위함.

들어가는 인자는 ./pipex infile "cmd1" "cmd2" ... outfile 의 형태로 들어가게 된다. 실제 쉘에서 이용하는 코드와, 이것을 구현하기 위해 넣어야 하는 인자를 비교하면

```shell
< infile cat | wc -l > outfile
```
과 같은 명령은

```shell
./pipex infile cat "wc -l" outfile
```
로 입력하는 형식이 과제에서 요구하는 사항.

## 보너스 과제 조건

들어가는 인자는
```shell
./pipex here_doc "구분문자" cmd1 cmd2 ... outfile의 형식이다.
```
실제 쉘에서는 다음과 같이 동작하게 된다.
```shell
<< a cat | wc -l >> outfile
```
해당 명령어에 대응하는 인자는 다음과 같다.
```shell
./pipex here_doc "a" "cat" "wc -l" ... outfile
```
> 보너스를 할 계획이 있다면 << 명령이 무엇인지, >> 와 > 는 무슨 차이가 있는지 터미널에 입력하고, 관련 정보를 사전에 찾아보세요.
>
> 기본 과제는 "|" 가 하나 들어가지만, 보너스 과제는 "|" 가 여러개 들어가도 동작할 수 있는 loop형태의 flow 를 작성해야 합니다.


**허용 함수 정리**
--------
-------------
> :bulb: 지금부터 함수 정의를 읽어나가면서 머리속으로 하나씩 설계를 해 나가야 합니다. PIPEX 는 과제에서 사용할 수 있는 명령어들을 용도에 맞게 조합하는 것 만으로 프로젝트 클리어가 가능하기 때문이에요!

### 1. access()

 ```C
   - #include <unistd.h>
	int access(const char *pathname, int mode); 
 ```
#### 기능

프로그램의 실행 가능 여부를 bool 형태로 알려주는 함수.

#### 인자

**`const char * pathname`** : 실행 파일의 위치. 상대경로, 절대경로 전부 다 가능.

**`int mode`** : 함수를 실행해 확인하고 싶은 정보를 적어준다.

> R_OK : 파일 존재 여부, 읽기 권한 여부
> W_OK : 파일 존재 여부, 쓰기 권한 여부
> X_OK : 파일 존재 여부, 실행 권한 여부
> F_OK : 파일 존재 여부

#### 반환

해당 파일이  (mode)가 **가능**할 때 0, **불가능**할 때 1 을 반환한다.

- 예시

```C
#include <stdio.h>
#include <unistd.h>

int main(void){
	char *pathname = "./hello.txt";
	int mode = R_OK | W_OK;
	if( access( pathname, mode ) == 0 ){
		printf("읽고 쓸 수 있습니다.");
	}
	else{
		printf("권한이 없거나 존재하지 않습니다.");
	}
}
```

> pipe를 구현할 때 실행 가능한 프로그램인지 체크하는 행위는 실행하기 전에 반드시 진행되어야 합니다. (shell 에 여러가지 아무 텍스트나,  실행 권한이 없는 파일을 실행해보고 반응을 살펴보세요.)

-------------
### 2. dup()

```c
#include <unistd.h>
int dup(int fd);
```

- fd로 전달받은 파일을 복제한다.

#### open과의 차이점

- `open()`으로 새로 할당하면, 파일포인터(읽고있는 현재 index) 가 당연히 맨 앞에서 시작되지만, `dup()` 을 실행하면 읽고있던 File Pointer도 함께 복제됩니다. 읽고있는 현 상태를 저장할 때 유용합니다.

-------------
### 3. dup2()

```C
#include <unistd.h>
int dup2(int fd1, int fd2);
```

- fd1 로 전달받은 파일을 원하는 fd2가 복제받도록 하는 함수
- fd2 가 이미 열려있는 file discriptor 라면 자동적으로  `close()`를 실행합니다.  

> 이 기능이 왜 있는지 처음에는 이상하게 느껴질 수 있습니다. 그러나, fd2 에 들어갈 값은 표준 입/출력과 표준에러(fd 0, 1, 2)도 포함입니다. 
>
> 이 시점에서 라 피신 때 평가를 위해 외웠던 "|" 의 정의에 대해 기억해야 합니다. 파이프는 쉘에서 **`이전 명령의 표준 출력을 받아서 다음 명령의 표준 입력으로 전달해주는 기능`** 입니다. `dup2()` 가 과제에서 중요한 역할을 할 거 같지 않나요????

-------------
### 4. execve()

```C
#include <unistd.h>

int execve(const char *pathname,
           char * const _Nullable argv[],
           char * const _Nullable envp[]);
```

#### 기능

인자로 받은 프로그램을 실행한다.  이 함수를 콜 한 프로세스는 인자로 넣어준 pathname 와 인자들로 이루어진 **`새로운 프로세스`**를 실행합니다, 이 과정에서 프로세스는 힙, 스택, 모든 데이터 세그먼트를  **`초기화된`** 상태로 시작하게 됩니다.

> 여기서 우리는 위에 강조한 두가지 단어에 주목해야합니다.
>
> 1. 현재 프로세스는 **`에러가 없으면`** 코드의 아래줄로 이동할 일 없이 해당 명령을 실행하고 즉시 종료된다는 것을 인지해야 합니다.
> 2. 데이터는 모두 초기화 됩니다. 특히 힙 영역이 초기화가 된다는 것은 자명하게도 기본적인 malloc() 이 다 할당 헤제된 다는 것을 의미합니다.   man execve(2) 의 Effect on process attributes 에 상세하게 나와있으나, 일단 exit() 처럼 우리가 과제 통과에 필요한 close() 와 free()를 알아서 생각하는게 건강에 좋습니다.

비슷한 다른 명령어들과의 차이점은 이차원배열 argv[] 의 형태로 인자를 전달한다는 점이다.

#### 인자

- **`const char *pathname`**: 실행 파일의 경로, 상대 경로와 절대 경로 전부 가능.

- **`char * const argv[]`**: 인자로 들어갈 **`벡터`** . 우리가 사용하는 argv 의 형태로 넣어줘야 한다는 뜻이다. 첫 번째에는 우리가 c언어에서 받는것처럼 pathname 이 그대로 들어있어야 한다.

  > 우리가 ls -al /home/user/data 을 확인한다면
  >
  > argv[0] = "ls"
  >
  > argv[1] = "-al"
  >
  > argv[2] = "/home/user/data" 일 것입니다.

- **`char * const _Nullable envp[]`**: 인자로 넣어줄 환경 변수입니다. 

  #### 환경 변수는 [해당 문서](./readme/Env.md) 함고.

#### 반환

- 에러가 없으면 반환값은 없습니다.

- 에러가 발생하면 -1을 반환하며, errno 에 결과상태가 반환되어서 확인할 수도 있습니다.
- 보통 execve() 코드 아래줄에 에러가 반환되었을 때 정상적으로 종료할 수 있는 코드를 적어줘야 합니다.

#### 예시

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* execve_ls.c */

int main(int argc, char *argv[], char **envp)
{
	(void)argv[];
  static char *newargv[] = { "/bin/ls", "-a", "-l", NULL };
//   static char *newenviron[] = { NULL }; 새로운 환경변수가 필요하면 만들면 됩니다.

   if (argc != 2) {
       fprintf(stderr, "Usage: %s <file-to-exec>\n", argv[0]);
       exit(EXIT_FAILURE);
   }
   execve(newargv[0], newargv, envp);
   perror("execve");   /* execve() returns only on error */
   exit(EXIT_FAILURE);
}
```

> 프로세스가 즉시 종료되면 아무것도 할 수 없습니다. 메인 프로세스를 살리고, 프로그램 실행은 다른 곳에서 해야 할것입니다. 이 고민을 들고 다음 명령어를 살펴보세요.

-------------
### 5. Fork()

```c
#include <unistd.h>
pid_t fork(void);
```

#### 기능

- 현재 프로세스를 복제한 새로운 프로세스를 생성하고, 자식 프로세스의 pid 를 반환한다. 

>  fork() 를 통해서 생성된 프로세스를 Child process(자식 프로세스), 원 프로세스를 Parent process(부모 프로세스)  라고 부릅니다. 자식 프로세스는 부모 프로세스의 모든 진행 상황을 그대로 복사한다고 생각하면 됩니다.

#### 인자

- pid_t (반환값) : 반환받을 int 인데, `typedef` 로 별명을 지정한 자료형입니다.

#### 반환값

- 부모 프로세스의 경우 자식 프로세스의 Pid 를 반환한다.  
- 자식 프로세스의 경우 0 이 반환된다.  
- 에러 발생 시 -1 이 반환된다.

> 반환값을 이용해서 이 코드가 자식프로세스의 구동인지, 부모 프로세스의 구동인지 if문으로 행동을 제어할 수 있습니다. 

#### 예시

```C
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
   pid_t pid;

   if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
       perror("signal");
       exit(EXIT_FAILURE);
   }
   pid = fork();
   if (pid == -1) {
       perror("fork");
       exit(EXIT_FAILURE);
   }
   else if (pid == 0) {
       puts("hi i'm child.");
       exit(EXIT_SUCCESS);
   }
   else {
       printf("Child is PID %jd\n", (intmax_t) pid);
       puts("hi i'm parent.");
       exit(EXIT_SUCCESS);
   }
}
```

> Fork() 를 통해서 자식프로세스와 부모프로세스가 나눠졌고 두 프로그램은 fork에 이어서 다음 행동을 동시에 진행합니다. 그러나, if 문을 통해서 각각의 행동을 제어할 수 있다는 점을 기억하고, 이전 명령어와 합쳐서 설계를 한 뒤 다음 명령을 살펴보세요.

------

### 7. pipe()

```C
#include <unistd.h>

int pipe(int pipefd[2]);

```

#### 기능 

파이프를 만들고, 이를 사용할 수 있는 쓰기, 읽기 전용 fd 두 개를 반환합니다.

자세한 사항은 [pipe](./readme/pipe.md) 참고

> pipex 인 만큼 pipe() 는 중요한 기능입니다. 이 부분까지 다 읽고나면 의사코드를 작성해 보세요. 

-------------
### 10. unlink()

```C
#include <unistd.h>

int unlink(const char *pathname);
```

#### 기능

- 인자로 들어온 경로의 파일을 unlink 한다.
- 파일 시스템 상에서 Unlink 는 될 지라도 이미 open() 을 통해서 파일이 잡혀있으면 계속 사용할 수 있고, 이 파일을 close() 하거나, 프로그램을 종료하면 완벽하게 삭제된다.
- 이 기능은 [here_doc](./readme/here_doc.md) 을 구현하는 데 중요한 역할을 한다..

-------------

## 정리

- 위에 적어놓은 파이프("|") 의 정의를 다시 살펴보고, 그 문장을 어떻게 코드로 구현해야 하는지 잘 생각해야 합니다.
- 이 과제를 진행하고 나면 42 과제 구현에 대한 감을 잡을 수 있습니다. 화이팅하세요!