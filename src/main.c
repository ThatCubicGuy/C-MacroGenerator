#//vim:fdm=marker
#include <stdio.h>
#include <string.h>

#define MAXSTRLEN 2048

int int_Parse(const char* str)
{
    int n = 0, f = 1;
    if (str[0] == '-') {
        str += 1;
        f = -1;
    }
    for (int i = 0; '0' <= str[i] && str[i] <= '9'; ++i) {
        n = n * 10 + (str[i] - '0');
    }
    return n * f;
}

#pragma region Dec {{{
int dec(FILE* fout, int count)
{
    printf("Generating decrement macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT\n", count);
    fprintf(fout, "#pragma region Dec {{{\n\n#define qDEC_0 0\n");
    for (int i = 1; i <= count; ++i) {
        fprintf(fout, "#define qDEC_%d %d\n", i, i - 1);
    }
    fprintf(fout, "\n// Decrements the value of the given number,"
            " from %d down to 0.\n#define DEC(x) CAT(qDEC_,x)\n", count);
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Inc {{{
int inc(FILE* fout, int count)
{
    printf("Generating increment macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT\n", count);
    fprintf(fout, "#pragma region Inc {{{\n\n");
    for (int i = 0; i < count; ++i) {
        fprintf(fout, "#define qINC_%d %d\n", i, i + 1);
    }
    fprintf(fout, "#define qINC_%d %d\n", count, count);
    fprintf(fout, "\n// Increments the value of the given number,"
            " from 0 up to %d.\n#define INC(x) CAT(qINC_,x)\n", count);
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Cat {{{
int cat(FILE* fout, int count)
{
    char argstring[MAXSTRLEN] = "_1";
    char catstring[MAXSTRLEN] = "_1";
    char macrostring[MAXSTRLEN] = "";
    printf("Generating concat macro of up to %d uses\n\033[33mDEPENDENCIES\033[35m*\033[0m: EMPTY, IDENTITY\n", count);
    fprintf(fout, "#pragma region Cat {{{\n\n");
    for (int i = 2; i <= count; ++i) {
        sprintf(argstring + strlen(argstring), ", _%d", i);
        sprintf(catstring + strlen(catstring), "##_%d", i);
        sprintf(macrostring + strlen(macrostring), ", qCAT_%d", count - i + 2);
        fprintf(fout, "#define qCAT_%d_(%s) %s\n#define qCAT_%d(%s) qCAT_%d_(%s)\n",
            i, argstring, catstring, i, argstring, i, argstring);
    }
    fprintf(fout, "\n#define qCAT_SELECT(_0, %s, NAME, ...) NAME\n\n", argstring);
    fprintf(fout, "// Concatenates the values of up to %d symbols.\n", count);
    fprintf(fout, "#define CAT(...) qCAT_SELECT(,##__VA_ARGS__%s, IDENTITY, EMPTY)(__VA_ARGS__)\n", macrostring);
    for (int i = 0; i < count; ++i);
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Select {{{
int select(FILE* fout, int count)
{
    printf("Generating select macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT\n", count);
    fprintf(fout, "#pragma region Select {{{\n\n");
    char argstring[MAXSTRLEN] = "";
    for (int i = 0; i < count; ++i) {
        fprintf(fout, "#define qSELECT_%d(%sid, ...) id\n", i, argstring);
        sprintf(argstring + strlen(argstring), "_%d, ", i);
    }
    fprintf(fout, "// Selects the item at a zero-based index"
            " in the variadic argument list. Up to %d parameters.\n", count);
    fprintf(fout, "#define SELECT(INDEX, ...) CAT(qSELECT_, INDEX)(__VA_ARGS__)\n");
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Foreach {{{
int foreach(FILE* fout, int count)
{
    printf("Generating foreach macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT, EXPAND, NARGS\n", count);
    fprintf(fout, "#pragma region Foreach {{{\n\n");
    fprintf(fout, "#define qFOREACH_1(id, _0, ...) EXPAND(DEFER(id)(DEPAREN(_0)))\n");
    for (int i = 2; i <= count; ++i) {
        fprintf(fout, "#define qFOREACH_%d(id, _0, ...) EXPAND(DEFER(id)(DEPAREN(_0))) qFOREACH_%d(id, __VA_ARGS__)\n", i, i - 1);
    }
    fprintf(fout, "// Execute a macro for each of the parameters in a collection, up to %d times.\n", count);
    fprintf(fout, "#define FOREACH(id, ...) __VA_OPT__(CAT(qFOREACH_, NARGS(__VA_ARGS__))(id, __VA_ARGS__))\n");
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Foreach2 {{{
int foreach2(FILE* fout, int count)
{
    printf("Generating foreach2 macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT, NARGS, UNWRAP\n", count);
    fprintf(fout, "#pragma region Foreach2 {{{\n\n");
    fprintf(fout, "#define qFOREACH2_1(id, _0, ...) UNWRAP(DEFER(id)(DEPAREN(_0)))\n");
    for (int i = 2; i <= count; ++i) {
        fprintf(fout, "#define qFOREACH2_%d(id, _0, ...) UNWRAP(DEFER(id)(DEPAREN(_0))) qFOREACH2_%d(id, __VA_ARGS__)\n", i, i - 1);
    }
    fprintf(fout, "// Foreach for stacking with a subsequent foreach. Up to %d items.\n", count);
    fprintf(fout, "#define FOREACH2(id, ...) __VA_OPT__(CAT(qFOREACH2_, NARGS(__VA_ARGS__))(id, __VA_ARGS__))\n");
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Foreach_Args {{{
int foreach_args(FILE* fout, int count)
{
    printf("Generating foreach with base arguments macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT, DEPAREN, NARGS\n", count);
    fprintf(fout, "#pragma region Foreach_Args {{{\n\n");
    fprintf(fout, "#define qFOREACH_ARGS_1(id, BASEARGS, _0, ...) id(EXPAND(DEPAREN(BASEARGS), DEPAREN(_0)))\n");
    for (int i = 2; i <= count; ++i) {
        fprintf(fout, "#define qFOREACH_ARGS_%d(id, BASEARGS, _0, ...) "
                "id(EXPAND(DEPAREN(BASEARGS), DEPAREN(_0))) qFOREACH_ARGS_%d(id, BASEARGS, __VA_ARGS__)\n", i, i - 1);
    }
    fprintf(fout, "// Execute a macro for each of the items in a collection taking into account some common base arguments, up to %d times.\n", count);
    fprintf(fout, "#define FOREACH_ARGS(id, BASEARGS, ...) __VA_OPT__(CAT(qFOREACH_ARGS_, NARGS(__VA_ARGS__)(id, BASEARGS, __VA_ARGS__)))\n");
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Foreach_Index {{{
int foreach_index(FILE* fout, int count)
{
    printf("Generating foreach with index macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT, INC, NARGS\n", count);
    fprintf(fout, "#pragma region Foreach_Index {{{\n\n");
    fprintf(fout, "#define qFOREACH_INDEX_1(idx, id, _0, ...) id(idx, _0)\n");
    for (int i = 2; i <= count; ++i) {
        fprintf(fout, "#define qFOREACH_INDEX_%d(idx, id, _0, ...) id(idx, _0) qFOREACH_INDEX_%d(INC(idx)), id, __VA_ARGS__)\n", i, i - 1);
    }
    fprintf(fout, "// Executes a macro for each item in a collection while passing the index of the item as the first param, up to %d times.\n", count);
    fprintf(fout, "#define FOREACH_INDEX(start_index, id, ...) __VA_OPT__(CAT(qFOREACH_INDEX_, NARGS(__VA_ARGS__))(start_index, id, __VA_ARGS__))\n");
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region NArgs {{{
int nargs(FILE* fout, int count)
{
    printf("Generating number of args macro of up to %d args.\n\033[32mNO DEPENDENCIES\033[0m\n", count);
    fprintf(fout, "#pragma region NArgs {{{\n\n");
    fprintf(fout, "#define __NARGS_SELECT(");
    for (int i = count; i > 0; --i) {
        fprintf(fout, "_%d, ", i);
    }
    fprintf(fout, "N, ...) N\n");
    fprintf(fout, "// Get the number of variadic arguments. Up to 32 arguments.\n");
    fprintf(fout, "#define NARGS(...) __NARGS_SELECT(__VA_ARGS__ __VA_OPT__(,) %d", count);
    for (int i = count - 1; i >= 0; --i) {
        fprintf(fout, ", %d", i);
    }
    fprintf(fout, ")\n");
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Join {{{
int join(FILE* fout, int count)
{
    printf("Generating join macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT", count);
    fprintf(fout, "#pragma region Join {{{\n\n");
    fprintf(fout, "#define __JOIN_%d(SEPARATOR, _1, ...) CAT(SEPARATOR, _1)\n", count - 1);
    for (int i = count - 1; i > 1; --i) {
        fprintf(fout, "#define __JOIN_%d(SEPARATOR, _1, ...) "
            "CAT(CAT(SEPARATOR, _1), __VA_OPT__(__JOIN_%d(SEPARATOR, __VA_ARGS__)))\n", i - 1, i);
    }
    fprintf(fout, "// Joins the values of each argument together with the given separator, up to %d times.\n", count);
    fprintf(fout, "#define JOIN(SEPARATOR, _1, ...) CAT(_1, __VA_OPT__(__JOIN_1(SEPARATOR, __VA_ARGS__)))\n");
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Repeat {{{
int repeat(FILE* fout, int count)
{
    printf("Generating repeat macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT DEC INC\n", count);
    fprintf(fout, "#pragma region Repeat {{{\n\n");
    
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Take {{{
int take(FILE* fout, int count)
{
    printf("Generating take macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT\n", count);
    fprintf(fout, "#pragma region Take {{{\n\n");
    fprintf(fout, "#define qTAKE_0(...)\n#define qTAKE_1(id, ...) id\n");
    for (int i = 2; i <= count; ++i) {
        fprintf(fout, "#define qTAKE_%d(id, ...) id, qTAKE_%d(__VA_ARGS__)\n", i, i - 1);
    }
    fprintf(fout, "// Extracts the first COUNT items from a list. Up to %d items.\n", count);
    fprintf(fout, "#define TAKE(COUNT, ...) CAT(qTAKE_, COUNT)(__VA_ARGS__)\n");
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Skip {{{
int skip(FILE* fout, int count)
{
    printf("Generating skip macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT\n", count);
    fprintf(fout, "#pragma region Skip {{{\n\n");
    fprintf(fout, "#define qSKIP_0(...) __VA_ARGS__\n#define qSKIP_1(id, ...) __VA_ARGS__\n");
    for (int i = 2; i <= count; ++i) {
        fprintf(fout, "#define qSKIP_%d(id, ...) qSKIP_%d(__VA_ARGS__)\n", i, i - 1);
    }
    fprintf(fout, "// Skips the first COUNT items from a list. Up to %d items.\n", count);
    fprintf(fout, "#define SKIP(COUNT, ...) CAT(qSKIP_, COUNT)(__VA_ARGS__)\n");
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region First {{{
int first(FILE* fout, int count)
{
    printf("Generating first-where macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT, IF\n", count);
    fprintf(fout, "#pragma region First {{{\n\n");
    fprintf(fout, "#define qFIND_1(COND, id) IF(COND,id,)\n");
    for (int i = 2; i <= count; ++i) {
        fprintf(fout, "#define qFIND_%d(COND, id, ...) IF(COND,id,qFIND_%d(COND, __VA_ARGS__))\n", i, i - 1);
    }
    fprintf(fout, "// Finds the first item in a list that fulfills condition COND. Up to %d items.\n", count);
    fprintf(fout, "#define FIND(COND, ...) CAT(qFIND_, NARGS(__VA_ARGS__))(COND,__VA_ARGS__)\n");
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Where {{{
int where(FILE* fout, int count)
{
    printf("Generating where macro of up to %d uses\n\033[33mDEPENDENCIES\033[0m: CAT, IF, TRIMSTART\n", count);
    fprintf(fout, "#pragma region Where {{{\n\n");
    fprintf(fout, "#define qWHERE_1(COND, id) IF(COND,id,)\n");
    for (int i = 2; i <= count; ++i) {
        fprintf(fout, "#define qWHERE_%d(COND, id, ...) TRIMSTART(IF(COND,id,),) qWHERE_%d(COND, __VA_ARGS__)\n", i, i - 1);
    }
    fprintf(fout, "// Finds every item in a list that fulfills condition COND. Up to %d items.\n", count);
    fprintf(fout, "#define WHERE(COND, ...) CAT(qWHERE_, NARGS(__VA_ARGS__))(COND,__VA_ARGS__)\n");
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Eval {{{
int eval(FILE* fout, int count)
{
    const int POWER = 10;
    int realcount = 0;
    for (int i = 0; i < 32; ++i) {
        if ((count >> i) & 1) realcount = i;
    }
    printf("Generating eval macro of up to 10^%d uses\n\033[32mNO DEPENDENCIES\033[0m\n", realcount);
    fprintf(fout, "#pragma region Eval {{{\n\n");
    fprintf(fout, "#define E1(x) x\n");
    for (int i = 1; i <= realcount; ++i) {
        if (i == realcount) fprintf(fout, "// Force a macro to evaluate itself %d^%d times.\n#define EVAL(x) ", POWER, realcount);
        else fprintf(fout, "#define E%d(x) ", i + 1);
        for (int j = 0; j < POWER; ++j) {
            fprintf(fout, "E%d(", i);
        }
        fprintf(fout, "x");
        for (int j = 0; j < POWER; ++j) {
            fprintf(fout, ")");
        }
        fprintf(fout, "\n");
    }
    fprintf(fout, "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Not Countable

#pragma region Trim {{{
int trim(FILE* fout)
{
    printf("Generating trim macro.\n\033[33mDEPENDENCIES\033[0m: DEC, TAKE, NARGS, SELECT\n");
    fprintf(fout, "#pragma region Trim {{{\n\n"
            "#define qAPPEND_COMMA(...) __VA_OPT__(__VA_ARGS__,)\n"
            "#define qPREPEND_COMMA(...) __VA_OPT__(,__VA_ARGS__)\n"
            "#define TRIMEND(...) TAKE(DEC(NARGS(__VA_ARGS__)),__VA_ARGS__) qPREPEND_COMMA(SELECT(DEC(NARGS(__VA_ARGS__)),__VA_ARGS__))\n"
            "#define TRIMSTART(x, ...) qAPPEND_COMMA(x) __VA_ARGS__\n"
            "// Trim a list of leading / trailing commas.\n"
            "#define TRIM(...) TRIMEND(TRIMSTART(__VA_ARGS__))\n"
            "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Deparen {{{
int deparen(FILE* fout)
{
    printf("Generating deparen macro.\n");
    fprintf(fout, "#pragma region Deparen {{{\n"
            "// Source - https://stackoverflow.com/a/62984543\n"
            "// Posted by Nero\n"
            "// Retrieved 2026-03-10, License - CC BY-SA 4.0\n\n"
            "#define __VANISH\n"
            "#define __ESC_I(...) __VAN##__VA_ARGS__\n"
            "#define __ESC(...) __ESC_I(__VA_ARGS__)\n"
            "#define ISH(...) ISH __VA_ARGS__\n"
            "// Removes one set of parantheses from the given argument.\n"
            "#define DEPAREN(x) __ESC(ISH x)\n"
            "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region Defer {{{
int defer(FILE* fout)
{
    printf("Generating macros for deferrence (empty/defer/obstruct, identity, expand/unwrap, void)\n\033[32mNO DEPENDENCIES\033[0m\n");
    fprintf(fout, "#pragma region Utils {{{\n\n"
            "// 1 step\n"
            "#define EMPTY()\n"
            "// 2 steps\n"
            "#define DEFER(id) id EMPTY()\n"
            "// 4 steps\n"
            "#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()\n\n"
            "// EMPTY() ->\n\n"
            "// DEFER(x) -> x EMPTY()\n"
            "// x EMPTY() -> x\n\n"
            "// OBSTRUCT(x) -> x DEFER(EMPTY)()\n"
            "// x DEFER(EMPTY)() -> x EMPTY EMPTY() ()\n"
            "// x EMPTY EMPTY() () -> x EMPTY ()\n"
            "// x EMPTY () -> x\n\n"
            "#define IDENTITY(x) x\n\n"
            "#define EXPAND(...) __VA_ARGS__\n"
            "#define UNWRAP(...) __VA_ARGS__\n\n"
            "#define VOID(...)\n"
            "\n#pragma endregion }}}\n");
    return 0;
}
#pragma endregion }}}

#pragma region If {{{
int if_macro(FILE* fout)
{
    printf("Generating if (conditional) macro.\n\033[33mDEPENDENCIES\033[0m: CAT, SELECT\n");
    fprintf(fout, "#pragma region If {{{\n\n"
            "#define NOT_0 ~,\n"
            "#define NOT(x) SELECT(2,CAT(NOT_,x),1,0)\n"
            "#define COMPL_0 1\n"
            "#define COMPL_1 0\n"
            "#define COMPL(x) CAT(COMPL_,x)\n"
            "#define BOOL(x) COMPL(NOT(x))\n\n"
            "#define IF_0(t,f) f\n"
            "#define IF_1(t,f) t\n"
            "#define IF(EXPR,IF,ELSE) CAT(IF_,BOOL(EXPR))(IF,ELSE)\n"
            "\n#pragma endregion }}\n");
    return 0;
}
#pragma endregion }}}

#pragma endregion
#define MACRO_DEC (1ul << 0)
#define MACRO_INC (1ul << 1)
#define MACRO_CAT (1ul << 2)
#define MACRO_SELECT (1ul << 3)
#define MACRO_FOREACH (1ul << 4)
#define MACRO_FOREACH2 (1ul << 5)
#define MACRO_FOREACH_ARGS (1ul << 6)
#define MACRO_FOREACH_INDEX (1ul << 7)
#define MACRO_NARGS (1ul << 8)
#define MACRO_JOIN (1ul << 9)
#define MACRO_REPEAT (1ul << 10)
#define MACRO_TAKE (1ul << 11)
#define MACRO_SKIP (1ul << 12)
#define MACRO_FIRST (1ul << 13)
#define MACRO_WHERE (1ul << 14)
#define MACRO_EVAL (1ul << 15)
#define MACRO_TRIM (1ul << 16)
#define MACRO_DEPAREN (1ul << 17)
#define MACRO_DEFER (1ul << 18)
#define MACRO_IF_MACRO (1ul << 19)
#define MACROS_ALL (MACRO_DEC | MACRO_INC | MACRO_CAT | MACRO_SELECT | MACRO_FOREACH | MACRO_FOREACH2 | MACRO_FOREACH_ARGS | MACRO_FOREACH_INDEX | MACRO_NARGS | MACRO_JOIN | MACRO_REPEAT | MACRO_TAKE | MACRO_SKIP | MACRO_FIRST | MACRO_WHERE | MACRO_EVAL | MACRO_TRIM | MACRO_DEPAREN | MACRO_DEFER | MACRO_IF_MACRO)
int print_macros(FILE* f, int count, size_t macros)
{
    fprintf(f, "#ifndef CUBE_MACROS\n#define CUBE_MACROS\n");
    int macros_generated = 0;
    fprintf(f, "\n");
    if (macros & MACRO_DEC) macros_generated += !dec(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_INC) macros_generated += !inc(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_CAT) macros_generated += !cat(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_SELECT) macros_generated += !select(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_FOREACH) macros_generated += !foreach(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_FOREACH2) macros_generated += !foreach2(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_FOREACH_ARGS) macros_generated += !foreach_args(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_FOREACH_INDEX) macros_generated += !foreach_index(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_NARGS) macros_generated += !nargs(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_JOIN) macros_generated += !join(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_REPEAT) macros_generated += !repeat(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_TAKE) macros_generated += !take(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_SKIP) macros_generated += !skip(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_FIRST) macros_generated += !first(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_WHERE) macros_generated += !where(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_EVAL) macros_generated += !eval(f, count);
    fprintf(f, "\n");
    if (macros & MACRO_TRIM) macros_generated += !trim(f);
    fprintf(f, "\n");
    if (macros & MACRO_DEPAREN) macros_generated += !deparen(f);
    fprintf(f, "\n");
    if (macros & MACRO_DEFER) macros_generated += !defer(f);
    fprintf(f, "\n");
    if (macros & MACRO_IF_MACRO) macros_generated += !if_macro(f);
    fprintf(f, "\n#endif\n");
    return macros_generated;
}

int main(int argc, const char** argv)
{
    if (argc > 1) {
        if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "help") || !strcmp(argv[1], "--help")) {
            printf("Usage: gen-macros MACRO...\n  or:  gen-macros COUNT MACRO...\n");
            printf("Generate MACRO for COUNT uses. Default is 128.\n");
            printf("If given only COUNT, generates every macro for that COUNT.\n");
            return 0;
        }
        int count = 128;
        if ('0' <= argv[1][0] && argv[1][0] <= '9') {
            count = int_Parse(argv[1]);
            if (count <= 0) {
                fprintf(stderr, "Please specify a strictly positive number.");
                return 1;
            }
        } else ++argc, --argv;
        FILE* f = fopen("macros.h", "wt");
        size_t macros_to_generate = 0;
        if (argc > 2) {
            if (argv[2][0] == '*') {
                macros_to_generate = MACROS_ALL;
            } else for (int i = 2; i < argc; ++i) {
                fprintf(f, "\n");
                if (!strcmp(argv[i], "dec")) macros_to_generate |= MACRO_DEC; else
                if (!strcmp(argv[i], "inc")) macros_to_generate |= MACRO_INC; else
                if (!strcmp(argv[i], "cat")) macros_to_generate |= MACRO_CAT; else
                if (!strcmp(argv[i], "select")) macros_to_generate |= MACRO_SELECT; else
                if (!strcmp(argv[i], "foreach")) macros_to_generate |= MACRO_FOREACH; else
                if (!strcmp(argv[i], "foreach2")) macros_to_generate |= MACRO_FOREACH2; else
                if (!strcmp(argv[i], "foreach_args") ||
                    !strcmp(argv[i], "foreachargs")) macros_to_generate |= MACRO_FOREACH_ARGS; else
                if (!strcmp(argv[i], "foreach_index") ||
                    !strcmp(argv[i], "foreachindex")) macros_to_generate |= MACRO_FOREACH_INDEX; else
                if (!strcmp(argv[i], "nargs")) macros_to_generate |= MACRO_NARGS; else
                if (!strcmp(argv[i], "join")) macros_to_generate |= MACRO_JOIN; else
                if (!strcmp(argv[i], "repeat")) macros_to_generate |= MACRO_REPEAT; else
                if (!strcmp(argv[i], "take")) macros_to_generate |= MACRO_TAKE; else
                if (!strcmp(argv[i], "skip")) macros_to_generate |= MACRO_SKIP; else
                if (!strcmp(argv[i], "first")) macros_to_generate |= MACRO_FIRST; else
                if (!strcmp(argv[i], "where")) macros_to_generate |= MACRO_WHERE; else
                if (!strcmp(argv[i], "eval")) macros_to_generate |= MACRO_EVAL; else
                if (!strcmp(argv[i], "trim")) macros_to_generate |= MACRO_TRIM; else
                if (!strcmp(argv[i], "deparen")) macros_to_generate |= MACRO_DEPAREN; else
                if (!strcmp(argv[i], "defer")) macros_to_generate |= MACRO_DEFER; else
                if (!strcmp(argv[i], "if_macro")) macros_to_generate |= MACRO_IF_MACRO; else
                {
                    fprintf(stderr, "Unknown macro: `%s`.\n", argv[1]);
                    return 404;
                }
            }
        } else macros_to_generate = MACROS_ALL;
        int macros_generated = print_macros(f, count, macros_to_generate);
        printf("Macros generated: %d\n", macros_generated);
        return macros_generated < argc - 2;
    }
    fprintf(stderr, "gen-macros: missing operand\n");
    return 1;
}

#pragma region Currently useless lmao

#pragma region Cat (alphabetic) {{{
int cat_alphabetic(FILE* fout, int count)
{
    char argstring[MAXSTRLEN] = "a";
    char catstring[MAXSTRLEN] = "a";
    printf("Generating concat macro of up to %d uses\n", count);
    for (int i = 2; i <= count; ++i) {
        sprintf(argstring + strlen(argstring), ", %c", 'a' + i - 1);
        sprintf(catstring + strlen(catstring), "##%c", 'a' + i - 1);
        fprintf(fout, "#define __CAT%d_I(%s) %s\n#define __CAT%d(%s) __CAT%d_I(%s)\n",
            i, argstring, catstring, i, argstring, i, argstring);
    }
    return 0;
}
#pragma endregion }}}

#pragma region Func {{{
int func(FILE* fout, int count)
{
    printf("Generating func delegate macro of up to %d parameters.\n", count);
    char argstring[MAXSTRLEN] = "T1";
    fprintf(fout, "#define __FUNC0(NAME, TReturn) TReturn (*NAME)(void)\n");
    for (int i = 1; i <= count; ++i) {
        fprintf(fout, "#define __FUNC%d(NAME, TReturn, %s) TReturn (*NAME)(%s)\n", i, argstring, argstring);
        sprintf(argstring + strlen(argstring), ", T%d", i + 1);
    }
    fprintf(fout, "// Defines a function that takes a certain amount of parameters and returns a value.\n");
    fprintf(fout, "#define Func(TReturn, ...) typeof(CAT(__FUNC, NARGS(__VA_ARGS__))(, TReturn,##__VA_ARGS__))\n");
    return 0;
}
#pragma endregion }}}

#pragma region Action {{{
int action(FILE* fout, int count)
{
    printf("Generating action delegate macro of up to %d parameters.", count);
    char argstring[MAXSTRLEN] = "T1";
    fprintf(fout, "#define __ACTION0(NAME) void (*NAME)(void)\n");
    for (int i = 1; i <= count; ++i) {
        fprintf(fout, "#define __ACTION%d(NAME, %s) void (*NAME)(%s)\n", i, argstring, argstring);
        sprintf(argstring + strlen(argstring), ", T%d", i + 1);
    }
    fprintf(fout, "// Defines a function that takes a certain amount of parameters and does not return a value.\n");
    fprintf(fout, "#define Action(...) typeof(CAT(__ACTION, NARGS(__VA_ARGS__))(,##__VA_ARGS__))\n");
    return 0;
}
#pragma endregion }}}

#pragma endregion
