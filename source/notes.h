#pragma once
#include "tja.h"
#include <citro2d.h>

void notes_main(
	bool isDon,
	bool isKa,
	char tja_notes[Measure_Max][Max_Notes_Measure],
	MEASURE_T Measure[Measure_Max],
	int cnt,
	C2D_Sprite  sprites[12]);


void notes_init(TJA_HEADER_T Tja_Header);

enum Notes_Kind {
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

enum Sprite_Notes_Kind {	//�X�v���C�g�p
	dOn = 2,
	kA,
	bIg_don,
	bIg_ka,
	rEnda,
	rEnda_fini,
	bIg_renda,
	bIg_renda_fini,
	bAlloon,
	jUdge_ryou,
	jUdge_ka,
	jUdge_fuka,
};

enum Command_Kind {
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
	int num, notes_max;
	double x_ini, x, create_time, judge_time, bpm;
	int kind;
	bool flag = false;
	int sec;
	C2D_Sprite spr;

} NOTES_T;

typedef struct {
	int measure;
	double x,create_time;
	bool flag,isDisp;

} BARLINE_T;