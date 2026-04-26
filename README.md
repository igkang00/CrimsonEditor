# Crimson Editor

Windows용 프리웨어 텍스트 편집기. 1999~2005년에 개발되었으며, 신택스 하이라이팅, 다중 문서 인터페이스(MDI), 프로젝트 워크스페이스, 통합 FTP 등을 지원한다.

- **버전**: 3.71 (Korean)
- **저작권**: © 1999–2005 Ingyu Kang
- **빌드 환경**: Microsoft Visual C++ 6.0, MFC 6 (DLL 공유), Win32 / x86, MBCS

---

## TODO

- [ ] **VS 2022로 이관** — `.dsp`/`.dsw` → `.vcxproj`/`.sln`. MFC MBCS 컴포넌트 별도 설치 필요.
- [ ] **소스/헤더 파일 디렉토리 정리** — 현재 약 180개 파일이 루트에 평평하게 있음. App/Doc/View/Frame/Panels/Dialogs/Network/Util/Core 등으로 분리 예정 (이관 후 진행).

---

## 빌드 환경

`cedt.dsw`(워크스페이스) + `cedt.dsp`(프로젝트) — Visual Studio 6.0 / Developer Studio 시절의 포맷이다. `cedt.mak`은 nmake용 export 결과.

| 항목 | 값 |
| --- | --- |
| Target | Win32 (x86) Application |
| MFC | Use MFC 6 (`_AFXDLL` 공유 DLL) |
| 문자셋 | `_MBCS` (멀티바이트) |
| 외부 라이브러리 | `imm32.lib`, `htmlhelp.lib` |
| 추가 헤더 | `HtmlHelp.h`, `HtmlHelp.lib` (저장소 루트에 동봉) |
| 구성 | `Win32 Release`, `Win32 Debug` |
| 미리 컴파일된 헤더 | `StdAfx.h` / `StdAfx.cpp` |

> VS 2022로 마이그레이션 시 "MFC for x86/x64 with MBCS support" 컴포넌트를 별도 설치해야 한다 (VS 2017 이후로는 기본 포함이 아님).

---

## 아키텍처 개요

전형적인 MFC **MDI Document/View** 구조이다.

```
CCedtApp  (cedtapp.h)            ── CWinApp 파생, 애플리케이션 진입점
   │
   ├─ CMainFrame  (MainFrm.h)    ── CMDIFrameWnd, 메뉴/툴바/상태바/도킹창
   │     ├─ CStatusBarEx
   │     ├─ CToolBar
   │     ├─ CMDIFileTab          ── 상단 파일 탭 컨트롤
   │     ├─ CFileWindow          ── 좌측 도킹: Directory/Project/Remote 패널
   │     └─ COutputWindow        ── 하단 도킹: 외부 도구 출력 콘솔
   │
   ├─ CChildFrame (ChildFrm.h)   ── CMDIChildWnd, 분할창(SplitterWndEx)
   │
   ├─ CCedtDoc    (cedtDoc.h)    ── CDocument, 텍스트 버퍼/언두/신택스/파일 IO
   │     └─ CAnalyzedText : CList<CAnalyzedString, LPCTSTR>
   │
   └─ CCedtView   (cedtView.h)   ── CView, 캐럿/그리기/입력/매크로/명령
```

### 핵심 도메인 클래스 ([cedtElement.h](cedtElement.h))

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

> 100개 이상의 `.cpp`/`.h`가 한 폴더에 평탄하게 놓여 있다. 클래스 단위로 파일이 쪼개져 있고, 큰 클래스(`CCedtApp`, `CCedtDoc`, `CCedtView`, `CMainFrame`, `CFileWindow`)는 기능별로 여러 `.cpp`에 분산되어 있다.

### 1. 애플리케이션 (`CCedtApp`)

