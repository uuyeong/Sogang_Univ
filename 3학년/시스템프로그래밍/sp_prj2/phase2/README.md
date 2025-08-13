# myshell - Phase 2

## 명령어
- 파이프(`|`)를 이용한 여러 명령어 연결 지원
- 여러 단계의 파이프라인 (2개 이상 연결)
- 따옴표가 감싸진 인자(`"abc"`)도 정상적으로 처리

## 예외 처리
- 존재하지 않는 명령어 입력 시: `Command not found` 출력
- 파이프 구문에서 명령어가 없거나 잘못된 경우: 에러 출력
- `cat hello.txt | grep -i abc` → 정상 동작
- `cat hello.txt | grep -i "abc"` → 따옴표 제거 후 정상 동작 (리눅스 쉘과 동일하게 출력 구현)

## 함수에서 수정한 부분
- `parse_pipeline()`: `|` 기호 기준으로 명령어 체인 분리 구현
- `execute_pipeline()`: 각 명령어별로 pipe, fork, dup2 연결 흐름 구현
- `clean_quotes()`: 쌍따옴표로 감싸진 인자를 내부에서 제거 (abc와 "abc"를 동일하게 처리)
- `eval()`: 파이프가 포함된 명령인지 여부 판단 후 분기 실행

## 구현 흐름
- 명령어에 `|` 포함 → `parse_pipeline()`로 분할
- 각 명령을 `argv[]`로 변환 후 `execvp()`로 실행
- 마지막 명령어의 출력을 셸에 전달