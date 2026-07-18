# Regenerate the manual-test fixtures that cannot be committed as literal text.
#
# The small fixtures in this directory ARE committed — their exact bytes are the point (a
# BOM-less CP949 file is only a test if it really has no BOM), and a reviewer should be able to
# see them. This script exists for the two that cannot be:
#
#   big.txt  ~56 MB. Too large to commit; regenerate it when you need it.
#
# and to make the encoded fixtures reproducible, since an editor "helpfully" adding a BOM would
# quietly destroy what they test.
#
#   python tests/data/make-fixtures.py           # the small ones (idempotent)
#   python tests/data/make-fixtures.py --big     # also the 900,000-line file
#
# See docs/manual-test-plan.md.

import io
import os
import random
import sys

HERE = os.path.dirname(os.path.abspath(__file__))


def w(name, data):
    p = os.path.join(HERE, name)
    with open(p, 'wb') as f:
        f.write(data)
    print('  %-18s %8d bytes' % (name, len(data)))


# The same text in every encoding fixture, so a round-trip can be compared byte for byte.
# Deliberately mixes scripts: ASCII, Hangul, Han, and a character CP949 cannot hold.
SAMPLE = (
    'ascii line\r\n'
    '한글 줄입니다\r\n'          # Korean
    '漢字 한자\r\n'                      # Han + Korean
    'mixed 한글 and ascii\r\n'
    'punctuation: — “quoted” …\r\n'   # em dash, smart quotes, ellipsis
)


