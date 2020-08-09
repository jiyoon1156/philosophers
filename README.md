# philosophers

[[OS] 프로세스와 스레드의 차이 - Heee's Development Blog](https://gmlwjd9405.github.io/2018/09/14/process-vs-thread.html)

- thread = philosopher

![thread](https://user-images.githubusercontent.com/60052127/89723196-13936900-da2e-11ea-80d6-fd299ab30d2e.png)
![multi-thread](https://user-images.githubusercontent.com/60052127/89723199-18f0b380-da2e-11ea-9b8f-69cb43ba9738.png)
---

## 멀티 프로세스

- 프로그램/프로세스 여러 개 켜서 작업을 하는 것.

### 장점

- 여러 개의 자식 프로세스 중 하나에 문제가 발생하면 그 자식 프로세스만 죽는 것 이상으로 다른 영향이 확산되지 않는다.

---

## 멀티 스레드

- 하나의 프로세스 안에서 스레드를 여러 개 켜서 작업을 하는 것.

### 장점

- **1) 자원을 효율적으로 사용 :**
- 멀티 프로세스로 실행되는 작업을 멀티 스레드로 실행할 경우, 프로세스를 생성하여 자원을 할당하는 시스템 콜이 줄어들어 자원을 효율적으로 관리
- 프로세스 간의 Context Switching시 단순히 CPU 레지스터 교체 뿐만 아니라 RAM과 CPU 사이의 캐쉬 메모리에 대한 데이터까지 초기화되므로 오버헤드가 크기 때문
- 스레드는 프로세스 내의 메모리를 공유하기 때문에 독립적인 프로세스와 달리 스레드 간 데이터를 주고 받는 것이 간단해지고 시스템 자원 소모가 줄어들게 된다.
- **2) 처리 비용 감소 및 응답 시간 단축 :**
- 또한 프로세스 간의 통신(IPC)보다 스레드 간의 통신의 비용이 적으므로 작업들 간의 통신의 부담이 줄어든다.
–> 스레드는 Stack 영역을 제외한 모든 메모리를 공유하기 때문
- 프로세스 간의 전환 속도보다 스레드 간의 전환 속도가 빠르다.
–> Context Switching시 스레드는 Stack 영역만 처리하기 때문

---

## thread_create, thread_join, thread_detach

[[리눅스] 스레드(Thread) 개념과 예제(pthread_create, pthread_join, pthread_detach)](https://reakwon.tistory.com/56)

- thread_join은 스레드가 끝날 때까지 기다려줌. 이걸 사용하지 않으면 그냥 프로그램이 종료됨
- thread_detach: 때에 따라서는 스레드가 독립적으로 동작하길 원할 수도 있습니다. 단지 pthread_create 후에 pthread_join으로 기다리지 않구요. 나는 기다려주지 않으니 끝나면 알아서 끝내도록 하라는 방식입니다. thread가 할일이 끝나면 알아서 자원을 해제하고 없어진다고 한다.

## **스레드 동기화** pthread_mutex_init, pthread_mutex_lock, pthread_mutex_unlock, pthread_mutex_destroy

[[리눅스] 뮤텍스(mutex)를 이용한 스레드(pthread) 동기화 예제 코드(pthread_mutex_init, pthread_mutex_lock,pthread_mutex_unlock,pthread_mutex_destroy)](https://reakwon.tistory.com/98)

스레드를 사용할때는 주의해야함 점이 있습니다. **서로 동시에 실행하기 때문에 발생하는 공유자원의 잘못된 접근때문입니다.** 

—>포크 사용 문제!!!!????!!!! 서로 동시에 포크를 집으려하니까 문제 생김 이걸 동기화로 해결!

그 때문에 **공유자원을 하나의 스레드만 사용하게 하는 방법**이 필요한데 이를 동기화라고 합니다.

## Return Value

If successful, the *pthread_mutex_destroy*() and *pthread_mutex_init*() functions shall return zero; otherwise, an error number shall be returned to indicate the error.

- 성공시 0 리턴 실패 시 error number 리턴

---

## Deadlock 교착상태란?

[](https://namu.wiki/w/%EC%8B%9D%EC%82%AC%ED%95%98%EB%8A%94%20%EC%B2%A0%ED%95%99%EC%9E%90%20%EB%AC%B8%EC%A0%9C)

- 간단하게 생각해, 만약 모든 철학자들이 동시에 자신의 왼쪽 포크를 잡는다면, 모든 철학자들이 자기 오른쪽의 포크가 사용 가능해질 때까지 기다려야 한다. 그런데 모든 철학자들이 그러고 있다. 이 상태에서는 모든 철학자가 영원히 2번 상태에 머물러있어 아무것도 진행할 수가 없게 되는데, 이것이 교착(Deadlock)상태이다.
- 따라서 philosopher 가 1명일 때도 교착상태! 포크가 1개밖에 없으니까

---

## Semaphore vs Mutex

[세마포어 (Semaphore) > 도리의 디지털라이프](http://blog.skby.net/%EC%84%B8%EB%A7%88%ED%8F%AC%EC%96%B4-semaphore/)

- 아래꺼는 좀 극단적인 설명인듯ㅠ

[Semaphore란? 세마포어와 뮤텍스의 차이는?](https://jwprogramming.tistory.com/13)

## **sem_wait==mutex_lock,  sem_post== mutex_unlock**

## sem_open

```
sem_t *sem_open(const char *name, int oflag,mode_t mode, unsigned int value);
```

[sem_open(3) - Linux manual page](https://man7.org/linux/man-pages/man3/sem_open.3.html)

```
On error, sem_open() returns SEM_FAILED, with errno set to indicate the error.
근데 sem_failed 값이 -1 임.
```

- 권한 : -:0, r:4, w:2, x:1 모드 0660 user:소유자권한, group:그룹권한, others:그외사용자 그니까 0660이면 user 에 r+w group에 r+w others에 none 준거

## sem_unlink

sem_unlink()함수는 name인 이름을 가진 세마포어를 제거한다.만일 현재 name으로 이름이 명명된 세마포어가 어떤 프로세스에 의해 참조 되고 있다면, sem_unlink()는 해당 세마포어에 영향을 미치지 않는다. 만일 하나 혹은 그 이상의 프로세스가 sem_unlink()가 호출 되었을 때, 해당 세마포어를 열고 있다면, 모든 참조되고 있는 세마포어가 sem_close(), _exit(), 혹은 exec.에 의해 종료될 때까지, 세마포어 제거는 연기된다. 재생성 혹은 재연결을 하기 위한 sem_open() 호출은 sem_unlink()을 호출한 후에 새로운 세마포어에 대하여 참조를 한다. sem_unlink() 호출은 모든 참조들이 제거되기 전까지 잠그지 않는다. 함수는 즉시 반환한다.

## Return Value

- 성공시 0, 실패시 -1

[sem_wait](https://pubs.opengroup.org/onlinepubs/007908799/xsh/sem_wait.html)

## Waitpid 함수

pid - wait할 자식 프로세스의 유형 

- -1 > pid : 그룹ID가 절대값과 같은 차일드 프로세스를 waiting합니다.
- -1 == pid : 아무 자식 프로세스 ID라도 waiting합니다.
- 0 == pid : 자신과 같은 프로세스 그룹ID를 가진 차일드 프로세스를 waiting합니다.
- 0 < pid : 넘겨진 pid인 자식 프로세스만 waiting합니다.

option - 0 또는 아래의 상수의 조합으로 설정됩니다. 

- 0 : 결과를 return할 때까지 block합니다.
- WNOHANG : 현재 종료된 자식 프로세스가 없으면 block하지 않고 바로 0을 반환함.
- WUNTRACED : 자식 프로세스가 STOP하면 반환함
- WCONTINUED: STOP되었던 자식 프로세스가 재 실행되면 반환함.

**[WIFEXITED](https://www.joinc.co.kr/modules/moniwiki/wiki.php/manSearch?google=none&name=WIFEXITED)(status)**

자식이 정상적으로 종료되었다면 non-zero 이다.

return vaule

- 자식 프로세스가 정상적으로 종료되었다면 TRUE
**[WEXITSTATUS](https://www.joinc.co.kr/modules/moniwiki/wiki.php/manSearch?google=none&name=WEXITSTATUS)(status)**

[exit](https://www.joinc.co.kr/modules/moniwiki/wiki.php/manSearch?google=none&name=exit)()를 호출하기 위한 인자나 return 값이 설정되고 종료된 자식의 반환 코드의 최하위 8비트를 평가한다. 이 매크로는 정상정료 - [WIFEXITED](https://www.joinc.co.kr/modules/moniwiki/wiki.php/manSearch?google=none&name=WIFEXITED)(status) - 일때만 평가된다.

return value

- 자식 프로세스가 정상 종료되었을 때 반환한 값 (exit code 값이 담김)

file descriptor

- 0 : 표준 입력 / 1 : 표준 출력 / 2: 표준 오류

## philosopher_three leaks

[https://42born2code.slack.com/archives/CN9RHKQHW/p1582397659003300](https://42born2code.slack.com/archives/CN9RHKQHW/p1582397659003300)

fork 되면서 첫 번째 프로세스 이후에는 leak  이 있음

- mac os 시스템의 문제라고 함.. malloc 안해도 fork 하면 leak이 생김

[https://stackoverflow.com/questions/54671093/c-leak-on-fork-without-malloc](https://stackoverflow.com/questions/54671093/c-leak-on-fork-without-malloc)
