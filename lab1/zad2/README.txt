Biblioteka potrafi zrealizowac mergowanie plikow czyli w moim przypadku stworzenie tablicy glownej, usuwanie blokow, konkretnych wierszy.
Testy i pomiary z zadan 2 i 3 mozna wykonac poleceniami opisanymi ponizej.
Zestaw zostal zrealizowany w calosci.


Aby wywylac podstawowe dzialanie programu:
-make main
nastepnie ./output ...
W programie uzytkownik musi poprawnie podac 4 pierwsze argumenty tj. 1. create_table 2.rozmiar(ilosc par) 
3. merge_files 4. lista plikow. Nastepne argumenty mowia o czynnosciach ktore uzytkownik chce wykonac na plikach.
przykladowe wywolanie:
./output create_table 2 merge_files a.txt:b.txt c.txt:d.txt remove_block 1 remove_row 2 0


Aby przeprowadzic testy z zadania 2 nalezy wpisac:
-make test

Komendy realizujace polecenie z zadania 3a:
make main_static
make main_shared
make main_dynamic

komenda realizujaca polecenie 3b:
make zad3b