def small():
    print('small fixtures:')

    # --- baseline -------------------------------------------------------------
    lines = ['/* ascii.c - plain C, no surprises. Baseline for every other fixture. */', '']
    for i in range(1, 201):
        lines.append('int  value_%03d = %d;' % (i, i * 7))
        if i % 20 == 0:
            lines.append('')
            lines.append('/* block %d */' % (i // 20))
    lines += ['', 'int main(void) { return 0; }', '']
    w('ascii.c', '\r\n'.join(lines).encode('ascii'))

    # --- Korean source, CP949, NO BOM ----------------------------------------
    # The migration misdetected exactly this shape as UTF-8 once and turned every Korean byte
    # into U+FFFD. CP949 has no BOM to detect by, so the guess is all there is.
    kr = [
        '/* korean.c - CP949, BOM 없음. 자동 감지가 UTF-8로 오판하면 */',
        '/* 모든 한글이 U+FFFD로 변합니다. */',
        '',
        '#include <stdio.h>',
        '',
        '/* 주석: 사용자 이름을 출력한다 */',
        'void 출력(void)',
        '{',
        '\tchar 이름[64] = "홍길동";',
        '\tprintf("이름: %s\\n", 이름);   /* 끝줌 주석 */',
        '}',
        '',
        '/* 긴 한글 줄: ' + '가나다라마바사아자차카타파하' * 6 + ' */',
        '',
        'int main(void) { 출력(); return 0; }',
        '',
    ]
    w('korean.c', '\r\n'.join(kr).encode('cp949'))

    # --- the same text, four encodings ---------------------------------------
    # CP949 cannot represent the em dash / smart quotes, so the ANSI fixture drops that line —
    # which is itself the point: it is what a CP949 file can actually hold.
    w('utf8-nobom.txt', SAMPLE.encode('utf-8'))
    w('utf8-bom.txt', b'\xef\xbb\xbf' + SAMPLE.encode('utf-8'))
    w('utf16le.txt', b'\xff\xfe' + SAMPLE.encode('utf-16-le'))
    w('utf16be.txt', b'\xfe\xff' + SAMPLE.encode('utf-16-be'))

    ansi = ''.join(l + '\r\n' for l in SAMPLE.splitlines() if not l.startswith('punctuation'))
    w('cp949.txt', ansi.encode('cp949'))

    # --- emoji inside C string literals ---------------------------------------
    # astral.txt already covers caret movement over astral and BMP emoji. This one is the
    # analyzer's escape branch: it does fwd += 2 unconditionally, so a backslash followed by a
    # surrogate pair can be split down the middle.
    emoji_c = (
        '/* emoji.c - the analyzer\'s escape branch, and BMP emoji that render two cells wide. */\r\n'
        '\r\n'
        'const char *a = "\\\U0001F600";        /* backslash then an astral pair */\r\n'
        'const char *b = "before \\\U0001F600 after";\r\n'
        'const char *c = "\\\\\U0001F600";       /* escaped backslash, then the pair */\r\n'
        'const char *d = "✅ ⭐";          /* BMP: U+2705, U+2B50 - not astral */\r\n'
        'const char *e = "\U0001F600\U0001F600\U0001F600";\r\n'
        'const char *f = "한글 \U0001F600 한글";  /* Hangul either side of a pair */\r\n'
        '\r\n'
        'int main(void) { return 0; }\r\n'
    )
    w('emoji.c', emoji_c.encode('utf-8'))

    # --- one very long line ---------------------------------------------------
    # Evaluate Line, user-tool stdin, and the memory-safety truncation sites all care where the
    # 2,048 and 32,767 boundaries fall, so cross both.
    body = ''.join('%d+%d " " ' % (i, i) for i in range(1, 6000))
    long_line = 'x = ' + body[:41000] + ' end\r\n'
    w('long-line.txt', long_line.encode('ascii'))

    # --- over-long tokens in the parsed config files --------------------------
    # The memory-safety pass bounded 20 `stream >> buf` sites with `stream.width(N)`. width()
    # does not DISCARD the rest of an over-long token — it leaves the remainder in the stream,
    # where it comes back as the *next* token. So the failure mode to look for is a desynced
    # parser (or a hang), not an overrun. Two buffer sizes matter:
    #
    #   kProjectTokenBufSize = 4096   src/panels/FileWndProject.cpp   (.prj readers)
    #   MAX_WORD_LENGTH      =  255   src/core/cedtElement.h          (.key / .dic readers)

    # A project entry whose path is longer than one getline() can hold. The path lives in an
    # attribute read by `getline(szText, 4096, L'>')`, not by `>>` — if the closing '>' is not
    # found within 4095 chars getline sets failbit, and every later read fails too. The
    # `while(_tcsicmp(szText, "</project>"))` loops never see their terminator when that happens.
    deep = '\\'.join('directory_level_%03d' % i for i in range(1, 260))
    long_path = 'C:\\%s\\file.c' % deep                      # ~5,200 chars, clear of the 4,096
    prj = (
        '<project version="Crimson Editor 3.90">\r\n'
        '\t<localfile path="%s"/>\r\n'
        '</project>\r\n'
        '\r\n'
        '<workspace version="Crimson Editor 3.90">\r\n'
        '</workspace>\r\n'
    ) % long_path
    w('long-path.prj', prj.encode('utf-8'))

    # A project file that ends exactly at a `<localfile` token, with no attributes and no '>'.
    # LoadProjectItem does `getline(...); nLen = _tcslen(szText); if( szText[nLen-1] == '/' )`
    # — when getline extracts nothing, nLen is 0 and that indexes szText[-1].
    w('truncated.prj', (
        '<project version="Crimson Editor 3.90">\r\n'
        '\t<localfile'
    ).encode('utf-8'))

    # Keyword and dictionary files whose tokens straddle MAX_WORD_LENGTH (255). The 300-char
    # entries should come back split, with the tail treated as a second keyword/word.
    kw = ['[-COMMENT-:GLOBAL]', '# over-long keyword fixture', '', '[KEYWORDS0:GLOBAL]']
    kw.append('short_keyword')
    kw.append('k' * 254)          # fits
    kw.append('m' * 255)          # exactly the buffer
    kw.append('n' * 300)          # over — tail becomes a second token
    kw.append('after_the_long_one')
    w('long-token.key', ('\r\n'.join(kw) + '\r\n').encode('ascii'))

    dic = ['alpha', 'w' * 254, 'x' * 255, 'y' * 300, 'omega']
    w('long-token.dic', ('\r\n'.join(dic) + '\r\n').encode('ascii'))

    # A document to open against long-token.key. The interesting line is the last one: if
    # width() split the 300-char keyword, its 45-char tail is now a keyword in its own right,
    # and this line is where that spurious entry shows up as colour.
    doc = [
        '-- open with Syntax Type > Ada, against long-token.key',
        '',
        'short_keyword                 -- a keyword: must colour',
        'k' * 254 + '   -- 254 chars, fits the buffer: must colour',
        'm' * 255 + '   -- 255 chars, exactly the buffer: must colour',
        'n' * 300 + '   -- 300 chars: LookupTable refuses >255 outright, so NO colour',
        'n' * 45 + '   -- the tail of the 300-char keyword. Colour here = width() desync',
        'after_the_long_one            -- the token after it: must still colour',
    ]
    w('long-token.ada', ('\r\n'.join(doc) + '\r\n').encode('ascii'))


def big():
    print('big fixture (this takes a moment):')
    words = ['int', 'value', 'count', 'buffer', 'index', 'result', 'data', 'item',
             'node', 'list', 'size', 'name', 'flag', 'temp', 'offset', 'length']
    korean = ['변수', '값', '개수', '버퍼', '색인', '결과', '데이터', '항목']

    rnd = random.Random(20260717)   # fixed seed: the same file every time
    p = os.path.join(HERE, 'big.txt')
    with io.open(p, 'w', encoding='utf-8', newline='') as f:
        for i in range(900000):
            n = rnd.randint(3, 12)
            parts = [rnd.choice(words) for _ in range(n)]
            if i % 7 == 0:                       # ~14% of lines carry Korean
                parts.insert(rnd.randint(0, n), rnd.choice(korean))
            f.write('    ' + ' '.join(parts) + '  // line %d\r\n' % i)
    print('  %-18s %8.1f MB' % ('big.txt', os.path.getsize(p) / 1048576.0))


if __name__ == '__main__':
    small()
    if '--big' in sys.argv:
        big()
    else:
        print('\n(big.txt skipped - pass --big for the 900,000-line file)')
