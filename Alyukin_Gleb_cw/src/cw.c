#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

#pragma pack(push,1)

typedef struct {
    uint16_t signature;         // Сигнатура = 0x4D42
    uint32_t filesize;          // Размер файла
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixelArrOffset;    // Смещение пикселей
} BitmapFileHeader;

// Информационный заголовок BMP
typedef struct {
    uint32_t headerSize;            // Размер заголовка
    int32_t  width;                 // Ширина в пикселях
    int32_t  height;                // Высота в пикселях
    uint16_t planes;                // Количество плоскостей
    uint16_t bitsPerPixel;          // Биты на пиксель (24)
    uint32_t compression;           // Сжатие (без сжатия)
    uint32_t imageSize;             // Размер всех пикселей
    int32_t  xPixelsPerMeter;       // Расширение по X
    int32_t  yPixelsPerMeter;       // Расширение по Y
    uint32_t colorsInColorTable;    // Количество используемых цветов
    uint32_t importantColorCount;   // Количество важных цветов
} BitmapInfoHeader;
#pragma pack(pop)

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} Rgb;

// Структура для работы с BMP
typedef struct {
    BitmapFileHeader fileHeader;
    BitmapInfoHeader infoHeader;
    Rgb *pixels;  // height*width
} BMPImage;

// Состояние операции
enum { OP_NONE, OP_INFO, OP_REPLACE, OP_RGBFILTER, OP_SPLIT, OP_SQUARE_RHOMBUS, OP_HSV };

// Глобальные переменные для опций
static char *input_file = NULL;
static char *output_file = "output.bmp";
static int op = OP_NONE;
static Rgb old_color = {0,0,0}, new_color = {0,0,0}, line_color = {0,0,0};
static char *component_name = NULL;
static int component_value = -1;
static int parts_x = 0, parts_y = 0, line_thick = 0;

BMPImage* read_bmp(const char *fn);                  // Чтение BMP
int write_bmp(const char *fn, BMPImage *img);       // Запись BMP
int parse_color(const char *str, Rgb *px);          // rgb
void free_bmp(BMPImage *img);                        // Освобождение памяти
void print_info(const BMPImage *img);                // --info
void replace_color(BMPImage *img);                   // --color_replace
void rgb_filter(BMPImage *img);                      // --rgbfilter
void split_grid(BMPImage *img);                      // --split
void print_usage(const char *prog);                  // вывод справки
void hsv_transform(BMPImage *img);

int main(int argc, char **argv) {
    printf("Course work for option 4.2, created by Gleb Alyukin.\n");

    // Описание опций c помощью getopt
    static struct option longopts[] = {
        {"help",            no_argument,       0, 'h'},
        {"input",           required_argument, 0, 'i'},
        {"output",          required_argument, 0, 'o'},
        {"info",            no_argument,       0,  0 },
        {"color_replace",   no_argument,       0,  0 },
        {"old_color",       required_argument, 0,  1 },
        {"new_color",       required_argument, 0,  2 },
        {"rgbfilter",       no_argument,       0,  0 },
        {"component_name",  required_argument, 0,  3 },
        {"component_value", required_argument, 0,  4 },
        {"split",           no_argument,       0,  0 },
        {"number_x",        required_argument, 0,  5 },
        {"number_y",        required_argument, 0,  6 },
        {"thickness",       required_argument, 0,  7 },
        {"color",           required_argument, 0,  8 },
        {"hsv",             required_argument, 0,  0 },
        {0,0,0,0}
    };

    int opt, idx;
    while ((opt = getopt_long(argc, argv, "hi:o:", longopts, &idx)) != -1) {
        switch (opt) {
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 0:
                if (strcmp(longopts[idx].name, "info") == 0) op = OP_INFO;
                else if (strcmp(longopts[idx].name, "color_replace") == 0) op = OP_REPLACE;
                else if (strcmp(longopts[idx].name, "rgbfilter") == 0) op = OP_RGBFILTER;
                else if (strcmp(longopts[idx].name, "split") == 0) op = OP_SPLIT;
                else if (strcmp(longopts[idx].name, "hsv") == 0) op = OP_HSV;
                break;
            case 1:  // --old_color
                if (!parse_color(optarg, &old_color)) {
                    fprintf(stderr, "Error: invalid --old_color format\n");
                    return 43;
                }
                break;
            case 2:  // --new_color
                if (!parse_color(optarg, &new_color)) {
                    fprintf(stderr, "Error: invalid --new_color format\n");
                    return 44;
                }
                break;
            case 3:  // --component_name
                component_name = optarg;
                break;
            case 4:  // --component_value
                component_value = atoi(optarg);
                if (component_value < 0 || component_value > 255) {
                    fprintf(stderr, "Error: --component_value must be 0-255\n");
                    return 45;
                }
                break;
            case 5:  // --number_x
                parts_x = atoi(optarg);
                break;
            case 6:  // --number_y
                parts_y = atoi(optarg);
                break;
            case 7:  // --thickness
                line_thick = atoi(optarg);
                break;
            case 8:  // --color
                if (!parse_color(optarg, &line_color)) {
                    fprintf(stderr, "Error: invalid --color format\n");
                    return 46;
                }
                break;
            case '?': // неизвестная опция
            default:
                print_usage(argv[0]);
                return 40;
        }
    }

    // Проверка на наличие --input
    if (!input_file) {
        fprintf(stderr, "Error: --input is required\n");
        print_usage(argv[0]);
        return 41;
    }

    // Загрузка BMP
    BMPImage *img = read_bmp(input_file);
    if (!img) return 43;

    // Проверка выбора только одной операции
    if (op == OP_NONE) {
        fprintf(stderr, "Error: specify one of --info, --color_replace, --rgbfilter, --split\n");
        free_bmp(img);
        return 47;
    }

    // Проверка на наличие аргументов --color_replace
    if (op == OP_REPLACE &&
        (old_color.red + old_color.green + old_color.blue < 0 ||
         new_color.red + new_color.green + new_color.blue < 0))
    {
        fprintf(stderr, "Error: --old_color and --new_color required\n");
        free_bmp(img);
        return 48;
    }

    // Проверка аргументов для --rgbfilter
    if (op == OP_RGBFILTER &&
        (!component_name ||
         (strcmp(component_name,"red") &&
          strcmp(component_name,"green") &&
          strcmp(component_name,"blue"))) )
    {
        fprintf(stderr, "Error: --component_name [red|green|blue] required\n");
        free_bmp(img);
        return 49;
    }

    // Проверка аргументов для --split
    if (op == OP_SPLIT && (parts_x<2 || parts_y<2 || line_thick<1)) {
        fprintf(stderr, "Error: --number_x>1 --number_y>1 --thickness>0 required\n");
        free_bmp(img);
        return 46;
    }


    // Выполнение операций
    if (op == OP_INFO) {
        print_info(img);
    } else {
        if      (op == OP_REPLACE)          replace_color(img);
        else if (op == OP_RGBFILTER)        rgb_filter(img);
        else if (op == OP_SPLIT)            split_grid(img);
        else if (op == OP_HSV)              hsv_transform(img);
        // Сохранение результата
        if (!write_bmp(output_file, img)) {
            fprintf(stderr, "Error: failed to save %s\n", output_file);
            free_bmp(img);
            return 44;
        }
        printf("Saved: %s\n", output_file);
    }

    free_bmp(img);
    return 0;
}

