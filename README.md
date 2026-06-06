# Crimson Editor

Windows용 프리웨어 텍스트 편집기. 1999~2005년에 개발되었으며, 신택스 하이라이팅, 다중 문서 인터페이스(MDI), 프로젝트 워크스페이스, 통합 FTP 등을 지원한다.

- **버전**: 3.71 (Korean)
- **저작권**: © 1999–2005 Ingyu Kang
- **빌드 환경**: Visual Studio 2026 (v145 toolset), MFC (DLL 공유), Win32 / x86, MBCS

---

## TODO

- [x] **VS 2026 이관 완료** — `.dsp`/`.dsw` → `.vcxproj`/`.sln`, v145 toolset, MFC Dynamic + MBCS. 옛 VC6 빌드 파일은 `cedt371-original` 브랜치에 보존.
- [x] **소스/헤더 파일 디렉토리 정리 완료** — 평탄하던 ~180개 파일을 `src/{include,core,app,doc,view,frame,panels,dialogs,util,network}` 트리로 분리. `third_party/htmlhelp/`에 외부 SDK 동봉본 격리.
- [ ] **Unicode 빌드 검토** — 현재 MBCS만 지원. 한국어 입력기/IME 처리([cedtViewEditCompose.cpp](src/view/cedtViewEditCompose.cpp))와 코드 전반의 `char`/`TCHAR` 가정 검토 필요.

---

## 빌드 환경

[cedt.sln](cedt.sln) + [cedt.vcxproj](cedt.vcxproj) — Visual Studio 2026 빌드 시스템. 옛 VC6 빌드 파일(`.dsw`/`.dsp`/`.mak`/`.clw`)은 `cedt371-original` 브랜치에 보존되어 있다.

| 항목 | 값 |
| --- | --- |
| IDE | Visual Studio 2026 (18.x) |
| Toolset | `v145` |
| Target | Win32 (x86) Application |
| MFC | Dynamic (`_AFXDLL` 공유 DLL) |
| 문자셋 | `_MBCS` (멀티바이트) |
| 외부 라이브러리 | `imm32.lib`, `htmlhelp.lib` |
| 추가 헤더 | `HtmlHelp.h`, `HtmlHelp.lib` ([third_party/htmlhelp/](third_party/htmlhelp/)) |
| 미리 컴파일된 헤더 | [src/include/StdAfx.h](src/include/StdAfx.h) / [src/include/StdAfx.cpp](src/include/StdAfx.cpp) |

### 빌드 구성

Debug/Release × KR/US를 4가지 구성으로 분리한다. 각 구성에서 [cedt_kr.rc](cedt_kr.rc) / [cedt_us.rc](cedt_us.rc) 중 하나만 컴파일되어 한국어 EXE와 영어 EXE가 별도 산출된다.

| Configuration | 산출물 |
| --- | --- |
| `Debug-KR \| Win32`   | `Debug-KR\cedt_kr.exe` |
| `Release-KR \| Win32` | `Release-KR\cedt_kr.exe` |
| `Debug-US \| Win32`   | `Debug-US\cedt_us.exe` |
| `Release-US \| Win32` | `Release-US\cedt_us.exe` |

### 빌드 준비

Visual Studio Installer의 **개별 구성 요소** 탭에서 다음을 추가 설치한다 (VS 2017 이후 기본 포함이 아님):

- **C++ MFC for latest v145 build tools (x86 & x64)** — MFC 본체. 최신 VS에서는 MBCS 라이브러리도 이 컴포넌트에 통합되어 있다.

### 참고

- 저장소에 동봉된 [HtmlHelp.lib](third_party/htmlhelp/HtmlHelp.lib)는 옛 SDK 라이브러리로 `/SAFESEH`를 지원하지 않는다. Release 구성은 `ImageHasSafeExceptionHandlers=false`로 설정되어 있다.
- VC6 → 현대 toolset 이관 과정에서 한 호환성 처리는 다음과 같다:
    - `<fstream.h>` / `<strstrea.h>` → [fstream_compat.h](src/include/fstream_compat.h) shim으로 흡수 (글로벌 네임스페이스의 `ofstream` 등 using-선언)
    - `ios::nocreate` 제거 (비표준 — 현대 `ifstream`은 파일이 없으면 그냥 fail)
    - `istrstream` → `std::istringstream`, `eatwhite()` → `>> std::ws`
    - VC6 시절 for-loop 스코프 누수에 의존한 변수 재사용 자리에 타입 재선언
    - MFC 핸들러 시그니처 갱신: `OnActivateApp(BOOL, HTASK)` → `(BOOL, DWORD)`, `OnNcHitTest` 반환형 `UINT` → `LRESULT`
    - `CStringArray::GetAt()`의 반환형을 `const CString&`로 받기 (현대 MFC가 const 반환)

