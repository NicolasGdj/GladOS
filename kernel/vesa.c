//
// Created by cartignh on 23/06/2022.
//

#include "vesa.h"
#include "bioscall.h"
#include "mem.h"
#include <stdint.h>
#include "stdio.h"
#include "string.h"
#include "segment.h"
#include <stdbool.h>

// VESA info block
static struct VbeInfoBlock vib;
static struct vbe_mode_info_structure vesa_info;

// Useful stuff here on Linux sources -> https://code.woboq.org/linux/linux/arch/x86/boot/video-vesa.c.html#vginfo

#define SWITCH_TO_VIDEO_MODE false

// It must be called in 32-bit protected
// mode, without paging, and with CS, DS, ES and SS pointing to 32-bit segments
// starting at 0.
void switch_to_vesa() {
    // Here, paging is not yet active

    // mov ax, 0x4F02	; set VBE mode
    // mov bx, 0x4118	; VBE mode number; notice that bits 0-13 contain the mode number and bit 14 (LFB) is set and bit 15 (DM) is clear.
    // int 0x10         ; call VBE BIOS

    struct bios_regs *regs = mem_alloc(sizeof(struct bios_regs));
    regs->eax = 0x4f02; // Set video mode
    regs->ebx = 0x4118; // Linear buffer + 1024 x 768 (16 million colors / 24 bits)
    regs->ds = 0;
    regs->es = 0;
    regs->fs = 0;
    regs->gs = 0;

    // INT 0x10, AX=0x4F02, BX=mode, ES:DI=CRTCInfoBlock

    if (SWITCH_TO_VIDEO_MODE)
        do_bios_call(regs, 0x10);

    // v86_bios(0x10, {ax:0x4f00, es:SEG(vib), di:OFF(vib)}, &out);
    regs = mem_alloc(sizeof(struct bios_regs));
    regs->eax = 0x4f00;
    //vib_regs->es = KERNEL_DS; //SEG(vib);
    regs->ds = KERNEL_DS;
    regs->es = KERNEL_DS;
    regs->fs = KERNEL_DS;
    regs->gs = KERNEL_DS;
    regs->edi = (unsigned int) &vib; //OFF(vib);

    if (SWITCH_TO_VIDEO_MODE)
        do_bios_call(regs, 0x10);


    // #define RED 0x00FF0000






    // Get framebuffer address from 0x4F01 function call
    // Input: AX = 0x4F01
    // Input: CX = VESA mode number from the video modes array
    /// Input: ES:DI = Segment:Offset pointer of where to store the VESA Mode Information Structure shown below.
    // Output: AX = 0x004F on success, other values indicate a BIOS error or a mode-not-supported error.
    //struct vbe_mode_info_structure *vesa_info = (struct vbe_mode_info_structure *) 0x3000; //mem_alloc(sizeof(struct vbe_mode_info_structure));
    //struct bios_regs *regs = mem_alloc(sizeof(struct bios_regs));
    (void) vesa_info;
    regs->eax = 0x4f02;
    regs->ebx = 0;
    regs->ds = 0;
    regs->es = 0;
    regs->fs = 0;
    regs->gs = 0;
    regs->ecx = 0x4118;
    regs->edi = 0x3000;

    if (SWITCH_TO_VIDEO_MODE)
        do_bios_call(regs, 0x10);

    (void) vesa_info;

    printf("wow");
}

uint16_t findMode(int x, int y, int d) {
    (void) x;
    (void) y;
    (void) d;
    return 1;
    /*struct VbeInfoBlock *ctrl = (VbeInfoBlock *)0x2000;
    struct ModeInfoBlock *inf = (ModeInfoBlock *)0x3000;
    uint16_t *modes;
    int i;
    uint16_t best = 0x13;
    int pixdiff, bestpixdiff = DIFF(320 * 200, x * y);
    int depthdiff, bestdepthdiff = 8 >= d ? 8 - d : (d - 8) * 2;

    strncpy(ctrl->VbeSignature, "VBE2", 4);
    intV86(0x10, "ax,es:di", 0x4F00, 0, ctrl); // Get Controller Info
    if ( (uint16_t)tss.eax != 0x004F ) return best;

    modes = (uint16_t*)REALPTR(ctrl->VideoModePtr);
    for ( i = 0 ; modes[i] != 0xFFFF ; ++i ) {
        intV86(0x10, "ax,cx,es:di", 0x4F01, modes[i], 0, inf); // Get Mode Info

        if ( (uint16_t)v86.tss.eax != 0x004F ) continue;

        // Check if this is a graphics mode with linear frame buffer support
        if ( (inf->attributes & 0x90) != 0x90 ) continue;

        // Check if this is a packed pixel or direct color mode
        if ( inf->memory_model != 4 && inf->memory_model != 6 ) continue;

        // Check if this is exactly the mode we're looking for
        if ( x == inf->XResolution && y == inf->YResolution &&
             d == inf->BitsPerPixel ) return modes[i];

        // Otherwise, compare to the closest match so far, remember if best
        pixdiff = DIFF(inf->Xres * inf->Yres, x * y);
        depthdiff = (inf->bpp >= d)? inf->bpp - d : (d - inf->bpp) * 2;
        if ( bestpixdiff > pixdiff ||
             (bestpixdiff == pixdiff && bestdepthdiff > depthdiff) ) {
            best = modes[i];
            bestpixdiff = pixdiff;
            bestdepthdiff = depthdiff;
        }
    }
    if ( x == 640 && y == 480 && d == 1 ) return 0x11;
    return best;*/
}