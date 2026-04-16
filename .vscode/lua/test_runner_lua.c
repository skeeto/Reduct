#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int silent_print(lua_State *L)
{
    return 0;
}

void run_test(const char* filename, FILE* logs)
{
    printf("[INFO] Running %s...\n", filename);

    FILE *f = fopen(filename, "rb");
    if (f == NULL) 
    {
        printf("[FAIL] Could not open file.\n\n");
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* input = malloc(size);
    if (input == NULL) 
    {
        fclose(f);
        printf("[FAIL] Memory allocation failed.\n\n");
        return;
    }

    fread(input, 1, size, f);
    fclose(f);

    lua_State *L = luaL_newstate();
    if (L == NULL) 
    {
        free(input);
        printf("[FAIL] Lua initialization failed.\n\n");
        return;
    }
    
    luaL_openlibs(L);

    if (luaL_loadbuffer(L, input, size, filename) != LUA_OK)
    {
        printf("%s\n", lua_tostring(L, -1));
        printf("[FAIL] Parse error.\n\n");
        lua_close(L);
        free(input);
        return;
    }

    if (lua_pcall(L, 0, 0, 0) != LUA_OK)
    {
        printf("%s\n", lua_tostring(L, -1));
        printf("[FAIL] Eval error.\n\n");
        lua_close(L);
        free(input);
        return;
    }
    lua_close(L);

    printf("[PASS] Test passed.\n");
    printf("[INFO] Benchmarking...\n");

    long iterations = 0;
    const double seconds = 3.0;
    clock_t start = clock();
    clock_t now = start;

    while (1) 
    {
        L = luaL_newstate();
        luaL_openlibs(L);
        
        lua_pushcfunction(L, silent_print);
        lua_setglobal(L, "print");

        if (luaL_loadbuffer(L, input, size, filename) == LUA_OK) {
            lua_pcall(L, 0, 0, 0);
        }
        
        lua_close(L);
        iterations++;
        
        now = clock();
        if ((double)(now - start) / CLOCKS_PER_SEC >= seconds) 
        {
            break;
        }
    }

    printf("[BENCHMARK] %ld iterations/sec\n\n", (long)(iterations / seconds));
    fprintf(logs, "LUA %s: %ld iter/sec\n", filename, (long)(iterations / seconds));

    free(input);
}

int main(int argc, char **argv)
{
    if (argc < 2) 
    {
        fprintf(stderr, "Usage: %s <test_files...>\n", argv[0]);
        return 1;
    }

    FILE* logs = fopen("benchmark.log", "a+");
    if (logs == NULL) 
    {
        perror("Could not open benchmark.log");
        return 1;
    }
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm);
    fprintf(logs, "--- LUA Benchmarks %s ---\n", timeStr);

    for (int i = 1; i < argc; i++) 
    {
        run_test(argv[i], logs);
    }

    fwrite("\n", 1, 1, logs);
    fclose(logs);
    printf("All tests completed. Results saved to benchmark.log\n");
    
    return 0;
}
