############################################################################
#    Copyright (C) 2004 by Sad Mephisto                                    #
#    sad@antisocial.com                                                    #
#                                                                          #
#    This program is free software; you can redistribute it and#or modify  #
#    it under the terms of the GNU General Public License as published by  #
#    the Free Software Foundation; either version 2 of the License, or     #
#    (at your option) any later version.                                   #
#                                                                          #
#    This program is distributed in the hope that it will be useful,       #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#    GNU General Public License for more details.                          #
#                                                                          #
#    You should have received a copy of the GNU General Public License     #
#    along with this program; if not, write to the                         #
#    Free Software Foundation, Inc.,                                       #
#    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
############################################################################
#!/bin/bash
# $Id: kaduinstaller.sh,v 1.1 2004/04/06 11:46:16 romek Exp $
# KaduInstaller - Copyright (C) 2004 Sad Mephisto <sad@antisocial.com>

# ./configure, make, make install....  tylko �eby �adnie wygl�da�o
zainstaluj()
{
	rm -fr log_install
	./configure `cat log_prefix 2> /dev/null` `cat log_configureoptions 2>/dev/null` > log_install 2> log_install && make >> log_install 2>>log_install && make install 2>> log_install >> log_install && gotowe &
	sleep 3
	dialog --backtitle "Instalator Kadu" --begin 3 2 --infobox "Instalowanie Kadu... czekaj cierpliwie" 3 45  --and-widget --begin 10 2 --exit-label "Zaczekaj..." --tailbox log_install 13 75 
		
}

# make uninstall
odinstaluj()
{
	$?=0
	dialog --stdout --defaultno --backtitle "Instalator Kadu" --title "Deinstalacja" --yesno "Czy na pewno chcesz odinstalowa� Kadu?" 6 50
		if [ $? == "0" ]; then
			dialog --stdout --backtitle "Instalator Kadu" --infobox "Deinstalacja... zaczekaj" 3 30
			make uninstall > log_uninstall 2>> log_uninstall &
			dialog --backtitle "Instalator Kadu" --ok-label "OK" --msgbox "Kadu zosta�o odinstalowane" 5 30 
		fi;
}

# Wyb�r modu��w do kompilacji.
# Usuwa plik .config, i zapisuje go po swojemu.
# TODO: Statyczna kompilacja
wybierzmoduly()
{
	rm -fr log_selectedmodule
	dialog --separate-output --checklist "Wybierz modu�y do zainstalowania" 0 75 0 \
    		encryption "Obs�uga szyfrowania SSL" on \
    		speech "Obs�uga syntezatora mowy" 0 \
    		sound "Ogolne wsparcie dla d�wi�ku" on \
    		ext_sound "Obs�uga zewn�trznego programu obs�ugujacego d�wi�k" on \
    		dsp_sound "Wsparcie DSP dla d�wi�ku" 0 \
    		arts_sound "Wsparcie aRts! dla d�wi�ku" 0 \
    		esd_sound "Wsparcie ESD dla d�wi�ku" 0 \
    		nas_sound "Wsparcie Network Audio System dla d�wi�ku" 0 \
		sms "Obs�uga wsys�ania SMS" on \
		default_sms "Obs�uga standardowych bramek SMS" on \
    		"echo" "Przyk�adowy modu� Echo" 0 \
    		autoresponder "Przyk�adowy modu� Autoresponder" 0 2>log_selectedmodule

	rm -fr .config
	
	MODULY="encryption speech sound ext_sound dsp_sound arts_sound esd_sound nas_sound echo autoresponder sms default_sms"
	for MODUL in $MODULY; do
		if [[ `cat log_selectedmodule |grep $MODUL` != "" ]]; then
			echo "module_$MODUL=m" >> .config
		else
			echo "module_$MODUL=n" >> .config
		fi;
	done;
}

# Menu z opcjami uruchomienia ./configure
dodatkoweopcje()
{
	rm -fr log_configureoptions
	dialog --checklist "Wybierz modu�y do zainstalowania" 0 75 0 \
    		"--enable-debug" 	"W��cza tryb debug" 0 \
    		"--disable-modules"	"Wy��cza obs�ug� modu��w" 0 \
		"--disable-voice"	"Wy��cza obs�ug� rozm�w g�osowych" 0 \
		"--with-existing-libgadu" "Linkuje z istniej�cym libgadu" 0 \
    		 2>log_configureoptions
}

# Szuka mo�liwych przyczyn nieudanej kompilacji (jeszcze zanim wszystko zaczniemy)
szukajproblemow()
{
	# Sprawd�, czy na pewno masz roota:
	if [ $UID != "0" ]; then
		dialog --stdout --defaultno --backtitle "Instalator Kadu" --title "Brak uprawnie� administratora" --yesno "Nie pracujesz jako root\nCzy chcesz kontynuowa� instalacj�?" 6 50
		if [ $? == "1" ]; then
			goodbye
		fi;
	fi;
	
	# Sprawd�, czy czy masz ustawion� zmienn� QTDIR
	if [ $QTDIR == $TAKIEJZMIENNEJNIEMA ]; then
		dialog --stdout --defaultno --backtitle "Instalator Kadu" --title "Brak ustawionej zmiennej \$QTDIR" --yesno "Zmienna \$QTDIR nie jest ustawiona. Mo�esz mie� problemy z kompilacj�\nCzy chcesz kontynuowa� instalacj�?" 6 75
		if [ $? == "1" ]; then
			goodbye
		fi;
	fi;
}

