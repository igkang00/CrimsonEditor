# Testing

Unit tests for Crimson Editor live in [../tests/](../tests/) as a separate `cedt_tests` project inside the same solution. Built with **Google Test** pulled in through **vcpkg manifest mode** — [../tests/vcpkg.json](../tests/vcpkg.json) declares the `gtest` dependency, and vcpkg downloads/builds it on the first test build.

| Item | Value |
| --- | --- |
| Framework | Google Test (vcpkg `gtest`) |
| Project | [../tests/cedt_tests.vcxproj](../tests/cedt_tests.vcxproj) |
| Configurations | `Debug \| Win32`, `Release \| Win32` |
| C++ standard | C++17 (gtest requirement) |
| MFC / charset | Dynamic + `_MBCS` (matches the main app) |
| Subsystem | Console |
| Entry point | `gtest_main` (no custom `main()` yet — see §3) |

Current coverage: **60 tests across 12 suites, all green.**

---

## 1. Modules under test

**Leaf utilities** — minimal MFC dependency, pure algorithms / C-style functions:

| Module | Test file |
| --- | --- |
| [../src/util/RegExp.cpp](../src/util/RegExp.cpp) | [../tests/RegExp_test.cpp](../tests/RegExp_test.cpp) |
| [../src/util/evaluate.cpp](../src/util/evaluate.cpp) | [../tests/evaluate_test.cpp](../tests/evaluate_test.cpp) |
| [../src/util/date.cpp](../src/util/date.cpp) | [../tests/date_test.cpp](../tests/date_test.cpp) |
| [../src/util/encode.cpp](../src/util/encode.cpp) | [../tests/encode_test.cpp](../tests/encode_test.cpp) |
| [../src/util/PathName.cpp](../src/util/PathName.cpp) | [../tests/PathName_test.cpp](../tests/PathName_test.cpp) |

**Domain containers and language spec** — pull in `CString` / `CFile` / MFC containers:

| Module | Test file |
| --- | --- |
| [../src/util/SortStringArray.cpp](../src/util/SortStringArray.cpp) | [../tests/SortStringArray_test.cpp](../tests/SortStringArray_test.cpp) |
| [../src/core/cedtElement.cpp](../src/core/cedtElement.cpp) → `CMemText` | [../tests/CMemText_test.cpp](../tests/CMemText_test.cpp) |
| [../src/core/cedtElement.cpp](../src/core/cedtElement.cpp) → `CUndoBuffer` | [../tests/CUndoBuffer_test.cpp](../tests/CUndoBuffer_test.cpp) |
| [../src/core/cedtElement.cpp](../src/core/cedtElement.cpp) → `CKeywords` | [../tests/CKeywords_test.cpp](../tests/CKeywords_test.cpp) |
| [../src/core/cedtElement.cpp](../src/core/cedtElement.cpp) → `CDictionary` | [../tests/CDictionary_test.cpp](../tests/CDictionary_test.cpp) |
| [../src/core/cedtElement.cpp](../src/core/cedtElement.cpp) → `CLangSpec` | [../tests/CLangSpec_test.cpp](../tests/CLangSpec_test.cpp) |
| [../src/core/cedtElement.cpp](../src/core/cedtElement.cpp) → `CAnalyzedString` | [../tests/CAnalyzedString_test.cpp](../tests/CAnalyzedString_test.cpp) |

The remaining "hard" group is the Doc / View / Frame / Dialog layer; it needs a UI-simulation strategy and is not covered yet — see §3.

---

## 2. Running locally

Prerequisite: vcpkg installed and integrated user-wide (`vcpkg integrate install`).

```
msbuild tests\cedt_tests.vcxproj /p:Configuration=Debug /p:Platform=Win32
tests\Debug\cedt_tests.exe
```

(Both commands assume the repository root as the working directory.)

The test project includes the source-under-test `.cpp` files directly in its `ClCompile` list, so the main `cedt.vcxproj` is untouched. New tests should follow the same `<Module>_test.cpp` naming and be added both as a `ClCompile` entry in [../tests/cedt_tests.vcxproj](../tests/cedt_tests.vcxproj) and as a `<Filter>Tests</Filter>` entry in [../tests/cedt_tests.vcxproj.filters](../tests/cedt_tests.vcxproj.filters).

