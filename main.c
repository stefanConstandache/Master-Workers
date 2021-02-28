#include "mpi.h"
#include <math.h>
#include <stdio.h> 
#include <string.h> 
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#define MASTER 0
#define HORROR 1
#define COMEDY 2
#define FANTASY 3
#define SF 4
#define SizeOfLine 3000
#define NR_MAX_THREADS sysconf(_SC_NPROCESSORS_CONF) - 1

pthread_barrier_t barrier;
char fileNameInput[30];
char fileNameOutput[30] = "";

char vowels[10] = "aeiouAEIOU";

char **paragraph_horror;
char **paragraph_horrorCopy;
int okey_horror = 1;
int dimension_horror;

char **paragraph_comedy;
int dimension_comedy;

char **paragraph_fantasy;
int dimension_fantasy;

char **paragraph_sf;
int dimension_sf;

char **paragraphToWrite;
int fileDimension = 0;

void *f(void *arg) {
    int thread_id = *(int  *)arg;

    // Deschid fisierul de input in fiecare thread.
    FILE *fp;
    fp = fopen(fileNameInput, "r");

    // Primul thread parcurge tot fisierul
    // si afla cate linii are acesta,
    // dupa care mut cursorul la inceputul fisierului.
    if (thread_id == 0) {
        char line[SizeOfLine];
        
        while(!feof(fp)) {
            fgets(line, SizeOfLine, fp);
            fileDimension++;
        }

        if (line[strlen(line)- 1] == '\n') {
            fileDimension--;
        }

        fseek(fp, 0, SEEK_SET);

        paragraphToWrite = malloc(fileDimension * sizeof(char*));
    }

    // Celelalte threaduri asteapta ca primul thread
    // sa calculeze numarul de linii din fisier.
    pthread_barrier_wait(&barrier);

    if (thread_id == 0) {
        int nrLinie = 0;
        
        char line[SizeOfLine];
        fgets(line, SizeOfLine, fp);

        while (!feof(fp)) {
            int ok = 0;
            int randLiber = 1;
            int i = 1;
            char **paragraph = malloc(i * sizeof(*paragraph));
            paragraph[i-1] = malloc(SizeOfLine * sizeof(*paragraph[i-1]));

            // Caut prima aparitie a liniei corespunzatoare
            // paragrafului care va fi procesat de threadul curent.
            while (!feof(fp) && strcmp(line, "horror\n") != 0) {
                fgets(line, SizeOfLine, fp);
                nrLinie++;
            }

            // Copiez in paragraful care va fi trimis liniile corespunzatoare din fisier.
            while (!feof(fp) && strcmp(line, "\n") != 0) {
                ok = 1;
                strcpy(paragraph[i-1], line);

                fgets(line, SizeOfLine, fp);
                if (strcmp(line, "\n") != 0) {
                    i++;
                    paragraph = realloc(paragraph, i * sizeof(*paragraph));
                    paragraph[i-1] = malloc(SizeOfLine * sizeof(*paragraph[i-1]));
                }
            }
            
            if (strcmp(line, "\n") != 0 && feof(fp) && ok) {
                randLiber = 0;
                strcpy(paragraph[i-1], line);
            }

            // Trimit catre workerul respectiv dimensiunea paragrafului.
            MPI_Send(&i, 1, MPI_INT, HORROR, 0, MPI_COMM_WORLD);

            // Daca dimensiunea paragrafului este mai mare ca 1
            // atunci trimit pe rand linii paragrafului care trebuie procesat.
            if (i > 1 && !feof(fp)) {
                for (int j = 0; j < i; j++) {
                    MPI_Send(&(paragraph[j][0]), SizeOfLine, MPI_BYTE, HORROR, i, MPI_COMM_WORLD);
                }
            }
            else if (i > 1 && feof(fp)) {
                for (int j = 0; j < i; j++) {
                    MPI_Send(&(paragraph[j][0]), SizeOfLine, MPI_BYTE, HORROR, 0, MPI_COMM_WORLD);
                }
            }

            // Astept sa primesc inapoi paragraful procesat.
            // Paragraful procesat o sa fie primit direct la linia corespunzatoare din textul final.
            if (i > 1) {
                for (int j = nrLinie; j < nrLinie + i; j++) {
                    paragraphToWrite[j] = malloc(SizeOfLine * sizeof(char));
                    MPI_Recv(&(paragraphToWrite[j][0]), SizeOfLine, MPI_BYTE, HORROR, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
                }

                if (randLiber == 1 && ok) {
                    paragraphToWrite[nrLinie + i] = malloc(SizeOfLine * sizeof(char));
                    strcpy(paragraphToWrite[nrLinie + i], "\n");
                }
            }

            free(paragraph);
            nrLinie += randLiber + i - 1;
        }
    }
    else if (thread_id == 1) {
        int nrLinie = 0;

        char line[SizeOfLine];
        fgets(line, SizeOfLine, fp);

        while (!feof(fp)) {
            int ok = 0;
            int randLiber = 1;
            int i = 1;
            char **paragraph = malloc(i * sizeof(*paragraph));
            paragraph[i-1] = malloc(SizeOfLine * sizeof(*paragraph[i-1]));


            // Caut prima aparitie a liniei corespunzatoare
            // paragrafului care va fi procesat de threadul curent.
            while (!feof(fp) && strcmp(line, "comedy\n") != 0) {
                fgets(line, SizeOfLine, fp);
                nrLinie++;
            }

            // Copiez in paragraful care va fi trimis liniile corespunzatoare din fisier.
            while (!feof(fp) && strcmp(line, "\n") != 0) {
                ok = 1;
                strcpy(paragraph[i-1], line);

                fgets(line, SizeOfLine, fp);
                if (strcmp(line, "\n") != 0) {
                    i++;
                    paragraph = realloc(paragraph, i * sizeof(*paragraph));
                    paragraph[i-1] = malloc(SizeOfLine * sizeof(*paragraph[i-1]));
                }
            }

            if (strcmp(line, "\n") != 0 && feof(fp) && ok) {
                randLiber = 0;
                strcpy(paragraph[i-1], line);
            }

            // Trimit catre workerul respectiv dimensiunea paragrafului.
            MPI_Send(&i, 1, MPI_INT, COMEDY, 0, MPI_COMM_WORLD);

            // Daca dimensiunea paragrafului este mai mare ca 1
            // atunci trimit pe rand linii paragrafului care trebuie procesat.
            if (i > 1 && !feof(fp)) {
                for (int j = 0; j < i; j++) {
                    MPI_Send(&(paragraph[j][0]), SizeOfLine, MPI_BYTE, COMEDY, i, MPI_COMM_WORLD);
                }
            }
            else if (i > 1 && feof(fp)) {
                for (int j = 0; j < i; j++) {
                    MPI_Send(&(paragraph[j][0]), SizeOfLine, MPI_BYTE, COMEDY, 0, MPI_COMM_WORLD);
                }
            }


            // Astept sa primesc inapoi paragraful procesat.
            // Paragraful procesat o sa fie primit direct la linia corespunzatoare din textul final.
            if (i > 1) {
                for (int j = nrLinie; j < nrLinie + i; j++) {
                    paragraphToWrite[j] = malloc(SizeOfLine * sizeof(char));
                    MPI_Recv(&(paragraphToWrite[j][0]), SizeOfLine, MPI_BYTE, COMEDY, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
                }

                if (randLiber == 1 && ok) {
                    paragraphToWrite[nrLinie + i] = malloc(SizeOfLine * sizeof(char));
                    strcpy(paragraphToWrite[nrLinie + i], "\n");
                }
            }

            free(paragraph);
            nrLinie += randLiber + i - 1;
        }
    }
    else if (thread_id == 2) {
        int nrLinie = 0;

        char line[SizeOfLine];
        fgets(line, SizeOfLine, fp);

        while (!feof(fp)) {
            int ok = 0;
            int randLiber = 1;
            int i = 1;
            char **paragraph = malloc(i * sizeof(*paragraph));
            paragraph[i-1] = malloc(SizeOfLine * sizeof(*paragraph[i-1]));


            // Caut prima aparitie a liniei corespunzatoare
            // paragrafului care va fi procesat de threadul curent.
            while (!feof(fp) && strcmp(line, "fantasy\n") != 0) {
                fgets(line, SizeOfLine, fp);
                nrLinie++;
            }

            // Copiez in paragraful care va fi trimis liniile corespunzatoare din fisier.
            while (!feof(fp) && strcmp(line, "\n") != 0) {
                ok = 1;
                strcpy(paragraph[i-1], line);

                fgets(line, SizeOfLine, fp);
                if (strcmp(line, "\n") != 0) {
                    i++;
                    paragraph = realloc(paragraph, i * sizeof(*paragraph));
                    paragraph[i-1] = malloc(SizeOfLine * sizeof(*paragraph[i-1]));
                }
            }

            if (strcmp(line, "\n") != 0 && feof(fp) && ok) {
                randLiber = 0;
                strcpy(paragraph[i-1], line);
            }

            // Trimit catre workerul respectiv dimensiunea paragrafului.
            MPI_Send(&i, 1, MPI_INT, FANTASY, 0, MPI_COMM_WORLD);

            // Daca dimensiunea paragrafului este mai mare ca 1
            // atunci trimit pe rand linii paragrafului care trebuie procesat.
            if (i > 1 && !feof(fp)) {
                for (int j = 0; j < i; j++) {
                    MPI_Send(&(paragraph[j][0]), SizeOfLine, MPI_BYTE, FANTASY, i, MPI_COMM_WORLD);
                }
            }
            else if (i > 1 && feof(fp)) {
                for (int j = 0; j < i; j++) {
                    MPI_Send(&(paragraph[j][0]), SizeOfLine, MPI_BYTE, FANTASY, 0, MPI_COMM_WORLD);
                }
            }


            // Astept sa primesc inapoi paragraful procesat.
            // Paragraful procesat o sa fie primit direct la linia corespunzatoare din textul final.
            if (i > 1) {
                for (int j = nrLinie; j < nrLinie + i; j++) {
                    paragraphToWrite[j] = malloc(SizeOfLine * sizeof(char));
                    MPI_Recv(&(paragraphToWrite[j][0]), SizeOfLine, MPI_BYTE, FANTASY, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
                }

                if (randLiber == 1 && ok) {
                    paragraphToWrite[nrLinie + i] = malloc(SizeOfLine * sizeof(char));
                    strcpy(paragraphToWrite[nrLinie + i], "\n");
                }
            }

            free(paragraph);
            nrLinie += randLiber + i - 1;
        }
    }
    else if (thread_id == 3) {
        int nrLinie = 0;

        char line[SizeOfLine];
        fgets(line, SizeOfLine, fp);

        while (!feof(fp)) {
            int ok = 0;
            int randLiber = 1;
            int i = 1;
            char **paragraph = malloc(i * sizeof(*paragraph));
            paragraph[i-1] = malloc(SizeOfLine * sizeof(*paragraph[i-1]));

            // Caut prima aparitie a liniei corespunzatoare
            // paragrafului care va fi procesat de threadul curent.
            while (!feof(fp) && strcmp(line, "science-fiction\n") != 0) {
                fgets(line, SizeOfLine, fp);
                nrLinie++;
            }

            // Copiez in paragraful care va fi trimis liniile corespunzatoare din fisier.
            while (!feof(fp) && strcmp(line, "\n") != 0) {
                ok = 1;
                strcpy(paragraph[i-1], line);

                fgets(line, SizeOfLine, fp);
                if (strcmp(line, "\n") != 0) {
                    i++;
                    paragraph = realloc(paragraph, i * sizeof(*paragraph));
                    paragraph[i-1] = malloc(SizeOfLine * sizeof(*paragraph[i-1]));
                }
            }

            if (strcmp(line, "\n") != 0 && feof(fp) && ok) {
                randLiber = 0;
                strcpy(paragraph[i-1], line);
            }

            // Trimit catre workerul respectiv dimensiunea paragrafului.
            MPI_Send(&i, 1, MPI_INT, SF, 0, MPI_COMM_WORLD);

            // Daca dimensiunea paragrafului este mai mare ca 1
            // atunci trimit pe rand linii paragrafului care trebuie procesat.
            if (i > 1 && !feof(fp)) {
                for (int j = 0; j < i; j++) {
                    MPI_Send(&(paragraph[j][0]), SizeOfLine, MPI_BYTE, SF, i, MPI_COMM_WORLD);
                }
            }
            else if (i > 1 && feof(fp)) {
                for (int j = 0; j < i; j++) {
                    MPI_Send(&(paragraph[j][0]), SizeOfLine, MPI_BYTE, SF, 0, MPI_COMM_WORLD);
                }
            }

            // Astept sa primesc inapoi paragraful procesat.
            // Paragraful procesat o sa fie primit direct la linia corespunzatoare din textul final.
            if (i > 1) {
                for (int j = nrLinie; j < nrLinie + i; j++) {
                    paragraphToWrite[j] = malloc(SizeOfLine * sizeof(char));
                    MPI_Recv(&(paragraphToWrite[j][0]), SizeOfLine, MPI_BYTE, SF, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
                }

                if (randLiber == 1 && ok) {
                    paragraphToWrite[nrLinie + i] = malloc(SizeOfLine * sizeof(char));
                    strcpy(paragraphToWrite[nrLinie + i], "\n");
                }
            }

            free(paragraph);
            nrLinie += randLiber + i - 1;
        }
    }

    fclose(fp);
    pthread_exit(NULL);

}

void *process_horror(void *arg) {
    int thread_id = *(int  *)arg;
    int i;

    while(thread_id * 20 + 1 < dimension_horror) {

        // Se calculeaza ce linii va procesa threadul respectiv.
        int start = thread_id * 20 + 1;
        int end;
        if ((thread_id + 1) * 20 + 1 < dimension_horror) {
            end = (thread_id + 1) * 20 + 1;
        }
        else {
            end = dimension_horror;
        }
        
        // Threadul proceseaza liniile aferente.
        for (i = start; i < end; i++) {

            // Parcurg linia de procesat caracter cu caracter.
            for(int j = 0; j < strlen(paragraph_horror[i]); j++) {

                // Daca caracterul nu este o vocala si este litera atunci
                // se va copia de doua ori in linia paragrafului care
                // va fi trimis inapoi la Master, altfel se va copia doar o singura data.
                // (literele mari se vor transforma mai intai in litere mici)
                if(strchr(vowels, paragraph_horror[i][j]) == NULL && isalpha(paragraph_horror[i][j])) {
                    strncat(paragraph_horrorCopy[i], &paragraph_horror[i][j], 1);
                    if (paragraph_horror[i][j] >= 65 && paragraph_horror[i][j] <= 90) {
                        char c = paragraph_horror[i][j] + 32;
                        strncat(paragraph_horrorCopy[i], &c, 1);
                    } else {
                        strncat(paragraph_horrorCopy[i], &paragraph_horror[i][j], 1);
                    }
                } else {
                    strncat(paragraph_horrorCopy[i], &paragraph_horror[i][j], 1);
                }
            }
        }

        // Maresc numarul ID-ului threadului actual pentru
        // a continua procesarea paragrafului daca e nevoie.
        thread_id += NR_MAX_THREADS;
    }

    pthread_exit(NULL);
}

void *process_comedy(void *arg) {
    int thread_id = *(int  *)arg;
    int i;

    while(thread_id* 20 + 1 < dimension_comedy) {

        // Se calculeaza ce linii va procesa threadul respectiv.
        int start = thread_id * 20 + 1;
        int end;
        if ((thread_id + 1) * 20 + 1 < dimension_comedy) {
            end = (thread_id + 1) * 20 + 1;
        }
        else {
            end = dimension_comedy;
        }
        
        // Threadul proceseaza liniile aferente.
        for (i = start; i < end; i++) {
            int nrLetter = 1;

            // Parcurg linia de procesat caracter cu caracter.
            for(int j = 0; j < strlen(paragraph_comedy[i]); j++) {

                // Daca caracterul curent este o litera si pozitia acestuia
                // in cuvant este para atunci se va transforma in litera mare.
                if(nrLetter % 2 == 0 && isalpha(paragraph_comedy[i][j]) && paragraph_comedy[i][j] >= 97) {
                    paragraph_comedy[i][j] -= 32;
                }

                // La terminarea cuvantului resetez pozitia caracterului.
                if (paragraph_comedy[i][j] == ' ') {
                    nrLetter = 0;
                }

                nrLetter++;
            }
        }

        // Maresc numarul ID-ului threadului actual pentru
        // a continua procesarea paragrafului daca e nevoie.
        thread_id += NR_MAX_THREADS;
    }

    pthread_exit(NULL);
}

void *process_fantasy(void *arg) {
    int thread_id = *(int  *)arg;
    int i;

    while(thread_id* 20 + 1 < dimension_fantasy) {

        // Se calculeaza ce linii va procesa threadul respectiv.
        int start = thread_id * 20 + 1;
        int end;
        if ((thread_id + 1) * 20 + 1 < dimension_fantasy) {
            end = (thread_id + 1) * 20 + 1;
        }
        else {
            end = dimension_fantasy;
        }
        
        // Threadul proceseaza liniile aferente.
        for (i = start; i < end; i++) {
            int okey = 1;

            // Parcurg linia de procesat caracter cu caracter.
            for(int j = 0; j < strlen(paragraph_fantasy[i]); j++) {

                // Daca este primul caracter din cuvant si acesta este
                // o litera atunci se va transforma in litera mare.
                if(okey == 1 && isalpha(paragraph_fantasy[i][j])) {
                    if (paragraph_fantasy[i][j] >= 97) {
                        paragraph_fantasy[i][j] -= 32;
                    }

                    okey = 0;
                }

                // Daca se trece la urmatorul cuvant atunci se reseteaza variabila
                // care anunta ce este primul caracter din cuvant.
                if (okey == 0 && paragraph_fantasy[i][j] == ' ') {
                    okey = 1;
                }
            }
        }

        // Maresc numarul ID-ului threadului actual pentru
        // a continua procesarea paragrafului daca e nevoie.
        thread_id += NR_MAX_THREADS;
    }

    pthread_exit(NULL);
}

void *process_sf(void *arg) {
    int thread_id = *(int  *)arg;
    int i;

    while(thread_id* 20 + 1 < dimension_sf) {

        // Se calculeaza ce linii va procesa threadul respectiv.
        int start = thread_id * 20 + 1;
        int end;
        if ((thread_id + 1) * 20 + 1 < dimension_sf) {
            end = (thread_id + 1) * 20 + 1;
        }
        else {
            end = dimension_sf;
        }
        
        // Threadul proceseaza liniile aferente.
        for (i = start; i < end; i++) {
            int nrWord = 1;
            int gotFirst = 0;
            int gotLast = 0;
            int firstLetter = 0;
            int lastLetter = 0;
            char word[100] = "";

            // Parcurg caracter cu caracter linia de procesat.
            for(int j = 0; j < strlen(paragraph_sf[i]); j++) {
                // Anunt ca s-a gasit prima litera din cuvantul care trebuie inversat.
                if (nrWord == 7 && (isalpha(paragraph_sf[i][j]) || paragraph_sf[i][j] == '.') && gotFirst == 0) {
                    firstLetter = j;
                    gotFirst = 1;
                }

                // Anunt ca s-a gasit ultima litera din cuvant.
                if (nrWord == 7 && !(isalpha(paragraph_sf[i][j]) || paragraph_sf[i][j] == '.') && gotLast == 0) {
                    lastLetter = j;
                    gotLast = 1;
                }
                // Pana nu se ajunge la ultima litera din cuvant formez cuvantul in alta variabila.
                if (gotFirst && !gotLast) {
                    strncat(word, &paragraph_sf[i][j], 1);
                }


                // Daca lungimea cuvantului e mai mare de 1 atunci
                // se copiaza caracterele inversate din variabila
                // care contine cuvantul in paragraful care va fi transmis.
                if (nrWord == 7 && lastLetter - firstLetter > 1) {
                    for(int k = firstLetter; k < lastLetter; k++) {
                        paragraph_sf[i][k] = word[lastLetter - 1 - k];
                    }
                }

                // Daca s-a trecut de al 7-lea cuvant atunci resetez valorile.
                if (paragraph_sf[i][j] == ' ' && nrWord == 7) {
                    nrWord = 1;
                    gotFirst = 0;
                    gotLast = 0;

                    strcpy(word, "");
                } 
                // Daca se ajunge la caracterul "spatiu" se incrementeaza
                // numarul de cuvinte parcurse.
                else if (paragraph_sf[i][j] == ' ') {
                    nrWord++;
                }
            }
        }

        // Maresc numarul ID-ului threadului actual pentru
        // a continua procesarea paragrafului daca e nevoie.
        thread_id += NR_MAX_THREADS;
    }

    pthread_exit(NULL);
}

void *receiver(void *arg) {
    int rank = *(int  *)arg;
    int true = 1;
    while (true) {
        int paragraphDimension;
        MPI_Status status1;

        // Primesc dimensiunea paragrafului.
        MPI_Recv(&paragraphDimension, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status1);

        // Daca dimensiunea primita e 1 inseamna ca nu se va mai
        // trimite niciun paragraf catre workerul respectiv si se inchide threadul de citire.
        if (paragraphDimension == 1) {
            break;
        }

        // Aloc memorie pentru variabila care va primi paragraful.
        char **paragraph = malloc(paragraphDimension * sizeof(*paragraph));
        
        if (okey_horror && rank == HORROR) {
            paragraph_horrorCopy = malloc(paragraphDimension * sizeof(*paragraph));
        } else if (rank == HORROR) {
            paragraph_horrorCopy = realloc(paragraph_horrorCopy, paragraphDimension * sizeof(*paragraph));
        }

        
        for (int k = 0; k < paragraphDimension; k++) {
            paragraph[k] = calloc(SizeOfLine , sizeof(*paragraph[k]));
            
            if (rank == HORROR) {
                paragraph_horrorCopy[k] = calloc(SizeOfLine , sizeof(*paragraph[k]));
            }
        }

        
        if (rank == HORROR && okey_horror) {
            okey_horror = 0;
        }

        MPI_Status status2;

        // Primesc linie cu linie paragraful care trebuie procesat.
        for (int j = 0; j < paragraphDimension; j++) {
            MPI_Recv(&(paragraph[j][0]), SizeOfLine, MPI_BYTE, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status2);
        }

        if (rank == HORROR) {
            strcpy(paragraph_horrorCopy[0], paragraph[0]);
        }

        int numberOfThreads;

        // Calculez numarul de threaduri care trebuie
        // deschise pentru a procesa paragraful (maxim P - 1).
        if ((paragraphDimension - 1) % 20 == 0) {
            int nr = (paragraphDimension - 1) / 20;
            if (nr < NR_MAX_THREADS) {
                numberOfThreads = nr;
            }
            else {
                numberOfThreads = NR_MAX_THREADS;
            }
        }
        else {
            int nr = (paragraphDimension - 1) / 20 + 1;
            if (nr < NR_MAX_THREADS) {
                numberOfThreads = nr;
            }
            else {
                numberOfThreads = NR_MAX_THREADS;
            }
        }

        // Pentru a procesa paragrafele in threadurile din workeri
        // folosesc variabile globale care "pointeaza" catre paragraful primit.
        pthread_t threads[numberOfThreads];
        int thread_id[numberOfThreads];
        if (rank == HORROR) {
            paragraph_horror = paragraph;
            dimension_horror = paragraphDimension;
        }
        else if (rank == COMEDY) {
            paragraph_comedy = paragraph;
            dimension_comedy = paragraphDimension;
        }
        else if (rank == FANTASY) {
            paragraph_fantasy = paragraph;
            dimension_fantasy = paragraphDimension;
        }
        else if (rank == SF) {
            paragraph_sf = paragraph;
            dimension_sf = paragraphDimension;
        }

        // Deschid threadurile.
        for (int i = 0; i < numberOfThreads; i++) {
            thread_id[i] = i;

            if (rank == HORROR) {
                pthread_create(&threads[i], NULL, process_horror, &thread_id[i]);
            }
            else if (rank == COMEDY) {
                pthread_create(&threads[i], NULL, process_comedy, &thread_id[i]);
            }
            else if (rank == FANTASY) {
                pthread_create(&threads[i], NULL, process_fantasy, &thread_id[i]);
            }
            else if (rank == SF) {
                pthread_create(&threads[i], NULL, process_sf, &thread_id[i]);
            }
        }

        for (int i = 0; i < numberOfThreads; i++) {
            pthread_join(threads[i], NULL);
        }

        if (rank == HORROR) {
            paragraph = paragraph_horrorCopy;
        }

        // Dupa ce paragraful a fost procesat il trimit inapoi la procesul Master.
        for (int j = 0; j < paragraphDimension; j++) {
            MPI_Send(&(paragraph[j][0]), SizeOfLine, MPI_BYTE, MASTER, j, MPI_COMM_WORLD);
        }

        // Daca s-a primit o linie de paragraf cu tagul 0 inseamna ca
        // era ultima linie de procesat si se iese inchide threadul de citire.
        if (status2.MPI_TAG == 0) {
            break;
        }

    }

    pthread_exit(NULL);
}
  
int main (int argc, char *argv[])
{ 
    int numtasks, rank, provided;

    // Creez procesele (5)
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == MASTER) {
        int i;

        // In procesul Master, dupa numele fisierului de input
        // calculez numele fisierului de output.
        strcpy(fileNameInput, argv[1]);
        for(i = 0; i < strlen(fileNameInput); i++) {
            if (fileNameInput[i] == '.' && i != 0) {
                break;
            }

            strncat(fileNameOutput, &fileNameInput[i], 1);
        }
        strcat(fileNameOutput, ".out");

        pthread_t threads[4];
        int thread_id[4];

        pthread_barrier_init(&barrier, NULL, 4);

        // Deschid 4 threaduri care o sa citeasca fisierul
        // si o sa trimita workerilor paragrafele respective.
        for (i = 0; i < 4; i++) {
            thread_id[i] = i;
            pthread_create(&threads[i], NULL, f, &thread_id[i]);
        }

        for (i = 0; i < 4; i++) {
            pthread_join(threads[i], NULL);
        }


        FILE *fp;
        fp = fopen(fileNameOutput, "w");

        // Scriu in fisierul de output textul procesat.
        for (i = 0; i < fileDimension; i++) {
            fwrite(paragraphToWrite[i], sizeof(char), strlen(paragraphToWrite[i]), fp);
        }

        free(paragraphToWrite);
        fclose(fp);

        pthread_barrier_destroy(&barrier);
    } 
    // In fiecare worker pornesc un thread care va primi paragraful procesat din procesul Master.
    else if (rank == HORROR) {
        pthread_t master_thread;
        pthread_create(&master_thread, NULL, receiver, &rank);

        pthread_join(master_thread, NULL);
    }
    else if (rank == COMEDY) {
        pthread_t master_thread;
        pthread_create(&master_thread, NULL, receiver, &rank);

        pthread_join(master_thread, NULL);
    }
    else if (rank == FANTASY) {
        pthread_t master_thread;
        pthread_create(&master_thread, NULL, receiver, &rank);

        pthread_join(master_thread, NULL);
    }
    else if (rank == SF) {
        pthread_t master_thread;
        pthread_create(&master_thread, NULL, receiver, &rank);

        pthread_join(master_thread, NULL);
    }
  
    MPI_Finalize(); 
} 