#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // fork, sleep
#include <sys/wait.h>   // wait
#include <pthread.h>    // pthread
#include <semaphore.h>  // sem_t

#define KAT_SAYISI 10
#define DAIRE_SAYISI 4

pthread_mutex_t su_mutex;   // Su tesisatý için mutex
pthread_mutex_t elektrik_mutex; // Elektrik için mutex

// Dairelerin yaptýðý iþleri temsil eden thread fonksiyonu
void* daire_islemi(void* arg) {
    int daire_no = *(int*)arg;
    printf("   -> Daire %d inþa ediliyor.\n", daire_no);

    // Su tesisatý kurulsun (kilitlenerek)
    pthread_mutex_lock(&su_mutex);
    printf("   -> Daire %d: Su tesisatý kuruyor...\n", daire_no);
    sleep(1); // zaman alýyor
    pthread_mutex_unlock(&su_mutex);

    // Elektrik kablolamasý (kilitlenerek)
    pthread_mutex_lock(&elektrik_mutex);
    printf("   -> Daire %d: Elektrik döþeniyor...\n", daire_no);
    sleep(1);
    pthread_mutex_unlock(&elektrik_mutex);

    printf("   -> Daire %d tamamlandý.\n", daire_no);
    pthread_exit(NULL);
}

int main() {
    // Mutex baþlatýlýyor
    pthread_mutex_init(&su_mutex, NULL);
    pthread_mutex_init(&elektrik_mutex, NULL);

    printf("Apartman inþaatý baþlýyor!\n");

    for (int kat = 1; kat <= KAT_SAYISI; kat++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork hatasý");
            exit(1);
        }

        if (pid == 0) {
            // Alt katlar tamamlanmadan bu kata geçilmemeli!
            printf("KAT %d inþaatý baþlýyor (PID: %d)\n", kat, getpid());

            pthread_t threadler[DAIRE_SAYISI];
            int daire_no[DAIRE_SAYISI];

            for (int i = 0; i < DAIRE_SAYISI; i++) {
                daire_no[i] = (kat - 1) * DAIRE_SAYISI + (i + 1); // Daire numarasý
                pthread_create(&threadler[i], NULL, daire_islemi, &daire_no[i]);
            }

            // Thread’lerin bitmesini bekle
            for (int i = 0; i < DAIRE_SAYISI; i++) {
                pthread_join(threadler[i], NULL);
            }

            printf("KAT %d inþaatý tamamlandý!\n", kat);
            exit(0); // Her process iþini bitirince çýkmalý
        } else {
            // Parent process: sýradaki kata geçmeden önce alt kat bitsin
            wait(NULL);
        }
    }

    printf("Apartman inþaatý tamamlandý!\n");

    // Mutex'leri yok et
    pthread_mutex_destroy(&su_mutex);
    pthread_mutex_destroy(&elektrik_mutex);

    return 0;
}
