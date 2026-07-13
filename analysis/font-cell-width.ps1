# font-cell-width.ps1 — what width does GDI actually draw a character at?
#
# Column mode places characters on a grid of cells: a character is one cell wide or two.
# docs/refactoring-column-mode.md decides that width from the union of an East Asian Width
# table and a measurement (advance > 1.2 x the narrow cell). This script produces the
# measurement, so the threshold can be checked against reality rather than assumed.
#
# It is what established the finding the plan rests on: a FIXED-PITCH FONT IS NOT A
# DUAL-WIDTH FONT. Consolas has no Hangul glyphs, Windows font-links them elsewhere, and the
# linked advance is 1.43x the Latin one — not 2x. So the font cannot give you a grid, and the
# editor has to impose one.
#
#   .\analysis\font-cell-width.ps1
#   .\analysis\font-cell-width.ps1 -Faces Consolas,"Cascadia Mono" -PointSize 12
#
# The narrow cell is the advance of a SPACE, because alignment padding is done with spaces —
# see the same document. In a fixed-pitch font it equals tmAveCharWidth, and the script prints
# both so a font where they diverge is visible rather than silent.

param(
    [string[]] $Faces = @("Consolas", "D2Coding", "Courier New"),
    [int]      $PointSize = 10,
    [double]   $WideThreshold = 1.2,
    [string]   $PrivateFont = "runtime\fonts\D2Coding.ttf"   # bundled; loaded so it need not be installed
)

