# Linux Syslog Bilgilerinin Çift Yönlü Bağlı Liste ile Gösterilmesi

Bu çalışma, Veri Yapıları ve Algoritmalar dersi kapsamında Linux işletim sistemindeki sistem günlüklerinin (syslog) anlaşılması ve bu verilerin işlenmesi için uygun bağlı liste (linked list) veri yapısının tasarlanması amacıyla hazırlanmıştır.

## Projenin Amacı ve Teknik Detaylar
Projede Linux log kayıtları (tarih, makine adı, servis adı ve mesaj detayları) C dilinde yazılmış dinamik bir veri yapısı olan **Çift Yönlü Bağlı Liste (Doubly Linked List)** kullanılarak RAM üzerinde tutulmuştur.

## Tercih Edilen Veri Yapısı ve Syslog İlişkisi
Sistem logları doğası gereği kronolojik olarak artar. Ancak sistem yöneticileri veya siber güvenlik analistleri bir log incelemesi yaparken genellikle en güncel olan, yani en son üretilen loglara bakmak isterler. 

Bu projede Tek Yönlü Bağlı Liste yerine **Çift Yönlü Bağlı Liste** kullanılmasının temel teknik sebebi budur. Çift yönlü bağlı listede bulunan `onceki` (prev) işaretçisi (pointer) sayesinde, listenin sonundan (Tail) başlanarak geriye doğru çok hızlı ve düşük maliyetli (O(1) karmaşıklığında) bir ters kronolojik okuma yapılabilmektedir.

## Kurulum ve Çalıştırma
Projenin kaynak kodlarını derlemek ve çalıştırmak için sisteminizde GCC derleyicisi kurulu olmalıdır. Terminal ekranında aşağıdaki komutları sırasıyla çalıştırarak uygulamayı test edebilirsiniz:

```bash
# Kodu derleme
gcc main.c -o syslog_app

# Uygulamayı çalıştırma
./syslog_appDosya ve Fonksiyon Yapısı
LogKaydi (Struct): Sistem log verilerini ve bellekteki bağlantı adreslerini (onceki / sonraki) tutan temel veri yapısıdır.

sistemeLogEkle(): Gelen yeni Syslog kaydı için Heap üzerinde bellekte dinamik yer ayıran (malloc) ve düğümü kronolojik olarak listenin sonuna bağlayan fonksiyondur.

eskiLoglardanGuncelleOkuma(): sonraki işaretçilerini takip ederek logları baştan sona (eskiden yeniye) doğru listeleyen standart okuma fonksiyonudur.

guncelLoglardanEskiyeOkuma(): Projenin ana amacı olan çift yönlü bağlı listenin avantajını kullanarak; onceki işaretçileri yardımıyla logları sondan başa (yeniden eskiye) performanslı bir şekilde okuyan güvenlik/analist okuması fonksiyonudur.

ramTemizle(): Sonsuz döngüde çalışan sistemlerde hafıza sızıntısını (Memory Leak) önlemek amacıyla, program kapanmadan önce tahsis edilen dinamik belleği işletim sistemine iade eden (free) fonksiyondur.