---

## 아키텍처 개요

전형적인 MFC **MDI Document/View** 구조이다.

```
CCedtApp  (src/app/cedtapp.h)        ── CWinApp 파생, 애플리케이션 진입점
   │
   ├─ CMainFrame  (src/frame/MainFrm.h)  ── CMDIFrameWnd, 메뉴/툴바/상태바/도킹창
   │     ├─ CStatusBarEx
   │     ├─ CToolBar
   │     ├─ CMDIFileTab              ── 상단 파일 탭 컨트롤
   │     ├─ CFileWindow              ── 좌측 도킹: Directory/Project/Remote 패널
   │     └─ COutputWindow            ── 하단 도킹: 외부 도구 출력 콘솔
   │
   ├─ CChildFrame (src/frame/ChildFrm.h) ── CMDIChildWnd, 분할창(SplitterWndEx)
   │
   ├─ CCedtDoc    (src/doc/cedtDoc.h)    ── CDocument, 텍스트 버퍼/언두/신택스/파일 IO
   │     └─ CAnalyzedText : CList<CAnalyzedString, LPCTSTR>
   │
   └─ CCedtView   (src/view/cedtView.h)  ── CView, 캐럿/그리기/입력/매크로/명령
```

### 핵심 도메인 클래스 ([src/core/cedtElement.h](src/core/cedtElement.h))

| 클래스 | 역할 |
| --- | --- |
| `CAnalyzedString` / `CAnalyzedText` | 줄 단위 분석 결과를 담는 텍스트 버퍼 |
| `CFormatedString` / `CFormatedText` | 화면 워드랩/포맷팅 결과 |
| `CLangSpec`, `CKeywords` | 언어 사양과 키워드 맵 |
| `CDictionary` | 단어 자동완성용 사전 |
| `CUndoBuffer` | 언두/리두 히스토리 |
| `CUserCommand`, `CMacroBuffer` | 사용자 도구·매크로 |
| `CSyntaxType`, `COutputPattern`, `CFileFilter` | 신택스/출력 파서/파일 필터 설정 |
| `CFtpAccount` | FTP 접속 정보 |
| `CMemText` | 메모리 상의 임시 텍스트 |

---

## 소스 파일 그룹

`.cpp`/`.h`는 [src/](src/) 아래 도메인별 디렉토리로 분리되어 있다. 큰 클래스(`CCedtApp`, `CCedtDoc`, `CCedtView`, `CMainFrame`, `CFileWindow`)는 기능별로 여러 `.cpp`에 분산되어 있다.

### 1. 애플리케이션 (`CCedtApp`) — [src/app/](src/app/)

| 파일 | 역할 |
| --- | --- |
| [cedtapp.cpp](src/app/cedtapp.cpp), [cedtapp.h](src/app/cedtapp.h) | `InitInstance`, 메시지 맵, 글로벌 상태 |
| [cedtAppConf.cpp](src/app/cedtAppConf.cpp) | 사용자 설정/색상 스킴/매크로 저장·로드 |
| [cedtAppDirectory.cpp](src/app/cedtAppDirectory.cpp) | Directory 패널 명령 처리 |
| [cedtAppFile.cpp](src/app/cedtAppFile.cpp) | 문서 열기/저장/스폰, 셸 커맨드 처리 |
| [cedtAppFilter.cpp](src/app/cedtAppFilter.cpp) | 파일 필터 갱신·콜백 |
| [cedtAppHndr.cpp](src/app/cedtAppHndr.cpp) | 메뉴/명령 핸들러 (폰트·탭·색상 등) |
| [cedtAppProject.cpp](src/app/cedtAppProject.cpp) | 프로젝트 워크스페이스 |
| [cedtAppRegistry.cpp](src/app/cedtAppRegistry.cpp) | 레지스트리 입출력 (셸 통합 포함) |
| [cedtAppSearch.cpp](src/app/cedtAppSearch.cpp) | Find In Files |
| [cedtAppView.cpp](src/app/cedtAppView.cpp) | 모든 뷰 일괄 갱신 |

### 2. 문서 (`CCedtDoc`) — [src/doc/](src/doc/)

