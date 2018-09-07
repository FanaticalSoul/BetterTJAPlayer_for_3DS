#pragma once
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
	Rest = 0,	//�x��
	Don,		//�h��
	Ka,			//�J�c
	BigDon,		//�h��(��)
	BigKa,		//�J�c(��)
	Renda,		//�A�ŊJ�n
	BigRenda,	//�A��(��)�J�n
	Balloon,	//���D�J�n
	RendaEnd,	//�A�ŏI��
	Potato,		//���������J�n
	BigRendaEnd,//��A�ŏI��
};

enum Sprite_Notes_knd {	//�X�v���C�g�p
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