#ifndef DEBUG_H
#define DEBUG_H

/*
	Wy�wietla komunikat debuguj�cy na konsoli.
	Sk�adnia jak w printf.
*/
#define kdebug(format,...) \
	_kdebug(__FILE__,__LINE__,format,##__VA_ARGS__)	
	
/*
	Wy�wietla komunikat debuguj�cy zawieraj�cy
	nazw� aktualnie wykonywanej funkcji.
	Z za�o�enia makro to powinno by� wywo�ane
	w pierwszej linii ka�dej funkcji. Dzi�ki
	temu mo�na b�dzie w przysz�o�ci �ledzi�
	dzia�anie programu.
*/	
#define kdebugf() \
	kdebug("%s\n",__PRETTY_FUNCTION__)

/*
	Funkcja pomocnicza. Nie u�ywa�.
*/
void _kdebug(const char* file,const int line,const char* format,...);

#endif
