﻿#pragma once
#include "tja.h"
#include <citro2d.h>

void notes_main(
	bool isDon,
	bool isKa,
	char tja_notes[Measure_Max][Max_Notes_Measure],
	MEASURE_T Measure[Measure_Max],
	int cnt,
	C2D_Sprite  sprites[Sprite_Number]);
void toggle_auto();


void notes_init(TJA_HEADER_T Tja_Header);

enum Notes_knd {
	Rest = 0,	//休符
	Don,		//ドン
	Ka,			//カツ
	BigDon,		//ドン(大)
	BigKa,		//カツ(大)
	Renda,		//連打開始
	BigRenda,	//連打(大)開始
	Balloon,	//風船開始
	RendaEnd,	//連打終了
	Potato,		//お芋音符開始
	BigRendaEnd,//大連打終了
	BalloonEnd,	//風船終了
};

enum Sprite_Notes_knd {	//スプライト用
	dOn = 2,
	kA,
	bIg_don,
	bIg_ka,
	rEnda_start,
	rEnda_int,
	rEnda_fini,
	bIg_renda_start,
	bIg_renda_int,
	bIg_renda_fini,
	bAlloon,
	bAlloon_1,
	bAlloon_2,
	bAlloon_3,
	bAlloon_4,
	bAlloon_5,
	bAlloon_6,
	jUdge_ryou,
	jUdge_ka,
	jUdge_fuka,
};

enum Command_knd {
	STart = 1,
	ENd,
	BPmchange,
	GOgostart,
	GOgoend,
	MEasure,
	SCroll,
	DElay,
	SEction,
	BRanchstart,
	BRanchend,
	N,
	E,
	M,
	LEvelhold,
	BMscroll,
	HBscroll,
	BArlineoff,
	BArlineon,
};

typedef struct {
	int num, notes_max, knd, renda_id;
	double x_ini, x, create_time, judge_time, pop_time,bpm,scroll;
	bool flag;
	C2D_Sprite spr;

} NOTES_T;

typedef struct {
	int measure;
	double x,x_ini,create_time,scroll;
	bool flag,isDisp;

} BARLINE_T;

typedef struct {
	int id,start_id,finish_id,knd;
	double start_x, finish_x;
	bool flag;
}RENDA_T;

typedef struct {
	int id, start_id, finish_id,
		need_hit,current_hit;
	bool flag;
}BALLOON_T;