# myshell - Phase 1

## 명령어
- 외부 명령어: `ls`, `mkdir`, `rmdir`, `touch`, `cat`, `echo`, `rm`
- 내부 명령어: `cd <dir>`, `exit`

## 예외 처리
- 존재하지 않는 명령어 입력 시: `Command not found` 출력
- `cd` 명령어에 인자 없을 경우: `cd: missing operand` 출력
- 잘못된 디렉토리 이동 시: `cd: No such file or directory` 메시지 출력

## 함수에서 수정한 부분
- `main()`: 프롬프트 출력 및 명령 루프 구현
- `eval()`: 명령어 파싱 → fork → execvp 실행 흐름 구현
- `builtin_command()`: `cd`, `exit`, `&` 처리 기능 직접 구현
- `parseline()`: 공백 기반 명령어 분할 및 `&` 감지

