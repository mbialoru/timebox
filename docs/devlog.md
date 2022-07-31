# Dev log

## Pomiar czasu i rozdzielczości
Więc tak, rozdzielczość zegara systemowego według adjtimex (pakiet)
to jest jedna mikrosekunda. Wartość `tick` definiuje ilość mikrosekund jakie
upływają między kolejnymi tyknięciami zegara. Zatem nasza pełna możliwość
modyfikacji szybkości zegara to przedział 2000 mikrosekund, +/- 1000 od 10000
jakie większość dystrybycji ma w domyślnych wartościach.

I teraz sprawa istotna - próbkowanie zegara, metodą clock_gettime kosztuje czas.
No i czas ten jest na tyle duży, że nasza utrzymywalna rozdzielczość wychodzi
nam mniejsza o jeden rząd wielkości. Dodatkowo trzeba wziąć pod uwagę szum jaki
pojawia się przy mierzeniu czasu takim sposobem. Jest pewna w miarę stała
wartość oscylująca w wąskim przedziale, ale raz na jakiś czas opóźnienie
uzyskania pomiaru rośnie o kilka rzędów wielkości.

### Pomysły
- librt nie zmienia nic zauważalnego
- tryb oszczędzania energii zdaje się mieć coś do powiedzenia
- testowanie kilku trybów, zrobiłem i wykresy wychodzą wyraźnie różne
- Jest jeszcze opcja kernela rt, na raspberrypi ktoś to zrobił i opisał
  - link: https://www.get-edi.io/Real-Time-Linux-on-the-Raspberry-Pi/
  - Wymaga kontenera, kompilacji krzyżowej i ręcznej instalacji
  - Latencja spadła z 5ms to 200us także nie mało, może gra jest warta świeczki
  - Raczej dla poszukiwaczy bardzo wysokiej precyzji

## Jiffies
To jednostka używana w kernelu do określania czasu - jest tożsama z ilością
1/CONFIG_HZ w sekundach. Mała uwaga - mierzenie jej za pomocą `getconf CLK_TCK`
albo za pomocą C API linuxa - daje wartość USER_HZ a nie globalną. Czyli jeżeli
kernel był zbudowany z 1000 Hz to dostaniemy z programu wartość 100, bo tyle
wynosi (zwykle) USER_HZ

Zdaje się, że rozdzielczość timerów w systemie nie może wynosić lepiej niż 1
jiffy w prosty sposób ? - Tak było w starych kernelach, przed wprowadzeniem
`hrtimers` https://www.kernel.org/doc/html/latest/timers/hrtimers.html

## Interfejsowanie z Arduino
To było w sumie łatwiejsze niż się początkowo spodziewałem. Na arduino ustawiłem
pomiar napięcia analogowo z pinu A0 i kazałem mu co ~500ms puszczać to na serial
w cpp można całkiem łatwo otworzyć połączenie przez input stream - wysyłanie też
da się zrobić za pomocą output stream. Zwyczajnie zczytujemy ze strumienia

### Pomysły
- Zamówiłem moduł gps, trzeba go połączyć z arduino i przechwycić informacje PPS
-

## Moduł GPS arduino
Załatwiłem ublox neo 6m, na płytce GY-NEO6MV2, zalutowanie i przypięcie pinów
nie jest trudne, natomiast te drobne rzeczy jakie w koło tego są, potrafią
skutecznie odebrać radość z życia. Po pierwsze na mojej płytce wygląda na to, że
piny RX i TX są "zamienione" - żaden z przykładowych kodów nie pracował
poprawnie dopóki nie zmieniłem tej kolejności (czasem to nie wystarczało, ale
był to warunek konieczny) Także znajomość swojej płytki i tego który pin w
kodzie jest którym tak naprawdę, jest tutaj bardzo ważna. Druga sprawa to
biblioteki jakich należy użyć - trochę przykładów w internecie jest, ale ta
konkretna polecana, nowsza biblioteka nie miała żadnego, a jej api nie jest
kompatybilne z przykładami.
Mogą być poważne problemy przy korzystaniu z platformio na vscode. Arch wymagał
instalacji reguł udev, dodania do grup i przelogowania. Zanim to zrobiłem, nie
można było się dogadać z urządzeniem bo to co wysyłał na serial port to jakieś
śmieci - niezależnie od ustalonej szybkości w baudach.
Moduł gps pierwsze uruchomienie potrafi ponoć cały dzień nawet mielić szukając
sygnału - także nowe urządzenie po podłączeniu może nie zadziałać od razu, a
nawet po dłuższej chwili nie. Gdy pierwsze uruchomienie ma za sobą, z reguły
odpala z miejsca po podaniu napięcia. Można je zasilać 3.3v albo 5v - bez
większego znaczenia

## Praca z typami związanymi z czasem w C++
Ogółem to dość zawiły temat, bo typy używane z czasów archaicznych w C mieszają
się z wprowadzonymi do C++ Dobrym przykładem są `struct tm` i `time_t` tm
dostarcza nam czas przyjemnie podzielony na pola z którymi łatwo się pracuje -
ale maksymalna rozdzielczość z jaką możemy tutaj cokolwiek zmieniać to +/- 1s i
nic mniej. Tymczasem typy wbudowane z nagłóka `<chrono> -> std::chrono`
pozwalają na dużo więcej, ale manipulowanie nimi jest naprawdę niewygodne.
Efektywnie to jest inny sposób wyrażania unix time (time from epoch) czyli w
sekundach od Czwartku, 1-szego stycznia 1970 roku, godzina 00:00:00. Nie trudno
sobie wyobrazić, że wyznaczanie tej nadłożonej ilości sekund jest niewygodne.
Co prawda opracowałem sposób, można go znaleźć w prototypach

C++20 będzie wprowadzał bardzo wygodne narzędzia do pracy z czasem, ale nie mamy
tej za bardzo tutaj wygody, jakoże na dzień dzisiejszy (21/07/2022) kompilator
GCC nie wspiera tych zmian. Nagłówek `<format>` jest niedostępny.

## Vagrant
Vagrantfile `config.vm.synced_folder ".", "/vagrant", mount_options: ["vers=3,tcp"]`

Uruchomić w powłoce dla udostępniania nfs
`$ sudo firewall-cmd --permanent --zone=libvirt --add-service=nfs3`
`$ sudo firewall-cmd --permanent --zone=libvirt --add-service=nfs`
`$ sudo firewall-cmd --permanent --zone=libvirt --add-service=mountd`
`$ sudo firewall-cmd --permanent --zone=libvirt --add-service=rpc-bind`
`$ sudo firewall-cmd --permanent --zone=libvirt --add-port=2049/tcp`
`$ sudo firewall-cmd --permanent --zone=libvirt --add-port=2049/udp`