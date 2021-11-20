#include "file.h"

int get_file_size(img_info* img, FILE * file){
  fseek(file, 0, SEEK_END);
  img->size = ftell(file);
  fseek(file, 0, SEEK_SET);
  if(img->size<0){
      return -1;
  }
  return 0;
}

int read_file(img_info* img, FILE *file){
    img->data = (unsigned char*) malloc (img->size);
    fread(img->data, sizeof(unsigned char), img->size, file);

    if(img->size<0){
      return -1;
    }
    return 0;
}