| 파일 | 역할 |
| --- | --- |
| [cedtapp.cpp](cedtapp.cpp), [cedtapp.h](cedtapp.h) | `InitInstance`, 메시지 맵, 글로벌 상태 |
| [cedtAppConf.cpp](cedtAppConf.cpp) | 사용자 설정/색상 스킴/매크로 저장·로드 |
| [cedtAppDirectory.cpp](cedtAppDirectory.cpp) | Directory 패널 명령 처리 |
| [cedtAppFile.cpp](cedtAppFile.cpp) | 문서 열기/저장/스폰, 셸 커맨드 처리 |
| [cedtAppFilter.cpp](cedtAppFilter.cpp) | 파일 필터 갱신·콜백 |
| [cedtAppHndr.cpp](cedtAppHndr.cpp) | 메뉴/명령 핸들러 (폰트·탭·색상 등) |
| [cedtAppProject.cpp](cedtAppProject.cpp) | 프로젝트 워크스페이스 |
| [cedtAppRegistry.cpp](cedtAppRegistry.cpp) | 레지스트리 입출력 (셸 통합 포함) |
| [cedtAppSearch.cpp](cedtAppSearch.cpp) | Find In Files |
| [cedtAppView.cpp](cedtAppView.cpp) | 모든 뷰 일괄 갱신 |

### 2. 문서 (`CCedtDoc`)

| 파일 | 역할 |
| --- | --- |
| [cedtDoc.cpp](cedtDoc.cpp), [cedtDoc.h](cedtDoc.h) | 문서 기본/플래그/원격 경로 |
| [cedtDocAnal.cpp](cedtDocAnal.cpp) | 텍스트 분석 (워드 토큰화) |
| [cedtDocDictionary.cpp](cedtDocDictionary.cpp) | 자동완성 사전 |
| [cedtDocEdit.cpp](cedtDocEdit.cpp), [cedtDocEditAdv.cpp](cedtDocEditAdv.cpp) | 편집 연산 (기본/고급) |
| [cedtDocFile.cpp](cedtDocFile.cpp) | 인코딩(ASCII/Unicode/UTF-8)·줄끝(DOS/UNIX/MAC)·백업 |
| [cedtDocHndr.cpp](cedtDocHndr.cpp) | 문서 명령 핸들러 |
| [cedtDocMap.cpp](cedtDocMap.cpp) | 라인 좌표 매핑 |
| [cedtDocSearch.cpp](cedtDocSearch.cpp) | 검색·치환 (RegExp 포함) |
| [cedtDocSyntax.cpp](cedtDocSyntax.cpp) | 신택스/언어 사양 적용 |
| [cedtDocUndo.cpp](cedtDocUndo.cpp) | 언두/리두 |
| [cedtDocView.cpp](cedtDocView.cpp) | 뷰 동기화 |

### 3. 뷰 (`CCedtView`)

| 파일 | 역할 |
| --- | --- |
| [cedtView.cpp](cedtView.cpp), [cedtView.h](cedtView.h) | 뷰 기본/정적 멤버 |
| [cedtViewAction.cpp](cedtViewAction.cpp) | 사용자 액션 디스패치 |
| [cedtViewCaret.cpp](cedtViewCaret.cpp) | 캐럿 위치/모양 |
| [cedtViewCommand.cpp](cedtViewCommand.cpp) | 사용자 도구 실행, 자식 프로세스 IO |
| [cedtViewDraw.cpp](cedtViewDraw.cpp) | 화면 렌더링 |
| [cedtViewEdit.cpp](cedtViewEdit.cpp), [cedtViewEditAdv.cpp](cedtViewEditAdv.cpp) | 편집 명령 |
| [cedtViewEditCompose.cpp](cedtViewEditCompose.cpp) | IME 조합 처리 |
| [cedtViewEvent.cpp](cedtViewEvent.cpp) | 키보드/마우스 이벤트 |
| [cedtViewFont.cpp](cedtViewFont.cpp) | 폰트 메트릭 |
| [cedtViewFormat.cpp](cedtViewFormat.cpp) | 워드랩·포맷팅 |
| [cedtViewHighlight.cpp](cedtViewHighlight.cpp) | 신택스 하이라이팅 |
| [cedtViewHndrEdit.cpp](cedtViewHndrEdit.cpp), [cedtViewHndrMisc.cpp](cedtViewHndrMisc.cpp) | 메뉴 핸들러 |
| [cedtViewMacro.cpp](cedtViewMacro.cpp) | 매크로 기록/재생 |
| [cedtViewMap.cpp](cedtViewMap.cpp), [cedtViewMapAdv.cpp](cedtViewMapAdv.cpp) | 화면↔문서 좌표 변환 |
| [cedtViewMetric.cpp](cedtViewMetric.cpp) | 글자/줄 메트릭 |
| [cedtViewMisc.cpp](cedtViewMisc.cpp) | 기타 |
| [cedtViewMove.cpp](cedtViewMove.cpp) | 캐럿 이동 |
| [cedtViewPrint.cpp](cedtViewPrint.cpp) | 인쇄/미리보기 |
| [cedtViewScroll.cpp](cedtViewScroll.cpp) | 스크롤 |
| [cedtViewSearch.cpp](cedtViewSearch.cpp) | 검색 (뷰 측) |
| [cedtViewSelect.cpp](cedtViewSelect.cpp) | 선택 영역 |
| [cedtViewUndo.cpp](cedtViewUndo.cpp) | 뷰 측 언두 연동 |

