# Apartman İnşaatı Simülasyonu - Process Thread ve Senkronizasyon Modellenmesi

Bu proje, apartman inşaatı sürecini modelleyerek process ve thread yönetimi ile senkronizasyon mekanizmalarının pratik uygulamasını gösterir. Her kat bir process, her daire bir thread olarak modellenmiştir.
## Proje Sahipleri
```c
Sıdıka Firat 21360859044
```
```c
Tala Kibar 21360859116
```
## İçindekiler

- [Proje Açıklaması](#proje-aciklamasi)
- [Özellikler](#ozellikler)
- [Kurulum](#kurulum)
- [Kullanım](#kullanim)
- [Çalışma Mantığı](#calisma-mantigi)
- [Senkronizasyon Mekanizmaları](#senkronizasyon-mekanizmalari)
- [Kod Analizi](#kod-analizi)
- [Örnek Çıktı](#ornek-cikti)
- [Katkıda Bulunma](#katkida-bulunma)
- [Lisans](#lisans)

## Proje Açıklaması

Bu C programı, 10 katlı bir apartmanın inşaat sürecini simüle eder. Her katta 4 daire bulunur ve inşaat süreci şu şekilde modellenir:

- **Her kat = Bir process (fork ile oluşturulur)**
- **Her daire = Bir thread (pthread ile oluşturulur)**
- **Su tesisatı ve elektrik kaynakları = Mutex ile korunur**
- **Katlar sıralı olarak inşa edilir (alt kat bitmeden üst kata geçilmez)**

## Özellikler

- **Multi-Process Yapısı**: Her kat için ayrı process oluşturma (fork)
- **Multi-Threading**: Her kat içinde paralel daire inşaatı (pthread)
- **Mutex Senkronizasyonu**: 
  - Su tesisatı için mutex kilidi
  - Elektrik sistemi için mutex kilidi
- **Process Sıralama**: Alt kat bitmeden üst kata geçilmez (wait)
- **Thread Senkronizasyonu**: Tüm dairelerin tamamlanmasını bekleme (pthread_join)
- **Gerçek Zamanlı Simülasyon**: İnşaat süreçlerinin zaman gecikmeli modellenmesi

## Kurulum

### Gereksinimler

- **GCC Compiler**: C99 standartını destekleyen C derleyicisi
- **POSIX Uyumlu Sistem**: Linux, macOS veya Unix-like sistem
- **pthread kütüphanesi**: POSIX threads desteği
- **Make** (isteğe bağlı): Derleme otomasyonu için

### Kurulum Adımları

```bash
# Projeyi klonlayın
git clone https://github.com/sidikafirat/ProcessThreadveSenkronizasyonModellenmesi.git

# Proje dizinine gidin
cd ProcessThreadveSenkronizasyonModellenmesi

# Programı derleyin
gcc -o apartman_insaat main.c -lpthread

# Alternatif olarak Makefile kullanın (eğer varsa)
make
```

## Kullanım

### Temel Kullanım

```bash
# Programı çalıştırın
./apartman_insaat
```

### Çıktı Formatı

Program çalıştırıldığında şu şekilde çıktı verir:
- Her katın başlangıç ve bitiş mesajları
- Her dairenin inşaat süreçleri
- Su tesisatı ve elektrik kurulum bilgileri
- Process ID'leri ile kat takibi

## Çalışma Mantığı

### Apartman Yapısı
```
Apartman: 10 Kat × 4 Daire = 40 Daire
```

### İnşaat Süreci Akışı

1. **Ana Process**: Apartman inşaatını başlatır
2. **Kat Process'leri**: Her kat için `fork()` ile yeni process oluşturulur
3. **Daire Thread'leri**: Her kat içinde 4 daire için pthread oluşturulur
4. **İnşaat İşlemleri**: 
   - Su tesisatı kurulumu (mutex korumalı)
   - Elektrik döşenmesi (mutex korumalı)
5. **Senkronizasyon**: 
   - Thread'ler: Tüm daireler bitene kadar kat bekler
   - Process'ler: Alt kat bitene kadar üst kata geçilmez

### Process-Thread Hiyerarşisi

```
Ana Process
├── Kat 1 Process
│   ├── Daire 1 Thread → Su + Elektrik
│   ├── Daire 2 Thread → Su + Elektrik
│   ├── Daire 3 Thread → Su + Elektrik
│   └── Daire 4 Thread → Su + Elektrik
├── Kat 2 Process (Kat 1 bitince başlar)
│   └── ...
└── ...
```

## Senkronizasyon Mekanizmaları

### 1. Mutex Kilitleri

**Su Tesisatı Mutex (`su_mutex`):**
```c
pthread_mutex_lock(&su_mutex);
// Kritik bölge: Su tesisatı kurulumu
printf("Daire %d: Su tesisatı kuruyor...\n", daire_no);
sleep(1);
pthread_mutex_unlock(&su_mutex);
```

**Elektrik Mutex (`elektrik_mutex`):**
```c
pthread_mutex_lock(&elektrik_mutex);
// Kritik bölge: Elektrik döşenmesi
printf("Daire %d: Elektrik döşeniyor...\n", daire_no);
sleep(1);
pthread_mutex_unlock(&elektrik_mutex);
```

### 2. Process Senkronizasyonu

**Fork ve Wait:**
```c
pid_t pid = fork();
if (pid == 0) {
    // Alt process: Kat inşaatı
    // ...
    exit(0);
} else {
    // Ana process: Alt katın bitmesini bekle
    wait(NULL);
}
```

### 3. Thread Senkronizasyonu

**Thread Join:**
```c
// Thread'leri oluştur
for (int i = 0; i < DAIRE_SAYISI; i++) {
    pthread_create(&threadler[i], NULL, daire_islemi, &daire_no[i]);
}

// Tüm thread'lerin bitmesini bekle
for (int i = 0; i < DAIRE_SAYISI; i++) {
    pthread_join(threadler[i], NULL);
}
```

## Kod Analizi

### Ana Yapı

```c
#define KAT_SAYISI 10
#define DAIRE_SAYISI 4

pthread_mutex_t su_mutex;      // Su tesisatı mutex'i
pthread_mutex_t elektrik_mutex; // Elektrik mutex'i
```

### Kritik Bölgeler

**Su Tesisatı Kurulumu:**
- Aynı anda sadece bir daire su tesisatı kurabilir
- Kaynak paylaşımı sorunu çözülür
- Mutex ile korunur

**Elektrik Döşenmesi:**
- Aynı anda sadece bir daire elektrik döşeyebilir
- Elektrik kaynağı mutex ile korunur

### Thread Fonksiyonu

```c
void* daire_islemi(void* arg) {
    int daire_no = *(int*)arg;
    
    // Daire inşaatı başlıyor
    printf("-> Daire %d inşa ediliyor.\n", daire_no);
    
    // Su tesisatı (kritik bölge)
    pthread_mutex_lock(&su_mutex);
    printf("-> Daire %d: Su tesisatı kuruyor...\n", daire_no);
    sleep(1);
    pthread_mutex_unlock(&su_mutex);
    
    // Elektrik (kritik bölge)
    pthread_mutex_lock(&elektrik_mutex);
    printf("-> Daire %d: Elektrik döşeniyor...\n", daire_no);
    sleep(1);
    pthread_mutex_unlock(&elektrik_mutex);
    
    printf("-> Daire %d tamamlandı.\n", daire_no);
    pthread_exit(NULL);
}
```

## Örnek Çıktı

```
Apartman inşaatı başlıyor!
KAT 1 inşaatı başlıyor (PID: 12345)
   -> Daire 1 inşa ediliyor.
   -> Daire 2 inşa ediliyor.
   -> Daire 3 inşa ediliyor.
   -> Daire 4 inşa ediliyor.
   -> Daire 1: Su tesisatı kuruyor...
   -> Daire 1: Elektrik döşeniyor...
   -> Daire 1 tamamlandı.
   -> Daire 2: Su tesisatı kuruyor...
   -> Daire 2: Elektrik döşeniyor...
   -> Daire 2 tamamlandı.
   -> Daire 3: Su tesisatı kuruyor...
   -> Daire 3: Elektrik döşeniyor...
   -> Daire 3 tamamlandı.
   -> Daire 4: Su tesisatı kuruyor...
   -> Daire 4: Elektrik döşeniyor...
   -> Daire 4 tamamlandı.
KAT 1 inşaatı tamamlandı!
KAT 2 inşaatı başlıyor (PID: 12346)
...
Apartman inşaatı tamamlandı!
```

## Performans Analizi

### Zaman Karmaşıklığı
- **Sıralı İnşaat**: O(n × m × t) - n: kat sayısı, m: daire sayısı, t: işlem süresi
- **Bu Implementasyon**: O(n × t) - Paralel daire inşaatı sayesinde

### Kaynak Kullanımı
- **Process Sayısı**: 10 (her kat için bir process)
- **Thread Sayısı**: 4 (her katta 4 daire thread'i)
- **Mutex Sayısı**: 2 (su ve elektrik için)

### Senkronizasyon Faydaları
- **Deadlock Önleme**: Mutex'ler sıralı olarak alınır
- **Race Condition Önleme**: Kritik bölgeler korunur
- **Kaynak Paylaşımı**: Su ve elektrik kaynakları güvenle paylaşılır

## Katkıda Bulunma

1. Bu repository'yi fork edin
2. Yeni bir feature branch oluşturun (`git checkout -b feature/YeniOzellik`)
3. Değişikliklerinizi commit edin (`git commit -am 'Yeni özellik eklendi'`)
4. Branch'inizi push edin (`git push origin feature/YeniOzellik`)
5. Pull Request oluşturun

### Geliştirme Önerileri

- **Farklı İnşaat Senaryoları**: Asansör kurulumu, dış cephe işleri gibi
- **Görselleştirme**: İnşaat sürecinin grafik arayüzle gösterimi
- **İstatistik Toplama**: Süre, kaynak kullanımı analizi
- **Hata Simülasyonu**: Deadlock, starvation durumları
- **Makefile**: Otomatik derleme ve test süreçleri

## Teorik Arka Plan

### Process vs Thread Karşılaştırması

| Özellik | Process | Thread |
|---------|---------|---------|
| Bellek Alanı | Bağımsız | Paylaşımlı |
| Oluşturma Maliyeti | Yüksek | Düşük |
| İletişim | IPC gerekli | Doğrudan paylaşım |
| Hata Yalıtımı | İyi | Zayıf |
| Bu Projede | Her kat | Her daire |

### Senkronizasyon Kavramları

**Race Condition:** Bu projede su tesisatı ve elektrik için mutex kullanmassaydık, birden fazla daire aynı anda bu kaynaklara erişmeye çalışabilirdi.

**Deadlock Önleme:** Mutex'ler sıralı olarak alındığı için deadlock riski minimum düzeyde.

**Resource Sharing:** Su tesisatı ve elektrik kaynakları apartment genelinde paylaşılır, mutex'ler ile korunur.

## Eğitim Değeri

Bu proje aşağıdaki konuları pratik olarak öğretir:

- **Process oluşturma**: `fork()` sistem çağrısının kullanımı
- **Thread yönetimi**: `pthread_create()` ve `pthread_join()` kullanımı
- **Mutual Exclusion**: `pthread_mutex_lock/unlock()` ile kritik bölge koruması
- **Process senkronizasyonu**: `wait()` ile parent-child koordinasyonu
- **Resource sharing**: Paylaşılan kaynakların güvenli kullanımı

## Sorun Giderme

### Derleme Hataları

```bash
# pthread library eksikse
sudo apt-get install libc6-dev

# gcc yoksa
sudo apt-get install gcc

# Derleme komutu
gcc -o apartman_insaat main.c -lpthread -std=c99
```

### Çalışma Zamanı Hataları

- **Segmentation Fault**: Mutex'lerin initialize edildiğinden emin olun
- **Deadlock**: Mutex'lerin sıralı olarak alındığından emin olun
- **Zombie Process**: Her child process'in `exit(0)` ile sonlandığından emin olun

## Lisans

Bu proje MIT lisansı altında lisanslanmıştır. Detaylar için [LICENSE](LICENSE) dosyasına bakın.

## İletişim

- **Geliştirici**: Sıdıka Fırat
- **GitHub**: [@sidikafirat](https://github.com/sidikafirat)
- **E-posta**: [E-posta adresiniz]

## Kaynaklar

- [POSIX Threads Programming](https://computing.llnl.gov/tutorials/pthreads/)
- [Advanced Programming in the UNIX Environment](http://www.apuebook.com/)
- [Operating System Concepts - Silberschatz](https://www.os-book.com/)
- [Linux System Programming](http://man7.org/linux/man-pages/)

## Versiyon Geçmişi

- **v1.0.0** - İlk sürüm
  - 10 katlı apartman simülasyonu
  - Fork-based process yönetimi
  - Pthread-based thread yönetimi
  - Mutex-based resource protection

---

Bu proje, işletim sistemleri ve concurrent programming konularını pratik bir senaryoyla öğrenmek için tasarlanmıştır. Apartman inşaatı metaforu ile process ve thread kavramları somutlaştırılmıştır.
