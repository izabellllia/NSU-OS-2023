#include <stdio.h>

int main() {
    char buff[BUFSIZ];
    FILE *in = popen("echo Some text", "r");
    if (in == NULL) {
        perror("problem with popen");
        return -1;
    }
    FILE *out = popen("tr [:lower:] [:upper:]", "w");
    if (out == NULL) {
        perror("problem with popen");
        pclose(in);
        return -1;
    }
    while (fgets(buff, BUFSIZ, in) != NULL) {
        if (fputs(buff, out) == EOF) {
            perror("problem with fputs");
            pclose(in);
            pclose(out);
            return -1;
        }
    }
    pclose(in);
    pclose(out);
    return 0;
}