| 파일 | 역할 |
| --- | --- |
| [cedtDoc.cpp](src/doc/cedtDoc.cpp), [cedtDoc.h](src/doc/cedtDoc.h) | 문서 기본/플래그/원격 경로 |
| [cedtDocAnal.cpp](src/doc/cedtDocAnal.cpp) | 텍스트 분석 (워드 토큰화) |
| [cedtDocDictionary.cpp](src/doc/cedtDocDictionary.cpp) | 자동완성 사전 |
| [cedtDocEdit.cpp](src/doc/cedtDocEdit.cpp), [cedtDocEditAdv.cpp](src/doc/cedtDocEditAdv.cpp) | 편집 연산 (기본/고급) |
| [cedtDocFile.cpp](src/doc/cedtDocFile.cpp) | 인코딩(ASCII/Unicode/UTF-8)·줄끝(DOS/UNIX/MAC)·백업 |
| [cedtDocHndr.cpp](src/doc/cedtDocHndr.cpp) | 문서 명령 핸들러 |
| [cedtDocMap.cpp](src/doc/cedtDocMap.cpp) | 라인 좌표 매핑 |
| [cedtDocSearch.cpp](src/doc/cedtDocSearch.cpp) | 검색·치환 (RegExp 포함) |
| [cedtDocSyntax.cpp](src/doc/cedtDocSyntax.cpp) | 신택스/언어 사양 적용 |
| [cedtDocUndo.cpp](src/doc/cedtDocUndo.cpp) | 언두/리두 |
| [cedtDocView.cpp](src/doc/cedtDocView.cpp) | 뷰 동기화 |

### 3. 뷰 (`CCedtView`) — [src/view/](src/view/)

| 파일 | 역할 |
| --- | --- |
| [cedtView.cpp](src/view/cedtView.cpp), [cedtView.h](src/view/cedtView.h) | 뷰 기본/정적 멤버 |
| [cedtViewAction.cpp](src/view/cedtViewAction.cpp) | 사용자 액션 디스패치 |
| [cedtViewCaret.cpp](src/view/cedtViewCaret.cpp) | 캐럿 위치/모양 |
| [cedtViewCommand.cpp](src/view/cedtViewCommand.cpp) | 사용자 도구 실행, 자식 프로세스 IO |
| [cedtViewDraw.cpp](src/view/cedtViewDraw.cpp) | 화면 렌더링 |
| [cedtViewEdit.cpp](src/view/cedtViewEdit.cpp), [cedtViewEditAdv.cpp](src/view/cedtViewEditAdv.cpp) | 편집 명령 |
| [cedtViewEditCompose.cpp](src/view/cedtViewEditCompose.cpp) | IME 조합 처리 |
| [cedtViewEvent.cpp](src/view/cedtViewEvent.cpp) | 키보드/마우스 이벤트 |
| [cedtViewFont.cpp](src/view/cedtViewFont.cpp) | 폰트 메트릭 |
| [cedtViewFormat.cpp](src/view/cedtViewFormat.cpp) | 워드랩·포맷팅 |
| [cedtViewHighlight.cpp](src/view/cedtViewHighlight.cpp) | 신택스 하이라이팅 |
| [cedtViewHndrEdit.cpp](src/view/cedtViewHndrEdit.cpp), [cedtViewHndrMisc.cpp](src/view/cedtViewHndrMisc.cpp) | 메뉴 핸들러 |
| [cedtViewMacro.cpp](src/view/cedtViewMacro.cpp) | 매크로 기록/재생 |
| [cedtViewMap.cpp](src/view/cedtViewMap.cpp), [cedtViewMapAdv.cpp](src/view/cedtViewMapAdv.cpp) | 화면↔문서 좌표 변환 |
| [cedtViewMetric.cpp](src/view/cedtViewMetric.cpp) | 글자/줄 메트릭 |
| [cedtViewMisc.cpp](src/view/cedtViewMisc.cpp) | 기타 |
| [cedtViewMove.cpp](src/view/cedtViewMove.cpp) | 캐럿 이동 |
| [cedtViewPrint.cpp](src/view/cedtViewPrint.cpp) | 인쇄/미리보기 |
| [cedtViewScroll.cpp](src/view/cedtViewScroll.cpp) | 스크롤 |
| [cedtViewSearch.cpp](src/view/cedtViewSearch.cpp) | 검색 (뷰 측) |
| [cedtViewSelect.cpp](src/view/cedtViewSelect.cpp) | 선택 영역 |
| [cedtViewUndo.cpp](src/view/cedtViewUndo.cpp) | 뷰 측 언두 연동 |

