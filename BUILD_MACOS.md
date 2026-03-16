# libemb macOS 빌드 가이드

## 📋 사전 요구사항

### 1. Xcode 설치
```bash
# Xcode 명령줄 도구 설치
xcode-select --install

# 또는 App Store에서 Xcode 설치
# https://apps.apple.com/us/app/xcode/id497799835
```

### 2. CMake 3.20+ 설치
```bash
# Homebrew를 통한 설치 (권장)
brew install cmake

# 또는 직접 설치
# https://cmake.org/download/
```

### 3. (선택) clang-format 설치
```bash
brew install clang-format
```

---

## 🏗️ 호스트 빌드 (macOS)

### 기본 빌드
```bash
# 테스트와 예제 포함
cmake -B build/host \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_BUILD_EXAMPLES=ON \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build/host
```

### 테스트 실행
```bash
ctest --test-dir build/host --output-on-failure
```

### AddressSanitizer 포함 빌드 (메모리 안전성)
```bash
cmake -B build/host_asan \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_ENABLE_ASAN=ON \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build/host_asan
ctest --test-dir build/host_asan --output-on-failure
```

---

## 🔧 문제 해결

### Issue 1: CMake가 C++17을 지원하지 않음
```bash
# Xcode 버전 확인
xcode-select -p

# 최신 버전으로 업데이트
sudo rm -rf /Library/Developer/CommandLineTools
sudo xcode-select --install
```

### Issue 2: GoogleTest 다운로드 실패
```bash
# 네트워크 확인
ping github.com

# CMake 캐시 정리 후 재시도
rm -rf build/host
cmake -B build/host \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_BUILD_EXAMPLES=ON
```

### Issue 3: Linking 에러 (-Wl,--gc-sections)
**✅ 자동 해결됨**: CMake가 자동으로 macOS 호환 옵션 사용

---

## 📊 예상 빌드 시간

| 빌드 유형 | 시간 | 설명 |
|---------|------|------|
| 첫 빌드 | ~30초 | GoogleTest 다운로드 + 컴파일 |
| 증분 빌드 | ~2초 | 변경사항만 컴파일 |
| Clean 빌드 | ~30초 | 처음부터 빌드 |

---

## ✅ 정상 빌드 출력

```
-- Detected CMAKE_CXX_COMPILER_ID: AppleClang
-- C++ Standard: 17
-- Configuring done
-- Generating done
-- Build files have been written to: /home/user/claude1/build/host

[ 10%] Built target libemb_driver_ssd1306
[ 20%] Built target gtest
...
[100%] Built target display_driver

Test project /home/user/claude1/build/host
    Start 1: test_RingBuffer
1/2 Test #1: test_RingBuffer ..................   Passed
    Start 2: test_Ssd1306
2/2 Test #2: test_Ssd1306 .....................   Passed

100% tests passed, 0 tests failed out of 2
```

---

## 🔗 참고 자료

- [CMake 공식 문서](https://cmake.org/documentation/)
- [Xcode 릴리스 노트](https://developer.apple.com/xcode/release-notes/)
- [libemb CLAUDE.md](./CLAUDE.md) - 전체 프로젝트 가이드

---

## 💡 팁

### 병렬 빌드로 속도 향상
```bash
cmake --build build/host -- -j$(sysctl -n hw.ncpu)
```

### 상세 빌드 정보 출력
```bash
cmake --build build/host --verbose
```

### 특정 대상만 빌드
```bash
cmake --build build/host --target test_RingBuffer
```

---

마지막 업데이트: 2026-03-16
