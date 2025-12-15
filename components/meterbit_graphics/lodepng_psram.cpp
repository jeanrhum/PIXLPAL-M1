// lodepng_psram.cpp
#include "esp_heap_caps.h"

// 1. Tell LodePNG we will supply allocators
#define LODEPNG_NO_COMPILE_ALLOCATORS

// 2. Provide PSRAM allocators
static void* lodepng_psram_malloc(size_t size) {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}

static void* lodepng_psram_realloc(void* ptr, size_t size) {
    return heap_caps_realloc(ptr, size,
                              MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}

static void lodepng_psram_free(void* ptr) {
    heap_caps_free(ptr);
}

// 3. Redirect LodePNG's allocators
#define lodepng_malloc  lodepng_psram_malloc
#define lodepng_realloc lodepng_psram_realloc
#define lodepng_free    lodepng_psram_free

// 4. NOW compile LodePNG implementation
#include "lodepng.cpp"
