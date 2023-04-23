      .text
      .globl ctx_sw

  # Structure de la pile en entree :
  #   %esp + 4 : adresse de l'ancien contexte
  #   %esp + 8 : adresse du nouveau contexte

  ctx_sw:
      # sauvegarde du contexte de l'ancien processus
      movl 4(%esp), %eax
      movl %ebx, (%eax)
      movl %esp, 4(%eax)
      movl %ebp, 8(%eax)
      movl %esi, 12(%eax)
      movl %edi, 16(%eax)
      # Save tss+4 into the stack
      movl tss+4, %ecx
      movl %ecx, 20(%eax)

      # restauration du contexte du nouveau processus
      movl 8(%esp), %eax
      movl (%eax), %ebx
      movl 4(%eax), %esp
      movl 8(%eax), %ebp
      movl 12(%eax), %esi
      movl 16(%eax), %edi
      # Pop tss+4 from the stack
      movl 20(%eax), %ecx
      movl %ecx, tss+4
      # On passe la main au nouveau processus
      ret