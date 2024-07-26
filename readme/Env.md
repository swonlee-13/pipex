# 환경 변수

쉘에서 환경 변수를 출력해 보셨나요?

```sh
$ env
$ TERM_SESSION_ID=w0t0p0:C23F26C3-B1DA-442B-9A9C-67740CA97ED8
SSH_AUTH_SOCK=/private/tmp/com.apple.launchd.kHqt7EJKia/Listeners
LC_TERMINAL_VERSION=3.5.2
COLORFGBG=15;0
ITERM_PROFILE=Default
XPC_FLAGS=0x0
LANG=ko_KR.UTF-8
PWD=/Users/user
SHELL=/bin/zsh
__CFBundleIdentifier=com.googlecode.iterm2
TERM_FEATURES=T3LrMSc7UUw9Ts3BFGsSyHNoSxF
TERM_PROGRAM_VERSION=3.5.2
TERM_PROGRAM=iTerm.app
PATH=/usr/bin:/bin:/usr/sbin:/sbin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/local/bin:/Library/Apple/usr/bin:/Applications/iTerm.app/Contents/Resources/utilities
LC_TERMINAL=iTerm2
COLORTERM=truecolor
COMMAND_MODE=unix2003
TERM=xterm-256color
TERMINFO_DIRS=/Applications/iTerm.app/Contents/Resources/terminfo:/usr/share/terminfo
HOME=/Users/user
TMPDIR=/var/folders/1v/rxxr_ryx1_lcmxr0hfbkxrj80000gn/T/
USER=user
XPC_SERVICE_NAME=0
LOGNAME=user
ITERM_SESSION_ID=w0t0p0:C23F26C3-B1DA-442B-9A9C-67740CA97ED8
__CF_USER_TEXT_ENCODING=0x0:3:51
SHLVL=1
OLDPWD=/Users/user
ZSH=/Users/user/.oh-my-zsh
PAGER=less
LESS=-R
LSCOLORS=Gxfxcxdxbxegedabagacad
LS_COLORS=di=1;36:ln=35:so=32:pi=33:ex=31:bd=34;46:cd=34;43:su=30;41:sg=30;46:tw=30;42:ow=30;43
```

터미널에 env 를 입력하면 다음과 같은 값들이 쏟아집니다. 이게 환경변수입니다. 환경변수는 ${USER} 과 같이 쉘 등에서 활용할 수 있는 값으로써 스크립트 등을 유저나 환경에 구애받지 않고 작성하는 데 도움이 됩니다.

그런데, int main() 에도 이 환경 변수를 쓸 수 있다는 사실을 아시나요???

int main() 에는 사실 3번째 인자가 있답니다. (mac 에는 네번째 인자도 있어요)

```c
#include <stdio.h>
#include <unistd.h>

int main(int ac, char **av, char **envp)
{
	for (int i = 0; envp[i]; i++) {
		printf("%s\n", envp[i]);
	}
	printf("\n\n");
}
```

이 코드를 실행시켜 보면

```sh
TERM_SESSION_ID=w0t0p0:C23F26C3-B1DA-442B-9A9C-67740CA97ED8
SSH_AUTH_SOCK=/private/tmp/com.apple.launchd.kHqt7EJKia/Listeners
LC_TERMINAL_VERSION=3.5.2
COLORFGBG=15;0
ITERM_PROFILE=Default
XPC_FLAGS=0x0
LANG=ko_KR.UTF-8
PWD=/Users/user
SHELL=/bin/zsh
__CFBundleIdentifier=com.googlecode.iterm2
TERM_FEATURES=T3LrMSc7UUw9Ts3BFGsSyHNoSxF
TERM_PROGRAM_VERSION=3.5.2
TERM_PROGRAM=iTerm.app
PATH=/usr/bin:/bin:/usr/sbin:/sbin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/local/bin:/Library/Apple/usr/bin:/Applications/iTerm.app/Contents/Resources/utilities
LC_TERMINAL=iTerm2
COLORTERM=truecolor
COMMAND_MODE=unix2003
TERM=xterm-256color
TERMINFO_DIRS=/Applications/iTerm.app/Contents/Resources/terminfo:/usr/share/terminfo
HOME=/Users/user
TMPDIR=/var/folders/1v/rxxr_ryx1_lcmxr0hfbkxrj80000gn/T/
USER=user
XPC_SERVICE_NAME=0
LOGNAME=user
ITERM_SESSION_ID=w0t0p0:C23F26C3-B1DA-442B-9A9C-67740CA97ED8
__CF_USER_TEXT_ENCODING=0x0:3:51
SHLVL=1
OLDPWD=/Users/user
INFOPATH=/opt/homebrew/share/info:
ZSH=/Users/user/.oh-my-zsh
PAGER=less
LESS=-R
LSCOLORS=Gxfxcxdxbxegedabagacad
LS_COLORS=di=1;36:ln=35:so=32:pi=33:ex=31:bd=34;46:cd=34;43:su=30;41:sg=30;46:tw=30;42:ow=30;43
_=/Users/user/./a.out
```

똑같은 텍스트가 출력되는 것을 볼 수 있습니다!! (차이가 있다면 제가 자체적으로 지웠는데 지우다 만거)  
이 환경변수는 내 시스템 정보나 경로 등을 찾을 때 필요한 정보들이 많이 담겨있습니다. 과제를 할 때도 물론 중요하겠죠. 환경 변수의 특징에 대해서 간당히 정리해봅시다.

## 환경 변수의 특징 

1. 값들은 key=value 로 구성됩니다
2. 하나의 변수에 다양한 정보를 담고싶으면 : 으로 구분합니다.

## 구현

여기서 우리는 PATH 라는 환경변수와 특징 2번을 잘 생각해야 합니다.
ls 는 /bin/안에 있고, 어떤 명령어는 /sbin/안에 있을 수도 있고
어떤 파일은 /Applications/iterm.app 와 같은 곳에 있을 수도 있습니다.
그런데 우리는 항상 경로를 빼고 ls, cat 등의 명령어만 작성하게 됩니다. 그렇다면 파일의 경로는 도대체 어떻게 찾아가는걸까요?

이 부분에 대해서 PATH 를 관찰해보면서 하나하나 설계 해 보시길 바라겠습니다.
