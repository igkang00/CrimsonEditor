D2Coding font — required bundle files
=====================================

The installer (installer.iss) bundles the D2Coding coding font as an optional,
checked-by-default task ("installd2coding"). For the build to succeed, drop the
following two files into this folder:

  D2Coding.ttf   The regular (non-ligature) D2Coding TrueType font.
  OFL.txt        The SIL Open Font License 1.1 text that ships with D2Coding.

Source
------
Naver's official repository:  https://github.com/naver/d2codingfont

Download the latest release ZIP, then copy:
  - D2Coding/D2Coding-Ver<ver>.ttf   ->   runtime\fonts\D2Coding.ttf
  - LICENSE (OFL 1.1)                ->   runtime\fonts\OFL.txt

Notes
-----
- We bundle the regular face, NOT "D2Coding ligature". The app's default screen
  font (cedtAppConf.cpp, m_lfScreen[0]) uses the face name "D2Coding".
- License: SIL OFL 1.1 permits bundling/redistribution as long as the license
  text is included (OFL.txt, installed to <InstallDir>\fonts) and the reserved
  font name is preserved. We ship the font unmodified, so both conditions hold.
- D2Coding.ttf and OFL.txt are committed to the repo as build assets, the same
  way other installer-referenced runtime assets are (cedt.dic, cedt.ico, ...),
  so the installer builds from a clean checkout. See installer.iss [Files]/[Tasks].
- Bundled version: D2Coding Ver 1.3.2 (2018-05-24), regular face.