// Вывод справки
void print_usage(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("  -h, --help               Показать эту справку\n");
    printf("  -i, --input <file.bmp>   Имя входного файла (required)\n");
    printf("  -o, --output <file.bmp>  Имя выходного файла (default output.bmp)\n");
    printf("  --info                   Вывести информацию о BMP-файле\n");
    printf("  --color_replace          Заменить цвет (--old_color r.g.b --new_color r.g.b)\n");
    printf("  --rgbfilter              Применить RGB фильтр (--component_name & --component_value)\n");
    printf("  --split                  Разделить на части (--number_x, --number_y, --thickness & --color)\n");
    printf("  --hsv         из RGB в HSV\n");
}

// Чтение BMP
BMPImage* read_bmp(const char *fn) {
    FILE *f = fopen(fn, "rb");
    if (!f) {
        perror("fopen");
        return NULL;
    }

    BMPImage *img = calloc(1, sizeof(*img));
    if (!img) {
        fclose(f);
        return NULL;
    }

    // Проверяем сигнатуру
    if (fread(&img->fileHeader, sizeof(img->fileHeader), 1, f) != 1 ||
        img->fileHeader.signature!=0x4D42)
    {
        fprintf(stderr,"Error: Not a BMP file\n");
        fclose(f);
        free(img);
        return NULL;
    }

    // Проверяем 24 бита
    if (fread(&img->infoHeader, sizeof(img->infoHeader), 1, f) !=1 ||
        img->infoHeader.bitsPerPixel != 24 || img->infoHeader.compression != 0)
    {
        fprintf(stderr,"Error: Unsupported BMP format\n");
        fclose(f);
        free(img);
        return NULL;
    }

    int w = img->infoHeader.width;
    int h = abs(img->infoHeader.height);
    int rowBytes = (w*3 + 3)&~3;
    img->pixels = malloc(w*h*sizeof(Rgb));
    if (!img->pixels) {
        fprintf(stderr,"Error: Memory allocation error\n");
        fclose(f);
        free(img);
        return NULL;
    }

    fseek(f, img->fileHeader.pixelArrOffset, SEEK_SET);
    uint8_t *buffer = malloc(rowBytes);
    if (!buffer) {
        fprintf(stderr,"Error: Memory allocation error\n");
        fclose(f);
        free_bmp(img);
        return NULL;
    }

    for (int y = 0; y < h; y++) {
        if (fread(buffer, 1, rowBytes, f) != rowBytes) {
            fprintf(stderr,"Error: could not read file\n");
            free(buffer);
            fclose(f);
            free_bmp(img);
            return NULL;
        }
        memcpy(&img->pixels[y*w], buffer, w*sizeof(Rgb));
    }
    free(buffer);
    fclose(f);
    return img;
}

