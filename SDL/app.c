#include "sim.h"

void fill_init_field(int (*field)[SIM_Y_SIZE]) {
  for (int x = 0; x < SIM_X_SIZE; ++x) {
    for (int y = 0; y < SIM_Y_SIZE; ++y) {
      if (simRand() % 2 && simRand() % 2) {
        field[x][y] = 1;
      }
    }
  }
}

void display_epoch(int (*field)[SIM_Y_SIZE]) {
  for (int x = 0; x < SIM_X_SIZE; ++x) {
    for (int y = 0; y < SIM_Y_SIZE; ++y) {
      if (field[x][y]) {
        simPutPixel(x, y, 0x00FFFFFF);
      } else {
        simPutPixel(x, y, 0);
      }
    }
  }
}

int count_alive_pixels(int x, int y, int (*field)[SIM_Y_SIZE]) {
  int d[3] = {-1, 0, 1};
  int res = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (0 <= x + d[i] && x + d[i] < SIM_X_SIZE && 0 <= y + d[j] && y + d[j] < SIM_Y_SIZE) {
        if (field[x + d[i]][y + d[j]]) {
          if (d[i] == 0 && d[j] == 0) {
            continue;
          }
          res++;
        }
      }
    }
  }
  return res;
}

void set_new_field_x_y(int x, int y, int pixel, int count, int (*new_field)[SIM_Y_SIZE]) {
  if (pixel) {
    if (count != 2 && count != 3) {
      new_field[x][y] = 0;
    } else {
      new_field[x][y] = 1;
    }
  } else {
    if (count == 3) {
      new_field[x][y] = 1;
    } else {
      new_field[x][y] = 0;
    }
  }
}

void update_field(int (*field)[SIM_Y_SIZE], int (*new_field)[SIM_Y_SIZE]) {
  for (int x = 0; x < SIM_X_SIZE; ++x) {
    for (int y = 0; y < SIM_Y_SIZE; ++y) {
      int count = count_alive_pixels(x, y, field);
      set_new_field_x_y(x, y, field[x][y], count, new_field);
    }
  }
  for (int x = 0; x < SIM_X_SIZE; ++x) {
    for (int y = 0; y < SIM_Y_SIZE; ++y) {
      field[x][y] = new_field[x][y];
    }
  }
}

void app(void) {
  int field[SIM_X_SIZE][SIM_Y_SIZE];
  int new_field[SIM_X_SIZE][SIM_Y_SIZE];
  fill_init_field(field);
//  while (1) {
//    display_epoch(field);
//    simFlush();
//    update_field(field, new_field);
//  }
  for (int i = 0; i < 2; ++i) {
    display_epoch(field);
    simFlush();
    update_field(field, new_field);
  }
}