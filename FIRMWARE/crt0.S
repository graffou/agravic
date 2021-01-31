  .section .text

/* =================================================== [ exceptions ] === */
/* This section has to be down here, since we have to disable rvc for it  */

  .section .vectors, "ax"
  .option norvc;
  .org 0x00
  cpu_boot:
  //Boot addr is 0: go to reset handler
  jal x0, reset_handler
  // Trap handler addr is 4
  default_exc_handler:
  jal x0, default_exc_handler // loop

  // There are 29 words left before IRQ traps: use them to initialize most registers
  reset_handler:
  /* set all registers to zero */ //PG: should not be required if registers were properly reset
  mv  x1, x0
  mv  x2, x1
  mv  x3, x1
  mv  x4, x1
  mv  x5, x1
  mv  x6, x1
  mv  x7, x1
  mv  x8, x1
  mv  x9, x1
  mv x10, x1
  mv x11, x1
  mv x12, x1
  mv x13, x1
  mv x14, x1
  mv x15, x1
  mv x16, x1
  mv x17, x1
  mv x18, x1
  mv x19, x1
  mv x20, x1
  mv x21, x1
  mv x22, x1
  mv x23, x1
  mv x24, x1
  mv x25, x1
  mv x26, x1
  mv x27, x1
  mv x28, x1
  mv x29, x1
  // continue to reset handler
  jal x0, reset_handler2

  // IRQ vector (32)
  IRQ_vector:
  nop //
  mret
  mret
  mret
  mret // SW IRQ
  mret
  mret
  mret
  j timer_irq_handler // Timer IRQ
  mret
  mret
  mret
  mret
  mret
  mret
  mret
  mret
  mret // First user IRQ
  mret
  mret
  mret
  mret
  mret
  mret
  mret
  mret
  mret
  mret
  mret
  mret
  mret
  mret
  mret
  reset_handler2:
  mv x30, x1
  mv x31, x1

  /* stack initilization */
  la   x2, _stack_start

_start:
  .global _start

  /* clear BSS */
  la x26, _bss_start
  la x27, _bss_end

  bge x26, x27, zero_loop_end

zero_loop:
  sw x0, 0(x26)
  addi x26, x26, 4
  ble x26, x27, zero_loop
zero_loop_end:

  la x26, _sdata
  la x27, _bss_start
  la x25, _sidata

  bge x26, x27, init_loop_end

init_loop:
  lw x1, 0(x25)
  sw x1, 0(x26)
  addi x26, x26, 4  
  addi x25, x25, 4
/* PG: blt instead of ble avoids overwriting 1st bss data */
  blt x26, x27, init_loop
init_loop_end:

  call    __libc_init_array

main_entry:
  /* jump to main program entry point (argc = argv = 0) */
  addi x10, x0, 0
  addi x11, x0, 0
  jal x1, main

