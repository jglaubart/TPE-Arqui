#include <stdint.h>
#include "idtLoader.h"
#include "defs.h"
#include "interrupts.h"

#define KEYBOARD_INTERRUPT 0x21
#define TIMER_TICK_INTERRUPT 0x20
#define SYSCALL_INTERRUPT 0x80
#define DIVIDE_BY_ZERO_EXCEPTION 0x00
#define INVALID_OPCODE_EXCEPTION 0x06

#pragma pack(push)		/* Push de la alineación actual */
#pragma pack (1) 		/* Alinear las siguiente estructuras a 1 byte */

/* Descriptor de interrupcion */
typedef struct descriptors{
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop)		/* Reestablece la alinceación actual */

DESCR_INT * idt = (DESCR_INT *) 0;	// IDT de 255 entradas comenzando en la posicion 0



/* EXCEPCIONES */
extern void exception_divideByZero(void);
extern void exception_invalidOpCode(void);

/* INTERRUPCIONES DE HARDWARE */
extern void interrupt_keyboard(void);
extern void interrupt_timerTick(void);

/* SYSCALLS */
extern void interrupt_syscall();



static void setup_IDT_entry (int index, uint64_t offset);

void load_idt() {
  _cli(); // Deshabilito interrupciones para cargar la IDT

  //Interrupciones de Hardware
  setup_IDT_entry (KEYBOARD_INTERRUPT, (uint64_t)&interrupt_keyboard); //keyboard -> keyboard.c
  setup_IDT_entry (TIMER_TICK_INTERRUPT, (uint64_t)&interrupt_timerTick); //timer tick -> time.c

  //Excepciones
  setup_IDT_entry (DIVIDE_BY_ZERO_EXCEPTION, (uint64_t)&exception_divideByZero);
  setup_IDT_entry (INVALID_OPCODE_EXCEPTION, (uint64_t)&exception_invalidOpCode);


  //Interrupciones de Software
  setup_IDT_entry (SYSCALL_INTERRUPT, (uint64_t)&interrupt_syscall); //syscalls -> syscall

  //Solo interrupcion timer tick habilitadas
  picMasterMask(0xFC);
  picSlaveMask(0xFF);
        
  _sti(); // Habilito interrupciones
}

static void setup_IDT_entry (int index, uint64_t offset) {
  idt[index].selector = 0x08;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = ACS_INT;
  idt[index].cero = 0;
  idt[index].other_cero = (uint64_t) 0;
}