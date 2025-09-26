#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Prototypes from convert.c */
void oct_to_bin(const char *oct, char *out);
void oct_to_hex(const char *oct, char *out);
void hex_to_bin(const char *hex, char *out);
void to_sign_magnitude(int32_t n, char *out32);
void to_ones_complement(int32_t n, char *out32);
void to_twos_complement(int32_t n, char *out32);


#define LINE_CAP  1024
#define OUT_CAP   2048

static int run_test_line(const char *func, const char *arg1, const char *expected)
{
    char out[OUT_CAP];//buffer for output

    if (strcmp(func, "oct_to_bin") == 0) {
        oct_to_bin(arg1, out);//calls oct to bin function
    } else if (strcmp(func, "oct_to_hex") == 0) {
        oct_to_hex(arg1, out);//calls oct to hex function
    } else if (strcmp(func, "hex_to_bin") == 0) {
        hex_to_bin(arg1, out);//calls hex to bin function
    } else if (strcmp(func, "to_sign_magnitude") == 0) {
        int32_t n = (int32_t)strtol(arg1, NULL, 10);
        to_sign_magnitude(n, out);//calls sign magnitude function
    } else if (strcmp(func, "to_ones_complement") == 0) {
        int32_t n = (int32_t)strtol(arg1, NULL, 10);
        to_ones_complement(n, out);//calls one's complement function
    } else if (strcmp(func, "to_twos_complement") == 0) {
        int32_t n = (int32_t)strtol(arg1, NULL, 10);//calls two's complement function
        to_twos_complement(n, out);//calls two's complement function
    } else {
        printf("Unknown function: %s\n", func);//handles unknown function
        return 0;
    }

    int pass = (strcmp(out, expected) == 0);//checks if output matches expected
    printf("%s %s -> Expected: \"%s\", Got: \"%s\" [%s]\n",
           func, arg1, expected, out, pass ? "PASS" : "FAIL");
    return pass;
}

int main(int argc, char **argv)
{
    const char *path = (argc >= 2) ? argv[1] : "a2_test.txt";//default test file
    FILE *f = fopen(path, "r");//opens test file
    if (!f) {
        fprintf(stderr, "Failed to open test file: %s\n", path);//error handling
        return 1;
    }

    char line[LINE_CAP];//buffer for reading lines
    int total = 0, passed = 0, skipped = 0;

    while (fgets(line, sizeof(line), f)) {
        /* Trim leading spaces */
        char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        /* Skip comments and blank lines */
        if (*p == '#' || *p == '\n' || *p == '\0') {
            ++skipped;
            continue;
        }

        /* Tokenize: function_name input expected */
        char func[128], arg1[256], expected[OUT_CAP];
        func[0] = arg1[0] = expected[0] = '\0';

        /* We read the first three whitespace-delimited tokens.
           The expected output in the provided tests contains no spaces, so this is safe. */
        int n = sscanf(p, "%127s %255s %2047s", func, arg1, expected);
        if (n < 3) {
            fprintf(stderr, "Malformed line (need 3 tokens): %s", line);
            continue;
        }

        ++total;
        if (run_test_line(func, arg1, expected)) ++passed;
    }

    fclose(f);
    printf("Summary: %d/%d tests passed (%d skipped/comment lines)\n", passed, total, skipped);
    return (passed == total) ? 0 : 2;
}
