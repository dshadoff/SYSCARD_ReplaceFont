# PC Engine System Card Font Fix

The PC Engine System card contains both 12x12 and 16x16 fonts for the SJIS characters set; this is 
generally considered to be "kanji support", but there are also standard (and some non-standard !)
characters included.

As many people have noticed, however, the font design for the Western characters leaves much to be desired.
Lack of descenders, weird mismatched character types, and misaligned base lines are just some of the issues.

Here's how it was done:

## Viewing the Characters:

First, I needed to see how the characters appeared, and what SJIS codes they corresponded with.
testkanji.c (and .iso) are the product of this.
- The direction keys adjust the SJIS code used for the first character in the matrix.
- The 'II' key prints a matrix of SJIS characters starting with this code.  16 characters across, and 8 rows down.
- The 'I' key toggles the flag between 12x12 and 16x16 font display
- The 'SELECT' key will page forward (0x80 kanji) for convenience

If you review the whole set, you'll notice that many codes appear multiple times - they aren't really duplicated
in the ROM; these are just artifacts of the calculations in the decode routine.  So we'll only have to replace one
copy.


## Finding Where the Codes Are Stored in the ROM:

Using the 'testkanji' program, I set a breakpoint for $E060 (the EX_GETFONT function), and traced it through.
It turns out that the characters aren't stored in a compressed format, and actually take up a significant portion
of the ROM.  Also, for the ones I was interested in, both Japanese and American System 3 ROMs had stored those
characters at the same locations, for both 12x12 and 16x16 sizes.


## Extracting the Existing Fonts:

I used FEIDIAN (a PHP program which tries to be a universal font extractor/implantor... and it seems pretty good).
The following are the character groups I needed, their respective locations in the ROM, and their feidian commands:

## A-Z,a-z - Alphabetic characters (52 characters, which luckily are all sequential)

12x12:  (bank $18:$A84C in memory)
no-header ROM:
```
    php feidian.php -r 12,12,26,2,0x3084C syscard.pce A-z.bmp
```
ROM with header:
```
    php feidian.php -r 12,12,26,2,0x30A4C syscard.pce A-z.bmp
```

16x16:  (bank $08:$AEC0 in memory)
no-header ROM:
```
    php feidian.php -r 16,16,26,2,0x10EC0 syscard.pce A-z.bmp
```
ROM with header: 
```
    php feidian.php -r 16,16,26,2,0x110C0 syscard.pce A-z.bmp
```

## 0-9 - Numeric digits (10 characters)

12x12:  (bank $18:$A798 in memory)
no-header ROM:
```
    php feidian.php -r 12,12,10,1,0x30798 syscard.pce A-z.bmp
```
ROM with header:
```
    php feidian.php -r 12,12,10,1,0x30998 syscard.pce A-z.bmp
```

16x16:  (bank $08:$AD80 in memory)
no-header ROM:
```
    php feidian.php -r 16,16,10,1,0x10D80 syscard.pce A-z.bmp
```
ROM with header:
```
    php feidian.php -r 16,16,10,1,0x10F80 syscard.pce A-z.bmp
```


## ?! - Punctuation which looked odd until i changed it (2 characters)

12x12:  (bank $18:$A090 in memory)
no-header ROM:
```
    php feidian.php -r 12,12,2,1,0x30090 syscard.pce A-z.bmp
```
ROM with header:
```
    php feidian.php -r 12,12,2,1,0x30290 syscard.pce A-z.bmp
```

16x16:  (bank $08:$A100 in memory)
no-header ROM:
```
    php feidian.php -r 16,16,2,1,0x10100 syscard.pce A-z.bmp
```
ROM with header:
```
    php feidian.php -r 16,16,2,1,0x10300 syscard.pce A-z.bmp
```



## Modifying Fonts:

Honestly, I didn't attempt to repair the existing fonts; I just replaced them with another set of fonts I
found here, after making minor adjustments:
[http://www.pvv.org/~roarl/nethack/nethack.html](http://www.pvv.org/~roarl/nethack/nethack.html)

I used a program called Pixelformer, and output a BMP after setting the colorspace to Monochrome.
Then, I reversed the process, using FEIDIAN again.

Note: To do the same, take the appropriate command above, switch '-r' to '-i', and swap the sequence of
the ROM and BMP names.

The command sequence for a ROM with header would be:
```
php feidian.php -i 12,12,26,2,0x30A4C insert_alphabet12.bmp syscard3.pce
php feidian.php -i 16,16,26,2,0x110C0 insert_alphabet16.bmp syscard3.pce
php feidian.php -i 12,12,10,1,0x30998 insert_num12.bmp syscard3.pce
php feidian.php -i 16,16,10,1,0x10F80 insert_num16.bmp syscard3.pce
php feidian.php -i 12,12,2,1,0x30290 insert_quest12.bmp syscard3.pce
php feidian.php -i 16,16,2,1,0x10300 insert_quest16.bmp syscard3.pce
```

I am also including the IPS file for "Version 3.0" Super System cards; this expects a headered ROM, but
should work equally well on both Japanese and American versions, as the font information is stored at the
same location in both.
