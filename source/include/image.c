#include "image.h"

#define STBI_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>
#include <sixel/sixel.h>
#include <string.h>

int image_display_image_in_sixel(const char *image_name)
{
    int width, height, channels_infile;
    const int desired_channels = 3;

    unsigned char* data = stbi_load(image_name, &width, &height, &channels_infile, desired_channels);
    if(data == NULL) {
        fprintf(stderr, "STBI can't read the image file: %s %s", image_name, stbi_failure_reason());

        return -1;
    }
    

    sixel_encoder_t *encoder;
    SIXELSTATUS status = sixel_encoder_new(&encoder, NULL);
    if(SIXEL_FAILED(status))
    {
        fprintf(stderr, "sixel encoder new failed\n");
        goto cleanup;
    }

    status = sixel_encoder_encode(encoder, image_name);
    if(SIXEL_FAILED(status))
        fprintf(stderr, "sixel encoder encode failed\n");
    printf("\n");

cleanup:

    if(encoder)
        sixel_encoder_unref(encoder);

    stbi_image_free(data);

    return SIXEL_SUCCEEDED(status) ? 0 : -1;
};