### 4. 메인 프레임 / 자식 프레임 — [src/frame/](src/frame/)

| 파일 | 역할 |
| --- | --- |
| [MainFrm.cpp](src/frame/MainFrm.cpp), [MainFrm.h](src/frame/MainFrm.h) | `CMDIFrameWnd`, 도킹 컨트롤 모음 |
| [MainFrmHndr.cpp](src/frame/MainFrmHndr.cpp) | 메인 프레임 명령 핸들러 |
| [MainFrmDropTarget.cpp](src/frame/MainFrmDropTarget.cpp), [MainFrmDropTarget.h](src/frame/MainFrmDropTarget.h) | OLE Drag&Drop |
| [ChildFrm.cpp](src/frame/ChildFrm.cpp), [ChildFrm.h](src/frame/ChildFrm.h) | MDI 자식, 분할창 |
| [SplitterWndEx.cpp](src/frame/SplitterWndEx.cpp), [SplitterWndEx.h](src/frame/SplitterWndEx.h) | 확장 분할창 |
| [StatusBarEx.cpp](src/frame/StatusBarEx.cpp), [StatusBarEx.h](src/frame/StatusBarEx.h) | 진행률·스플래시 메시지 가능한 상태바 |

### 5. 도킹 패널 — [src/panels/](src/panels/)

| 파일 | 역할 |
| --- | --- |
| [SizeCBar.cpp](src/panels/SizeCBar.cpp), [SizeCBar.h](src/panels/SizeCBar.h) | 리사이즈 가능한 컨트롤 바 베이스 |
| [FileWnd.cpp](src/panels/FileWnd.cpp), [FileWnd.h](src/panels/FileWnd.h) | 좌측 패널 (Directory / Project / Remote) |
| [FileWndDirectory.cpp](src/panels/FileWndDirectory.cpp) | Directory 트리 |
| [FileWndProject.cpp](src/panels/FileWndProject.cpp) | 프로젝트 트리 |
| [FileWndRemote.cpp](src/panels/FileWndRemote.cpp) | 원격(FTP) 트리 |
| [FileWndDropTarget.cpp](src/panels/FileWndDropTarget.cpp) | 패널 D&D 타겟 |
| [OutputWindow.cpp](src/panels/OutputWindow.cpp), [OutputWindow.h](src/panels/OutputWindow.h) | 하단 출력/입력 콘솔 |
| [FileTab.cpp](src/panels/FileTab.cpp), [FileTab.h](src/panels/FileTab.h) | MDI 파일 탭 컨트롤 |
| [FileTabDropTarget.cpp](src/panels/FileTabDropTarget.cpp) | 파일 탭 D&D |
| [XPTabCtrl.cpp](src/panels/XPTabCtrl.cpp), [XPTabCtrl.h](src/panels/XPTabCtrl.h) | XP 스타일 탭 컨트롤 |

### 6. 다이얼로그 — [src/dialogs/](src/dialogs/)

#### 환경설정 (Preferences) — [src/dialogs/preferences/](src/dialogs/preferences/)

[prefdialog.cpp](src/dialogs/preferences/prefdialog.cpp), [prefdialog.h](src/dialogs/preferences/prefdialog.h)을 시트로 하는 탭 페이지들:

- [PrefDialogGeneral.cpp](src/dialogs/preferences/PrefDialogGeneral.cpp) — 일반
- [PrefDialogFile.cpp](src/dialogs/preferences/PrefDialogFile.cpp) — 파일/인코딩
- [PrefDialogBackup.cpp](src/dialogs/preferences/PrefDialogBackup.cpp) — 백업
- [PrefDialogDirectory.cpp](src/dialogs/preferences/PrefDialogDirectory.cpp) — 작업 디렉토리
- [PrefDialogVisual.cpp](src/dialogs/preferences/PrefDialogVisual.cpp) — 시각 효과
- [PrefDialogColors.cpp](src/dialogs/preferences/PrefDialogColors.cpp) — 색상
- [PrefDialogFonts.cpp](src/dialogs/preferences/PrefDialogFonts.cpp) — 폰트
- [PrefDialogSyntax.cpp](src/dialogs/preferences/PrefDialogSyntax.cpp) — 신택스
- [PrefDialogPrint.cpp](src/dialogs/preferences/PrefDialogPrint.cpp) — 인쇄
- [PrefDialogOutput.cpp](src/dialogs/preferences/PrefDialogOutput.cpp) — 출력 창
- [PrefDialogTools.cpp](src/dialogs/preferences/PrefDialogTools.cpp) — 사용자 도구
- [PrefDialogCommands.cpp](src/dialogs/preferences/PrefDialogCommands.cpp) — 명령
- [PrefDialogMacros.cpp](src/dialogs/preferences/PrefDialogMacros.cpp) — 매크로
- [PrefDialogFilters.cpp](src/dialogs/preferences/PrefDialogFilters.cpp) — 파일 필터
- [PrefDialogAssoc.cpp](src/dialogs/preferences/PrefDialogAssoc.cpp) — 파일 연결

