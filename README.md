# Projet Kernel UwU

## Phase 5 - protection & appels systèmes, userland

### 1 - Protection mémoire : Séparer la mémoire du noyau et de l'application (en utilisant la mémoire virtuelle si votre projet le requiert)

- https://ensiwiki.ensimag.fr/images/6/61/Psys_kernel_user.pdf
- https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_PC_:_2019_-_BALLEYDIER_Loic,_PIERRET_Arnaud#Phase_5_:_S.C3.A9paration_des_espaces_m.C3.A9moire_noyau_et_utilisateur_:_gestion_de_processus_utilisateur
- https://ensiwiki.ensimag.fr/images/a/a0/PSE_PIT.pdf
- https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_PC_:_2021_-_D%27EMMANUELE_Valentin,_VIGNAL_Nathan

- First : kernel -> user mode (Uniquement par retour d'int ou de deroutement, avec l'instruction iret
- user -> kernel -> Déroutement logiciel, appel de int
- Implementer deux piles (user & kernel) par processus

- syscalls: in `kernel/syscalls.h` + in `user/syscalls.S`
    - `syscalls.S` calls `syscalls.h` methods by using int $49
    
User -> Kernel:
* Passage vers la nouvelle pile 
* Sauvegarde de ESP, EIP, EFLAGS
* Exécution du prog traitant

Kernel -> User:
* Restauration de ESP, EIP et EFLAGS utilisateurs depuis la pile kernel

Traitant (cf `kernel/clock_isr.S`:
* Sauvegarde des registres
* Traitement de l'exception
* Restauration des registre
* iret

**Kernel stack**

```
|          | MAX (32 bit slots)
+----------+ 
|   ul_sw  | a pointer to the assembly function that switches to userland using iret
+----------+  <- 512 + 6
|  pt_func | the pointer to the function to run
+----------+  <- 512 + 5
|    CS    | Code Segment: 0x10 (kernel) or 0x43 (user)
+----------+  <- 512 + 4
|  EFLAGS  | Enable/disable interruptions: 0x0002 (kernel) or 0x0202 (user)
+----------| <- 512 + 3
|   %esp   | user_stack address
+----------+ <- 512 + 2
|    SS    | Stack Segment: 0x18 (kernel) or 0x4B (user) 
+----------+ <- 512 + 1
|    ...   |
+----------|  <- p->kernel_stack
|          | 0
```

**User stack**

```
     |          | MAX
     +----------+  <- internal kernel_stack_size
     |   arg    |
     +----------+  <- ebp + 4
     |   exit   |
     +----------+  <- ssize, ebp
     | local va |
     +----------|
     | local va |
     +----------+
     |  ret val |
     +----------+
     |    ...   |
     +----------|  <- p->stack
     |          | 0
```