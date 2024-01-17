#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <math.h>  

#define PPM_SCALER 25
#define WIDTH 51
#define HEIGHT 51
#define SAMPLE_SIZE 10


typedef float Layer[HEIGHT][WIDTH];

static inline int clampi (int x, int low, int high) 
{
    if (x < low) x = low;
    if (x > high) x = high;
    return x;
    
    
}

void layerDrawRect(Layer layer, int x, int y, int w, int h, float value) 
{
    assert (w > 0);
    assert (h > 0);
    int x0 = clampi(x, 0, WIDTH-1);
    int y0 = clampi(y, 0, HEIGHT-1);
    int x1 = clampi(x0 + w - 1, 0, WIDTH-1);
    int y1 = clampi(y0 + h - 1, 0, HEIGHT-1);

    for (int xi = x0; xi <= x1; xi++){
        for (int yi=y0; yi <=y1; yi++){
            layer[yi][xi] = value;
        }
    }
}

void layerDrawCircle(Layer layer, int cx, int cy, int r, float value)
{
    assert(r > 0);
    int x0 = clampi(cx - r, 0, WIDTH-1);
    int y0 = clampi(cy - r, 0, HEIGHT-1);
    int x1 = clampi(cx + r, 0, WIDTH-1);
    int y1 = clampi(cy + r, 0, HEIGHT-1);
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            int dx = x - cx;
            int dy = y - cy;
            if (dx*dx + dy*dy <= r*r) {
                layer[y][x] = value;
            }
        }
    }
}

void layerSaveAsPPM(Layer layer, const char *file_path)
{
    FILE * f = fopen(file_path, "wb");

    if (f == NULL) {
        fprintf(stderr, "Error opening file %s: %s\n", file_path, strerror(errno));
        exit(1);
    }

    fprintf(f, "P6\n%d %d 255\n", WIDTH * PPM_SCALER, HEIGHT * PPM_SCALER);
    
    for (int y = 0; y < HEIGHT * PPM_SCALER; y++) {
        for (int x = 0; x < WIDTH * PPM_SCALER; x++){
            float s = layer[y / PPM_SCALER][x / PPM_SCALER];
            char pixel [3] = {
                (char) floorf(255 * s),
                0,
                0
            };
                fwrite(pixel, sizeof(pixel), 1, f);
        }
    }

    fclose(f);
}

void layerSaveAsBin(Layer layer, const char *file_path)
{
    FILE * f = fopen(file_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "Error opening file %s: %s\n", file_path, strerror(errno));
        exit(1);
    }

    fwrite(layer, sizeof(Layer), 1, f);
    fclose(f);
    
    
}

void layerLoadFromBin(Layer layer, const char *file_path)
{
    assert(0 && "TODO: layerLoadFromBin is not implemented yet!");
}

float feedForward(Layer inputs, Layer weights)
{
    float outputs = 0.0f;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            outputs += inputs[y][x] * weights[y][x];
        }
    }
    
    return outputs;
}

static Layer inputs;
static Layer weights;

int randRange(int low, int high)
{
    assert (low < high);
    return rand() % (high - low ) + low;
}

int main(void)
{
    char file_path[256];

    for (int i = 0; i <= SAMPLE_SIZE; ++i) {
        printf("[INFO] generating rect %d\n", i);

        layerDrawRect(inputs, 0, 0, WIDTH, HEIGHT, 0.0f);
        int x = randRange(0, WIDTH);
        int y = randRange(0, HEIGHT);
        int w = randRange(1, WIDTH);
        int h = randRange(1, HEIGHT);
        layerDrawRect(inputs, x, y, w, h, 1.0f);

        snprintf(file_path, sizeof(file_path), "rect-%02d.bin", i);
        layerSaveAsBin(inputs, file_path);
        snprintf(file_path, sizeof(file_path), "rect-%02d.ppm", i);
        layerSaveAsPPM(inputs, file_path);
        //Hello
    }

    return 0;
}
