/*!
 *******************************************************************************
 *
 * @file			start.asm
 *
 * @brief			In dieser Datei beginnt die Ausführung von Quelltext.
 *
 * @details			In dieser Datei, an dem Symbol '_PowerON_Reset', beginnt
 *					die Ausführung von Quelltext. Dies wird mit der
 *					Linkeroption '-e_PowerON_Reset' festgelegt (siehe
 *					Projectproperties -> C/C++ Build -> Settings ->
 *					Tool Settings -> Linker -> Output). Aus dieser Datei
 *					heraus wird, nach Initialisierung der Kontrollregister,
 *					der Sektionen und der Hardware, main( ) aufgerufen.
 *					Wenn main( ) zurückkehrt wird _exit( 0 ) aufgerufen.
 *
 * @par				Vorlage
 *					Vorlage dieser Datei ist die Datei reset_program.asm eines
 *					mit e² Studio und KPIT GNURX erstellten Sample-Project.
 *
 * @author			Christopher Zink
 *
 * @date			Juni 2015
 *
 * @version			1.0
 *
 * @see				r01uh0041ej0160_rx63n631.pdf für die Beschreibung der
 *					Register und r01us0032ej0120_rxsm.pdf für die Beschreibung
 *					der Assemblerbefehle.
 *
 *******************************************************************************
 */

/*!
 * @cond			DOXYGEN_IGNORE_ASM
 */

	#include "bsp.h"

	.list
	.section .text

	.global _PowerON_Reset
	.extern _hardwareSetup
	.extern _main
#if CALL_EXIT_ON_END
	.extern _exit
#endif

	/* Das sind alles Sektionen, die in
	   <Projectname>_HardwareDebug_auto.gsi
	   definiert werden. Das Linkerskript
	   *.gsi wird aus den Einstellungen unter
	   Projectproperties -> C/C++ Build ->
	   Settings -> Tool Settings -> Linker ->
	   Sections generiert. */
	.extern _data
	.extern _mdata
	.extern _ebss
	.extern _bss
	.extern _edata
	.extern _ustack
	.extern _istack
	.extern _rvectors_start

_PowerON_Reset:

	/* Den Userstackpointer (USP) initialisieren.
	   MVTC = move to control-regiser. */
	MVTC		#_ustack, USP

	/* Den Interruptstackpointer (ISP) initialisieren.  */
	MVTC		#_istack, ISP

	/* Das Interrupttableregister (INTB) initialisieren.  */
	MVTC		#_rvectors_start, INTB

	/* Das Floatingpointstatusword (FPSW) belegen. */
	MVTC		#100h, FPSW

	/* Die Datensektion vom ROM in 's RAM laden.
	   MOV   = move,
	   SUB   = subtract,
	   SMOVF = string move forward. */
    MOV			#_mdata, R2
    MOV			#_data,  R1
    MOV			#_edata, R3
    SUB			R1,      R3
    SMOVF

	/* Die BSS-Sektion mit 0en belegen.
	   SSTR.B = string store, byte-by-byte. */
    MOV			#00h,   R2
    MOV			#_ebss, R3
    MOV			#_bss,  R1
    SUB			R1,     R3
    SSTR.B

	/* Die Hardware initialisieren.
	   hardwareSetup( ) ist in
	   hwsetup.c implementiert.
	   BSR = branch to subroutine. */
    BSR.A		_hardwareSetup
    NOP

	/* Die Interrupts aktivieren.
	   SETPSW = set flag of psw. */
	MVTC		#0, PSW
	SETPSW		I

	/* Den Prozessormodus von 'supervisior' zu 'user' wechseln.
	   MVFC   = move from control-regiser,
	   OR     = or
	   PUSH.L = push long,
	   ADD    = add,
	   PC     = program counter,
	   RTE    = return from exception. */
#if RUN_IN_USERMODE
    MVFC		PSW,        R1
    OR			#00100000h, R1
    PUSH.L		R1
    MVFC		PC,         R1
    ADD			#10,        R1
    PUSH.L		R1
    RTE
    NOP
    NOP
#endif

	/* main( ) aufrufen. */
    BSR.A		_main

#if CALL_EXIT_ON_END
    /* exit( 0 ) aufrufen. */
	MOV.L		#0, R1
    BSR.A		_exit
#else
	/* Endlosschleife. Damit es (wirklich)
	   keinen Ausweg gibt, Interrupts aus.
	   Nicht maskierbare Interrupts
	   können immer noch feuern!
	   CLRPSW = clear prosessor status word flag,
	   I      = interrupt flag,
	   BRA    = branch allways. */
	CLRPSW		I
	infinitloop:
		BRA			infinitloop;
#endif
    
    /* Ende. */
    .text    
    .end

/*!
 * @endcond
 */
