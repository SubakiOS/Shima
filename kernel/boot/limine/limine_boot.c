#include <stddef.h>
#include <stdint.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0,

};

__attribute((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_paging_mode_request paging_mode_request = {
    .id = LIMINE_PAGING_MODE_REQUEST_ID,
    .revision = 0,
    .mode = LIMINE_PAGING_MODE_X86_64_4LVL,
    .max_mode = LIMINE_PAGING_MODE_X86_64_4LVL,
    .min_mode = LIMINE_PAGING_MODE_X86_64_4LVL
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = 
LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] =
LIMINE_REQUESTS_END_MARKER;


struct limine_framebuffer *limine_get_framebuffer(void) {
    if  (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
        return NULL;
    }
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        return NULL;
    }
    struct limine_framebuffer *fb = 
    framebuffer_request.response->framebuffers[0];

    if (fb->memory_model != LIMINE_FRAMEBUFFER_RGB || fb->bpp != 32) {
        return NULL;
    }

    return fb;
}

uint64_t limine_get_hhdm_offset(void) {
    if (hhdm_request.response == NULL) {
        return 0;
    };

    return hhdm_request.response->offset;
}

uint64_t  limine_get_paging_mode(void) {
    if (paging_mode_request.response == NULL) {
        return UINT64_MAX;
    }
    return paging_mode_request.response->mode;
}

struct limine_memmap_response *limine_get_memmap(void) {
    if (memmap_request.response == NULL) {
        return NULL;
    }

    return memmap_request.response;
}
struct limine_file *limine_get_font(void) {
    if (module_request.response == NULL ||
        module_request.response->module_count == 0) {
        return NULL;
    }

    for (uint64_t i = 0; i < module_request.response->module_count; i++) {
        struct limine_file *file = module_request.response->modules[i];

        if (file == NULL || file->path == NULL) {
            continue;
        }

        const char *path = file->path;
        const char target[] = "cp850-8x16.psf";

        size_t path_len = 0;
        size_t target_len = sizeof(target) - 1;

        while (path[path_len] != '\0') {
            path_len++;
        }

        if (path_len < target_len) {
            continue;
        }

        size_t j = 0;

        while (j < target_len &&
               path[path_len - target_len + j] == target[j]) {
            j++;
        }

        if (j == target_len) {
            return file;
        }
    }

    return NULL;
}