#### 기능별 다이얼로그

| 파일 | 역할 |
| --- | --- |
| [AboutDialog.cpp](src/dialogs/AboutDialog.cpp) | About |
| [FindDialog.cpp](src/dialogs/FindDialog.cpp), [ReplaceDialog.cpp](src/dialogs/ReplaceDialog.cpp), [AskReplaceDialog.cpp](src/dialogs/AskReplaceDialog.cpp) | 찾기/치환 |
| [FindInFilesDialog.cpp](src/dialogs/FindInFilesDialog.cpp) | 파일에서 찾기 |
| [GoToDialog.cpp](src/dialogs/GoToDialog.cpp) | 줄 번호로 이동 |
| [FolderDialog.cpp](src/util/FolderDialog.cpp) | 폴더 선택 (util 그룹에 위치) |
| [ReloadAsDialog.cpp](src/dialogs/ReloadAsDialog.cpp) | 인코딩 재지정 후 리로드 |
| [UserInputDialog.cpp](src/dialogs/UserInputDialog.cpp) | 매크로/명령용 입력 |
| [MacroDefineDialog.cpp](src/dialogs/MacroDefineDialog.cpp) | 매크로 정의 |
| [DocumentSummary.cpp](src/dialogs/DocumentSummary.cpp) | 문서 요약 |
| [DummyDialog.cpp](src/dialogs/DummyDialog.cpp) | 자리잡이 |
| FTP 관련 | [FtpSettingsDialog.cpp](src/dialogs/FtpSettingsDialog.cpp), [FtpAdvancedDialog.cpp](src/dialogs/FtpAdvancedDialog.cpp), [FtpPasswordDialog.cpp](src/dialogs/FtpPasswordDialog.cpp), [FtpTransferDialog.cpp](src/dialogs/FtpTransferDialog.cpp), [OpenRemoteDialog.cpp](src/dialogs/OpenRemoteDialog.cpp) |

### 7. 네트워크 / 원격 파일 — [src/network/](src/network/)

| 파일 | 역할 |
| --- | --- |
| [FtpClnt.cpp](src/network/FtpClnt.cpp), [FtpClnt.h](src/network/FtpClnt.h) | FTP 클라이언트 |
| [RemoteFile.cpp](src/network/RemoteFile.cpp), [RemoteFile.h](src/network/RemoteFile.h) | 원격 파일 처리 |

### 8. 유틸리티 / 공통 — [src/util/](src/util/)

| 파일 | 역할 |
| --- | --- |
| [Utility.cpp](src/util/Utility.cpp), [Utility.h](src/util/Utility.h) | 헬퍼 함수 |
| [PathName.cpp](src/util/PathName.cpp), [PathName.h](src/util/PathName.h) | 경로 조작 |
| [RegExp.cpp](src/util/RegExp.cpp), [RegExp.h](src/util/RegExp.h) | 정규식 엔진 ([RegExp.html](RegExp.html) 문서 동봉) |
| [SortStringArray.cpp](src/util/SortStringArray.cpp) | 정렬되는 문자열 배열 |
| [ColorListBox.cpp](src/util/ColorListBox.cpp) | 색상 콤보용 리스트박스 |
| [HyperLink.cpp](src/util/HyperLink.cpp) | 클릭 가능한 하이퍼링크 스태틱 |
| [VerticalStatic.cpp](src/util/VerticalStatic.cpp) | 수직 텍스트 스태틱 |
| [Separator.cpp](src/util/Separator.cpp) | 구분선 컨트롤 |
| [FolderDialog.cpp](src/util/FolderDialog.cpp) | 폴더 선택 다이얼로그 |
| [CmdLine.cpp](src/util/CmdLine.cpp) | 커맨드라인 파서 (인스턴스 간 IPC 포함) |
| [registry.cpp](src/util/registry.cpp), [registry.h](src/util/registry.h) | 레지스트리 헬퍼 |
| [encode.cpp](src/util/encode.cpp), [encode.h](src/util/encode.h) | 문자 인코딩 변환 |
| [date.cpp](src/util/date.cpp), [date.h](src/util/date.h) | 날짜·시각 포맷 |
| [evaluate.cpp](src/util/evaluate.cpp), [evaluate.h](src/util/evaluate.h) | 수식 평가기 |

