#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_resize2.h"
#include <sixel/sixel.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

static int get_terminal_width_pixels(int fallback_px) {
    struct winsize w;

    // Próbujemy pobrać wymiary z wyjścia standardowego (STDOUT_FILENO = 1)
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {

        // 1. Jeśli terminal przekazuje bezpośrednią rozdzielczość w pikselach
        if (w.ws_xpixel > 0) {
            return w.ws_xpixel;
        }

        // 2. Jeśli podaje tylko kolumny, przeliczamy kolumny na piksele
        //    (zakładając standardową szerokość fontu ~8-10px na znak)
        if (w.ws_col > 0) {
            return w.ws_col * 9;
        }
    }

    return fallback_px;
}

int image_display_image_in_sixel(const char *image_name)
{
    int width, height, channels_infile;
    const int desired_channels = 3;

    unsigned char *data = stbi_load(image_name, &width, &height, &channels_infile, desired_channels);
    if(data == NULL) {
        fprintf(stderr, "STBI can't read the image file: %s %s", image_name, stbi_failure_reason());

        return -1;
    }
    int term_px_width = get_terminal_width_pixels(800);
    int max_width = term_px_width - 40;
    if(max_width < 1)
        max_width = 1;

    int new_width = width;
    int new_height = height;

    if(width > max_width) {
        new_width = max_width;
        new_height = (int)((double)height * new_width / width);
        if(new_height < 1)
            new_height = 1;
    }

    unsigned char *resized_data = malloc((size_t)new_width * new_height * desired_channels);
    if(resized_data == NULL) {
        fprintf(stderr, "Can't allocate memory fo buffer\n");

        return -1;
    }

    stbir_resize_uint8_linear(data, width, height, 0, resized_data, new_width, new_height, 0, STBIR_RGB);
    stbi_image_free(data);

    sixel_encoder_t *encoder = NULL;
    SIXELSTATUS status = sixel_encoder_new(&encoder, NULL);
    if(SIXEL_FAILED(status))
    {
        fprintf(stderr, "sixel encoder new failed\n");
        goto cleanup;
    }

    status = sixel_encoder_encode_bytes(encoder, resized_data, new_width, new_height, SIXEL_PIXELFORMAT_RGB888, NULL, 0);
    if(SIXEL_FAILED(status))
        fprintf(stderr, "sixel encoder encode failed\n");
    printf("\n");

cleanup:

    if(encoder)
        sixel_encoder_unref(encoder);

    free(resized_data);

    return SIXEL_SUCCEEDED(status) ? 0 : -1;
};
