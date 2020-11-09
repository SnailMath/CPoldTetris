#include <appdef.hpp>

/*
 * This program was written by SnailMath for the calculator classpad II. 
 *
 * Before I had access to the hollyhock sdk written by The6P4C, ported by Stellaris-code, I wrote this program in assembly.
 * I wrote a loader to load this, but this version will work with the hollyhock loader.
 *
 * This was only a proof-of-concept, the code is not clean at all. A part of the subroutines from the sdk are overwritten with data, 
 * because with my old launcher this area was free to use. 
 *
 */

//The hollyhock data
APP_NAME("Tetris")
APP_DESCRIPTION("My old Tetris, ported to hollyhock. Press Shift + Clear to quit.  github.com/SnailMath/CPoldTetris")
APP_AUTHOR("SnailMath")
APP_VERSION("1.0.0")

extern "C"
void main() {
    // Put your app's code here!
    asm("\n\
!_main:\n\
sts.l pr, @-r15\n\
\n\
mov.l lcdback, r2\n\
jsr @r2\n\
nop\n\
\n\
reset:\n\
!========== M A I N ==============================================================\n\
\n\
!mov.l clearScreen, r2\n\
!jsr @r2\n\
!nop\n\
\n\
mov #0, r1\n\
mov.l VRam, r0\n\
mov.l VRamEnde, r2\n\
black:\n\
  mov.l r1, @r0\n\
  add #4, r0\n\
  cmp/eq r2, r0\n\
bf black\n\
\n\
mov.l setCursor2, r2\n\
mov #0, r4\n\
jsr @r2\n\
mov #0, r5\n\
\n\
mov.l print, r2\n\
mova tetris30, r0\n\
mov r0, r4\n\
jsr @r2\n\
mov #0, r5\n\
\n\
mov.l greygrey, r1\n\
mov.l feldbaseA21b, r0\n\
mov.l feldendAb, r2\n\
feldloop:\n\
  mov.l r1, @r0\n\
  add #4, r0\n\
  cmp/eq r2, r0\n\
bf feldloop\n\
\n\
!reset Vars\n\
mov.l timerinit, r2\n\
mova timer, r0\n\
mov.l r2, @r0\n\
\n\
mov #0, r14 !score = 0\n\
\n\
!r14 feldBase - score\n\
!r13 tile Base\n\
!r12 tile nr\n\
!r11 tile orientation\n\
!r10 tile x\n\
!r9 tile y\n\
!r8 key1+2\n\
!r7 key3+4\n\
!r6 keyTimer\n\
!r5 color arg for drawTile\n\
!r4 timer\n\
!r3,r2,r1 temp\n\
!r0 key	//2 wait for a keypress; 1 key pressed, debouncing; 0 waiting for key released; 3 debouncing \n\
\n\
mov #1, r0 !mov #2, r0 !key\n\
mov.l debounce, r6\n\
\n\
mov #-1, r12	! tile nr\n\
mainNewTile:\n\
\n\
\n\
add #1, r12	! tile\n\
mov #7, r11 !11 is reinitialized in a few lines, so we can use it now for other stuff (comparing)\n\
cmp/eq r12, r11\n\
bf TnoOk\n\
	mov #0, r12 !tile (overflow)\n\
TnoOk:\n\
\n\
\n\
mov #0, r11	! orientation\n\
mov #4, r10	! x\n\
bsr Tcheck\n\
mov #4, r9	! y\n\
!bt reset\n\
!bra okNew\n\
bf okNew\n\
!nop\n\
!bf okNew\n\
!	!       !Error while drawing new tile, game Over\n\
	mov.l setCursor, r2\n\
	mov #7, r4\n\
	jsr @r2\n\
	mov #10, r5\n\
	mov.l print, r2\n\
	mova GameOver, r0\n\
	mov r0, r4\n\
	jsr @r2\n\
	mov #1, r5\n\
	mov.l drawScreen, r2\n\
	jsr @r2\n\
	nop\n\
	mov.l gotimer, r0\n\
	gotimerloop:\n\
	dt r0\n\
	bf gotimerloop\n\
	nop\n\
	bra reset\n\
	nop\n\
        .align 2\n\
	lcdback:	.long _LCD_VRAMBackup\n\
        gotimer:        .long 0x08000000\n\
        print:          .long _Debug_PrintString\n\
  	setCursor2:	.long _Debug_SetCursorPosition\n\
        GameOver:       .string \" GAME  OVER \"\n\
        .align 2\n\
	tetris30:	.string \"     Tetris by SnailMath   \"\n\
.align 2\n\
feldbaseA21b:	.long feldbase21 !0x8c060000\n\
!feldbaseA18:	.long feldbase21 + 60\n\
feldendAb:	.long feldbase21 + 420\n\
greygrey:	.long 0x84108410\n\
\n\
okNew:\n\
\n\
bsr drawTile\n\
mov #1, r5\n\
BSR subDraw\n\
nop\n\
\n\
\n\
mainResetTimer:\n\
\n\
!faster\n\
!mov.l timer, r4\n\
!mov r0, r1\n\
!mov.w timersub, r2\n\
!cmp/hi r2,r4\n\
!bf tofast\n\
!sub r2, r4\n\
!tofast:\n\
!mova timer, r0\n\
!mov.l r4, @r0\n\
!mov r1, r0\n\
mov r0, r1\n\
mov.l timer, r4\n\
mov r4, r2 !the value subtracted\n\
!shlr2 r2\n\
shlr2 r2\n\
shlr8  r2\n\
sub r2, r4\n\
add #1, r4\n\
mova timer, r0\n\
mov.l r4, @r0\n\
mov r1, r0\n\
\n\
\n\
!========== L O O O P ==========\n\
\n\
main:\n\
\n\
\n\
!========== G E T   K E Y ==========\n\
!read keys\n\
mov.l keyAddr, r2\n\
mov.w @r2, r8\n\
shll16 r8\n\
add #2, r2\n\
mov.w @r2, r3\n\
add r3, r8\n\
add #2, r2\n\
mov.w @r2, r7\n\
shll16 r7\n\
add #2, r2\n\
mov.w @r2, r3\n\
add r3, r7\n\
add #2, r2\n\
mov.w @r2, r3\n\
add #2, r2\n\
mov.w @r2, r3\n\
\n\
mov.l clear8, r3\n\
cmp/eq r3, r8\n\
bf noclear\n\
	!Break\n\
	mov.l lcdrest, r2\n\
	jsr @r2\n\
	nop\n\
	mov.l drawScreen, r2\n\
	jsr @r2\n\
	nop\n\
	lds.l @r15+, pr\n\
	rts\n\
	nop\n\
	.align 2\n\
lcdrest:\n\
	.long _LCD_VRAMRestore\n\
noclear:\n\
\n\
!check if keys need to be checked\n\
cmp/eq #2, r0	!waiting for a key\n\
   bt newKey	!jump if waiting\n\
cmp/eq #0, r0	!waiting for key release\n\
   bt relKey !jump if waiting\n\
dt r6 !keyTimer !debouncing timer\n\
   bf noInput	!check timer\n\
dt r0 !after delay next step\n\
mov.l debounce, r6\n\
bra noInput\n\
nop\n\
\n\
!check if key released\n\
relKey: !release Key\n\
!Test, if down is still pressed, this will ignore the wait, so down repeats.\n\
mov.l down7, r3\n\
cmp/eq r3, r7\n\
bf noDownRepeat\n\
	mov #1, r0\n\
	mov #1, r4 !reset the falling timer (pice falls right away)\n\
noDownRepeat:\n\
mov #0, r2\n\
cmp/eq r2,r8\n\
bf noInput !key not feleased\n\
cmp/eq r2,r7\n\
bf noInput\n\
!key released\n\
	!mov.l debounce, r6 !start debounce timer\n\
	mov #3, r0\n\
bra noInput\n\
nop\n\
\n\
!get new key\n\
newKey:\n\
 !up 	  7 00800000\n\
 !left 	  8 00004000\n\
 !down	  7 00400000\n\
 !right	  8 00008000\n\
 !key	  7 80000000\n\
 !back <- 8 00000080\n\
 !clear	  8 00020000\n\
mov.l shift8, r3\n\
cmp/eq r3, r8\n\
bf noshift\n\
	!New game\n\
	!mov.l reset, r2\n\
	bra reset!jsr @r2\n\
	nop\n\
noshift:\n\
mov.l up7, r3\n\
cmp/eq r3, r7\n\
bf noup\n\
	bsr drawTile\n\
	mov #0, r5\n\
\n\
	mov #1, r5\n\
	add r5, r11\n\
	mov #4, r5\n\
	cmp/eq r5, r11		!if the rotation was 3, rotating 1 more step will/should cause it to overflow to 0\n\
	bf rotOk\n\
		mov #0, r11\n\
	rotOk:\n\
\n\
   !test \n\
	bsr Tcheck\n\
	nop\n\
	bf rotNoErr\n\
		mov #-1, r5\n\
		add r5, r11\n\
		cmp/eq r5, r11	!if it was 3, we added 1, it overflowed to 0 and if we now subtract 1, we have -1 instead of 3, \n\
		bf rotNoErr	!so we need to underflow manually...\n\
			mov #3, r11\n\
	rotNoErr:\n\
\n\
	bsr drawTile\n\
	mov #1, r5\n\
\n\
	BSR subDraw\n\
	mov #1, r0\n\
noup:\n\
mov.l left8, r3\n\
cmp/eq r3, r8\n\
bf noleft\n\
	bra left\n\
	nop\n\
noleft:\n\
mov.l down7, r3\n\
cmp/eq r3, r7\n\
bf noDown\n\
	mov #1, r0\n\
	mov #1, r4 !reset the falling timer (pice falls right away)\n\
noDown:\n\
mov.l right8, r3\n\
cmp/eq r3, r8\n\
bf noright\n\
	bra  right\n\
	nop\n\
noright:\n\
mov.l key7, r3\n\
cmp/eq r3, r7\n\
bf nokey7\n\
	left:\n\
	!bsr drawTile\n\
	!mov #0, r5\n\
	!mov #-1, r5\n\
	!add r5, r10\n\
	!bsr drawTile\n\
	!mov #1, r5\n\
	!BSR subDraw\n\
	!mov #1, r0\n\
   !remove current\n\
	bsr drawTile\n\
	mov #0, r5\n\
   !modify \n\
	mov #-1, r5\n\
	add r5, r10\n\
   !test \n\
	bsr Tcheck\n\
	nop\n\
	bf leftNoErr\n\
		mov #1, r5\n\
		add r5, r10\n\
	leftNoErr:\n\
   !draw new\n\
	bsr drawTile\n\
	mov #1, r5\n\
   !update screen\n\
	BSR subDraw\n\
	mov #1, r0\n\
nokey7:\n\
mov.l back8, r3\n\
cmp/eq r3, r8\n\
bf noback\n\
	right:\n\
   !remove\n\
	bsr drawTile\n\
	mov #0, r5\n\
   !modify\n\
	mov #1, r5\n\
	add r5, r10\n\
   !test \n\
	bsr Tcheck\n\
	nop\n\
	bf rightNoErr\n\
		mov #-1, r5\n\
		add r5, r10\n\
	rightNoErr:\n\
   !draw\n\
	bsr drawTile\n\
	mov #1, r5\n\
   !update Screen\n\
	BSR subDraw\n\
	mov #1, r0\n\
noback:\n\
nokey:\n\
noInput:\n\
\n\
dt r4\n\
bf main\n\
!timer elapsed\n\
\n\
\n\
	bsr drawTile\n\
	mov #0, r5\n\
\n\
	mov #1, r5\n\
	add r5, r9 !Move tile 1 down\n\
	\n\
   !test \n\
	bsr Tcheck !Check if it does not collide\n\
	nop\n\
	bf downNext\n\
\n\
		!It collides\n\
		mov #-1, r5 !Move it back\n\
		add r5, r9\n\
\n\
		bsr drawTile !Place it back on the screen\n\
		mov #1, r5\n\
\n\
		BSR subDraw !Redraw the screen\n\
		nop\n\
\n\
		!check if a line is full (Empty registers r1-r4 r7-r11) r8 grey, r10/9 coord, r11 addr, r2 current addr, !!r7 white \n\
			mov.w grey, r8\n\
			!mov #-1, r7\n\
			mov.l feldbaseA21, r11\n\
			mov #21 r9 !y !Warning, r9 goes from 20 to 1, not from 20 to 0, so no check in the invisible line at the top \n\
					!(0,1,2 are invisible, 3-20 are visible)\n\
			winNexty:\n\
			dt r9\n\
				mov #10 r10 !x\n\
				winNextx:\n\
					dt r10 !r10 from 9 to 0 \n\
					mov r9, r2		!r2 is the address (y address)\n\
					shll2 r2   !mal 4	!multiply y by 4\n\
					add r9, r2 !add (*5)	!add y (aka y*5)\n\
					shll r2    !mal 2 (*10)	!mul y by 2 (aka y*10)\n\
					add r10, r2		!add x \n\
					shll r2			!mul by 2 (1 word per block)\n\
					add r11, r2		!add the base addr\n\
					mov.w @r2,r3\n\
					cmp/eq r3, r8 !check if grey\n\
						bt winYcontinue ! if 1 block is wrong, next line\n\
				tst r10,r10 !if r10 = 0\n\
				bf winNextx\n\
				!all blocks have been tested, no block was wrong, it is full\n\
				!mov #10, r3\n\
				add #18, r2 ! go to the right of the column\n\
				mov r2, r3 ! move down\n\
				add #-20, r3!from here\n\
				whiteloop:\n\
					mov.w @r3, r7\n\
					mov.w r7, @r2\n\
					add #-2, r2\n\
					add #-2, r3\n\
					cmp/eq r3, r11\n\
				bf whiteloop\n\
				add #1,r9 	!If the line was full and the rest was moved, the same line needs to be checked again.\n\
				add #1, r14	!add score +1\n\
			winYcontinue:\n\
			tst r9,r9\n\
			bf winNexty	\n\
			\n\
\n\
\n\
		bra mainNewTile !next tile\n\
		nop\n\
\n\
	downNext:\n\
	!It does not collide\n\
	bsr drawTile\n\
	mov #1, r5\n\
\n\
	BSR subDraw\n\
	nop\n\
\n\
bra mainResetTimer\n\
nop\n\
\n\
\n\
\n\
\n\
\n\
.align 2\n\
\n\
!Debug\n\
!setCursor:	.long 0x8002e418\n\
!numPrint:	.long 0x801335bc\n\
\n\
!clearScreen:	.long 0x800394d0 \n\
!reset:		.long 0x80133694\n\
VRam:		.long 0x8C000000\n\
VRamEnde:	.long 0x8C052800 !320 wide, 528 tall, 2 byte per pixel\n\
keyAddr:	.long 0xa44B0000\n\
!return:		.long endofprog !break the program\n\
up7:		.long 0x00800000 \n\
left8:		.long 0x00004000\n\
down7:		.long 0x00400000 \n\
right8:		.long 0xffff8000\n\
key7:		.long 0x80000000\n\
back8:		.long 0x00000080 \n\
clear8:		.long 0x80020000 !shift clear\n\
shift8:		.long 0x80000080 !shift back\n\
timerinit:	.long 0x00020000\n\
timer:		.long 0x00000001 ! -256\n\
debounce:	.long 0x00000004 !f the keys are bouncing, make higher, if the keys are not responding, make lower\n\
!timersub:	.word 256 !2048  !at first, i subtracted 256, but it speeded up to fast. Now I divide by 32 and subtract\n\
\n\
\n\
!========== T I L E ==========\n\
\n\
!=== DRAW ===\n\
\n\
\n\
!r14 feldBase\n\
!r13 tile Base\n\
!r12 tile nr\n\
!r11 tile orientation\n\
!r10 tile x\n\
!r9 tile y\n\
!r8 key1+2\n\
!r7 key3+4\n\
!r6 keyTimer\n\
!r5 color arg for drawTile\n\
!r4 timer\n\
!r3,r2,r1 temp\n\
!r0 key	//2 wait for a keypress; 1 key pressed, debouncing; 0 waiting for key released; 3 debouncing \n\
drawTile:\n\
mov.l r14, @-r15	!feldBase?\n\
mov.l r13, @-r15	!tilebase?\n\
mov.l r12, @-r15	!tile nr\n\
mov.l r11, @-r15	!tile ori\n\
mov.l r10, @-r15	!tile x\n\
mov.l r9, @-r15		!tile y\n\
mov.l r8, @-r15		!key		actual address\n\
mov.l r7, @-r15		!key		tmp\n\
mov.l r6, @-r15		!key timer	color while drawing\n\
!mov.l r5, @-r15	!add or remove tile  COLOR 0: color = 0x8410, 1: color = tilecolor (doesn't need to be saved)\n\
mov.l r4, @-r15		!timer		actual x\n\
mov.l r3, @-r15		!	  	actual y\n\
mov.l r2, @-r15		!	  	address while drawing\n\
mov.l r1, @-r15		!	  	number while drawing\n\
mov.l r0, @-r15		!key state	tmp\n\
\n\
mova tilesBase, r0\n\
mov r0, r13\n\
!mova feldbase21, r0\n\
mov.l feldbaseA21, r14\n\
\n\
\n\
!current tile is at tileBase + 34*nr + orient*8\n\
!r8 = r12*34 (Tile offset)\n\
mov r12, r8\n\
shll2 r8\n\
shll2 r8\n\
add r12, r8\n\
shll r8\n\
\n\
add r13, r8 !Tile address (without color)\n\
\n\
tst r5, r5		!check if color = 0\n\
mov.w grey, r6	!load grey if no other color specified\n\
bt nocolor\n\
	mov r8, r6	!load color if specified\n\
	add #32, r6 !address of color information\n\
	mov.w @r6, r6 !color information\n\
nocolor:\n\
!mov r6, r5\n\
\n\
!r7 = r11 * 8 (Orientation offset)\n\
mov r11, r7	\n\
shll2 r7\n\
shll r7\n\
add r7, r8	!Tile address (with orientation)\n\
\n\
!for all the four blocks: load field with corresponding color\n\
mov #4, r1\n\
	loadTileToFeld:\n\
	mov.b @r8, r4	!load x coordinate of the current block into r4\n\
	add #1, r8\n\
	add r10, r4	!move the pice to the correct location\n\
	mov.b @r8, r3\n\
	add #1, r8\n\
	add r9, r3\n\
	! r4|r3 is the current block, change it to the color\n\
		mov r3, r2		!r2 is the address (y address)\n\
		shll2 r2   !mal 4	!multiply y by 4\n\
		add r3, r2 !add (*5)	!add y (aka y*5)\n\
		shll r2    !mal 2 (*10)	!mul y by 2 (aka y*10)\n\
		add r4, r2		!add x \n\
		shll r2			!mul by 2 (1 word per block)\n\
		add r14, r2		!add the base addr\n\
		mov.w r6, @r2\n\
	dt r1\n\
bf loadTileToFeld\n\
\n\
mov.l @r15+, r0\n\
mov.l @r15+, r1\n\
mov.l @r15+, r2\n\
mov.l @r15+, r3\n\
mov.l @r15+, r4\n\
!mov.l @r15+, r5 !not saved \n\
mov.l @r15+, r6\n\
mov.l @r15+, r7\n\
mov.l @r15+, r8\n\
mov.l @r15+, r9\n\
mov.l @r15+, r10\n\
mov.l @r15+, r11\n\
mov.l @r15+, r12\n\
mov.l @r15+, r13\n\
rts\n\
mov.l @r15+, r14\n\
\n\
!grey:	.word 0x8410\n\
\n\
!=== CHECK === \n\
!Check if the current pice conflicts with others; Nothing will change\n\
\n\
!T bit is used for return. 	0 everything ok\n\
!				1 error\n\
\n\
Tcheck:\n\
!r14 feldBase\n\
!r13 tile Base\n\
!r12 tile nr\n\
!r11 tile orientation\n\
!r10 tile x\n\
!r9 tile y\n\
!r8 key1+2\n\
!r7 key3+4\n\
!r6 keyTimer\n\
!r5 color arg for drawTile\n\
!r4 timer\n\
!r3,r2,r1 temp\n\
!r0 key	//2 wait for a keypress; 1 key pressed, debouncing; 0 waiting for key released; 3 debouncing \n\
mov.l r14, @-r15	!feldBase?\n\
mov.l r13, @-r15	!tilebase?	later: xmax = 10\n\
mov.l r12, @-r15	!tile nr	later: ymax = 21\n\
mov.l r11, @-r15	!tile ori	later: grey\n\
mov.l r10, @-r15	!tile x\n\
mov.l r9, @-r15		!tile y\n\
mov.l r8, @-r15		!key		actual address\n\
mov.l r7, @-r15		!key		tmp\n\
mov.l r6, @-r15		!key timer	color while drawing\n\
!mov.l r5, @-r15	!add or remove tile  COLOR 0: color = 0x8410, 1: color = tilecolor (doesn't need to be saved)\n\
mov.l r4, @-r15		!timer		actual x\n\
mov.l r3, @-r15		!	  	actual y\n\
mov.l r2, @-r15		!	  	address while drawing\n\
mov.l r1, @-r15		!	  	number while drawing\n\
mov.l r0, @-r15		!key state	tmp\n\
\n\
mova tilesBase, r0\n\
mov r0, r13\n\
!mova feldbase21, r0\n\
mov.l feldbaseA21, r14\n\
\n\
\n\
!current tile is at tileBase + 34*nr + orient*8\n\
!r8 = r12*34 (Tile offset)\n\
mov r12, r8\n\
shll2 r8\n\
shll2 r8\n\
add r12, r8\n\
shll r8\n\
\n\
add r13, r8 !Tile address (without color)\n\
!mov r6, r5\n\
\n\
!r7 = r11 * 8 (Orientation offset)\n\
mov r11, r7	\n\
shll2 r7\n\
shll r7\n\
add r7, r8	!Tile address (with orientation)\n\
\n\
mov.w grey, r11	!now we don't need the tile info (number/orientation because we have the acutual address in a reg) but we need the background color\n\
mov #10, r13 !xmax\n\
mov #21, r12 !ymax\n\
\n\
!for all the four blocks: check field against background color\n\
mov #4, r1\n\
	checkTileToFeld:\n\
	mov.b @r8, r4	!load x coordinate of the current block into r4\n\
	add #1, r8\n\
	add r10, r4	!move the pice to the correct location\n\
	mov.b @r8, r3\n\
	add #1, r8\n\
	add r9, r3\n\
	! r4|r3 is the current block, test its color\n\
		!check if r4|r3 is inside the playfield\n\
		cmp/hi r4, r13\n\
		bf outside\n\
		cmp/hi r3, r12\n\
		bt yinside\n\
		outside:\n\
			bra checkTileEnd\n\
			sett !mov #1, r5\n\
		yinside:\n\
		mov r3, r2		!r2 is the address (y address)\n\
		shll2 r2   !mal 4	!multiply y by 4\n\
		add r3, r2 !add (*5)	!add y (aka y*5)\n\
		shll r2    !mal 2 (*10)	!mul y by 2 (aka y*10)\n\
		add r4, r2		!add x \n\
		shll r2			!mul by 2 (1 word per block)\n\
		add r14, r2		!add the base addr\n\
			mov.w @r2, r7\n\
			cmp/eq r7, r11	!check if pixel is blank\n\
			bt checkTileNoCheck	!proceed\n\
				bra checkTileEnd \n\
				sett !mov #1, r5	!return 1 if error\n\
		checkTileNoCheck:\n\
	dt r1\n\
bf checkTileToFeld\n\
\n\
clrt !mov #0, r5 !return 0 if everything is ok\n\
\n\
checkTileEnd:\n\
mov.l @r15+, r0\n\
mov.l @r15+, r1\n\
mov.l @r15+, r2\n\
mov.l @r15+, r3\n\
mov.l @r15+, r4\n\
!mov.l @r15+, r5 !not saved \n\
mov.l @r15+, r6\n\
mov.l @r15+, r7\n\
mov.l @r15+, r8\n\
mov.l @r15+, r9\n\
mov.l @r15+, r10\n\
mov.l @r15+, r11\n\
mov.l @r15+, r12\n\
mov.l @r15+, r13\n\
rts\n\
mov.l @r15+, r14\n\
\n\
grey:	.word 0x8410\n\
\n\
\n\
\n\
!========== D R A W   S U B R O U T I N E ==========\n\
subDraw: !Sub draw\n\
mov.l r0, @-r15\n\
mov.l r1, @-r15\n\
mov.l r2, @-r15\n\
mov.l r3, @-r15\n\
mov.l r4, @-r15\n\
mov.l r5, @-r15\n\
mov.l r6, @-r15\n\
mov.l r7, @-r15\n\
mov.l r8, @-r15\n\
mov.l r9, @-r15\n\
mov.l r10, @-r15\n\
mov.l r11, @-r15\n\
mov.l r12, @-r15\n\
sts.l pr, @-r15\n\
mov.l r13, @-r15\n\
mov.l r14, @-r15\n\
\n\
\n\
!'r0 pixel\n\
!'r1 feldx\n\
!'r2 feldy\n\
!'r3 subx\n\
!'r4 suby\n\
!'r5 color2\n\
!'r14 feldBase\n\
!'r8 (320-31)\EE2 = h'242\n\
!'r8 (320-27)\EE2 = h'24A\n\
!'r9(-31\EE320+32)\EE2=h'ffffB2C0\n\
!'r9(-27\EE320+28)\EE2=h'ffffBCB8\n\
!'r10(32\EE320-8\EE32)\EE2=0x4E00\n\
!'r10(28\EE320-10\EE28)\EE2=43D0\n\
!'r11 = 10\n\
!'r12 = 18\n\
\n\
!mova feldbase18, r0\n\
mov.l feldbaseA18, r14\n\
mov.l pixel, r0\n\
mov.w h24A, r8\n\
mov.w hBCB8, r9\n\
mov.w h4E00, r10\n\
mov #10, r11\n\
mov #18, r12\n\
mov #0, r2 !feldy\n\
fieldy: !'/while feldy<18:\n\
	mov #0, r1 !feldx\n\
	feldxeq10: !'/while feldx<10:\n\
		mov r2, r5\n\
		shll2 r5   !mal 4\n\
		add r2, r5 !add (*5)\n\
		shll r5    !mal 2 (*10)	\n\
		add r1, r5\n\
		shll r5\n\
		add r14, r5\n\
		mov.w @r5, r5\n\
		mov #27, r4\n\
		subygr0: !'/while suby>0\n\
			mov #27, r3\n\
			subxgr0: !'/while subx>0\n\
				mov.w r5, @r0\n\
				add #2,r0 !1 px to the right\n\
				dt r3\n\
			bf subxgr0\n\
			add r8, r0 !27 pixels to the left an 1 down //0x242  //+320-31  \EE2  = 0x242\n\
			dt r4\n\
		bf subygr0\n\
		add r9, r0 !0x5200 //back 27 pixels up and 28 pixels right   //-31\EE320 +32  \EE2 = 0xFFFF B2C0\n\
		add #1, r1\n\
		cmp/eq r11, r1 !r11 = 10\n\
	bf feldxeq10\n\
	add r10, r0 !'add(32\EE320-8\EE32)\EE2, r0 ' = 0x4E00\n\
	add #1, r2\n\
	cmp/eq r12, r2 !18\n\
bf fieldy\n\
\n\
!Score\n\
\n\
mov #2, r5\n\
mov #0, r6\n\
\n\
mov.l numPrint, r2\n\
mov.l @r15+, r14\n\
jsr @r2\n\
mov r14, r4\n\
\n\
\n\
!Update screen\n\
mov.l drawScreen, r2\n\
jsr @r2\n\
\n\
mov.l @r15+, r13\n\
lds.l @r15+, pr\n\
mov.l @r15+, r12\n\
mov.l @r15+, r11\n\
mov.l @r15+, r10\n\
mov.l @r15+, r9\n\
mov.l @r15+, r8\n\
mov.l @r15+, r7\n\
mov.l @r15+, r6\n\
mov.l @r15+, r5\n\
mov.l @r15+, r4\n\
mov.l @r15+, r3\n\
mov.l @r15+, r2\n\
mov.l @r15+, r1\n\
rts\n\
mov.l @r15+, r0\n\
\n\
.align 2\n\
  pixel:	!.long 0x8C001928\n\
		.long 0x8c003c28\n\
! vram 0x8c000000 + (320 * free space on the top + free space left )*2\n\
! tetris: 10 x 18     box 28x28 px\n\
! tetris field: 280x504 px\n\
! screen: 320x528\n\
! space: left+right = 40, 20 per site\n\
!        top+bottom = 24, all on top, no on the bottom\n\
! 0x8c000000 + (320 * 24 + 20)*2\n\
! 0x8c000000 + 7700 * 2 = 0x8c0000 + 15400 = 8c000000 + 0x3c28 = 0x8c003c28\n\
\n\
\n\
  setCursor:	.long _Debug_SetCursorPosition\n\
  numPrint:	.long _Debug_PrintNumberHex_Byte\n\
  !feldbase:	.long 0x8c060000 !only a test\n\
  drawScreen:	.long _LCD_Refresh\n\
\n\
  h24A:		.word 0x024A\n\
  hBCB8:	.word 0xBCB8\n\
  h4E00:	.word 0x43D0\n\
\n\
!========== T I L E S ===========\n\
\n\
.align 2\n\
tilesBase: !I (Cyan) !The x coord is positive, the y coord is negative, the basepoint of the tile is in the bottom left corner, the screen has its base in the upper left corner, so 1 to the right is x=1 and 1 up is y=-1 = 0xff\n\
		.word 0x0000; .word 0x0100; .word 0x0200; .word 0x0300 \n\
		.word 0x0000; .word 0x00FF; .word 0x00FE; .word 0x00FD \n\
		.word 0x0000; .word 0x0100; .word 0x0200; .word 0x0300\n\
		.word 0x0000; .word 0x00FF; .word 0x00FE; .word 0x00FD; .word 0b0000011111111111 !Color \n\
	!O (Yellow)\n\
		.word 0x0000; .word 0x00FF; .word 0x0100; .word 0x01FF \n\
		.word 0x0000; .word 0x00FF; .word 0x0100; .word 0x01FF \n\
		.word 0x0000; .word 0x00FF; .word 0x0100; .word 0x01FF \n\
		.word 0x0000; .word 0x00FF; .word 0x0100; .word 0x01FF; .word 0b1111111111100000\n\
	!T (Purple)\n\
		.word 0x0000; .word 0x0100; .word 0x0200; .word 0x01FF \n\
		.word 0x0000; .word 0x00FF; .word 0x00FE; .word 0x01FF \n\
		.word 0x00FF; .word 0x01FF; .word 0x02FF; .word 0x0100 \n\
		.word 0x0100; .word 0x01FF; .word 0x01FE; .word 0x00FF; .word 0b1000000000011111\n\
	!S (Green)\n\
		.word 0x0000; .word 0x0100; .word 0x01FF; .word 0x02FF \n\
		.word 0x00FE; .word 0x00FF; .word 0x01FF; .word 0x0100 \n\
		.word 0x0000; .word 0x0100; .word 0x01FF; .word 0x02FF \n\
		.word 0x00FE; .word 0x00FF; .word 0x01FF; .word 0x0100; .word 0b0000011000000000\n\
	!Z (Red)\n\
		.word 0x00FF; .word 0x01FF; .word 0x0100; .word 0x0200 \n\
		.word 0x0000; .word 0x00FF; .word 0x01FF; .word 0x01FE \n\
		.word 0x00FF; .word 0x01FF; .word 0x0100; .word 0x0200 \n\
		.word 0x0000; .word 0x00FF; .word 0x01FF; .word 0x01FE; .word 0b1111100000011111\n\
	!J (Blue)\n\
		.word 0x0000; .word 0x0100; .word 0x0200; .word 0x00FF \n\
		.word 0x0000; .word 0x00FF; .word 0x00FE; .word 0x01FE \n\
		.word 0x00FF; .word 0x01FF; .word 0x02FF; .word 0x0200 \n\
		.word 0x0000; .word 0x0100; .word 0x01FF; .word 0x01FE; .word 0b0000000000011111\n\
	!L (Orange)\n\
		.word 0x0000; .word 0x0100; .word 0x0200; .word 0x02FF \n\
		.word 0x0000; .word 0x00FF; .word 0x00FE; .word 0x0100 \n\
		.word 0x0000; .word 0x00FF; .word 0x01FF; .word 0x02FF \n\
		.word 0x00FE; .word 0x01FE; .word 0x01FF; .word 0x0100; .word 0b1111110000000000\n\
\n\
.align 2\n\
feldbaseA21: .long feldbase21 !0x8c060000\n\
feldbaseA18: .long feldbase21 + 60\n\
!feldendA:    .long feldbase21 + 420\n\
feldbase21:\n\
");
};

//The area "feldbase21" will override the next part in the ram. There will be some subroutines for some gui stuff. This is not needed, so this won't matter to me...