### 9. 전역 헤더 / PCH — [src/include/](src/include/)

| 파일 | 역할 |
| --- | --- |
| [StdAfx.cpp](src/include/StdAfx.cpp), [StdAfx.h](src/include/StdAfx.h) | PCH (MFC 헤더 일괄 포함) |
| [cedtHeader.h](src/include/cedtHeader.h) | 전역 매크로/상수, 헤더 일괄 포함 |
| [cedtColors.h](src/include/cedtColors.h) | 색상 인덱스 정의 |
| [resource.h](src/include/resource.h) | 리소스 ID |
| [fstream_compat.h](src/include/fstream_compat.h) | VC6 `<fstream.h>` 호환 shim |

### 10. 도메인 코어 — [src/core/](src/core/)

[cedtElement.h](src/core/cedtElement.h) / [cedtElement.cpp](src/core/cedtElement.cpp) 한 쌍 안에 50+ 전역 상수 + 13개 도메인 클래스가 모여 있다. App/Doc/View 모든 모듈이 의존하는 "프로젝트 코어 정의" 단위 — 위의 "핵심 도메인 클래스" 표 참고.

### 11. 리소스

| 파일 | 역할 |
| --- | --- |
| [cedt_kr.rc](cedt_kr.rc) | 한국어 리소스 |
| [cedt_us.rc](cedt_us.rc) | 영어 리소스 |
| [res/](res/) | 아이콘, 비트맵, 커서, 매니페스트, `cedt.rc2` |
| [third_party/htmlhelp/](third_party/htmlhelp/) | HTML Help SDK 동봉본 (`HtmlHelp.h`, `HtmlHelp.lib`) |

---

## 디렉토리 구조 (요약)

```
CrimsonEditor/
├── cedt.sln, cedt.vcxproj, cedt.vcxproj.filters  # Visual Studio 2026
├── README.md, CLAUDE.md, LICENSE, .gitignore
├── cedt_kr.rc, cedt_us.rc                # 리소스 (KR/US 분리 빌드)
├── res/                                  # 아이콘·비트맵·커서·매니페스트, cedt.rc2
├── third_party/
│   └── htmlhelp/                         # HtmlHelp.h, HtmlHelp.lib (옛 SDK 동봉)
└── src/
    ├── include/                          # 전역 헤더 + PCH + 호환 shim
    │     ├── StdAfx.{h,cpp}              #   PCH
    │     ├── cedtHeader.h, cedtColors.h
    │     ├── resource.h
    │     └── fstream_compat.h            #   VC6 <fstream.h> 호환 shim
    ├── core/                             # 도메인 코어 (cedtElement)
    ├── app/                              # CCedtApp + cedtApp*.cpp
    ├── doc/                              # CCedtDoc + cedtDoc*.cpp
    ├── view/                             # CCedtView + cedtView*.cpp
    ├── frame/                            # MainFrm/ChildFrm/Splitter/StatusBar
    ├── panels/                           # FileWnd/FileTab/OutputWindow/SizeCBar/XPTabCtrl
    ├── dialogs/                          # 기능 다이얼로그
    │     └── preferences/                #   prefdialog + PrefDialog* 페이지
    ├── network/                          # FtpClnt, RemoteFile
    └── util/                             # Utility/PathName/RegExp/encode/...
```

---

## 셸 통합 / 다중 인스턴스

- **싱글 인스턴스 IPC**: 명명된 뮤텍스 `CrimsonEditor.CmdLine`로 첫 인스턴스를 감지하고, `WM_ANOTHER_INSTANCE` 메시지를 첫 창으로 보낸다 ([cedtapp.cpp:22](src/app/cedtapp.cpp#L22) 부근의 `#pragma data_seg("Shared")` 참고).
- **셸 확장**: `HKCR\*\shellex\ContextMenuHandlers\Crimson Editor` 등록으로 우클릭 메뉴 통합.
- **IE 통합**: "View Source Editor"로 등록 가능.

상수는 [src/include/cedtHeader.h](src/include/cedtHeader.h) 상단에 모여 있다.
