# 정적 분석 구성 (Static Analysis)

libemb 프로젝트의 자동화된 정적 분석 설정입니다.

## 개요

다음 도구를 사용한 코드 품질 검사:
- **CPPCheck**: 일반적인 C++ 버그 검출
- **Clang-Tidy**: Clang 기반 고급 정적 분석

---

## 1. CPPCheck

### 개요
범용 C++ 정적 분석 도구로 메모리 누수, 버퍼 오버플로우, 논리 오류 등을 검출합니다.

### GitHub Actions 워크플로우
- **파일**: `.github/workflows/static-analysis.yml`
- **실행 타이밍**: 매주 화요일 자동 실행, PR 시에도 실행
- **결과**: Artifact로 XML 및 텍스트 리포트 저장

### 로컬에서 실행

```bash
# 설치 (Ubuntu)
sudo apt-get install cppcheck

# 기본 실행
cppcheck include/ src/ port/

# 모든 검사 활성화 (권장)
cppcheck --enable=all \
  --suppress=missingIncludeSystem \
  --suppress=unusedFunction \
  --std=c++17 \
  --platform=unix64 \
  include/ src/ port/ benchmark/ test/

# XML 리포트 생성
cppcheck --xml --xml-version=2 \
  --output-file=cppcheck.xml \
  include/ src/
```

### 억제 규칙 (Suppressions)

특정 경고를 무시하려면 코드에 주석을 추가할 수 있습니다:

```cpp
// cppcheck-suppress unusedVariable
int unused_var = 42;

// cppcheck-suppress memleak
void* ptr = malloc(100);
```

또는 `cppcheck_suppressions.xml` 파일을 생성하여 일괄 관리:

```bash
cppcheck --suppressions-list=cppcheck_suppressions.xml ...
```

### 검사 범위

| 범주 | 설명 |
|------|------|
| error | 심각한 버그 |
| warning | 잠재적 버그 |
| style | 코드 스타일 |
| performance | 성능 문제 |
| portability | 이식성 문제 |
| information | 정보성 메시지 |

### 주요 검사 항목

- ✅ 메모리 누수
- ✅ 버퍼 오버플로우
- ✅ 사용하지 않은 변수
- ✅ 초기화되지 않은 변수
- ✅ 범위 벗어난 접근
- ✅ null 포인터 역참조
- ✅ 타입 오류
- ✅ 논리 오류

---

## 2. Clang-Tidy

### 개요
LLVM 프로젝트의 clang-tidy는 Clang 컴파일러를 기반으로 한 고급 정적 분석 도구입니다.

### 특징
- 코딩 표준 준수 검사 (CERT, CppCoreGuidelines)
- 코드 현대화 제안 (C++11/14/17/20)
- 성능 최적화 제안
- 자동 수정 기능 (Fix-it)

### 설정 파일
- **위치**: `.clang-tidy`
- **내용**: 검사 규칙, 옵션, 네이밍 컨벤션

### 로컬에서 실행

```bash
# 설치 (Ubuntu)
sudo apt-get install clang clang-tools

# 기본 실행
clang-tidy -checks='*' src/main.cpp

# CMake와 함께 사용
cmake -B build -DLIBEMB_ENABLE_CLANG_TIDY=ON -DCMAKE_CXX_COMPILER=clang++
cmake --build build

# 자동 수정 적용
clang-tidy --fix src/main.cpp

# 특정 검사만 실행
clang-tidy -checks='readability-*' src/main.cpp

# 결과를 파일로 저장
clang-tidy src/main.cpp 2>&1 | tee clang-tidy.log
```

### CMakeLists.txt 통합

```cmake
# CMake 설정 시 옵션 전달
cmake -B build -DLIBEMB_ENABLE_CLANG_TIDY=ON

# 또는 직접 지정
cmake -B build -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-checks=*"
```

### 주요 검사 그룹

| 그룹 | 설명 |
|------|------|
| `readability-*` | 코드 가독성 |
| `modernize-*` | C++17 표준 활용 |
| `performance-*` | 성능 최적화 |
| `cppcoreguidelines-*` | C++ Core Guidelines |
| `google-*` | Google C++ 스타일 (비활성화) |
| `llvm-*` | LLVM 스타일 (비활성화) |
| `cert-*` | CERT 보안 표준 |

