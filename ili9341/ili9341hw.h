#define ILI9341_NOP         0x00 ///< No-op register
#define ILI9341_SWRESET     0x01 ///< Software reset register
#define ILI9341_RDDID       0x04 ///< Read display identification information
#define ILI9341_RDDST       0x09 ///< Read Display Status

#define ILI9341_SLPIN       0x10 ///< Enter Sleep Mode
#define ILI9341_SLPOUT      0x11 ///< Sleep Out
#define ILI9341_PTLON       0x12 ///< Partial Mode ON
#define ILI9341_NORON       0x13 ///< Normal Display Mode ON

#define ILI9341_RDMODE      0x0A ///< Read Display Power Mode
#define ILI9341_RDMADCTL    0x0B ///< Read Display MADCTL
#define ILI9341_RDPIXFMT    0x0C ///< Read Display Pixel Format
#define ILI9341_RDIMGFMT    0x0D ///< Read Display Image Format
#define ILI9341_RDSELFDIAG  0x0F ///< Read Display Self-Diagnostic Result

#define ILI9341_INVOFF      0x20 ///< Display Inversion OFF
#define ILI9341_INVON       0x21 ///< Display Inversion ON
#define ILI9341_GAMMASET    0x26 ///< Gamma Set
#define ILI9341_DISPOFF     0x28 ///< Display OFF
#define ILI9341_DISPON      0x29 ///< Display ON

#define ILI9341_CASET       0x2A ///< Column Address Set
#define ILI9341_PASET       0x2B ///< Page Address Set
#define ILI9341_RAMWR       0x2C ///< Memory Write
#define ILI9341_RAMRD       0x2E ///< Memory Read

#define ILI9341_PTLAR       0x30 ///< Partial Area
#define ILI9341_VSCRDEF     0x33 ///< Vertical Scrolling Definition
#define ILI9341_MADCTL      0x36 ///< Memory Access Control
#define ILI9341_VSCRSADD    0x37 ///< Vertical Scrolling Start Address
#define ILI9341_PIXFMT      0x3A ///< COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1     0xB1 ///< Frame Rate Control (In Norm Mode/Full Colors)
#define ILI9341_FRMCTR2     0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3     0xB3 ///< Frame Rate control (In Part Mode/Full Colors)
#define ILI9341_INVCTR      0xB4 ///< Display Inversion Control
#define ILI9341_DFUNCTR     0xB6 ///< Display Function Control

#define ILI9341_PWCTR1      0xC0 ///< Power Control 1
#define ILI9341_PWCTR2      0xC1 ///< Power Control 2
#define ILI9341_PWCTR3      0xC2 ///< Power Control 3
#define ILI9341_PWCTR4      0xC3 ///< Power Control 4
#define ILI9341_PWCTR5      0xC4 ///< Power Control 5
#define ILI9341_VMCTR1      0xC5 ///< VCOM Control 1
#define ILI9341_VMCTR2      0xC7 ///< VCOM Control 2

#define ILI9341_RDID1       0xDA ///< Read ID 1
#define ILI9341_RDID2       0xDB ///< Read ID 2
#define ILI9341_RDID3       0xDC ///< Read ID 3
#define ILI9341_RDID4       0xDD ///< Read ID 4

#define ILI9341_GMCTRP1     0xE0 ///< Positive Gamma Correction
#define ILI9341_GMCTRN1     0xE1 ///< Negative Gamma Correction
//#define ILI9341_PWCTR6     0xFC

#define MHz                 1000000L
#define PIX_WIDTH           240
#define PIX_HEIGHT          320
#define PIX_BITCOUNT        (PIX_WIDTH * PIX_HEIGHT)
#define PIX_BYTECOUNT       (PIX_BITCOUNT / 8)
#define PIX_W32COUNT        (PIX_BITCOUNT / 32)
#define PIX_BYTE_STRIDE     (PIX_WIDTH / 8)

#define TEXT_HEIGHT         (PIX_HEIGHT / 8)
#define TEXT_WIDTH          (PIX_WIDTH / 8)
#define TEXT_CHARCOUNT      (TEXT_HEIGHT * TEXT_WIDTH)

#define CS_ENABLE           0
#define CS_DISABLE          1

