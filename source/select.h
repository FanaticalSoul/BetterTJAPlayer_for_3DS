﻿#pragma once


typedef struct {

	int level[5],x,y,tmp;
	char title[256],path[256],tja[256],wave[256],genre[256];
	bool course[5];
} LIST_T;

void load_file_list_tjafiles();
void disp_file_list();
void load_file_main();
void cursor_update(int knd);
void get_SelectedId(LIST_T *TMP, int *arg);
void draw_select(float x, float y, const char *text);
bool get_isGameStart();
void draw_option(u16 px, u16 py, unsigned int key);