### 4. 메인 프레임 / 자식 프레임

| 파일 | 역할 |
| --- | --- |
| [MainFrm.cpp](MainFrm.cpp), [MainFrm.h](MainFrm.h) | `CMDIFrameWnd`, 도킹 컨트롤 모음 |
| [MainFrmHndr.cpp](MainFrmHndr.cpp) | 메인 프레임 명령 핸들러 |
| [MainFrmDropTarget.cpp](MainFrmDropTarget.cpp), [MainFrmDropTarget.h](MainFrmDropTarget.h) | OLE Drag&Drop |
| [ChildFrm.cpp](ChildFrm.cpp), [ChildFrm.h](ChildFrm.h) | MDI 자식, 분할창 |
| [SplitterWndEx.cpp](SplitterWndEx.cpp), [SplitterWndEx.h](SplitterWndEx.h) | 확장 분할창 |
| [StatusBarEx.cpp](StatusBarEx.cpp), [StatusBarEx.h](StatusBarEx.h) | 진행률·스플래시 메시지 가능한 상태바 |

### 5. 도킹 패널

| 파일 | 역할 |
| --- | --- |
| [SizeCBar.cpp](SizeCBar.cpp), [SizeCBar.h](SizeCBar.h) | 리사이즈 가능한 컨트롤 바 베이스 |
| [FileWnd.cpp](FileWnd.cpp), [FileWnd.h](FileWnd.h) | 좌측 패널 (Directory / Project / Remote) |
| [FileWndDirectory.cpp](FileWndDirectory.cpp) | Directory 트리 |
| [FileWndProject.cpp](FileWndProject.cpp) | 프로젝트 트리 |
| [FileWndRemote.cpp](FileWndRemote.cpp) | 원격(FTP) 트리 |
| [FileWndDropTarget.cpp](FileWndDropTarget.cpp) | 패널 D&D 타겟 |
| [OutputWindow.cpp](OutputWindow.cpp), [OutputWindow.h](OutputWindow.h) | 하단 출력/입력 콘솔 |
| [FileTab.cpp](FileTab.cpp), [FileTab.h](FileTab.h) | MDI 파일 탭 컨트롤 |
| [FileTabDropTarget.cpp](FileTabDropTarget.cpp) | 파일 탭 D&D |
| [XPTabCtrl.cpp](XPTabCtrl.cpp), [XPTabCtrl.h](XPTabCtrl.h) | XP 스타일 탭 컨트롤 |

### 6. 다이얼로그

#### 환경설정 (Preferences)

[prefdialog.cpp](prefdialog.cpp), [prefdialog.h](prefdialog.h)을 시트로 하는 탭 페이지들:

