# Fonction pour simuler un retour d'interuption.
# On met toutes les valeurs des segments registre sur 0x4b (SS en mode user)
    .globl switch_to_userland
switch_to_userland:
    # $0x4b is SS_USER (see process.h)
    mov $0x4b, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    // Interruption to switch back to userland
    # add-symbol-file ./user/user.debug 0x1000000
    iret