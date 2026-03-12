#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Sistem loglarını tutacağımız Çift Yönlü Bağlı Liste (Doubly Linked List) düğüm yapısı.
 * Her bir düğüm bir log satırını temsil ediyor.
 */
typedef struct LogKaydi {
    char tarih_saat[30];     // Olayın yaşandığı zaman
    char makine_adi[50];     // Logun geldiği sunucu
    char servis_adi[50];     // Logu üreten servis (sshd, kernel vs.)
    char mesaj_icerigi[256]; // Olayın detayı
    
    struct LogKaydi* onceki;  // Ters kronolojik (sondan başa) okuma için kritik işaretçi
    struct LogKaydi* sonraki; // Normal (baştan sona) okuma için işaretçi
} LogKaydi;

// Listenin başını (en eski log) ve sonunu (en güncel log) tutan pointerlar
LogKaydi* ilk_log = NULL;
LogKaydi* son_log = NULL;

/*
 * Sisteme yeni bir log düştüğünde onu listeye ekleyen fonksiyon.
 * Yeni log her zaman zaman çizelgesinin sonuna (kuyruğa) eklenir.
 */
void sistemeLogEkle(const char* zaman, const char* makine, const char* servis, const char* detay) {
    // Yeni log kaydı için RAM'de yer açıyoruz
    LogKaydi* yeniLog = (LogKaydi*)malloc(sizeof(LogKaydi));
    
    // Parametre olarak gelen bilgileri düğümün içine kopyalıyoruz
    strcpy(yeniLog->tarih_saat, zaman);
    strcpy(yeniLog->makine_adi, makine);
    strcpy(yeniLog->servis_adi, servis);
    strcpy(yeniLog->mesaj_icerigi, detay);
    
    yeniLog->sonraki = NULL; // En sona eklendiği için bir sonraki eleman yok

    // Eğer liste şu an tamamen boşsa
    if (ilk_log == NULL) {
        yeniLog->onceki = NULL;
        ilk_log = yeniLog;
        son_log = yeniLog;
    } else {
        // Liste boş değilse, son elemanın arkasına bağlıyoruz
        son_log->sonraki = yeniLog;
        yeniLog->onceki = son_log; // Çift yönlü bağlantıyı burada kuruyoruz
        son_log = yeniLog;         // Artık yeni "son logumuz" bu oldu
    }
}

/*
 * Logları eskiden yeniye doğru normal bir şekilde ekrana basan fonksiyon.
 */
void eskiLoglardanGuncelleOkuma() {
    LogKaydi* gecici = ilk_log;
    printf("\n=== NORMAL SYSLOG OKUMASI (Bastan Sona) ===\n");
    while (gecici != NULL) {
        printf("[%s] %s | %s: %s\n", gecici->tarih_saat, gecici->makine_adi, gecici->servis_adi, gecici->mesaj_icerigi);
        gecici = gecici->sonraki;
    }
    printf("===========================================\n");
}

/*
 * Projenin asıl amacı: Güvenlik analizi için en güncel logdan eskiye doğru okuma.
 * Tek yönlü liste kullansaydık bu işlem çok maliyetli olurdu. 
 * Çift yönlü liste sayesinde 'onceki' pointer'ı ile anında geriye gidebiliyoruz.
 */
void guncelLoglardanEskiyeOkuma() {
    LogKaydi* gecici = son_log;
    printf("\n=== GUVENLIK/ANALIST OKUMASI (Sondan Basa - Ters Kronolojik) ===\n");
    while (gecici != NULL) {
         printf("[%s] %s | %s: %s\n", gecici->tarih_saat, gecici->makine_adi, gecici->servis_adi, gecici->mesaj_icerigi);
        gecici = gecici->onceki; // İşte çift yönlü liste kullanmamızın ana nedeni
    }
    printf("=================================================================\n");
}

/*
 * Program kapanırken bellekte sızıntı (memory leak) olmaması için 
 * RAM'de ayırdığımız yerleri işletim sistemine geri veriyoruz.
 */
void ramTemizle() {
    LogKaydi* gecici = ilk_log;
    LogKaydi* silinecek;
    
    while (gecici != NULL) {
        silinecek = gecici;
        gecici = gecici->sonraki;
        free(silinecek); // Hafızayı boşalt
    }
    ilk_log = NULL;
    son_log = NULL;
}

int main() {
    // Uygulamanın çalıştığını göstermek için simüle edilmiş örnek siber güvenlik logları
    sistemeLogEkle("Mar 12 21:05:10", "klu-sec-server", "kernel", "Booting Linux on physical node");
    sistemeLogEkle("Mar 12 21:12:45", "klu-sec-server", "ufw", "[UFW BLOCK] IN=eth0 OUT= MAC=... SRC=192.168.1.100");
    sistemeLogEkle("Mar 12 21:30:00", "klu-sec-server", "sshd", "Failed password for root from 10.0.0.5 port 22");
    sistemeLogEkle("Mar 12 21:45:12", "klu-sec-server", "sudo", "omer : TTY=pts/0 ; PWD=/home/omer ; USER=root ; COMMAND=/bin/bash");

    // 1. Standart okuma (Eskiden yeniye)
    eskiLoglardanGuncelleOkuma();

    // 2. Analist okuması (Yeniden eskiye - Projenin kalbi)
    guncelLoglardanEskiyeOkuma();

    // 3. Çıkış öncesi RAM temizliği
    ramTemizle();

    return 0;
}
