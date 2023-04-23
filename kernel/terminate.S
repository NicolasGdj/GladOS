# ISR: Interrupt Service Routine

# Cette directive sert a rendre l'etiquette publique
    .globl terminate
# Handles processes returning an integer by calling `exit()` with it
terminate:
    pushl %eax
    call exit