/* Some custom palette. */
#define TFT_ALICEBLUE 0xF7DF
#define TFT_ANTIQUEWHITE 0xFF5A
#define TFT_AQUA 0x07FF
#define TFT_AQUAMARINE 0x7FFA
#define TFT_AZURE 0xF7FF
#define TFT_BEIGE 0xF7BB
#define TFT_BISQUE 0xFF38
#define TFT_BLACK 0x0000 /*   0, 0, 0 */
#define TFT_BLANCHEDALMOND 0xFF59
#define TFT_BLUE 0x001F /*   0, 0, 255 */
#define TFT_BLUEVIOLET 0x895C
#define TFT_BROWN 0xA145
#define TFT_BURLYWOOD 0xDDD0
#define TFT_CADETBLUE 0x5CF4
#define TFT_CHARTREUSE 0x7FE0
#define TFT_CHOCOLATE 0xD343
#define TFT_CORAL 0xFBEA
#define TFT_CORNFLOWERBLUE 0x64BD
#define TFT_CORNSILK 0xFFDB
#define TFT_CRIMSON 0xD8A7
#define TFT_CYAN 0x07FF /*   0, 255, 255 */
#define TFT_DARKBLUE 0x0011
#define TFT_DARKCYAN 0x03EF /*   0, 128, 128 */
#define TFT_DARKCYAN2 0x0451
#define TFT_DARKGOLDENROD 0xBC21
#define TFT_DARKGRAY 0xAD55
#define TFT_DARKGREEN2 0x0320
#define TFT_DARKGREEN 0x03E0 /*   0, 128, 0 */
#define TFT_DARKGREY 0x7BEF /* 128, 128, 128 */
#define TFT_DARKKHAKI 0xBDAD
#define TFT_DARKMAGENTA 0x8811
#define TFT_DARKOLIVEGREEN 0x5345
#define TFT_DARKORANGE 0xFC60
#define TFT_DARKORCHID 0x9999
#define TFT_DARKRED 0x8800
#define TFT_DARKSALMON 0xECAF
#define TFT_DARKSEAGREEN 0x8DF1
#define TFT_DARKSLATEBLUE 0x49F1
#define TFT_DARKSLATEGRAY 0x2A69
#define TFT_DARKTURQUOISE 0x067A
#define TFT_DARKVIOLET 0x901A
#define TFT_DEEPPINK 0xF8B2
#define TFT_DEEPSKYBLUE 0x05FF
#define TFT_DIMGRAY 0x6B4D
#define TFT_DODGERBLUE 0x1C9F
#define TFT_FIREBRICK 0xB104
#define TFT_FLORALWHITE 0xFFDE
#define TFT_FORESTGREEN 0x2444
#define TFT_FUCHSIA 0xF81F
#define TFT_GAINSBORO 0xDEFB
#define TFT_GHOSTWHITE 0xFFDF
#define TFT_GOLD 0xFEA0
#define TFT_GOLDENROD 0xDD24
#define TFT_GRAY 0x8410
#define TFT_GREEN2 0x0400
#define TFT_GREEN 0x07E0 /*   0, 255, 0 */
#define TFT_GREENYELLOW 0xAFE5 /* 173, 255, 47 */
#define TFT_HONEYDEW 0xF7FE
#define TFT_HOTPINK 0xFB56
#define TFT_INDIANRED 0xCAEB
#define TFT_INDIGO 0x4810
#define TFT_IVORY 0xFFFE
#define TFT_KHAKI 0xF731
#define TFT_LAVENDER 0xE73F
#define TFT_LAVENDERBLUSH 0xFF9E
#define TFT_LAWNGREEN 0x7FE0
#define TFT_LEMONCHIFFON 0xFFD9
#define TFT_LIGHTBLUE 0xAEDC
#define TFT_LIGHTCORAL 0xF410
#define TFT_LIGHTCYAN 0xE7FF
#define TFT_LIGHTGOLDENRODYELLOW 0xFFDA
#define TFT_LIGHTGREEN 0x9772
#define TFT_LIGHTGREY 0xC618 /* 192, 192, 192 */
#define TFT_LIGHTGREY2 0xD69A
#define TFT_LIGHTPINK 0xFDB8
#define TFT_LIGHTSALMON 0xFD0F
#define TFT_LIGHTSEAGREEN 0x2595
#define TFT_LIGHTSKYBLUE 0x867F
#define TFT_LIGHTSLATEGRAY 0x7453
#define TFT_LIGHTSTEELBLUE 0xB63B
#define TFT_LIGHTYELLOW 0xFFFC
#define TFT_LIME 0x07E0
#define TFT_LIMEGREEN 0x3666
#define TFT_LINEN 0xFF9C
#define TFT_MAGENTA 0xF81F /* 255, 0, 255 */
#define TFT_MAROON 0x7800 /* 128, 0, 0 */
#define TFT_MAROON2 0x8000
#define TFT_MEDIUMAQUAMARINE 0x6675
#define TFT_MEDIUMBLUE 0x0019
#define TFT_MEDIUMORCHID 0xBABA
#define TFT_MEDIUMPURPLE 0x939B
#define TFT_MEDIUMSEAGREEN 0x3D8E
#define TFT_MEDIUMSLATEBLUE 0x7B5D
#define TFT_MEDIUMSPRINGGREEN 0x07D3
#define TFT_MEDIUMTURQUOISE 0x4E99
#define TFT_MEDIUMVIOLETRED 0xC0B0
#define TFT_MIDNIGHTBLUE 0x18CE
#define TFT_MINTCREAM 0xF7FF
#define TFT_MISTYROSE 0xFF3C
#define TFT_MOCCASIN 0xFF36
#define TFT_NAVAJOWHITE 0xFEF5
#define TFT_NAVY 0x000F /*   0, 0, 128 */
#define TFT_NAVY2 0x0010
#define TFT_OLDLACE 0xFFBC
#define TFT_OLIVE 0x7BE0 /* 128, 128, 0 */
#define TFT_OLIVE2 0x8400
#define TFT_OLIVEDRAB 0x6C64
#define TFT_ORANGE 0xFD20 /* 255, 165, 0 */
#define TFT_ORANGERED 0xFA20
#define TFT_ORCHID 0xDB9A
#define TFT_PALEGOLDENROD 0xEF55
#define TFT_PALEGREEN 0x9FD3
#define TFT_PALETURQUOISE 0xAF7D
#define TFT_PALEVIOLETRED 0xDB92
#define TFT_PAPAYAWHIP 0xFF7A
#define TFT_PEACHPUFF 0xFED7
#define TFT_PERU 0xCC27
#define TFT_PINK 0xF81F
#define TFT_PINK_2 0xFE19
#define TFT_PLUM 0xDD1B
#define TFT_POWDERBLUE 0xB71C
#define TFT_PURPLE 0x780F /* 128, 0, 128 */
#define TFT_PURPLE2 0x8010
#define TFT_RED 0xF800 /* 255, 0, 0 */
#define TFT_ROSYBROWN 0xBC71
#define TFT_ROYALBLUE 0x435C
#define TFT_SADDLEBROWN 0x8A22
#define TFT_SALMON 0xFC0E
#define TFT_SANDYBROWN 0xF52C
#define TFT_SEAGREEN 0x2C4A
#define TFT_SEASHELL 0xFFBD
#define TFT_SIENNA 0xA285
#define TFT_SILVER 0xC618
#define TFT_SKYBLUE 0x867D
#define TFT_SLATEBLUE 0x6AD9
#define TFT_SLATEGRAY 0x7412
#define TFT_SNOW 0xFFDF
#define TFT_SPRINGGREEN 0x07EF
#define TFT_STEELBLUE 0x4416
#define TFT_TAN 0xD5B1
#define TFT_TEAL 0x0410
#define TFT_THISTLE 0xDDFB
#define TFT_TOMATO 0xFB08
#define TFT_TURQUOISE 0x471A
#define TFT_VIOLET 0xEC1D
#define TFT_WHEAT 0xF6F6
#define TFT_WHITE 0xFFFF /* 255, 255, 255 */
#define TFT_WHITESMOKE 0xF7BE
#define TFT_YELLOW 0xFFE0 /* 255, 255, 0 */
#define TFT_YELLOWGREEN 0x9E66
