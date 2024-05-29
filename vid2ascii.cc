#include "gif_lib.h"
#include "stdio.h"
#include "stdlib.h"

#define ASCII_MAP_SIZE 11
#define clrscr() printf("\e[1;1H\e[2J")

char ascii_map[ASCII_MAP_SIZE] = {'@', '#', 'S', '%', '?', '*',
                                  '+', ';', ':', ',', '.'};

int rgb_to_grayscale(double r, double g, double b) {
  return 0.299 * r + 0.587 * g + 0.113 * b;
}

void downsample(unsigned char **row_pointers, int height, int width,
                int height_factor, int width_factor) {
  int new_height = (height + height_factor - 1) / height_factor;
  int new_width = (width + width_factor - 1) / width_factor;

  // create temp_row_pointers to hold new image
  unsigned char **temp_row_pointers =
      (unsigned char **)malloc(new_height * sizeof(unsigned char *));
  for (int i = 0; i < new_height; i++) {
    temp_row_pointers[i] =
        (unsigned char *)malloc(new_width * sizeof(unsigned char));
    int p_size =
        i == new_height - 1 ? height - height_factor * i : height_factor;
    for (int j = 0; j < new_width; j++) {
      int new_value = 0;
      int q_size = j == new_width - 1 ? width - width_factor * j : width_factor;

      for (int p = 0; p < p_size; p++) {
        for (int q = 0; q < q_size; q++) {
          new_value +=
              row_pointers[height_factor * i + p][width_factor * j + q];
        }
      }
      temp_row_pointers[i][j] = new_value / (p_size * q_size);
    }
  }

  // free old row_pointers
  for (int i = 0; i < height; i++) {
    free(row_pointers[i]);
  }
  free(row_pointers);

  // point row_pointers to the new array
  row_pointers = temp_row_pointers;

  height = new_height;
  width = new_width;
}

void draw_frame(unsigned char **row_pointers, int height, int width) {
  clrscr();
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int index = (int)row_pointers[i][j] /
                  ((256 + ASCII_MAP_SIZE - 1) / ASCII_MAP_SIZE);
      putchar(ascii_map[index]);
    }
    putchar('\n');
  }
}

void update_frame(unsigned char **row_pointers, GifFileType *gif, int frame,
                  int height, int width) {
  GifImageDesc desc = gif->SavedImages[frame].ImageDesc;
  int start_height = desc.Top;
  int start_width = desc.Left;
  for (int i = 0; i < desc.Height; i++) {
    for (int j = 0; j < desc.Width; j++) {
      printf("processing %d, %d\n", i, j);
      int row = i + start_height;
      int col = j + start_width;
      int index = row * width + col;

      int color = gif->SavedImages[i].RasterBits[index];
      int red = gif->SColorMap->Colors[color].Red;
      int green = gif->SColorMap->Colors[color].Green;
      int blue = gif->SColorMap->Colors[color].Blue;
      int grayscale = rgb_to_grayscale(red, green, blue);

      row_pointers[i][j] = grayscale;
    }
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: vid2ascii <in_file>");
    return 0;
  }

  int error = 0;
  GifFileType *gif = DGifOpenFileName(argv[1], &error);
  if (!gif) {
    printf("Error while opening the file: %s\n", "");
    return 1;
  }

  DGifSlurp(gif);

  int height = gif->SHeight;
  int width = gif->SWidth;
  int hf = height / 100;
  int wf = width / 50;

  unsigned char **row_pointers =
      (unsigned char **)malloc(height * sizeof(unsigned char *));
  for (int i = 0; i < height; i++) {
    row_pointers[i] = (unsigned char *)malloc(width * sizeof(unsigned char));
  }

  int frame = 0;
  // while (true) {
  update_frame(row_pointers, gif, frame++, height, width);
  printf("updated\n");
  downsample(row_pointers, height, width, hf, wf);
  printf("downsampled\n");
  draw_frame(row_pointers, height, width);
  // }
  // for (int i = 11; i < 12; i++) {
  // printf("image %d:\n", i + 1);
  // printf("width: %d\nheight: %d\n", gif->SavedImages[i].ImageDesc.Width,
  //        gif->SavedImages[i].ImageDesc.Height);
  // printf("top left coord (%d, %d)\n", gif->SavedImages[i].ImageDesc.Top,
  //        gif->SavedImages[i].ImageDesc.Left);
  // GifImageDesc desc = gif->SavedImages[i].ImageDesc;
  // int total_size = desc.Height * desc.Width;
  // for (int j = 0; j < desc.Height; j++) {
  //   for (int k = 0; k < desc.Width; k++) {
  //     int color = gif->SavedImages[i].RasterBits[j * desc.Width + k];
  //     int red = gif->SColorMap->Colors[color].Red;
  //     int green = gif->SColorMap->Colors[color].Green;
  //     int blue = gif->SColorMap->Colors[color].Blue;
  //     printf("pixel (%d, %d) of image %d has rgb value (%d, %d, %d) and "
  //            "grayscale value %d\n",
  //            desc.Top + j, desc.Left + k, i + 1, red, green, blue,
  //            rgb_to_grayscale(red, green, blue));
  //   }
  //   printf("\n");
  // }
  // printf("\n");
  // }

  // printf("num images: %d\n", gif->ImageCount);
  // printf("width: %d\n", (int)gif->SWidth);
  // printf("# extensions: %d\n", gif->SavedImages->ExtensionBlockCount);
  // printf("colors\n");
  // ColorMapObject *color_map = gif->SColorMap;
  // for (int i = 0; i < color_map->ColorCount; i++) {
  //   GifColorType color = color_map->Colors[i];
  //   printf("rgb %d, %d, %d\n", color.Red, color.Green, color.Blue);
  // }
}