hello()
{
	dialog --backtitle "Instalator Kadu" --ok-label "Kontynuuj" --msgbox "Witaj w programie instalacyjnym Kadu! \
	\nInstalator pomo�e Ci zainstalowa� lub uaktualni� Kadu" 6 75
		
}

pathmenu()
{
	PREFIXPATH=`dialog --stdout --backtitle "Instalator Kadu" --inputbox  "Podaj �cie�k� instalacji Kadu" 7 50 "/usr/local/"`
	#grunt to dobre escape'owanie backslashy :P
	echo -e -n "--prefix=\\\"$PREFIXPATH\\\"" > log_prefix
}

configmenu()
{
	CONFIGMENUCHOICE=`dialog --stdout --backtitle "Instalator Kadu" --menu "Menu konfiguracji" 10 75 4 \
	Modu�y		"Wybierz modu�y do instalacji" \
	Kompilacja	"Dodatkowe opcje kompilacji" \
	�cie�ka		"Ustaw docelow� �cie�k� instalacji" \
	Wr��		"Powr�t do menu g��wnego"`
	
	if [ $CONFIGMENUCHOICE == "Modu�y" ]; then
		wybierzmoduly
	elif [ $CONFIGMENUCHOICE == "Kompilacja" ]; then
		dodatkoweopcje
	elif [ $CONFIGMENUCHOICE == "�cie�ka" ]; then
		pathmenu	
	elif [ $CONFIGMENUCHOICE == "Wr��" ]; then
		mainmenu
	fi;
	
}



mainmenu(){
	MENUCHOICE=`dialog --stdout --no-cancel --backtitle "Instalator Kadu" --menu "Menu g��wne" 12 65 6 \
	Pomoc		"Przeczytaj pomoc dotycz�c� instalacji" \
	Aktualizacja	"�ci�gnij najnowsz� wersj� CVS" \
	Konfiguracja	"Ustaw parametry instalacyjne Kadu" \
	Instalacja	"Rozpocznij standardow� instalacj�" \
	Deinstalacja	"Odinstaluj Kadu" \
	Koniec		"Zako�cz instalacj�"`
	
	if [ $MENUCHOICE == "Instalacja" ]; then
		zainstaluj
	elif [ $MENUCHOICE == "Aktualizacja" ]; then
		dialog --backtitle "Instalator Kadu" --infobox "Gdy zostaniesz poproszony o has�o, naci�nij <ENTER>" 0 0
		cd ..
		cvs -d:pserver:anonymous@kadu.net:/usr/src/kadu login
		cvs -z3 -d:pserver:anonymous@kadu.net:/usr/src/kadu co kadu	
		cd kadu
		dialog --beep --backtitle "Instalator Kadu" --msgbox "�r�d�a Kadu zosta�y zaktualizowane!" 0 0
	elif [ $MENUCHOICE == "Konfiguracja" ]; then
		configmenu
	elif [ $MENUCHOICE == "Koniec" ]; then
		goodbye
	elif [ $MENUCHOICE == "Deinstalacja" ]; then
		odinstaluj
	elif [ $MENUCHOICE == "Pomoc" ]; then 
		pomoc
	fi;
	

}

pomoc()
{
	dialog --backtitle "Instalator Kadu" --ok-label "Zrozumia�em" --msgbox  " \
	Szczeg�owe informacje dotycz�ce wymaga� programowych i sprz�towych \n \
	znajduj� si� na stronie http://kadu.net/\n\n \
	W wi�kszo�ci przypadk�w nie jest konieczne dodatkowe konfigurowanie \n \
	parametr�w instalacyjnych Kadu. Warto jednak zajrze� do sekcji \n \
	\"Modu�y\" i wybra� te, kt�re uwa�asz za potrzebne.\n \
	W okienku listy wyboru modu�u klikni�cie na przycisku\n \
	\"Anuluj\" powoduje rezygnacj� z instalowania *wszystkich* modu��w. \n\n \
	Po wybraniu opcji \"Instaluj\" rozpoczyna si� proces kompilacji\n \
	i instalacji Kadu. Je�li proces instalacji si� zatrzyma z \n \
	komunikatem o b��dzie, przeczytaj plik \"log_install\" I sprawd� \n \
	w dokumentacji czy podobny problem nie zosta� opisany.\n \
	Je�li nie znajdziesz wskaz�wek, to przeszukaj \n \
	forum Kadu (http://kadu.net/forum) w poszukiwaniu odpowiedzi. \
	" 20 75
}

goodbye()
{
	dialog --backtitle "Instalator Kadu" --title "Koniec" --infobox "Pozdrowienia od\n   Kadu Team!" 0 0
	exit
}

gotowe()
{
	dialog --backtitle "Instalator Kadu" --title "Instalacja zako�czona" --infobox "Instalacja zako�czona. Je�li Kadu si� nie zainstalowa�o, to przejrzyj plik \"log_install\"" 4 50
	exit
}

# URUCHOM PROGRAM
hello
szukajproblemow
while [ 1 ]; do
	mainmenu
done;


