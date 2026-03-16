# GitHub Actions CI/CD Pipeline

libemb 프로젝트의 자동화된 CI/CD 파이프라인입니다.

## 워크플로우 개요

### 1. **Build & Test** (`build-test.yml`)

**실행 타이밍**:
- PR 생성/업데이트 시
- main으로 푸시 시
- 매주 월요일 자동 실행

**수행 작업**:
- ✅ 호스트 빌드 (GCC, Release)
  - 기본 빌드 (코드 커버리지 활성화)
  - AddressSanitizer 빌드 (메모리 안전성 검사)
- ✅ 단위 테스트 실행
- ✅ 코드 커버리지 수집
- ✅ 성능 벤치마크 실행
- ✅ ARM 크로스컴파일 테스트

**성공 기준**:
```
- 모든 테스트 통과
- 코드 커버리지 ≥ 70%
- 벤치마크 실행 완료
- ARM 빌드 성공
```

**Artifacts**:
- `coverage-report-*`: lcov 커버리지 HTML 리포트
- `benchmark-results-*`: 성능 벤치마크 JSON 결과
- `build-logs-*`: 빌드 실패 로그 (실패 시에만)

---

### 2. **Code Coverage Report** (`coverage-report.yml`)

**실행 타이밍**:
- main으로 푸시 시
- Build & Test 워크플로우 완료 후

**수행 작업**:
- ✅ 테스트 실행 및 커버리지 수집
- ✅ lcov HTML 리포트 생성
- ✅ 최소 커버리지 기준(70%) 검증
- ✅ Codecov에 업로드
- ✅ PR에 커버리지 코멘트 달기

**성공 기준**:
```
- 코드 커버리지 ≥ 70%
- Codecov 업로드 성공
```

**Artifacts**:
- `coverage-report/`: HTML 리포트 디렉토리
- `coverage_badge.json`: 배지용 JSON

---

### 3. **Performance Benchmark Report** (`benchmark-report.yml`)

**실행 타이밍**:
- main으로 푸시 시
- Build & Test 워크플로우 완료 후

**수행 작업**:
- ✅ 성능 벤치마크 실행
- ✅ JSON 결과 수집
- ✅ 분석 및 요약 생성
- ✅ 이전 실행과 비교
- ✅ PR에 결과 코멘트 달기

**성공 기준**:
```
- 벤치마크 실행 완료
- 결과 파일 생성
```

**Artifacts**:
- `benchmark-results/`: 벤치마크 JSON 및 분석
- `benchmark-baseline/`: 기준선 저장 (90일 보관)
- `benchmark-comparison/`: 이전 실행과의 비교

---

### 4. **ARM Cross-Compile Verification** (`cross-compile.yml`)

**실행 타이밍**:
- PR 생성/업데이트 시
- main으로 푸시 시
- 매주 목요일 자동 실행
- 수동 trigger 가능 (workflow_dispatch)

**지원 대상**:
- STM32F4
- ARM Cortex-M7

**수행 작업**:
- ✅ ARM toolchain 설치 및 검증
- ✅ 크로스컴파일
- ✅ 바이너리 크기 분석
- ✅ 심볼 테이블 분석
- ✅ PR에 빌드 결과 코멘트 달기

**성공 기준**:
```
- 크로스컴파일 성공
- 바이너리 생성 완료
```

**Artifacts**:
- `arm-build-*`: 빌드 결과 및 분석 리포트

---

## 실행 예시

### 수동 워크플로우 실행 (GitHub UI)

1. **Actions** 탭으로 이동
2. 원하는 워크플로우 선택
3. **Run workflow** 클릭
4. 옵션 선택 후 실행

```bash
# 또는 GitHub CLI 사용
gh workflow run build-test.yml -r main
gh workflow run cross-compile.yml -r main
```

---

## 성공 기준 및 대시보드

### 모든 워크플로우가 통과하기 위한 조건

| 워크플로우 | 조건 | 우선순위 |
|-----------|------|--------|
| Build & Test | 모든 테스트 통과 + 커버리지 ≥70% | ⭐⭐⭐ |
| Coverage Report | 커버리지 ≥70% | ⭐⭐⭐ |
| Benchmark Report | 벤치마크 실행 완료 | ⭐⭐ |
| Cross-Compile | ARM 빌드 성공 | ⭐⭐ |

### 배지 추가

README.md에 다음 배지를 추가할 수 있습니다:

```markdown
[![Build & Test](https://github.com/username/libemb/actions/workflows/build-test.yml/badge.svg)](https://github.com/username/libemb/actions/workflows/build-test.yml)
[![Coverage](https://codecov.io/gh/username/libemb/branch/main/graph/badge.svg)](https://codecov.io/gh/username/libemb)
```

---

## 로컬에서 CI/CD 파이프라인 재현

### 1. 빌드 및 테스트

```bash
# 호스트 빌드 (커버리지 활성화)
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_ENABLE_COVERAGE=ON

cmake --build build --parallel

# 테스트 실행
ctest --test-dir build --output-on-failure

# 커버리지 수집
bash scripts/generate_coverage.sh build

# 커버리지 검증
bash scripts/check_coverage.sh coverage_report/coverage.info
```

### 2. 성능 벤치마크

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DLIBEMB_BUILD_BENCHMARKS=ON

cmake --build build --parallel

# 벤치마크 실행
bash scripts/run_benchmarks.sh benchmark_results

# 분석
bash scripts/analyze_benchmarks.sh benchmark_results
```

### 3. ARM 크로스컴파일

```bash
# ARM toolchain 설치 (Ubuntu)
sudo apt-get install arm-none-eabi-gcc arm-none-eabi-g++

# 크로스컴파일
cmake -B build/arm \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake \
  -DLIBEMB_BUILD_TESTS=OFF

cmake --build build/arm --parallel

# 바이너리 분석
arm-none-eabi-size build/arm/<target>
```

---

## 트러블슈팅

### 1. "Code coverage requires GCC" 오류

**원인**: Clang으로 빌드 시도
**해결**: GitHub Actions는 GCC를 사용하므로 문제 없음. 로컬에서는 GCC 사용.

### 2. 벤치마크 실패

**원인**: Google Benchmark 다운로드 실패
**해결**: 네트워크 확인, 재시도 시 자동 실행

### 3. ARM 크로스컴파일 실패

**원인**: toolchain 파일 경로 오류
**확인**:
```bash
ls cmake/toolchains/arm-none-eabi.cmake
```

### 4. 커버리지 기준 미충족

**원인**: 테스트 커버리지 < 70%
**해결**: 새 코드에 대한 테스트 추가 필요

---

## 성능 최적화

### CI/CD 실행 시간 단축

**현재 구조**:
- Build & Test: ~2-3분
- Coverage Report: ~2-3분
- Benchmark: ~3-4분
- Cross-Compile: ~2-3분

**개선 옵션** (향후):
- 캐싱 활성화 (CMake, vcpkg)
- 병렬 실행 (matrix 확대)
- 비용 최적화 (self-hosted runner)

---

## 다음 단계

- [ ] Codecov 배지 추가
- [ ] GitHub Pages에서 커버리지 리포트 공개
- [ ] 성능 회귀 검출 자동화
- [ ] OWASP 정적 분석 통합
- [ ] 자동 release 생성

---

**문서 마지막 업데이트**: 2026-03-16
