# Cette directive sert a rendre l'etiquette publique
    .globl syscall

syscall:
    pushl %ebp
    mov %esp, %ebp
# Sauvegarde des registres importants
    pushl %esi
    pushl %edi
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

# Passage des paramètres de la pille à l'interruption
    movl 8(%ebp), %eax
    movl 12(%ebp), %ebx
    movl 16(%ebp), %ecx
    movl 20(%ebp), %edx
    movl 24(%ebp), %edi
    movl 28(%ebp), %esi

# Appel à la fonction C realisant le traitant
    int $49

# Restauration des registres importants
    addl $4, %esp
    popl %ebx
    popl %ecx
    popl %edx
    popl %edi
    popl %esi

    leave #popl %ebp
    ret