/* APP_NAME        */ .string "Tetris"
/* APP_DESCRIPTION */ .string "My old Tetris, ported to hollyhock. Press Shift + Clear to quit.  github.com/SnailMath/CPoldTetris"
/* APP_AUTHOR      */ .string "SnailMath / ThatLolaSnail"
/* APP_VERSION     */ .string "1.0.1"

/*
 * This program was written by SnailMath / ThatLolaSnail for the calculator classpad II. 
 *
 * Before I had access to the hollyhock sdk written by The6P4C, ported by Stellaris-code, I wrote this program in assembly.
 * I wrote a loader to load this, but this version will work with the hollyhock loader.
 *
 * This was only a proof-of-concept, the code is not clean at all. A part of the subroutines from the sdk are overwritten with data, 
 * because with my old launcher this area was free to use. 
 * 
 * The use of the number keys was added for DasHeiligeDönerhohn
 * 
 */

.align 1 !make sure everything is aligned.
.global _main
_main:


sts.l pr, @-r15

mov.l lcdback, r2
jsr @r2
nop

reset:
!========== M A I N ==============================================================

!mov.l clearScreen, r2
!jsr @r2
!nop

mov #0, r1
mov.l VRam, r0
mov.l VRamEnde, r2
black:
  mov.l r1, @r0
  add #4, r0
  cmp/eq r2, r0
bf black

mov.l setCursor2, r2
mov #0, r4
jsr @r2
mov #0, r5

mov.l print, r2
mova tetris30, r0
mov r0, r4
jsr @r2
mov #0, r5

mov.l greygrey, r1
mov.l feldbaseA21b, r0
mov.l feldendAb, r2
feldloop:
  mov.l r1, @r0
  add #4, r0
  cmp/eq r2, r0
bf feldloop

!reset Vars
mov.l timerinit, r2
mova timer, r0
mov.l r2, @r0

mov #0, r14 !score = 0

!r14 feldBase - score
!r13 tile Base
!r12 tile nr
!r11 tile orientation
!r10 tile x
!r9 tile y
!r8 key1+2
!r7 key3+4
!r6 keyTimer
!r5 color arg for drawTile
!r4 timer
!r3,r2,r1 temp
!r0 key	//2 wait for a keypress; 1 key pressed, debouncing; 0 waiting for key released; 3 debouncing 

mov #1, r0 !mov #2, r0 !key
mov.l debounce, r6

mov #-1, r12	! tile nr
mainNewTile:


add #1, r12	! tile
mov #7, r11 !11 is reinitialized in a few lines, so we can use it now for other stuff (comparing)
cmp/eq r12, r11
bf TnoOk
	mov #0, r12 !tile (overflow)
TnoOk:


mov #0, r11	! orientation
mov #4, r10	! x
bsr Tcheck
mov #4, r9	! y
!bt reset
!bra okNew
bf okNew
!nop
!bf okNew
!	!       !Error while drawing new tile, game Over
	mov.l setCursor2, r2
	mov #7, r4
	jsr @r2
	mov #10, r5
	mov.l print, r2
	mova GameOver, r0
	mov r0, r4
	jsr @r2
	mov #1, r5
	mov.l drawScreen2, r2
	jsr @r2
	nop
	mov.l gotimer, r0
	gotimerloop:
	dt r0
	bf gotimerloop
	nop
	bra reset
	nop
        .align 2
	lcdback:	.long _LCD_VRAMBackup
        gotimer:        .long 0x08000000
        print:          .long _Debug_PrintString
  	setCursor2:	.long _Debug_SetCursorPosition
        GameOver:       .string " GAME  OVER "
        .align 2
	tetris30:	.string "     Tetris by SnailMath   "
	drawScreen2:	.long _LCD_Refresh
.align 2
feldbaseA21b:	.long feldbase21 !0x8c060000
!feldbaseA18:	.long feldbase21 + 60
feldendAb:	.long feldbase21 + 420
greygrey:	.long 0x84108410

okNew:

