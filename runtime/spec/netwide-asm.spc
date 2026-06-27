# NASM-format Assembly Language Specification File for Crimson Editor
# by Jed Smith <jed@knows.it>
#
# Note: I know I'm defining it is case-insensitive here, and NASM
# really is case-sensitive, but NASM only cares about the case of
# user-defined symbols AFAIK.
#
# RANGE1 is so I can get '16' and '32' after BITS to light up.

$CASESENSITIVE=NO
$DELIMITERS=+-*/()[]{}|^&<>%~:,"'
$KEYWORDPREFIX=%:
$SPECIALVARIABLECHARS=~
$HEXADECIMALMARK=0x
$ESCAPECHAR=
$QUOTATIONMARK1="
$QUOTATIONMARK2='
$LINECOMMENT=;
$PAIRS1=()
$PAIRS2=[]
$PAIRS3={}