Add-Type @"
using System;
using System.Runtime.InteropServices;
public class GdiMeasure {
    [DllImport("gdi32.dll")] public static extern IntPtr CreateCompatibleDC(IntPtr hdc);
    [DllImport("gdi32.dll")] public static extern bool   DeleteDC(IntPtr hdc);
    [DllImport("gdi32.dll")] public static extern IntPtr SelectObject(IntPtr hdc, IntPtr h);
    [DllImport("gdi32.dll")] public static extern bool   DeleteObject(IntPtr h);
    [DllImport("user32.dll")] public static extern IntPtr GetDC(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern int    ReleaseDC(IntPtr hWnd, IntPtr hdc);
    [DllImport("gdi32.dll")] public static extern int    GetDeviceCaps(IntPtr hdc, int index);
    [DllImport("gdi32.dll", CharSet=CharSet.Unicode)]
    public static extern IntPtr CreateFontW(int h,int w,int esc,int ori,int weight,uint italic,
        uint underline,uint strike,uint charset,uint outPrec,uint clipPrec,uint quality,
        uint pitch,string face);
    [DllImport("gdi32.dll", CharSet=CharSet.Unicode)]
    public static extern bool GetTextExtentPoint32W(IntPtr hdc, string s, int c, out SIZE sz);
    [DllImport("gdi32.dll", CharSet=CharSet.Unicode)]
    public static extern bool GetTextMetricsW(IntPtr hdc, out TEXTMETRIC tm);
    [DllImport("gdi32.dll", CharSet=CharSet.Unicode)]
    public static extern int AddFontResourceExW(string file, uint flags, IntPtr reserved);
    [StructLayout(LayoutKind.Sequential)] public struct SIZE { public int cx, cy; }
    [StructLayout(LayoutKind.Sequential)] public struct TEXTMETRIC {
        public int tmHeight, tmAscent, tmDescent, tmInternalLeading, tmExternalLeading;
        public int tmAveCharWidth, tmMaxCharWidth, tmWeight, tmOverhang;
        public int tmDigitizedAspectX, tmDigitizedAspectY;
        public char tmFirstChar, tmLastChar, tmDefaultChar, tmBreakChar;
        public byte tmItalic, tmUnderlined, tmStruckOut, tmPitchAndFamily, tmCharSet;
    }
}
"@

# FR_PRIVATE — usable by this process without installing it
if( Test-Path $PrivateFont ) { [void][GdiMeasure]::AddFontResourceExW((Resolve-Path $PrivateFont), 0x10, [IntPtr]::Zero) }

# Characters chosen to sit on both sides of the question. EAW = East Asian Width.
$samples = @(
    @{ Name = "a";                 Text = "a";                          Eaw = "Na" }
    @{ Name = "space";             Text = " ";                          Eaw = "Na" }
    @{ Name = "Hangul U+AC00";     Text = [char]0xAC00;                 Eaw = "W"  }
    @{ Name = "Han U+6F22";        Text = [char]0x6F22;                 Eaw = "W"  }
    @{ Name = "star U+2605";       Text = [char]0x2605;                 Eaw = "A"  }
    @{ Name = "check U+2705";      Text = [char]0x2705;                 Eaw = "W"  }
    @{ Name = "star2 U+2B50";      Text = [char]0x2B50;                 Eaw = "W"  }
    @{ Name = "emoji U+1F600";     Text = [char]::ConvertFromUtf32(0x1F600); Eaw = "W" }
)

$screen = [GdiMeasure]::GetDC([IntPtr]::Zero)
$dpi    = [GdiMeasure]::GetDeviceCaps($screen, 90)   # LOGPIXELSY
[void][GdiMeasure]::ReleaseDC([IntPtr]::Zero, $screen)
$height = -[int][math]::Round($PointSize * $dpi / 72.0)

Write-Output ""
Write-Output "GDI advances at $PointSize pt / $dpi dpi.  A cell is the SPACE advance."
Write-Output "'wide?' is the MEASUREMENT half of the classifier only: advance > $WideThreshold x cell."
Write-Output "The table half (EAW W/F) is the other half; the classifier is their union."

foreach( $face in $Faces ) {
    $hdc = [GdiMeasure]::CreateCompatibleDC([IntPtr]::Zero)
    $hf  = [GdiMeasure]::CreateFontW($height,0,0,0,400,0,0,0,1,0,0,0,0,$face)
    $old = [GdiMeasure]::SelectObject($hdc, $hf)

    $tm = New-Object GdiMeasure+TEXTMETRIC
    [void][GdiMeasure]::GetTextMetricsW($hdc, [ref]$tm)

    $sz = New-Object GdiMeasure+SIZE
    [void][GdiMeasure]::GetTextExtentPoint32W($hdc, " ", 1, [ref]$sz)
    $cell = $sz.cx

    Write-Output ""
    $note = if( $cell -ne $tm.tmAveCharWidth ) { "  <-- DIVERGES from tmAveCharWidth" } else { "" }
    Write-Output "=== $face ===   cell(space)=$cell px   tmAveCharWidth=$($tm.tmAveCharWidth)$note"

    $rows = foreach( $s in $samples ) {
        $t  = [string]$s.Text
        $e  = New-Object GdiMeasure+SIZE
        [void][GdiMeasure]::GetTextExtentPoint32W($hdc, $t, $t.Length, [ref]$e)
        $ratio = if( $cell -gt 0 ) { [math]::Round($e.cx / $cell, 2) } else { 0 }

        [PSCustomObject]@{
            char    = $s.Name
            EAW     = $s.Eaw
            px      = $e.cx
            ratio   = $ratio
            "wide?" = if( $ratio -gt $WideThreshold ) { "2 cells" } else { "1 cell" }
            agrees  = if( ($ratio -gt $WideThreshold) -eq ($s.Eaw -eq "W") ) { "" } else { "table disagrees" }
        }
    }
    $rows | Format-Table -AutoSize | Out-String | Write-Output

    [void][GdiMeasure]::SelectObject($hdc, $old)
    [void][GdiMeasure]::DeleteObject($hf)
    [void][GdiMeasure]::DeleteDC($hdc)
}

Write-Output "'table disagrees' is not a bug: it is the reason the classifier is a UNION."
Write-Output "Neither source is complete, and under forced placement calling a narrow"
Write-Output "character wide costs a little air, while calling a wide one narrow makes it"
Write-Output "overlap its neighbour. So the union leans wide, deliberately."
