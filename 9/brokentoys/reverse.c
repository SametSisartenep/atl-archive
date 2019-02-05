#include <u.h>
#include <libc.h>

char *
reverse(char *s, int n)
{
	int i;
	char tmp;

	for(i = 0; i < n/2; i++){
		tmp = s[i];
		s[i] = s[n-i-1];
		s[n-i-1] = tmp;
	}
	return s;
}

char *
reversep(char *s, int n)
{
	char *bp, *ep;
	char tmp;

	for(bp = s, ep = s+n-1; bp != ep; bp++, ep--){
		tmp = *bp;
		*bp = *ep;
		*ep = tmp;
	}
	return s;
}

void
main()
{
	char s[] = "hello";

	print("%s → ", s);
	print("%s\n", reverse(s, strlen(s)));
	print("%s → ", s);
	print("%s\n", reversep(s, strlen(s)));
	exits(0);
}
