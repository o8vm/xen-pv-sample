static void sputchar(int);
static void doprint(void (*)(int), const char*, long*);
static void printn(void (*)(int), unsigned long, int, int, int);

// Local variable

static char* strptr;			/* string buffer pointer	*/

int sprintf(char* buf, const char* fmt, ...) {
  long* argptr = (long *) (&fmt + 1);	
  strptr = buf;				
  doprint(sputchar, fmt, argptr);
  *strptr = '\0';
  return 0;
}

static
void sputchar(int ch) {
  *strptr++ = (char) ch;
}

static
void doprint(void (*put)(int), const char* fmt, long* argptr) {
  char* p;
  char ch;
  unsigned long val;
  int digits;
  int zsup;

  for (;;) {
    while ((ch = *fmt++) != '%') {	/* Search conversion specifier	*/
      if (ch == '\0') return;
      (*put)((int) ch);
    }

    zsup = 1;				/* Default is zero suppless mode*/
    if (*fmt == '0') {
      zsup = 0; fmt++;			/* Zero(es) will be suppled	*/
    }

    digits = 0;				/* Analyze width specifier	*/
    while (*fmt >= '0' && *fmt <= '9') {
      digits *= 10;
      digits += *fmt++ - '0';
    }

    switch (ch = *fmt++) {		/* Analyze specifier		*/
      case 'c':				/* " Character "		*/
      case 'C':
	ch = (char) *argptr++;
	(*put)((int) ch);
	break;

      case 's':				/* " String "			*/
      case 'S':
	p = (char*) *argptr++;
	while ((ch = *p++))
	  (*put)((int) ch);
	break;

      case 'b':				/* " Binary "			*/
      case 'B':
        val = (unsigned long) *argptr++;
	printn(put, val, 2, digits, zsup);
	break;

      case 'd':				/* " Decimal "			*/
      case 'D':
	val = (unsigned long) *argptr++;
	printn(put, val, 10, digits, zsup);
	break;

      case 'o':				/* " Octal "			*/
      case 'O':
	val = (unsigned long) *argptr++;
	printn(put, val, 8, digits, zsup);

      case 'x':				/* " Hexadecimal "		*/
      case 'X':
	val = (unsigned long) *argptr++;
	printn(put, val, 16, digits, zsup);
	break;

      default:
	(*put)((int) '%'); (*put)((int) ch);
	break;
    }
  }
}

static void
printn(void (*put)(int), unsigned long val, int base, int digits, int zsup) {
  char  buf[ 33 ];
  char* p;
  int cnt;

  if (digits > 32)
    digits = 32;
  else if (digits <= 0)
    digits = -1;

  p = buf;
  cnt = 0;
  do {
    if (cnt == 33 || cnt == digits) return;
    *p++ = "0123456789ABCDEF"[val % base];
    cnt++;
  } while (val /= base);

  while (cnt < digits) {
    if (zsup)
      (*put)((int) ' ');
    else
      (*put)((int) '0');
    cnt++;
  }

  do {
    (*put)((int) (*--p));
  } while (p > buf);
}
