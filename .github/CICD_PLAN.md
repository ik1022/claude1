# CI/CD 파이프라인 계획 (Week 7)

## 개요
GitHub Actions를 사용하여 자동화된 CI/CD 파이프라인 구축

## 워크플로우 구조

### 1. **build-test.yml** (Pull Request / Push)
**트리거**: PR 생성, main으로 push
**목적**: 빌드, 테스트, 커버리지 검수

단계:
1. Linux 호스트 빌드 (GCC)
2. 단위 테스트 실행
3. 코드 커버리지 수집 (gcov/lcov)
4. 커버리지 리포트 생성 (70% 이상)
5. 벤치마크 실행 (성능 기준선)

### 2. **coverage-report.yml** (Main branch)
**트리거**: main으로 merge
**목적**: 커버리지 리포트 업로드 및 배포

단계:
1. 커버리지 데이터 수집
2. HTML 리포트 생성
3. GitHub Pages에 배포 (선택사항)

### 3. **benchmark-report.yml** (Main branch)
**트리거**: main으로 merge
**목적**: 성능 벤치마크 결과 저장 및 분석

단계:
1. 벤치마크 실행
2. JSON 결과 수집
3. 분석 스크립트 실행
4. 결과를 artifacts로 저장

### 4. **cross-compile.yml** (Scheduled / Manual)
**트리거**: 수동 trigger, 주 1회 자동 실행
**목적**: ARM 크로스컴파일 검증

단계:
1. ARM toolchain 설치 (arm-none-eabi-gcc)
2. STM32 크로스컴파일
3. 바이너리 크기 분석

## 필요한 의존성 관리

### GitHub Actions에서 설치할 패키지:
- Ubuntu 기본 도구: cmake, gcc, g++
- 테스트: google-test (FetchContent로 다운로드)
- 커버리지: gcov, lcov
- 벤치마크: google-benchmark (FetchContent로 다운로드)
- 크로스컴파일: arm-none-eabi-gcc (apt-get)

## 성공 기준

✅ **필수**:
- [ ] 호스트 빌드 성공 (GCC, -Wall -Werror)
- [ ] 모든 단위 테스트 통과
- [ ] 코드 커버리지 ≥ 70%
- [ ] 벤치마크 실행 성공

⚠️ **권장**:
- [ ] 성능 회귀 검출 (벤치마크 결과 비교)
- [ ] 커버리지 리포트 공개
- [ ] ARM 크로스컴파일 검증

## 구현 일정

**Week 7-1**: build-test.yml 작성 및 테스트
**Week 7-2**: coverage-report.yml 및 benchmark-report.yml
**Week 7-3**: cross-compile.yml 및 전체 통합 테스트
