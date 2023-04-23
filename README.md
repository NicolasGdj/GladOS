# GladOS - Système d'exploitation

GladOS est un système d'exploitation développé par moi-même et [@BlueskyFr](https://github.com/BlueskyFR) dans le cadre d'un projet à l'ENSIMAG.

## Qu'est-ce qu'un système d'exploitation ?

Un système d'exploitation (OS) est un ensemble de programmes qui gère les ressources matérielles et logicielles d'un ordinateur. Il est composé de plusieurs parties, notamment :

Kernel : Le noyau du système d'exploitation, responsable de la gestion des ressources et des services système.

User : La partie utilisateur, qui contient les applications et les utilitaires pour les utilisateurs.

Gestion de la mémoire : Le mécanisme qui gère l'allocation et la libération de la mémoire pour les processus.

Gestion des processus : Le mécanisme qui gère la création, l'exécution, la terminaison et la priorité des processus. Les processus peuvent avoir différents états, tels que zombie, en attente d'I/O, etc.

Shell inspiré de Glados du jeu Portal

L'utilisation de GladOS est démontrée par un shell qui s'inspire de Glados, un personnage du jeu vidéo Portal. Le shell inclut plusieurs commandes pour interagir avec le système.

Merci à @ChatGPT pour l'écriture de ce passage.

## Démonstration

[Vidéo Youtube](https://youtu.be/RgJ948pzJxw)

## Installation

### Prérequis

- Qemu
- make

### Commandes à réaliser

```bash
git clone https://github.com/NicolasGdj/GladOS.git
cd GladOS/
make clean && make
qemu-system-x86_64 -kernel kernel/kernel.bin
```

## Commandes du shell

- `help` : Affiche le message d'aide.
- `exit` : Quitte le shell.
- `echo <on/off>` : Active/Désactive le mode echo.
- `reboot` : Redémarre le système.
- `print <message>` : Affiche un message.
- `clear` : Efface le terminal.
- `sysinfo` : Affiche l'état du système.
- `kill <id>` : Tue un processus.
- `chprio <id> <prio>` : Change la priorité d'un processus.
- `sleep <time>` : Dors pendant `<time>` secondes.
- `tests` : Effectue une série de tests.
- `qi` : Démarre un test d'intelligence.
- `answer` : Compute the ultimate question of life, the Universe, and Everything.
- `cake` : Make a cake.
- `segfault` : Start a new dumb turret doing a segmentation fault.