### 네이밍 컨벤션 검사

`.clang-tidy`에서 설정된 규칙:

```
ClassCase: CamelCase
FunctionCase: camelCase
ParameterCase: camelCase
PrivateMemberCase: lower_case (suffix: _)
ConstantCase: UPPER_CASE
NamespaceCase: lower_case
```

### 예제

**입력 코드:**
```cpp
void MyFunction() {
    int unused_variable = 5;  // cppcoreguidelines-init-variables
    auto result = malloc(100); // modernize-use-new-delete
    if (result)
        printf("OK");  // readability-braces-around-statements
}
```

**clang-tidy 실행:**
```bash
$ clang-tidy example.cpp
example.cpp:2:5: warning: variable 'unused_variable' set but not used
example.cpp:3:18: warning: 'malloc' should not be used; consider 'new'
example.cpp:5:5: warning: statement should be enclosed in braces
```

**자동 수정 적용:**
```bash
$ clang-tidy --fix example.cpp
```

---

## 3. GitHub Actions 워크플로우 통합

### 워크플로우: `static-analysis.yml`

**실행 타이밍:**
- 모든 PR 시
- main으로 푸시 시
- 매주 화요일 자동 실행

**단계:**
1. CPPCheck 실행 및 결과 수집
2. Clang-Tidy 실행 및 경고 추출
3. 결과를 Artifact로 저장
4. PR에 코멘트 달기
5. 품질 요약 리포트 생성

**출력:**
- CPPCheck XML/텍스트 리포트
- Clang-Tidy 로그 및 요약
- 통합 품질 리포트

---

## 4. CI/CD 성공 기준

### 필수 조건

✅ 모든 **Critical** 이슈 해결
✅ **Error** 심각도 이슈 0개
✅ 워크플로우 완료

### 권장 사항

⚠️ **Warning** 최소화
⚠️ 코드 커버리지 + 정적 분석 조합
⚠️ 자동 수정(Fix-it) 검토 후 적용

---

## 5. 억제 및 예외 처리

### CPPCheck 억제

**인라인 방식:**
```cpp
// cppcheck-suppress unusedFunction
void internal_function() { }
```

**파일 무시:**
프로젝트 루트에 `.cppcheckignore` 파일:
```
test/
benchmark/
*.generated.cpp
```

### Clang-Tidy 억제

**인라인 방식:**
```cpp
// NOLINT
int unused = 5;

// NOLINTBEGIN ... NOLINTEND
// NOLINT(specific-check)
```

**체크 비활성화:**
`.clang-tidy`의 `Checks` 항목에서 제외:
```yaml
Checks: "*,-google-*,-fuchsia-*"
```

---

## 6. 결과 해석

### CPPCheck 심각도

| 레벨 | 의미 | 조치 |
|------|------|------|
| 🔴 error | 컴파일 실패 가능 | 즉시 수정 |
| 🟠 warning | 런타임 오류 가능 | 빠른 수정 |
| 🟡 style | 코드 스타일 문제 | 다음 PR에 수정 |
| 🟢 performance | 성능 개선 제안 | 검토 후 적용 |

### Clang-Tidy 결과

```
file.cpp:10:5: warning: variable 'x' set but not used [clang-diagnostic-unused-variable]
file.cpp:15:10: note: used here
```

**구성:**
- `file.cpp:10:5`: 파일, 라인, 열 위치
- `[category]`: 검사 범주
- `note`: 추가 정보

---

## 7. 성능 고려사항

### 실행 시간

| 도구 | 시간 | 컴파일 필요 |
|------|------|-----------|
| CPPCheck | ~30초 | ❌ 불필요 |
| Clang-Tidy | ~2-3분 | ✅ 필요 |

### 최적화

- CPPCheck: 병렬 처리 (-j 옵션)
- Clang-Tidy: 필요한 검사만 활성화

---

## 8. 향후 개선

- [ ] 정적 분석 결과 대시보드
- [ ] 성능 회귀 검출
- [ ] SonarQube 통합
- [ ] MISRA C++ 준수 검사
- [ ] 자동 수정 자동화

---

**문서 마지막 업데이트**: 2026-03-16
