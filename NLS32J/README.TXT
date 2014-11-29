/*********************************************************************/
/*******                NLSSAMPLE 32BIT Version             **********/
/*******                                                    **********/
/*******       Environment: OS/2 Japanese Version 2.0 up    **********/
/*******                 (OS/2 J2.0/J2.1/J3.0)              **********/
/*******                                                    **********/
/*******           IBM Japan PSP-AP Porting Support         **********/
/*********************************************************************/

The OS/2 Japanese version has a unique function: IME(Input Method Editor).
The IME interface (WinDBCSxxx) can change the input status (Alphanumeric,
Hiragana, Katakana, etc) dynamically.
 
NLS32.EXE is 32 bit sample program to control IME(*1) Interface.
This sample program controls IME function using WinDBCSxx function and
convert 32 bit.

The Japanese language is based on ideographic symbols rather than 	
the alphabet. There are 3 types of characters: HIRAHANA, KATAKANA,
and KANJI.  These 3 types of characters have thousands of characters 
beyond the limit of one-byte representation, i.e., beyond 256 characters.

If we use two bytes, namely 16 bits, we can represent 65,536 (256 x 256) 
characters. So we need two bytes for each character to create coded 
character sets for the Japanese language.

The number of DBCS characters is too large to assign each DBCS character
to a key on the keyboard.  Therefore, in Japanese we don't type DBCS 
characters directly.  In Japanese, we first type the pronunciation of the 
sentence we want to input using phonetic characters assigned to keys on 
the keyboard.  Secondary types are vowels.

Kana Kanji program is used to translate from 2 DBCS bytes to the proper 
Japanese characters (ideographic symbols characters) in HIRAGANA, KATAKANA, 
or KANJI.  If a PM application accesses the Kanakanji program, it uses 
the IME interface.  The IME interface is controlled by the Kanakanji 
program.
   
   -Input mode change Alphanumeric, HIRAGANA, KATAKANA, KANJI,
    ZENKAKU, HANKAKU (WinDBCSModeControl)

   -Enable/Disable Kanakanji program (WinDBCSIMEControl)

            ----------------------------------------
            |           PM Application              | 
            ----------------------------------------
                                |                        
                           IME Interface    
                                |
                    ------------------------- 
                   |  Presentation  Manager  |
                    -------------------------
                              |            |
                     ------------------    |
                    | Kanakanji Program|   |
                    |    (MONITOR)     |   |
                     ------------------    |
                              |            | 
                    --------------------------
                   |      Keyboard Driver     |
                    --------------------------



Environment:
  -Operating system
           OS/2 J2.0/J2.1
           OS/2 Warp J3.0

  -Toolkit
           Toolkit for OS/2 J2.0 or J2.1 (PMNLS32.LIB)

  -Compiler
           C Set J1.0/J2.0/J2.1


END
