      .text
      .globl ISR_IT_0E

  ISR_IT_0E:
      # Push parameters of syscall_handler function
      push %esi
      push %edi
      push %edx
      push %ecx
      push %ebx
      push %eax

      # Set kernel mode (set segment register values)
      mov $0x18, %bx
      mov %bx, %ds
      mov %bx, %es
      mov %bx, %fs
      mov %bx, %gs

      # Call the keyboard_ISR function
      call page_fault_handler

      # Set user mode
      mov $0x4b, %bx
      mov %bx, %ds
      mov %bx, %es
      mov %bx, %fs
      mov %bx, %gs

      # On restaure sauf %eax qui contient le retourne code de la fonction
      pop %eax
      pop %ebx
      pop %ecx
      pop %edx
      pop %edi
      pop %esi

      iret