- [PrefDialogGeneral.cpp](PrefDialogGeneral.cpp) — 일반
- [PrefDialogFile.cpp](PrefDialogFile.cpp) — 파일/인코딩
- [PrefDialogBackup.cpp](PrefDialogBackup.cpp) — 백업
- [PrefDialogDirectory.cpp](PrefDialogDirectory.cpp) — 작업 디렉토리
- [PrefDialogVisual.cpp](PrefDialogVisual.cpp) — 시각 효과
- [PrefDialogColors.cpp](PrefDialogColors.cpp) — 색상
- [PrefDialogFonts.cpp](PrefDialogFonts.cpp) — 폰트
- [PrefDialogSyntax.cpp](PrefDialogSyntax.cpp) — 신택스
- [PrefDialogPrint.cpp](PrefDialogPrint.cpp) — 인쇄
- [PrefDialogOutput.cpp](PrefDialogOutput.cpp) — 출력 창
- [PrefDialogTools.cpp](PrefDialogTools.cpp) — 사용자 도구
- [PrefDialogCommands.cpp](PrefDialogCommands.cpp) — 명령
- [PrefDialogMacros.cpp](PrefDialogMacros.cpp) — 매크로
- [PrefDialogFilters.cpp](PrefDialogFilters.cpp) — 파일 필터
- [PrefDialogAssoc.cpp](PrefDialogAssoc.cpp) — 파일 연결

#### 기능별 다이얼로그

| 파일 | 역할 |
| --- | --- |
| [AboutDialog.cpp](AboutDialog.cpp) | About |
| [FindDialog.cpp](FindDialog.cpp), [ReplaceDialog.cpp](ReplaceDialog.cpp), [AskReplaceDialog.cpp](AskReplaceDialog.cpp) | 찾기/치환 |
| [FindInFilesDialog.cpp](FindInFilesDialog.cpp) | 파일에서 찾기 |
| [GoToDialog.cpp](GoToDialog.cpp) | 줄 번호로 이동 |
| [FolderDialog.cpp](FolderDialog.cpp) | 폴더 선택 |
| [ReloadAsDialog.cpp](ReloadAsDialog.cpp) | 인코딩 재지정 후 리로드 |
| [UserInputDialog.cpp](UserInputDialog.cpp) | 매크로/명령용 입력 |
| [MacroDefineDialog.cpp](MacroDefineDialog.cpp) | 매크로 정의 |
| [DocumentSummary.cpp](DocumentSummary.cpp) | 문서 요약 |
| [DummyDialog.cpp](DummyDialog.cpp) | 자리잡이 |
| FTP 관련 | [FtpSettingsDialog.cpp](FtpSettingsDialog.cpp), [FtpAdvancedDialog.cpp](FtpAdvancedDialog.cpp), [FtpPasswordDialog.cpp](FtpPasswordDialog.cpp), [FtpTransferDialog.cpp](FtpTransferDialog.cpp), [OpenRemoteDialog.cpp](OpenRemoteDialog.cpp) |

### 7. 네트워크 / 원격 파일

| 파일 | 역할 |
| --- | --- |
| [FtpClnt.cpp](FtpClnt.cpp), [FtpClnt.h](FtpClnt.h) | FTP 클라이언트 |
| [RemoteFile.cpp](RemoteFile.cpp), [RemoteFile.h](RemoteFile.h) | 원격 파일 처리 |

### 8. 유틸리티 / 공통

