# myshell - Phase 3

## 명령어
- `&`를 이용한 백그라운드 명령어 실행
- `jobs`: 백그라운드 작업 목록 출력
- `fg %n`: 정지 중이거나 백그라운드 실행 중인 작업을 포그라운드로 전환
- `bg %n`: 정지된 작업을 백그라운드에서 재실행
- `kill %n` 또는 `kill PID`: 백그라운드 작업 종료

## 구현 특징
- 백그라운드 작업은 `[job_id] pid` 형식으로 출력
- 완료된 백그라운드 작업은 `Done`, `Terminated`, `Exit n` 형식으로 출력됨
  - `Done`: 정상 종료
  - `Terminated`: SIGKILL/SIGINT로 종료
  - `Exit n`: 명령어가 존재하지 않거나 오류(exit 1, 127 등)로 종료

## 시그널 처리
- `Ctrl+C` (SIGINT): 포그라운드 작업 종료, 쉘은 계속 실행됨
- `Ctrl+Z` (SIGTSTP): 포그라운드 작업 정지 → 백그라운드 job 목록에 `Stopped`로 반영됨
- `SIGCHLD`: 자식 프로세스 종료 및 상태 반영
  - 이 때 출력은 지연되어 사용자의 입력 이후에 출력됨

## 예외 처리
- 존재하지 않는 명령어 입력 시: `Command not found` 출력
- `fg`, `bg`, `kill` 명령에 잘못된 job 번호 전달 시 에러 메시지 출력
- `%n`, `PID` 형태 모두 인식
- 명령어 뒤에 붙은 `&`, 예: `sleep 1&`, `sleep 1 &` 모두 처리

## 구현 함수
- `parseline()`: `&` 여부 및 명령어 인자 구문 분석, 붙은 `&`도 인식하도록 구현
- `eval()`: 내부 명령어와 일반 명령어 처리 분기
- `execute_pipeline()`: 파이프라인 명령 실행 및 백그라운드 job 등록
- `sigchld_handler()`: 자식 종료 처리 및 상태 저장 (`done`, `terminated`, `exit_code`)
- `print_jobs()`: 현재 job 상태에 따른 출력 관리 (`+`, `-` 표시 포함)
- `do_fg()`, `do_bg()`, `do_kill()`: job 제어 명령 처리
- `main()`: 사용자 입력 처리 및 완료된 백그라운드 작업 출력