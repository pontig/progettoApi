# Prova Finale di Algoritmi e Strutture Dati

Obiettivo del progetto è implementare un algoritmo efficiente che permatta di programmare un tragitto lungo una autostrada utilizzando il minor numero di automobili ad autonomia limitata.

Utilizzando i comandi
* `aggiungi-stazione`
* `demolisci-stazione`

è possibile aggiungere e rimuovere stazioni di ove è possibile effettuare il cambio di automobile.

I comandi
* `aggiungi-auto`
* `rottama-auto`

permettono di aggiungere e rimuovere automobili in una data stazione con una data autonomia.

Il comando `pianifica-percorso` permette di pianificare un tragitto tra due stazioni, specificando la stazione di partenza e la stazione di arrivo.

Ogni stazione è identificata da un numero intero che indica la distanza dall'inizio dell'autostrada, mentre ogni automobile è identificata da un numero intero (non necessariamente univoco) che ne indica l'autonomia e dalla stazione in cui si trova.

Per ulteriori dettagli si veda il [file di specifica](Presentazione_PF_API.pdf).

## Strutture dati impiegate

Sia per la memorizzazione delle stazioni che per la memorizzazione delle automobili in ogni stazione sono stati utilizzati **alberi rosso-neri**, in quanto permettono di effettuare in tempo logaritmico le operazioni di inserimento, cancellazione e ricerca, che sono le operazioni principali richieste dal progetto.

Più nello specifico, per la memorizzazione delle stazioni è stato utilizzato un albero rosso-nero in cui ogni nodo contiene un intero che rappresenta la distanza dalla stazione di partenza e un puntatore alla struttura che memorizza le automobili presenti in quella stazione ('`Parking`')

Quest'ultima struttura contiene, oltre all'albero rosso-nero per la memorizzazione delle automobili, un singolo intero che rappresenta l'auto con la massima autonomia presente in quella stazione (dato che non c'è ragione di scegliere un'auto con autonomia minore di quella massima). Quest'ultimo valore viene aggiornato ad ogni inserimento o cancellazione di un'automobile.

## Algoritmi implementati

Al di fuori degli algoritmi di base per la gestione degli alberi rosso-neri, non sono stati implementati algoritmi notevoli.

## Debugging e testing

Il programma viene compilato tramite il comando

```/usr/bin/gcc -Wall -Werror -O2 -g3 -fdiagnostics-color=always -g source.c -o a.out```

Esso è stato testato utilizzando i test pubblici forniti dai professori, che si trovano nella cartella `open_tests/`.

Per eseguire i test in maniera automatica è possibile semplicemente eseguire lo script `run_tests.sh`, che automaticamente compila il programma e lo esegue su tutti i test pubblici, stampando a schermo il risultato di ciascun test.

Per testare l'efficienza del programma sono stati impiegati i tool `valgrind memcheck`, `valgrind callgrind` e `massif`, che permettono di valutare l'efficienza del programma in termini di memoria e tempo di esecuzione. L'output di questi tool impiegati sul test case #111 è stato salvato nella cartella `debug/`.