| 파일 | 역할 |
| --- | --- |
| [StdAfx.cpp](StdAfx.cpp), [StdAfx.h](StdAfx.h) | PCH (MFC 헤더 일괄 포함) |
| [cedtHeader.h](cedtHeader.h) | 전역 매크로/상수, 헤더 일괄 포함 |
| [cedtColors.h](cedtColors.h) | 색상 인덱스 정의 |
| [resource.h](resource.h) | 리소스 ID |
| [Utility.cpp](Utility.cpp), [Utility.h](Utility.h) | 헬퍼 함수 |
| [PathName.cpp](PathName.cpp), [PathName.h](PathName.h) | 경로 조작 |
| [RegExp.cpp](RegExp.cpp), [RegExp.h](RegExp.h) | 정규식 엔진 ([RegExp.html](RegExp.html) 문서 동봉) |
| [SortStringArray.cpp](SortStringArray.cpp) | 정렬되는 문자열 배열 |
| [ColorListBox.cpp](ColorListBox.cpp) | 색상 콤보용 리스트박스 |
| [HyperLink.cpp](HyperLink.cpp) | 클릭 가능한 하이퍼링크 스태틱 |
| [VerticalStatic.cpp](VerticalStatic.cpp) | 수직 텍스트 스태틱 |
| [Separator.cpp](Separator.cpp) | 구분선 컨트롤 |
| [CmdLine.cpp](CmdLine.cpp) | 커맨드라인 파서 (인스턴스 간 IPC 포함) |
| [registry.cpp](registry.cpp), [registry.h](registry.h) | 레지스트리 헬퍼 |
| [encode.cpp](encode.cpp), [encode.h](encode.h) | 문자 인코딩 변환 |
| [date.cpp](date.cpp), [date.h](date.h) | 날짜·시각 포맷 |
| [evaluate.cpp](evaluate.cpp), [evaluate.h](evaluate.h) | 수식 평가기 |

### 9. 리소스

| 파일 | 역할 |
| --- | --- |
| [cedt_kr.rc](cedt_kr.rc) / [cedt_kr.aps](cedt_kr.aps) / [cedt_kr.clw](cedt_kr.clw) | 한국어 리소스 |
| [cedt_us.rc](cedt_us.rc) / [cedt_us.aps](cedt_us.aps) / [cedt_us.clw](cedt_us.clw) | 영어 리소스 |
| [res/](res/) | 아이콘, 비트맵, 커서, 매니페스트, `cedt.rc2` |

`.rc.bak` 파일은 백업본이다.

---

## 디렉토리 구조 (요약)

```
CrimsonEditor/
├── cedt.dsw, cedt.dsp, cedt.mak    # VC6 워크스페이스/프로젝트/nmake
├── cedt.dep, cedt.ncb, cedt.opt, cedt.plg   # IDE 캐시(빌드 시 재생성)
├── cedtapp.{h,cpp} + cedtApp*.cpp  # CCedtApp
├── cedtDoc.{h,cpp} + cedtDoc*.cpp  # CCedtDoc
├── cedtView.{h,cpp} + cedtView*.cpp # CCedtView
├── MainFrm*.{h,cpp}                # CMainFrame
├── ChildFrm.{h,cpp}                # CChildFrame
├── FileWnd*.{h,cpp}                # 좌측 도킹 패널
├── FileTab*.{h,cpp}                # 상단 파일 탭
├── PrefDialog*.cpp                 # 환경설정 페이지
├── *Dialog.{h,cpp}                 # 기능 다이얼로그
├── Ftp*.{h,cpp}, RemoteFile.*      # FTP/원격
├── cedtElement.{h,cpp}             # 도메인 데이터 클래스
├── RegExp.*, encode.*, date.*, …   # 유틸리티
├── StdAfx.{h,cpp}                  # PCH
├── cedt_kr.rc, cedt_us.rc          # 리소스 (KR/US)
├── HtmlHelp.h, HtmlHelp.lib        # HTML Help SDK 동봉본
└── res/                            # 아이콘·비트맵·커서·매니페스트
```

---

## 셸 통합 / 다중 인스턴스

- **싱글 인스턴스 IPC**: 명명된 뮤텍스 `CrimsonEditor.CmdLine`로 첫 인스턴스를 감지하고, `WM_ANOTHER_INSTANCE` 메시지를 첫 창으로 보낸다 ([cedtapp.cpp:22](cedtapp.cpp#L22) 부근의 `#pragma data_seg("Shared")` 참고).
- **셸 확장**: `HKCR\*\shellex\ContextMenuHandlers\Crimson Editor` 등록으로 우클릭 메뉴 통합.
- **IE 통합**: "View Source Editor"로 등록 가능.

상수는 [cedtHeader.h](cedtHeader.h) 상단에 모여 있다.