// Сохранение BMP
int write_bmp(const char *fn, BMPImage *img) {
    FILE *f = fopen(fn, "wb");
    if (!f) {
        printf("Error: could not make file\n");
        return 0;
    }

    size_t w = img->infoHeader.width;
    size_t h = abs(img->infoHeader.height);

    fwrite(&img->fileHeader, sizeof(img->fileHeader),1 ,f);
    fwrite(&img->infoHeader, sizeof(img->infoHeader),1 ,f);

    int rowBytes = (w * 3 + 3)&~3;
    img->infoHeader.imageSize = rowBytes * h;
    img->fileHeader.filesize = img->fileHeader.pixelArrOffset + img->infoHeader.imageSize;

    uint8_t *buffer = calloc(1, rowBytes);
    for (int y = 0; y < h; y++) {
        memcpy(buffer, &img->pixels[y*w], w*sizeof(Rgb));
        fwrite(buffer,1,rowBytes,f);
    }
    free(buffer);
    fclose(f);
    return 1;
}

// Парсинг цвета rgb
int parse_color(const char *str, Rgb *px) {
    int r, g, b;
    if (sscanf(str, "%d.%d.%d", &r, &g, &b) != 3) return 0;
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) return 0;
    px->red = r;
    px->green = g;
    px->blue = b;
    return 1;
}

// Освобождение памяти
void free_bmp(BMPImage *img) {
    free(img->pixels);
    free(img);
}

// --info
void print_info(const BMPImage *img) {
    printf("Width:  %d\n", img->infoHeader.width);
    printf("Height: %d\n", img->infoHeader.height);
    printf("Bpp:    %u\n", img->infoHeader.bitsPerPixel);
}

// --color_replace
void replace_color(BMPImage *img) {
    int w = img->infoHeader.width;
    int h = abs(img->infoHeader.height);
    for (int i = 0; i < w*h; i++) {
        Rgb *p = &img->pixels[i];
        if (p->red==old_color.red && p->green==old_color.green && p->blue==old_color.blue)
            *p = new_color;
    }
}

// --rgbfilter
void rgb_filter(BMPImage *img) {
    int w = img->infoHeader.width;
    int h = abs(img->infoHeader.height);
    for (int i = 0; i < w*h; i++) {
        Rgb *p = &img->pixels[i];
        if (!strcmp(component_name,"red"))   p->red   = component_value;
        if (!strcmp(component_name,"green")) p->green = component_value;
        if (!strcmp(component_name,"blue"))  p->blue  = component_value;
    }
}

// --split
void split_grid(BMPImage *img) {
    int w = img->infoHeader.width;
    int h = abs(img->infoHeader.height);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            for (int i = 1; i < parts_x; i++) {
                int cx = (w * i) / parts_x;
                if (abs(x - cx) < line_thick) img->pixels[y*w + x] = line_color;
            }
            for (int j = 1; j < parts_y; j++) {
                int cy = (h * j) / parts_y;
                if (abs(y - cy) < line_thick) img->pixels[y*w + x] = line_color;
            }
        }
    }
}

Rgb hsv_to_rgb(int H, int S, int V) {
    double h = H * 2.0;
    double s = S / 225.0;
    double v = V / 225.0;
    
    double c = v * s;
    double x = c * (1 - fabs(fmod(h/60.0, 2.0) -1));
    double m = v - c;

    double r, g, b;
    if (h < 60) {r = c; g = x; b = 0; }
    else if (h < 120) {r = x; g = c; b = 0; }
    else if (h < 180) {r = 0; g = c; b = x; }
    else if (h < 240) {r = 0; g = x; b = c; }
    else if (h < 300) {r = x; g = 0; b = c; }
    else {r = c; g = 0; b = x;}    
    
    Rgb px;
    px.red = (int)((r + m) * 225.0);
    px.green = (int)((g + m) * 225.0);
    px.blue = (int)((b + m) * 225.0);
    return px;
}

// --hsv
void hsv_transform(BMPImage *img) {
    int w = img->infoHeader.width;
    int h = abs(img->infoHeader.height);
    for (int i = 0; i < w * h; i++) {
        Rgb *p = &img->pixels[i];

        double r = p->red;
        double g = p->red;
        double b = p->red;
        
        double cmax = fmax(r, fmax(g, b));
        double cmin = fmin(r, fmin(g, b));
        double delta = cmax - cmin;
    
        double H = 0, S = 0, V = cmax;

        if (delta > 0.00001) {
            if (cmax == r) H = 60.0 * fmod((g - b) / delta, 6.0);
            else if (cmax == g) H = 60.0 * ((b - r) / delta + 2);
            else H = 60.0 * ((r - g) / delta + 4);

            if (H == 0) H+=360;
        }

        if (cmax > 0.00001) S = delta/cmax;
        
        int h_scaled = (int)(H * 179.0 / 360.0); 
        int s_scaled = (int)(S * 225.0);
        int v_scaled = (int)(V * 225.0);

        *p = hsv_to_rgb(h_scaled, s_scaled, v_scaled);      
    }
}