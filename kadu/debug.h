#ifndef DEBUG_H
#define DEBUG_H

#include "../config.h"

/*
	<<< kdebug >>>
	Wy�wietla komunikat debuguj�cy na konsoli.
	Sk�adnia jak w printf.
*/
#ifdef DEBUG_ENABLED
#define kdebug(format,args...) \
	_kdebug(__FILE__,__LINE__,format,##args)
#else
#define kdebug(format,args...)
#endif
	
/*
	<<< kdebugf >>>
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
#ifdef DEBUG_ENABLED
void _kdebug(const char* file,const int line,const char* format,...);
#endif

#endif
