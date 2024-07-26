# **shell 의  다양한 redirect 기호들**

# **1. here_doc**

here_doc 은 쉘에 있는 강력한 기능입니다. 여러 줄의 텍스트를 쉽게 입력하고, 명령에게 전달할 수 있습니다. 

here_doc 은 다음과 같이 사용됩니다.

```shell
<< delimeter command arg1 arg2 ...
  
#예시
<< EOF cat
```

터미널에 해당 예시를 입력해 보세요. `heredoc>` 이라는 새로운 입력 환경이 생기고, 원하는 텍스트를 개행 고민 없이 마음껏 적을 수 있습니다. 입력을 끝내고 싶다면, 새 줄에서 delemiter 로 지정한 텍스트를 입력하면 됩니다. 그러면 지금까지 작성한 임시 텍스트가 표준 입력으로 들어가서 cat 으로 나오는 것을 확인 할 수 있습니다.

이 기능은 파일을 만들어서 내용을 작성 후 `<` 기호를 이용해서 굳이 가져올 필요 없게 만들어줍니다. 또한 다음과 같이 쉘 스크립트에서도 임시 파일을 작성할 수 있도록 하기도 합니다. 기능을 잘 알고있다면 한번쯤 써보게 되는 기능입니다.

```shell

if [ ! -d "/var/lib/mysql/wordpress" ]; then

        cat << EOF > /tmp/create_db.sql # 이 줄을 확인하세요 여기가 명령어입니다.
USE mysql;
FLUSH PRIVILEGES;
DELETE FROM     mysql.user WHERE User='';
DROP DATABASE IF EXISTS test;
DELETE FROM mysql.db WHERE Db='test';
DELETE FROM mysql.user WHERE User='root' AND Host NOT IN ('localhost', '127.0.0.1', '::1');
CREATE DATABASE ${DB_NAME} CHARACTER SET utf8 COLLATE utf8_general_ci;
CREATE USER '${DB_USER}'@'%' IDENTIFIED by '${DB_ROOT_PASSWORD}'; #환경변수가 들어간 다는 점도 체크해보세요 미니쉘 할 떄 필요합니다.
GRANT ALL PRIVILEGES ON ${DB_NAME}.* TO '${DB_USER}'@'%';
FLUSH PRIVILEGES;
EOF # 내용은 제끼고 마지막에 구분자고 끝내준 부분만 확인해주세요
```

예시는 제가 mariadb 초기 설정을 active 환경에서 하지 않고, 입력창 없이 한번에 진행하기 위해서 만든 쉘 스크립트의 일부입니다. `<<` 로 파일을 작성할 수 있는 환경을 만들었고, 명령의 결과는 `>` 를 통해서 새로운 파일에 작성이 되는 모습인데요, 우리가 이 기능을 몰랐다면

touch 로 파일을 생성하고, echo "USE mysql" >> /tmp/create_db.sql 과 같이 한줄한줄 입력해야 했을 수도 있습니다. 가독성도 떨어지죠.

## 이 기능의 원리는?

이 기능의 원리는 단순합니다.

> - here_doc 을 만나면, delimiter 를 들고가서
> - 계속 표준 입력으로부터 데이터를 받아오고, 그것을 임시 파일에 작성
> - delimeter 를 입력 받았다고 판단되면 함수를 종료
> - 임시파일이니까 파일을 unlink

해주는 작업이 필요할껍니다. 이 과정에서 표준 입력에 `heredoc>`같은거를 적어서 꾸며준다면 더 그럴듯해 보일겁니다.  

## here_doc 에 관해서 더 관찰하고 싶다면?

[이 곳](https://stackoverflow.com/questions/70672456/how-here-document-works-in-shell)을 읽어보세요. 쉘 스크립트가 어떻게 돌아가는지 관찰하기 위해서 strace 를 이용하는 모습입니다.

이 함수는 프로그램을 실행했을 때 어떤 시스템 콜이 이용되는지를 볼 수 있는 유용한 명령입니다.

(이게 리눅스에서 돌아가는 걸로 알고있고 저는 그냥 글만 읽었어요)

```bash
strace -f bash -c $'cat > file.txt << EOF\nline1\nline2\nEOF'
```

여기를 관찰하면서 어떤 과정이 있는지 잘 살펴보시고,

파일을 읽으면 파일 포인터가 움직이기 때문에, 파일을 닫고 다시 열어주는 모습을 볼 수 있을꺼에요. 이 부분을 열심히 생각해 보시면 도움이 많이 됩니다.

# **2. >> append 기호**

이 기호는 `>` 와 비슷한 기능을 가집니다. 명령은 보통 표준 출력에서 나오게 되는데, 이것을 파일로 바꿔주겠다는 명령어입니다. 그런데? 여기에서 파일에 **`이어쓰기 기능`**만 있으면 되는겁니다. Open() 명령어 공부는 생략했는데, open() 의 다양한 옵션을 읽어보시면 좋을겁니다.

> O_CREATE, O_TRUNK, O_EXCL 등 다양한 플래그를 읽어보세요

# **3. 일반 < > 기호**

이거도 그냥 생략하겠습니다. 표준 입/출력 대신 이 파일로 옮겨주는 일입니다.