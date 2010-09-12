#include <libale.h>

int main() {
    char * thestring, *upstring, *lowstring, *stripstring,*stripallstring;
    thestring = fsafegets(stdin,10);
    upstring = uppercase(thestring);
    lowstring = lowercase(thestring);
    stripstring = strip(thestring);
    stripallstring = stripall(thestring,'e');
    printf("the string: %s\n",thestring);
    printf("uppercase string: %s\n",upstring);
    printf("lowercase string: %s\n",lowstring);
    printf("stripped string: %s.\n",stripstring);
    printf("stripped of all es: %s.\n",stripallstring);
}