bsr drawTile
mov #1, r5
BSR subDraw
nop


mainResetTimer:

!faster
!mov.l timer, r4
!mov r0, r1
!mov.w timersub, r2
!cmp/hi r2,r4
!bf tofast
!sub r2, r4
!tofast:
!mova timer, r0
!mov.l r4, @r0
!mov r1, r0
mov r0, r1
mov.l timer, r4
mov r4, r2 !the value subtracted
!shlr2 r2
shlr2 r2
shlr8  r2
sub r2, r4
add #1, r4
mova timer, r0
mov.l r4, @r0
mov r1, r0


!========== L O O O P ==========

main:


!========== G E T   K E Y ==========
!read keys
mov.l keyAddr, r2
mov.w @r2, r8
shll16 r8
add #2, r2
mov.w @r2, r3
add r3, r8
add #2, r2
mov.w @r2, r7
shll16 r7
add #2, r2
mov.w @r2, r3
add r3, r7
add #2, r2
mov.w @r2, r3
add #2, r2
mov.w @r2, r3

mov.l clear8, r3
cmp/eq r3, r8
bf noclear
	!Break
	mov.l lcdrest, r2
	jsr @r2
	nop
	mov.l drawScreen, r2
	jsr @r2
	nop
	lds.l @r15+, pr
	rts
	nop
	.align 2
lcdrest:
	.long _LCD_VRAMRestore
noclear:

!check if keys need to be checked
cmp/eq #2, r0	!waiting for a key
   bt newKey	!jump if waiting
cmp/eq #0, r0	!waiting for key release
   bt relKey !jump if waiting
dt r6 !keyTimer !debouncing timer
   bf noInput	!check timer
dt r0 !after delay next step
mov.l debounce, r6
bra noInput
nop

!check if key released
relKey: !release Key
!Test, if down is still pressed, this will ignore the wait, so down repeats.
mov.l down7, r3
cmp/eq r3, r7
bf noDownRepeat
	mov #1, r0
	mov #1, r4 !reset the falling timer (pice falls right away)
noDownRepeat:
mov #0, r2
cmp/eq r2,r8
bf noInput !key not feleased
cmp/eq r2,r7
bf noInput
!key released
	!mov.l debounce, r6 !start debounce timer
	mov #3, r0
bra noInput
nop

!get new key
newKey:
 !up 	  7 00800000
 !left 	  8 00004000
 !down	  7 00400000
 !right	  8 00008000
 !key	  7 80000000
 !back <- 8 00000080
 !clear	  8 00020000
mov.l shift8, r3
cmp/eq r3, r8
bf noshift
	!New game
	!mov.l reset, r2
	bra reset!jsr @r2
	nop
noshift:

mov.l upchicken7, r3
cmp/eq r3,r7
bt up

mov.l up7, r3
cmp/eq r3, r7
bf noup

up:
	bsr drawTile
	mov #0, r5

	mov #1, r5
	add r5, r11
	mov #4, r5
	cmp/eq r5, r11		!if the rotation was 3, rotating 1 more step will/should cause it to overflow to 0
	bf rotOk
		mov #0, r11
	rotOk:

   !test 
	bsr Tcheck
	nop
	bf rotNoErr
		mov #-1, r5
		add r5, r11
		cmp/eq r5, r11	!if it was 3, we added 1, it overflowed to 0 and if we now subtract 1, we have -1 instead of 3, 
		bf rotNoErr	!so we need to underflow manually...
			mov #3, r11
	rotNoErr:

	bsr drawTile
	mov #1, r5

	BSR subDraw
	mov #1, r0
noup:

mov.l left8, r3
cmp/eq r3, r8
bf noleft
	bra left
	nop
noleft:

mov.l leftchicken7, r3
cmp/eq r3,r7
bf nochickenleft
	bra left
	nop
nochickenleft:

mov.l down7, r3
cmp/eq r3, r7
bf noDown
	mov #1, r0
	mov #1, r4 !reset the falling timer (pice falls right away)
noDown:

mov.l downchicken7, r3
cmp/eq r3, r7
bf nochickendown
	mov #1, r0
	mov #1, r4 !reset the falling timer (pice falls right away)
nochickendown:

mov.l right8, r3
cmp/eq r3, r8
bf noright
	bra  right
	nop
noright:

mov.l rightchicken8, r3
cmp/eq r3, r8
bf nochickenright
	bra right
	nop
nochickenright:

mov.l key7, r3
cmp/eq r3, r7
bf nokey7
	left:
	!bsr drawTile
	!mov #0, r5
	!mov #-1, r5
	!add r5, r10
	!bsr drawTile
	!mov #1, r5
	!BSR subDraw
	!mov #1, r0
   !remove current
	bsr drawTile
	mov #0, r5
   !modify 
	mov #-1, r5
	add r5, r10
   !test 
	bsr Tcheck
	nop
	bf leftNoErr
		mov #1, r5
		add r5, r10
	leftNoErr:
   !draw new
	bsr drawTile
	mov #1, r5
   !update screen
	BSR subDraw
	mov #1, r0
nokey7:

mov.l back8, r3
cmp/eq r3, r8
bf noback
	right:
   !remove
	bsr drawTile
	mov #0, r5
   !modify
	mov #1, r5
	add r5, r10
   !test 
	bsr Tcheck
	nop
	bf rightNoErr
		mov #-1, r5
		add r5, r10
	rightNoErr:
   !draw
	bsr drawTile
	mov #1, r5
   !update Screen
	BSR subDraw
	mov #1, r0
noback:
nokey:
noInput:

dt r4
bf main
!timer elapsed


	bsr drawTile
	mov #0, r5

	mov #1, r5
	add r5, r9 !Move tile 1 down
	
   !test 
	bsr Tcheck !Check if it does not collide
	nop
	bf downNext

		!It collides
		mov #-1, r5 !Move it back
		add r5, r9

		bsr drawTile !Place it back on the screen
		mov #1, r5

		BSR subDraw !Redraw the screen
		nop

		!check if a line is full (Empty registers r1-r4 r7-r11) r8 grey, r10/9 coord, r11 addr, r2 current addr, !!r7 white 
			mov.w grey, r8
			!mov #-1, r7
			mov.l feldbaseA21, r11
			mov #21 r9 !y !Warning, r9 goes from 20 to 1, not from 20 to 0, so no check in the invisible line at the top 
					!(0,1,2 are invisible, 3-20 are visible)
			winNexty:
			dt r9
				mov #10 r10 !x
				winNextx:
					dt r10 !r10 from 9 to 0 
					mov r9, r2		!r2 is the address (y address)
					shll2 r2   !mal 4	!multiply y by 4
					add r9, r2 !add (*5)	!add y (aka y*5)
					shll r2    !mal 2 (*10)	!mul y by 2 (aka y*10)
					add r10, r2		!add x 
					shll r2			!mul by 2 (1 word per block)
					add r11, r2		!add the base addr
					mov.w @r2,r3
					cmp/eq r3, r8 !check if grey
						bt winYcontinue ! if 1 block is wrong, next line
				tst r10,r10 !if r10 = 0
				bf winNextx
				!all blocks have been tested, no block was wrong, it is full
				!mov #10, r3
				add #18, r2 ! go to the right of the column
				mov r2, r3 ! move down
				add #-20, r3!from here
				whiteloop:
					mov.w @r3, r7
					mov.w r7, @r2
					add #-2, r2
					add #-2, r3
					cmp/eq r3, r11
				bf whiteloop
				add #1,r9 	!If the line was full and the rest was moved, the same line needs to be checked again.
				add #1, r14	!add score +1
			winYcontinue:
			tst r9,r9
			bf winNexty	
			


		bra mainNewTile !next tile
		nop

	downNext:
	!It does not collide
	bsr drawTile
	mov #1, r5

	BSR subDraw
	nop

bra mainResetTimer
nop





.align 2

!Debug
!setCursor:	.long 0x8002e418
!numPrint:	.long 0x801335bc