---

## 3. Roadmap beyond unit tests

The current `cedt_tests` covers pure algorithms and MFC data containers. Extending tests further — into `CCedtDoc`, `CCedtView`, `CMainFrame`, or dialogs — requires more infrastructure. Based on community practice (sources in §4), the planned approach is three layers:

| Layer | Project | Scope | Status |
| --- | --- | --- | --- |
| **L1 — Unit** | [../tests/cedt_tests.vcxproj](../tests/cedt_tests.vcxproj) | Algorithms, containers, domain classes that do not need `CWinApp` | Done (60 tests) |
| **L2 — Integration** | `cedt_integration_tests.vcxproj` (planned) | `CCedtDoc` and other CWinApp-dependent code; no real windows | Planned |
| **L3 — End-to-end** | `tests/e2e/` (planned) | Real `cedt_*.exe` driven by external UI automation | Planned |

### 3.1 Prerequisite for L2 — extract `cedt_core` as a **static library**, not a DLL

Two technical needs collide once we move past pure algorithm code:

1. **One `CWinApp` per process.** The main app instantiates `CCedtApp`. A test EXE that wants to call into MFC code must also create its own `CWinApp`. Two `CWinApp`s in the same binary will not initialise cleanly, so the testable code has to live in a library that itself contains no `CWinApp` — only the main EXE (and only the test EXE) gets to declare one.
2. **MFC must be initialised manually in a console test runner.** `gtest_main` is not enough; the test runner needs a custom `main()` that calls `AfxWinInit(GetModuleHandle(NULL), NULL, GetCommandLine(), 0)` before `RUN_ALL_TESTS()`.

The "library" can be either a DLL or a static `.lib`. **Static `.lib` is recommended here** for two project-specific reasons:

- **Release artefacts stay the same** — only `cedt_kr.exe` / `cedt_us.exe`. No extra DLL to ship, install, or version.
- **No resource-handle juggling** — dialog templates and other resources stay in the EXE just as today. A DLL split would force `AfxSetResourceHandle` calls in the right places.

Trade-off accepted: the core code is link-duplicated into the two EXEs and into the test EXE, costing some disk and some extra link time. Negligible at this project's size.

### 3.2 Why `CWnd`-derived classes resist mocking

`CWnd` message handlers are not virtual functions and depend on a live `HWND` and a message pump. Google Mock does not give you a handle on them. The common recommendation in the references below is to **extract logic into free functions or non-`CWnd` classes** and unit-test those, rather than trying to mock the UI surface directly. That keeps L2 productive without fighting MFC.

### 3.3 L3 (E2E) tooling candidates

Picking a UI-automation tool is itself a sub-decision; the usual candidates are WinAppDriver (Microsoft, currently in maintenance mode), FlaUI (.NET), PyWinAuto (Python), and AutoHotkey. Worth deferring until there is a concrete GUI regression that hurts.

---

## 4. Sources reviewed

- [MFC + GoogleTests in 10 Easy Steps (Google Groups)](https://groups.google.com/g/googletestframework/c/Zt4YtgT4dfo) — historical guide; the comments record its known pitfalls.
- [Using google test with MFC applications (TechnoGems)](http://technogems.blogspot.com/2012/11/using-google-test-with-mfc-applications.html) — custom `main()` + `AfxWinInit` pattern.
- [How to test MFC CWnd based classes using google test/mock?](https://unittesting1.blogspot.com/2016/04/how-to-test-mfc-cwnd-based-classes.html) — why direct mocking of `CWnd` fails.
- [Unit testing MFC with MSTest (JAWS)](https://msujaws.wordpress.com/2009/05/06/unit-testing-mfc-with-mstest/) — the CWinApp-conflict / library-extraction recommendation is framework-independent.
- [MFC Testing framework (CodeProject)](https://www.codeproject.com/Articles/1326/MFC-Testing-framework) — older, but the structural idea (drive a hidden dialog from a test runner) is reusable.
- [AfxWinInit — Microsoft Docs](https://learn.microsoft.com/en-us/cpp/mfc/reference/application-information-and-management) — official reference for the MFC bootstrap call.
