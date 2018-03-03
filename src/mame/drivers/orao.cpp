// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/***************************************************************************

        Orao driver by Miodrag Milanovic

        01/03/2008 Updated to work with latest SVN code
        23/02/2008 Sound support added.
        22/02/2008 Preliminary driver.

        Driver is based on work of Josip Perusanec

****************************************************************************/

#include "emu.h"
#include "includes/orao.h"

#include "cpu/m6502/m6502.h"
#include "sound/wave.h"

#include "screen.h"
#include "softlist.h"
#include "speaker.h"

#include "formats/orao_cas.h"


/* Address maps */
ADDRESS_MAP_START(orao_state::orao_mem)
	AM_RANGE( 0x0000, 0x5fff ) AM_RAM AM_SHARE("memory")
	AM_RANGE( 0x6000, 0x7fff ) AM_RAM AM_SHARE("video_ram") // video ram
	AM_RANGE( 0x8000, 0x9fff ) AM_READWRITE(orao_io_r, orao_io_w )
	AM_RANGE( 0xa000, 0xafff ) AM_RAM  // extension
	AM_RANGE( 0xb000, 0xbfff ) AM_RAM  // DOS
	AM_RANGE( 0xc000, 0xdfff ) AM_ROM
	AM_RANGE( 0xe000, 0xffff ) AM_ROM
ADDRESS_MAP_END


/* Input ports */
static INPUT_PORTS_START( orao )
	PORT_START("LINE.0")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_LEFT) PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_UP) PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(UCHAR_MAMEKEY(RIGHT))

	PORT_START("LINE.1")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Control") PORT_CODE(KEYCODE_LCONTROL) PORT_CODE(KEYCODE_RCONTROL)
	PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE.2")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F1") PORT_CODE(KEYCODE_F1) PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F2") PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F3") PORT_CODE(KEYCODE_F3) PORT_CHAR(UCHAR_MAMEKEY(F3))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F4") PORT_CODE(KEYCODE_F4) PORT_CHAR(UCHAR_MAMEKEY(F4))

	PORT_START("LINE.3")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE.4")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_R) PORT_CHAR('R')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_T) PORT_CHAR('T')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6) PORT_CHAR('6') PORT_CHAR('&')

	PORT_START("LINE.5")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5) PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4) PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE.6")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_O) PORT_CHAR('O')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_I) PORT_CHAR('I')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7) PORT_CHAR('7') PORT_CHAR('\'')

	PORT_START("LINE.7")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_8) PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9) PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE.8")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_W) PORT_CHAR('W')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1) PORT_CHAR('1') PORT_CHAR('!')

	PORT_START("LINE.9")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2) PORT_CHAR('2') PORT_CHAR('"')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3) PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE.10")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_L) PORT_CHAR('L')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_J) PORT_CHAR('J')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_K) PORT_CHAR('K')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_M) PORT_CHAR('M')

	PORT_START("LINE.11")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA) PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP) PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE.12")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')

	PORT_START("LINE.13")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE.14")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_G) PORT_CHAR('G')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_N) PORT_CHAR('N')

	PORT_START("LINE.15")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE.16")
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ch") PORT_CODE(KEYCODE_COLON)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Cj") PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Zh") PORT_CODE(KEYCODE_SLASH)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH2) PORT_CHAR(':') PORT_CHAR('*')

	PORT_START("LINE.17")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Del") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('^') PORT_CHAR('@')
	PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE.18")
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_P) PORT_CHAR('P')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Dj") PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Sh") PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS) PORT_CHAR(';') PORT_CHAR('+')

	PORT_START("LINE.19")
	PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS) PORT_CHAR('-') PORT_CHAR('=')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END

/* Machine driver */
MACHINE_CONFIG_START(orao_state::orao)
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6502, 1000000)
	MCFG_CPU_PROGRAM_MAP(orao_mem)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_SIZE(256, 256)
	MCFG_SCREEN_VISIBLE_AREA(0, 256-1, 0, 256-1)
	MCFG_SCREEN_UPDATE_DRIVER(orao_state, screen_update_orao)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_PALETTE_ADD_MONOCHROME("palette")


	/* audio hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, "cassette")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MCFG_CASSETTE_ADD( "cassette")
	MCFG_CASSETTE_FORMATS(orao_cassette_formats)
	MCFG_CASSETTE_DEFAULT_STATE(CASSETTE_STOPPED | CASSETTE_SPEAKER_ENABLED | CASSETTE_MOTOR_ENABLED)
	MCFG_CASSETTE_INTERFACE("orao_cass")

	MCFG_SOFTWARE_LIST_ADD("cass_list","orao")
MACHINE_CONFIG_END

/* ROM definition */
ROM_START( orao )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "bas12.rom", 0xc000, 0x2000, CRC(42ae6f69) SHA1(b9d4a544fae13a9c492af027545178addd557111) )
	ROM_LOAD( "crt12.rom", 0xe000, 0x2000, CRC(94ebdc94) SHA1(3959d717f96558823ccc806c842d2fb5ab0c3890) )
ROM_END

ROM_START( orao103 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "bas13.rom", 0xc000, 0x2000, CRC(35daf5da) SHA1(499c5a4bd930c26ec6226623c2793b4c7f771658) )
	ROM_LOAD( "crt13.rom", 0xe000, 0x2000, CRC(e7076014) SHA1(0e213287b0b520440af6a2a6297788a9356818c2) )
ROM_END

/* Driver */
//    YEAR  NAME      PARENT  COMPAT  MACHINE  INPUT  STATE       INIT     COMPANY         FULLNAME    FLAGS
COMP( 1984, orao,     0,      0,      orao,    orao,  orao_state, orao,    "PEL Varazdin", "Orao 102", 0 )
COMP( 1985, orao103,  orao,   0,      orao,    orao,  orao_state, orao103, "PEL Varazdin", "Orao 103", 0 )