!clearScreen:	.long 0x800394d0 
!reset:		.long 0x80133694
VRam:		.long 0x8C000000
VRamEnde:	.long 0x8C052800 !320 wide, 528 tall, 2 byte per pixel
keyAddr:	.long 0xa44B0000
!return:		.long endofprog !break the program
up7:		.long 0x00800000 
left8:		.long 0x00004000
down7:		.long 0x00400000 
right8:		.long 0xffff8000
key7:		.long 0x80000000
back8:		.long 0x00000080 
clear8:		.long 0x80020000 !shift clear
shift8:		.long 0x80000080 !shift back
timerinit:	.long 0x00020000
timer:		.long 0x00000001 ! -256
debounce:	.long 0x00000004 !f the keys are bouncing, make higher, if the keys are not responding, make lower
!timersub:	.word 256 !2048  !at first, i subtracted 256, but it speeded up to fast. Now I divide by 32 and subtract

upchicken7:	.long 0x00200000
rightchicken8:	.long 0x00000010
leftchicken7:	.long 0x10000000
downchicken7:	.long 0x00080000



!========== T I L E ==========

!=== DRAW ===


!r14 feldBase
!r13 tile Base
!r12 tile nr
!r11 tile orientation
!r10 tile x
!r9 tile y
!r8 key1+2
!r7 key3+4
!r6 keyTimer
!r5 color arg for drawTile
!r4 timer
!r3,r2,r1 temp
!r0 key	//2 wait for a keypress; 1 key pressed, debouncing; 0 waiting for key released; 3 debouncing 
drawTile:
mov.l r14, @-r15	!feldBase?
mov.l r13, @-r15	!tilebase?
mov.l r12, @-r15	!tile nr
mov.l r11, @-r15	!tile ori
mov.l r10, @-r15	!tile x
mov.l r9, @-r15		!tile y
mov.l r8, @-r15		!key		actual address
mov.l r7, @-r15		!key		tmp
mov.l r6, @-r15		!key timer	color while drawing
!mov.l r5, @-r15	!add or remove tile  COLOR 0: color = 0x8410, 1: color = tilecolor (doesn't need to be saved)
mov.l r4, @-r15		!timer		actual x
mov.l r3, @-r15		!	  	actual y
mov.l r2, @-r15		!	  	address while drawing
mov.l r1, @-r15		!	  	number while drawing
mov.l r0, @-r15		!key state	tmp

mova tilesBase, r0
mov r0, r13
!mova feldbase21, r0
mov.l feldbaseA21, r14


!current tile is at tileBase + 34*nr + orient*8
!r8 = r12*34 (Tile offset)
mov r12, r8
shll2 r8
shll2 r8
add r12, r8
shll r8

add r13, r8 !Tile address (without color)

tst r5, r5		!check if color = 0
mov.w grey, r6	!load grey if no other color specified
bt nocolor
	mov r8, r6	!load color if specified
	add #32, r6 !address of color information
	mov.w @r6, r6 !color information
nocolor:
!mov r6, r5

!r7 = r11 * 8 (Orientation offset)
mov r11, r7	
shll2 r7
shll r7
add r7, r8	!Tile address (with orientation)

!for all the four blocks: load field with corresponding color
mov #4, r1
	loadTileToFeld:
	mov.b @r8, r4	!load x coordinate of the current block into r4
	add #1, r8
	add r10, r4	!move the pice to the correct location
	mov.b @r8, r3
	add #1, r8
	add r9, r3
	! r4|r3 is the current block, change it to the color
		mov r3, r2		!r2 is the address (y address)
		shll2 r2   !mal 4	!multiply y by 4
		add r3, r2 !add (*5)	!add y (aka y*5)
		shll r2    !mal 2 (*10)	!mul y by 2 (aka y*10)
		add r4, r2		!add x 
		shll r2			!mul by 2 (1 word per block)
		add r14, r2		!add the base addr
		mov.w r6, @r2
	dt r1
bf loadTileToFeld

mov.l @r15+, r0
mov.l @r15+, r1
mov.l @r15+, r2
mov.l @r15+, r3
mov.l @r15+, r4
!mov.l @r15+, r5 !not saved 
mov.l @r15+, r6
mov.l @r15+, r7
mov.l @r15+, r8
mov.l @r15+, r9
mov.l @r15+, r10
mov.l @r15+, r11
mov.l @r15+, r12
mov.l @r15+, r13
rts
mov.l @r15+, r14

!grey:	.word 0x8410

!=== CHECK === 
!Check if the current pice conflicts with others; Nothing will change

!T bit is used for return. 	0 everything ok
!				1 error

Tcheck:
!r14 feldBase
!r13 tile Base
!r12 tile nr
!r11 tile orientation
!r10 tile x
!r9 tile y
!r8 key1+2
!r7 key3+4
!r6 keyTimer
!r5 color arg for drawTile
!r4 timer
!r3,r2,r1 temp
!r0 key	//2 wait for a keypress; 1 key pressed, debouncing; 0 waiting for key released; 3 debouncing 
mov.l r14, @-r15	!feldBase?
mov.l r13, @-r15	!tilebase?	later: xmax = 10
mov.l r12, @-r15	!tile nr	later: ymax = 21
mov.l r11, @-r15	!tile ori	later: grey
mov.l r10, @-r15	!tile x
mov.l r9, @-r15		!tile y
mov.l r8, @-r15		!key		actual address
mov.l r7, @-r15		!key		tmp
mov.l r6, @-r15		!key timer	color while drawing
!mov.l r5, @-r15	!add or remove tile  COLOR 0: color = 0x8410, 1: color = tilecolor (doesn't need to be saved)
mov.l r4, @-r15		!timer		actual x
mov.l r3, @-r15		!	  	actual y
mov.l r2, @-r15		!	  	address while drawing
mov.l r1, @-r15		!	  	number while drawing
mov.l r0, @-r15		!key state	tmp

mova tilesBase, r0
mov r0, r13
!mova feldbase21, r0
mov.l feldbaseA21, r14


!current tile is at tileBase + 34*nr + orient*8
!r8 = r12*34 (Tile offset)
mov r12, r8
shll2 r8
shll2 r8
add r12, r8
shll r8

add r13, r8 !Tile address (without color)
!mov r6, r5

!r7 = r11 * 8 (Orientation offset)
mov r11, r7	
shll2 r7
shll r7
add r7, r8	!Tile address (with orientation)

mov.w grey, r11	!now we don't need the tile info (number/orientation because we have the acutual address in a reg) but we need the background color
mov #10, r13 !xmax
mov #21, r12 !ymax

!for all the four blocks: check field against background color
mov #4, r1
	checkTileToFeld:
	mov.b @r8, r4	!load x coordinate of the current block into r4
	add #1, r8
	add r10, r4	!move the pice to the correct location
	mov.b @r8, r3
	add #1, r8
	add r9, r3
	! r4|r3 is the current block, test its color
		!check if r4|r3 is inside the playfield
		cmp/hi r4, r13
		bf outside
		cmp/hi r3, r12
		bt yinside
		outside:
			bra checkTileEnd
			sett !mov #1, r5
		yinside:
		mov r3, r2		!r2 is the address (y address)
		shll2 r2   !mal 4	!multiply y by 4
		add r3, r2 !add (*5)	!add y (aka y*5)
		shll r2    !mal 2 (*10)	!mul y by 2 (aka y*10)
		add r4, r2		!add x 
		shll r2			!mul by 2 (1 word per block)
		add r14, r2		!add the base addr
			mov.w @r2, r7
			cmp/eq r7, r11	!check if pixel is blank
			bt checkTileNoCheck	!proceed
				bra checkTileEnd 
				sett !mov #1, r5	!return 1 if error
		checkTileNoCheck:
	dt r1
bf checkTileToFeld

clrt !mov #0, r5 !return 0 if everything is ok

checkTileEnd:
mov.l @r15+, r0
mov.l @r15+, r1
mov.l @r15+, r2
mov.l @r15+, r3
mov.l @r15+, r4
!mov.l @r15+, r5 !not saved 
mov.l @r15+, r6
mov.l @r15+, r7
mov.l @r15+, r8
mov.l @r15+, r9
mov.l @r15+, r10
mov.l @r15+, r11
mov.l @r15+, r12
mov.l @r15+, r13
rts
mov.l @r15+, r14

grey:	.word 0x8410



!========== D R A W   S U B R O U T I N E ==========
subDraw: !Sub draw
mov.l r0, @-r15
mov.l r1, @-r15
mov.l r2, @-r15
mov.l r3, @-r15
mov.l r4, @-r15
mov.l r5, @-r15
mov.l r6, @-r15
mov.l r7, @-r15
mov.l r8, @-r15
mov.l r9, @-r15
mov.l r10, @-r15
mov.l r11, @-r15
mov.l r12, @-r15
sts.l pr, @-r15
mov.l r13, @-r15
mov.l r14, @-r15


!'r0 pixel
!'r1 feldx
!'r2 feldy
!'r3 subx
!'r4 suby
!'r5 color2
!'r14 feldBase
!'r8 (320-31)\EE2 = h'242
!'r8 (320-27)\EE2 = h'24A
!'r9(-31\EE320+32)\EE2=h'ffffB2C0
!'r9(-27\EE320+28)\EE2=h'ffffBCB8
!'r10(32\EE320-8\EE32)\EE2=0x4E00
!'r10(28\EE320-10\EE28)\EE2=43D0
!'r11 = 10
!'r12 = 18

!mova feldbase18, r0
mov.l feldbaseA18, r14
mov.l pixel, r0
mov.w h24A, r8
mov.w hBCB8, r9
mov.w h4E00, r10
mov #10, r11
mov #18, r12
mov #0, r2 !feldy
fieldy: !'/while feldy<18:
	mov #0, r1 !feldx
	feldxeq10: !'/while feldx<10:
		mov r2, r5
		shll2 r5   !mal 4
		add r2, r5 !add (*5)
		shll r5    !mal 2 (*10)	
		add r1, r5
		shll r5
		add r14, r5
		mov.w @r5, r5
		mov #27, r4
		subygr0: !'/while suby>0
			mov #27, r3
			subxgr0: !'/while subx>0
				mov.w r5, @r0
				add #2,r0 !1 px to the right
				dt r3
			bf subxgr0
			add r8, r0 !27 pixels to the left an 1 down //0x242  //+320-31  \EE2  = 0x242
			dt r4
		bf subygr0
		add r9, r0 !0x5200 //back 27 pixels up and 28 pixels right   //-31\EE320 +32  \EE2 = 0xFFFF B2C0
		add #1, r1
		cmp/eq r11, r1 !r11 = 10
	bf feldxeq10
	add r10, r0 !'add(32\EE320-8\EE32)\EE2, r0 ' = 0x4E00
	add #1, r2
	cmp/eq r12, r2 !18
bf fieldy

!Score

mov #2, r5
mov #0, r6

mov.l numPrint, r2
mov.l @r15+, r14
jsr @r2
mov r14, r4


!Update screen
mov.l drawScreen, r2
jsr @r2

mov.l @r15+, r13
lds.l @r15+, pr
mov.l @r15+, r12
mov.l @r15+, r11
mov.l @r15+, r10
mov.l @r15+, r9
mov.l @r15+, r8
mov.l @r15+, r7
mov.l @r15+, r6
mov.l @r15+, r5
mov.l @r15+, r4
mov.l @r15+, r3
mov.l @r15+, r2
mov.l @r15+, r1
rts
mov.l @r15+, r0

.align 2
  pixel:	!.long 0x8C001928
		.long 0x8c003c28
! vram 0x8c000000 + (320 * free space on the top + free space left )*2
! tetris: 10 x 18     box 28x28 px
! tetris field: 280x504 px
! screen: 320x528
! space: left+right = 40, 20 per site
!        top+bottom = 24, all on top, no on the bottom
! 0x8c000000 + (320 * 24 + 20)*2
! 0x8c000000 + 7700 * 2 = 0x8c0000 + 15400 = 8c000000 + 0x3c28 = 0x8c003c28


  setCursor:	.long _Debug_SetCursorPosition
  numPrint:	.long _Debug_PrintNumberHex_Byte
  !feldbase:	.long 0x8c060000 !only a test
  drawScreen:	.long _LCD_Refresh

  h24A:		.word 0x024A
  hBCB8:	.word 0xBCB8
  h4E00:	.word 0x43D0

!========== T I L E S ===========

.align 2
tilesBase: !I (Cyan) !The x coord is positive, the y coord is negative, the basepoint of the tile is in the bottom left corner, the screen has its base in the upper left corner, so 1 to the right is x=1 and 1 up is y=-1 = 0xff
		.word 0x0000; .word 0x0100; .word 0x0200; .word 0x0300 
		.word 0x0000; .word 0x00FF; .word 0x00FE; .word 0x00FD 
		.word 0x0000; .word 0x0100; .word 0x0200; .word 0x0300
		.word 0x0000; .word 0x00FF; .word 0x00FE; .word 0x00FD; .word 0b0000011111111111 !Color 
	!O (Yellow)
		.word 0x0000; .word 0x00FF; .word 0x0100; .word 0x01FF 
		.word 0x0000; .word 0x00FF; .word 0x0100; .word 0x01FF 
		.word 0x0000; .word 0x00FF; .word 0x0100; .word 0x01FF 
		.word 0x0000; .word 0x00FF; .word 0x0100; .word 0x01FF; .word 0b1111111111100000
	!T (Purple)
		.word 0x0000; .word 0x0100; .word 0x0200; .word 0x01FF 
		.word 0x0000; .word 0x00FF; .word 0x00FE; .word 0x01FF 
		.word 0x00FF; .word 0x01FF; .word 0x02FF; .word 0x0100 
		.word 0x0100; .word 0x01FF; .word 0x01FE; .word 0x00FF; .word 0b1000000000011111
	!S (Green)
		.word 0x0000; .word 0x0100; .word 0x01FF; .word 0x02FF 
		.word 0x00FE; .word 0x00FF; .word 0x01FF; .word 0x0100 
		.word 0x0000; .word 0x0100; .word 0x01FF; .word 0x02FF 
		.word 0x00FE; .word 0x00FF; .word 0x01FF; .word 0x0100; .word 0b0000011000000000
	!Z (Red)
		.word 0x00FF; .word 0x01FF; .word 0x0100; .word 0x0200 
		.word 0x0000; .word 0x00FF; .word 0x01FF; .word 0x01FE 
		.word 0x00FF; .word 0x01FF; .word 0x0100; .word 0x0200 
		.word 0x0000; .word 0x00FF; .word 0x01FF; .word 0x01FE; .word 0b1111100000011111
	!J (Blue)
		.word 0x0000; .word 0x0100; .word 0x0200; .word 0x00FF 
		.word 0x0000; .word 0x00FF; .word 0x00FE; .word 0x01FE 
		.word 0x00FF; .word 0x01FF; .word 0x02FF; .word 0x0200 
		.word 0x0000; .word 0x0100; .word 0x01FF; .word 0x01FE; .word 0b0000000000011111
	!L (Orange)
		.word 0x0000; .word 0x0100; .word 0x0200; .word 0x02FF 
		.word 0x0000; .word 0x00FF; .word 0x00FE; .word 0x0100 
		.word 0x0000; .word 0x00FF; .word 0x01FF; .word 0x02FF 
		.word 0x00FE; .word 0x01FE; .word 0x01FF; .word 0x0100; .word 0b1111110000000000

.align 2
feldbaseA21: .long feldbase21 !0x8c060000
feldbaseA18: .long feldbase21 + 60
!feldendA:    .long feldbase21 + 420
feldbase21:

