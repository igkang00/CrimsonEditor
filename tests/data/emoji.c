/* emoji.c - the analyzer's escape branch, and BMP emoji that render two cells wide. */

const char *a = "\😀";        /* backslash then an astral pair */
const char *b = "before \😀 after";
const char *c = "\\😀";       /* escaped backslash, then the pair */
const char *d = "✅ ⭐";          /* BMP: U+2705, U+2B50 - not astral */
const char *e = "😀😀😀";
const char *f = "한글 😀 한글";  /* Hangul either side of a pair */

int main(void) { return 0; }
