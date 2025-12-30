// MOCANU Cristian-Lucian, 314CB
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *open_ppm_file(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Eroare deschidere fisier!\n");
        return NULL;
    }
    return f;
}

int read_next_int(FILE *f, int *value)
{
    int c;
    while (1) {
        // Sarim peste whitespace.
        do {
            c = fgetc(f);
            if (c == EOF) {
                return -1;
            }
        } while (c == ' ' || c == '\n' || c == '\t' || c == '\r');
        // Sarim comentariile.
        if (c == '#') {
            while (c != '\n' && c != EOF) {
                c = fgetc(f);
            }
            continue;
        }
        // Verificam daca avem numar.
        fseek(f, -1, SEEK_CUR);
        if (fscanf(f, "%d", value) == 1) {
            return 0;
        } else {
            return -1;
        }
    }
}

int read_ppm_header(FILE *f, int *width, int *height, int *maxval)
{
    char magic[3];
    // Citim magic number.
    if (fscanf(f, "%2s", magic) != 1) {
        fprintf(stderr, "No magic number!\n");
        return -1;
    }
    // Verificam daca este cu adevarat magic number.
    if (strcmp(magic, "P3")) {
        fprintf(stderr, "Fisierul nu este PPM P3 valid!\n");
        return -1;
    }
    if (read_next_int(f, width) || read_next_int(f, height) ||
        read_next_int(f, maxval)) {
            fprintf(stderr, "Fisierul nu este PPM valid!\n");
            return -1;
        }
    if (*width <= 0 || *height <= 0 || *maxval != 255) {
        fprintf(stderr, "Valori header invalide!\n");
        return -1;
    }
    return 0;
}

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} pixel_t;

pixel_t *read_ppm_pixels(FILE *f, int width, int height, int maxval)
{
    pixel_t *img = malloc(width * height * sizeof(pixel_t));
    if (!img) {
        fprintf(stderr, "Eroare de alocare memorie!\n");
        return NULL;
    }
    for (int i = 0; i < width * height; ++i) {
        int r, g, b;
        if (read_next_int(f, &r) || read_next_int(f, &g) ||
            read_next_int(f, &b)) {
            fprintf(stderr, "Fisier PPM invalid!\n");
            free(img);
            return NULL;
        }
        if (r < 0 || r > maxval ||
            g < 0 || g > maxval ||
            b < 0 || b > maxval) {
            fprintf(stderr, "Fisier PPM invalid!\n");
            free(img);
            return NULL;
        }
        (img + i)->r = (unsigned char)r;
        (img + i)->g = (unsigned char)g;
        (img + i)->b = (unsigned char)b;
    }
    return img;
}

void print_ppm_pixels(int width, int height, pixel_t *img)
{
    for (int i = 0; i < width * height; ++i) {
        printf("Pixel %d: R: %d G: %d B: %d\n", i, 
               (img + i)->r, (img + i)->g, (img + i)->b);
    }
}

int save_ppm(int width, int height, pixel_t *img, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Eroare de salvare imagine!\n");
        return -1;
    }
    fprintf(f, "P3\n%d %d\n255\n", width,height);
    for (int i = 0; i < width * height; ++i) {
        fprintf(f, "%d %d %d\n", (img + i)->r, (img + i)->g, (img + i)->b);
    }
    fclose(f);
    return 0;
}

void invert_colors(int width, int height, pixel_t *img)
{
    for (int i = 0; i < width * height; ++i) {
        (img + i)->r = 255 - (img + i)->r;
        (img + i)->g = 255 - (img + i)->g;
        (img + i)->b = 255 - (img + i)->b;
    }
}

int main(void)
{
    FILE *f = open_ppm_file("ex1.ppm");
    if (!f) {
        return -1;
    }

    int width, height, maxval;
    if (read_ppm_header(f, &width, &height, &maxval)) {
        fclose(f);
        return -1;
    }

    printf("Imagine PPM P3 valida!\n");
    printf("Latime: %d\n", width);
    printf("Intaltime: %d\n", height);
    printf("Max color: %d\n", maxval);

    pixel_t *img = read_ppm_pixels(f, width, height, maxval);
    fclose(f);
    if (!img) {
        return -1;
    }

    invert_colors(width, height, img);

    if (save_ppm(width, height, img, "inverted.ppm")) {
        free(img);
        return -1;
    }

    print_ppm_pixels(width, height, img);
    free(img);
    return 0;
}
