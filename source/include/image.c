#include "image.h"

#define STBI_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>
#include <b64/cencode.h>
#include <sixel/sixel.h>
#include <string.h>

static char* base64_encode(const unsigned char *input, size_t input_size, size_t *output_size);

int image_display_image_in_kitty(const char *image_name)
{
    int width, height, channels_infile;
    const int desired_channels = 3;

    unsigned char* data = stbi_load(image_name, &width, &height, &channels_infile, desired_channels);
    if(data == NULL) {
        fprintf(stderr, "STBI can't read the image file: %s %s", image_name, stbi_failure_reason());

        return -1;
    }

    size_t rgb_size = width * height * desired_channels;

    size_t img_size = 0;
    char *img_64 = base64_encode(data, rgb_size, &img_size);
    if(img_64 == nullptr) {
        printf("Can't decode img to base64\n");
        stbi_image_free(data);

        return -1;
    }

    printf("\033_Ga=T,f=24,s=%d,v=%d,m=1;", width, height);

    size_t offset = 0;
    const int chunk_size = 4096;

    fwrite(img_64, 1, chunk_size, stdout);
    printf("\033\\");
    fflush(stdout);

    offset += chunk_size;

    while(offset < img_size) {
        printf("\033_m=%d;", (offset + chunk_size <= img_size) ? 1 : 0);
        int bytes_send = chunk_size;
        if(img_size - offset < chunk_size)
            bytes_send = img_size - offset;

        fwrite(img_64 + offset, 1, bytes_send, stdout);

        printf("\033\\");
        fflush(stdout);

        offset += chunk_size;
    }

    free(img_64);
    stbi_image_free(data);

    return 0;
}

static char* base64_encode(const unsigned char *input, size_t input_size, size_t *output_size)
{
    *output_size = 4 * ((input_size + 2) / 3) + 5;
    char *output = malloc(*output_size);
    if(output == nullptr) {
        fprintf(stderr, "Encoded b64 error, can't alloc memory\n");
        return nullptr;
    }

    base64_encodestate state;
    base64_init_encodestate(&state);

    int len = base64_encode_block((const char*)input, (int)input_size, output, &state);
    len += base64_encode_blockend(output + len, &state);

    *output_size = len;

    return output;
};

int image_display_image_in_sizel(const char *image_name)
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

cleanup:

    if(encoder)
        sixel_encoder_unref(encoder);

    stbi_image_free(data);

    return SIXEL_SUCCEEDED(status) ? 0 : -1;
};
