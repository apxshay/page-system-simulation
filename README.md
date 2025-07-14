# --- ITALIANO ---

Questo progetto è una simulazione di un software di un sistema operativo che implementa:

- gestore della memoria **MMU** con **RAM** e **disco** simulati  
- processi simulati con stato semplificato  
- allocazione delle **tabelle delle pagine** per ogni processo tramite algoritmo **First Fit**  
- assegnazione e mappatura dei frame di un processo nella rispettiva tabella delle pagine  
- generazione di **indirizzi virtuali** in maniera randomica, con o senza **principio di località**  
- traduzione di indirizzi virtuali → fisici tramite tabella delle pagine e/o **TLB**  
- **TLB** con algoritmi di rimpiazzo **FIFO** e **CLOCK**  
- calcolo di **TLB hit rate** e **TLB miss rate**  
- **swap out** di tabella delle pagine e frame di un processo su disco quando la RAM non ha spazio sufficiente  

La RAM è rappresentata da un array di interi, con uno spazio riservato all’inizio per le tabelle delle pagine, gestito da un **Page Table Manager**.  
Quando un processo viene creato, si controlla il Page Table Space in RAM e si cerca un blocco libero per allocare la nuova tabella. Se non ci sono blocchi liberi, viene scelta una tabella da rimpiazzare (**swap out** su disco) insieme ai suoi frame, usando un algoritmo che simula **CLOCK**.  
I processi sono ***“in CPU”*** quando vengono creati: generano indirizzi virtuali (anche con principio di località) che vengono tradotti, determinando **hit** o **miss** nel TLB.  
In caso di capacity miss, entrano in gioco gli algoritmi di rimpiazzo **FIFO** o **CLOCK**, che garantiscono performance simili.  
Il TLB utilizza un **flag di validità** per indicare se un processo ha perso il controllo della CPU (context switch).

## --- COME USARE L'APPLICAZIONE ---

Per eseguire la simulazione:  

- Compilare sempre prima con **`make -B`** per forzare la rigenerazione di tutti i file di output.  
- Eseguire con **`./simulation`** per usare le impostazioni di default.  
- Eseguire con **`./simulation config`** per usare un file di configurazione personalizzato.  

Accorgimenti:  

- Si consiglia di usare dimensioni di RAM, disco e numero di frame come **potenze di 2** per una gestione stabile.  
- RAM, disco e dimensione dei frame devono essere **coerenti tra loro**: la dimensione di RAM e disco deve essere multipla della dimensione di un frame. In caso contrario, potrebbero verificarsi errori di arrotondamento durante la traduzione degli indirizzi.  
- L’applicazione potrebbe dare errore in caso di:  
  - Numero di processi troppo elevato: troppi swap out possono riempire il disco (non gestito).  
  - Parametri incoerenti tra RAM, disco e frame: controllare bene la configurazione.

---

# --- ENGLISH ---

This project is a simulation of an operating system software that implements:

- a memory management unit (**MMU**) with simulated **RAM** and **disk**  
- simulated processes with a simplified state  
- allocation of **page tables** for each process using a **First Fit** algorithm  
- assignment and mapping of a process’s frames in its respective page table  
- generation of **virtual addresses** randomly, with or without **locality principle**  
- translation of virtual → physical addresses using page tables and/or a **TLB**  
- **TLB** with **FIFO** and **CLOCK** replacement algorithms  
- calculation of **TLB hit rate** and **TLB miss rate**  
- **swap out** of a process’s page table and frames to disk when RAM has insufficient space  

The RAM is represented by an array of integers, with a reserved space at the beginning for page tables, managed by a **Page Table Manager**.  
When a process is created, the Page Table Space in RAM is checked for a free block to allocate the new table. If there are no free blocks, a page table is selected to be replaced (**swap out** to disk) together with its frames, using an algorithm that simulates **CLOCK**.  
Processes are considered ***“in CPU”*** when they are created: they generate virtual addresses (also with locality of reference) that are translated, resulting in TLB **hits** or **misses**.  
In case of capacity misses, the **FIFO** or **CLOCK** replacement algorithms handle TLB updates, providing similar performance.  
The TLB uses a **validity flag** to indicate whether a process has lost CPU control (context switch).

## --- HOW TO USE THE APPLICATION ---

To run the simulation:  

- Always compile first with **`make -B`** to force regeneration of all output files.  
- Run **`./simulation`** to use the default settings.  
- Run **`./simulation config`** to use a custom configuration file.  

Tips:  

- It is recommended to use RAM, disk, and frame sizes that are **powers of 2** for more stable behavior.  
- RAM, disk, and frame size must be **consistent**: the RAM and disk sizes must be multiples of the frame size. Otherwise, rounding errors may occur during address translation.  
- The application may fail in the following cases:  
  - Too many generated processes: excessive swap out operations may fill up the disk (not handled).  
  - Inconsistent parameters between RAM, disk, and frames: always double-check your configuration.
