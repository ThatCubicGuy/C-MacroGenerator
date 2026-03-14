#include <stdio.h>
#include <string.h>

int int_Parse(const char* str)
{
    int n = 0;
    for (int i = 0; '0' <= str[i] && str[i] <= '9'; ++i) {
        n = n * 10 + (str[i] - '0');
    }
    return n;
}

int dec(FILE* fout, int count)
{
    printf("Generating decrement macro of up to %d uses.\n", count);
    fprintf(fout, "#define DEC_0 0\n");
    for (int i = 1; i <= count; ++i) {
        fprintf(fout, "#define DEC_%d %d\n", i, i - 1);
    }
    return 0;
}

int inc(FILE* fout, int count)
{
    printf("Generating increment macro of up to %d uses.\n", count);
    for (int i = 0; i < count; ++i) {
        fprintf(fout, "#define INC_%d %d\n", i, i + 1);
    }
    fprintf(fout, "#define INC_%d %d\n", count, count);
    return 0;
}

int cat(FILE* fout, int count)
{
    char argstring[1024] = "_1";
    char catstring[1024] = "_1";
    char macrostring[1024] = "";
    printf("Generating concat macro of up to %d uses.\n", count);
    for (int i = 2; i <= count; ++i) {
        sprintf(argstring + strlen(argstring), ", _%d", i);
        sprintf(catstring + strlen(catstring), "##_%d", i);
        sprintf(macrostring + strlen(macrostring), ", __CAT%d", count - i + 2);
        fprintf(fout, "#define __CAT%d_I(%s) %s\n#define __CAT%d(%s) __CAT%d_I(%s)\n",
            i, argstring, catstring, i, argstring, i, argstring);
    }
    fprintf(fout, "\n#define __CAT_SELECT(_0, %s, NAME, ...) NAME\n\n", argstring);
    fprintf(fout, "// Concatenates the values of up to %d symbols.\n", count);
    fprintf(fout, "#define CAT(...) __CAT_SELECT(,##__VA_ARGS__%s, IDENTITY, EMPTY)(__VA_ARGS__)\n", macrostring);
    for (int i = 0; i < count; ++i);
    return 0;
}

int select(FILE* fout, int count)
{
    printf("Generating select macro of up to %d uses.\n", count);
    char argstring[1024] = "id";
    for (int i = 0; i < count; ++i) {
        sprintf(argstring + strlen(argstring), ", _%d", i);
        fprintf(fout, "#define __SELECT_%d(%s) id\n", i, argstring);
    }
    fprintf(fout, "// Selects the item at a zero-based index"
            " in the variadic argument list. Up to %d parameters.\n", count);
    fprintf(fout, "#define SELECT(INDEX, ...) CAT(__SELECT_, INDEX)(__VA_ARGS__)\n");
    return 0;
}

int foreach(FILE* fout, int count)
{
    printf("Generating foreach macro of up to %d uses.\n", count);
    fprintf(fout, "#define __FOREACH_1(id, _0, ...) DEFER(id)(DEPAREN(_0))\n");
    for (int i = 2; i <= count; ++i) {
        fprintf(fout, "#define __FOREACH_%d(id, _0, ...) DEFER(id)(DEPAREN(_0)) __FOREACH_%d(id, __VA_ARGS__)\n", i, i - 1);
    }
    fprintf(fout, "// Execute a macro for each of the parameters in a collection, up to %d times.\n", count);
    fprintf(fout, "#define FOREACH(id, ...) __VA_OPT__(EVAL(DEFER(CAT(__FOREACH_, NARGS(__VA_ARGS__)))(id, __VA_ARGS__)))\n");
    return 0;
}

int foreach_args(FILE* fout, int count)
{
    printf("Generating foreach with base arguments macro of up to %d uses.\n", count);
    fprintf(fout, "#define __FOREACH_ARGS_1(id, BASEARGS, _0, ...) DEFER(id)(DEPAREN(BASEARGS), DEPAREN(_0))\n");
    for (int i = 2; i <= count; ++i) {
        fprintf(fout, "#define __FOREACH_ARGS_%d(id, BASEARGS, _0, ...) "
                "DEFER(id)(DEPAREN(BASEARGS), DEPAREN(_0)) __FOREACH_ARGS_%d(id, BASEARGS, __VA_ARGS__)\n", i, i - 1);
    }
    fprintf(fout, "// Execute a macro for each of the parameters in a collection, up to %d times. Allows extra parameters in front of the list parameter.\n", count);
    fprintf(fout, "#define FOREACH_WITH_ARGS(id, BASEARGS, ...) __VA_OPT__(EVAL(DEFER(CAT(__FOREACH_ARGS_, NARGS(__VA_ARGS__)))(id, BASEARGS, __VA_ARGS__)))\n");
    return 0;
}

int main(int argc, const char** argv)
{
    FILE* thing = fopen("macros.h", "wt");
    int count = 128;
    if (argc > 1) {
        count = int_Parse(argv[1]);
        if (argc > 2) {
            if (!strcmp(argv[2], "dec")) return dec(thing, count);
            if (!strcmp(argv[2], "inc")) return inc(thing, count);
            if (!strcmp(argv[2], "cat")) return cat(thing, count);
            if (!strcmp(argv[2], "select")) return select(thing, count);
            if (!strcmp(argv[2], "foreach")) return foreach(thing, count);
            if (!strcmp(argv[2], "foreach_args") ||
                !strcmp(argv[2], "foreachargs")) return foreach_args(thing, count);
            fprintf(stderr, "Unknown macro: `%s`.\n", argv[2]);
            if (strstr(argv[2], "foreach")) fprintf(stderr, "Did you mean `foreach_args`?\n");
            return 404;
        }
        return dec(thing, count);
    }
    return 0;
}

int cat_alphabetic(FILE* fout, int count)
{
    char argstring[1024] = "a";
    char catstring[1024] = "a";
    printf("Generating concat macro of up to %d uses.\n", count);
    for (int i = 2; i <= count; ++i) {
        sprintf(argstring + strlen(argstring), ", %c", 'a' + i - 1);
        sprintf(catstring + strlen(catstring), "##%c", 'a' + i - 1);
        fprintf(fout, "#define __CAT%d_I(%s) %s\n#define __CAT%d(%s) __CAT%d_I(%s)\n",
            i, argstring, catstring, i, argstring, i, argstring);
    }
    return 0;
}