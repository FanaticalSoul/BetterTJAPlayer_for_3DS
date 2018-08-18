#include "header.h"
#include "main.h"
#include "time.h"
#include "audio.h"

#define Notes_Area 400.0
#define Notes_Judge 93
#define Notes_Max 64

char buf_notes[160];

enum Notes_Kind {
	Rest,		//�x��
	Don,		//�h��
	Ka,			//�J�c
	BigDon,		//�h��(��)
	BigKa,		//�J�c(��)
	Renda,		//�A�ŊJ�n
	BigRenda,	//�A��(��)�J�n
	Balloon,	//���D�J�n
	RendaEnd,	//�A�ŏI��
	Potato,		//���������J�n
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

int notes_id_check(); 
//void notes_calc(bool isDon, bool isKa,double bpm, double tempo, double NowTime, int cnt, C2D_SpriteSheet spriteSheet);
void notes_calc(bool isDon, bool isKa, double bpm, double tempo, double NowTime, int cnt);

typedef struct {
	int notes_number, notes_max;
	double x_ini, x, create_time,judge_time,bpm;
	int kind;
	bool flag=false;
	int sec;
	C2D_Sprite spr;
} NOTES_T;
NOTES_T Notes[4096];

int bpm_count = 0;
int bpm_count2 = 1;	//����bpm�̎��Ԍv���p
int sec_count = 0;
int notes_main_count = 0;
int renda_flag = 0;
int notes_count;
double bar_x;
double speed;
double bpm_time;
double tempo_time[2], sec_time;
double SecTime[4] = { 0,0,0,0 };
bool bpm_tempo_flag = false, notes_load_flag = true;


int ctoi(char c) {

	switch (c) {
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	default : return 0;
	}
}

void notes_main(bool isDon,bool isKa, char tja_notes[2048][128], int cnt, char *tja_title, char *tja_subtitle, char *tja_level, char *tja_bpm, char *tja_wave, char *tja_offset, char *tja_balloon, char *tja_songvol, char *tja_sevol, char *tja_scoreinit, char *tja_scorediff, char *tja_course, char *tja_style, char *tja_game, char *tja_life, char *tja_demostart, char *tja_side, char *tja_scoremode, C2D_SpriteSheet spriteSheet) {

	double bpm = atof(tja_bpm);
	double tempo = 4;
	double NowTime = time_now(0);
	snprintf(buf_notes, sizeof(buf_notes), "NowTime0:%.2f", NowTime);
	debug_draw(0, 10, buf_notes);
	if (notes_main_count == 0) bpm_time = 60.0 / bpm * bpm_count2;

	//sftd_draw_textf(font, 100, 20, RGBA8(0, 255, 0, 255), 10, "BpmTime:%f", bpm_time);
	//sftd_draw_textf(font, 100, 30, RGBA8(0, 255, 0, 255), 10, "BpmCount2:%d", bpm_count2);
	
	if (bpm_time <= NowTime && cnt >= 0 && notes_load_flag == true) {
		
		//if (int(fmod(double(cnt), (3600 / bpm))) == 0 && cnt >= 0) {
		bpm_count2++;
		bpm_time = 60.0 / bpm * bpm_count2;

		if (bpm_count % 4 == 0) {

			switch (sec_count) {
			case 0:
				SecTime[0] = NowTime;
				break;
			case 1:
				SecTime[1] = NowTime;
				break;
			case 2:
				SecTime[2] = NowTime;
				break;
			case 3:
				SecTime[3] = NowTime;
				break;
			}
			bpm_tempo_flag = true;
			tempo_time[0] = NowTime;
			notes_count = 0;
			if (sec_count == 0) sec_time = NowTime;
			while (tja_notes[sec_count][notes_count] != ',') {
				if (notes_count == 0 && tja_notes[sec_count][0] == '#') {
					if (tja_notes[sec_count][1] == 'E' &&
						tja_notes[sec_count][2] == 'N' &&
						tja_notes[sec_count][3] == 'D') {
						notes_load_flag = false;
						break;
					}
					notes_count = 0;
					sec_count++;
					continue;
				}
				if (notes_count == 0 && strstr(tja_notes[sec_count], ",") == NULL) {
					notes_count = 0;
					sec_count++;
					continue;
				}
				notes_count++;
			}

			for (int i = 0; i < notes_count; i++) {

				int id = notes_id_check();
				
				if (id != -1 && tja_notes[sec_count][i] != '0') {

					speed = Notes_Area / (3600 / bpm * tempo);

					if (renda_flag == 0 || tja_notes[sec_count][i] == '8') {

						Notes[id].flag = true;
						Notes[id].notes_max = notes_count;	//�p�r�s��
						Notes[id].notes_number = i;
						Notes[id].x_ini = (Notes_Area / notes_count)*i + 400.0;
						Notes[id].x = Notes[id].x_ini;
						Notes[id].bpm = bpm;
						Notes[id].kind = ctoi(tja_notes[sec_count][i]);
						Notes[id].sec = sec_count;
						Notes[id].create_time = NowTime;
						Notes[id].judge_time = Notes[id].create_time + (Notes[id].x_ini - Notes_Judge) / (Notes_Area / (60 / bpm * tempo));
						Notes[id].bpm = bpm;

						switch (Notes[id].kind) {

						case Renda:
							renda_flag = 1;
							break;
						case BigRenda:
							renda_flag = 2;
							break;
						case RendaEnd:
							switch (renda_flag) {
							case 1:
								Notes[id].kind = 8;
								break;
							case 2:
								Notes[id].kind = 9;
								break;
							}
							renda_flag = 0;
							break;
						}
					}
				}
			}
			bar_x = 400.0;
			sec_count++;
		}
		bpm_count++;
	}
	notes_main_count++;

	if (bpm_tempo_flag == true) bar_x = Notes_Area - Notes_Area * (NowTime - tempo_time[0]) / (60.0*tempo / bpm);
	

	C2D_DrawRectangle(bar_x,86,0,1,46, C2D_Color32f(1,1,1,1), C2D_Color32f(1,1,1,1), C2D_Color32f(1,1,1,1), C2D_Color32f(1,1,1,1));
	notes_calc(isDon,isKa,bpm, tempo, NowTime, cnt);
	C2D_DrawRectangle(0 ,86, 0, 62, 58, C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1));
	snprintf(buf_notes, sizeof(buf_notes), "%s", tja_notes[sec_count - 1]);
	debug_draw(0, 40, tja_notes[sec_count - 1]);

	snprintf(buf_notes, sizeof(buf_notes), "notes_cnt:%d", notes_count);
	debug_draw(300, 30, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "bar_x:%2f", bar_x);
	debug_draw(300, 40, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "speed:%.2f", Notes_Area / (3600 / bpm * tempo));
	debug_draw(300, 50, buf_notes);
}

int notes_id_check() {

	for (int i = 0; i < Notes_Max; i++) {

		if (Notes[i].flag == false) {

			return i;
		}
	}
	return -1;
}

bool isJudgeDisp = false;	//�v������
double JudgeMakeTime;		//
int JudgeDispKind;			//

void make_judge(int kind,double NowTime) {
	isJudgeDisp = true;
	JudgeMakeTime = NowTime;
	JudgeDispKind = kind;
}

void calc_judge(double NowTime) {

	if (isJudgeDisp == true) {
		switch (JudgeDispKind) {
		case 0:		//��
			//sftd_draw_textf(font, 80, 80, RGBA8(255, 0, 0, 255), 10, "Ryou");
			debug_draw(80, 80, "ryou");
			break;
		case 1:		//��
			//sftd_draw_textf(font, 80, 80, RGBA8(255, 255, 255, 255), 10, "Ka");
			debug_draw(80, 80, "ka");
			break;
		case 2:		//�s��
			//sftd_draw_textf(font, 80, 80, RGBA8(0, 255, 255, 255), 10, "Huka");
			debug_draw(80, 80, "fuka");
			break;
		}
		if (NowTime - JudgeMakeTime >= 0.25) isJudgeDisp = false;
	}
}

void notes_judge(double NowTime, bool isDon, bool isKa) {
	
	bool isAuto = true;
	int CurrentJudgeNotes[2] = { -1,-1 };		//���ݔ��肷�ׂ��m�[�c �h��,�J�c
	double CurrentJudgeNotesLag[2] = { -1,-1 };	//���肷�ׂ��m�[�c�̌덷(s)

	for (int i = 0; i < Notes_Max; i++) {

		if (Notes[i].flag == true) {

			if (Notes[i].kind == Don ||
				Notes[i].kind == BigDon ||
				Notes[i].kind == Renda ||
				Notes[i].kind == BigRenda ||
				Notes[i].kind == Balloon ||
				Notes[i].kind == RendaEnd ||
				Notes[i].kind == Potato) {	//�h��

				if (CurrentJudgeNotesLag[0] > fabs(Notes[i].judge_time - NowTime) ||
					CurrentJudgeNotesLag[0] == -1) {

					CurrentJudgeNotes[0] = i;
					CurrentJudgeNotesLag[0] = fabs(Notes[i].judge_time - NowTime);
				}
			}
			else if (
				Notes[i].kind == Ka ||
				Notes[i].kind == BigKa) {	//�J�c

				if (CurrentJudgeNotesLag[1] > fabs(Notes[i].judge_time - NowTime) ||
					CurrentJudgeNotesLag[1] == -1) {

					CurrentJudgeNotes[1] = i;
					CurrentJudgeNotesLag[1] = fabs(Notes[i].judge_time - NowTime);
				}
			}
		}
	}

	for (int n = 0; n < 2; n++) {

		if (CurrentJudgeNotes[n] != -1) {

			if (isAuto == true &&	//�I�[�g
				Notes[CurrentJudgeNotes[n]].judge_time <= NowTime) {

				music_play(n);
				Notes[CurrentJudgeNotes[n]].flag = false;
			}
		}
	}

	if (isAuto == false) {			//�蓮
		
		if (isDon == true && CurrentJudgeNotes[0] != -1) {	//�h��

			if (CurrentJudgeNotesLag[0] <= 0.034) {			//��
				make_judge(0,NowTime);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
			else if (CurrentJudgeNotesLag[0] <= 0.117) {	//��
				make_judge(1, NowTime);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
			else if (CurrentJudgeNotesLag[0] <= 0.150) {	//�s��
				make_judge(2, NowTime);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
		}

		if (isKa == true && CurrentJudgeNotes[1] != -1) {			//�J�c

			if (CurrentJudgeNotesLag[1] <= 0.034) {			//��
				make_judge(0, NowTime);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
			else if (CurrentJudgeNotesLag[1] <= 0.117) {	//��
				make_judge(1, NowTime);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
			else if (CurrentJudgeNotesLag[1] <= 0.150) {	//�s��
				make_judge(2, NowTime);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
		}
	}

	snprintf(buf_notes, sizeof(buf_notes), "CurretDon:%d:::%.1f:::%.2f", CurrentJudgeNotes[0], Notes[CurrentJudgeNotes[0]].judge_time, Notes[CurrentJudgeNotes[0]].judge_time - NowTime );
	debug_draw(50, 160, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "CurretKa  :%d:::%.1f:::%.2f", CurrentJudgeNotes[1], Notes[CurrentJudgeNotes[1]].judge_time, Notes[CurrentJudgeNotes[1]].judge_time - NowTime);
	debug_draw(50, 170, buf_notes);	
	//sftd_draw_textf(font, 100, 40, RGBA8(0, 255, 0, 255), 10, "%f", CurrentJudgeNotesLag[0]);
	//sftd_draw_textf(font, 100, 50, RGBA8(0, 255, 0, 255), 10, "%f", CurrentJudgeNotesLag[1]);
}


void notes_calc(bool isDon,bool isKa,double bpm, double tempo, double NowTime, int cnt) {
	
	//int small_y = 95, big_y = 90;
	int notes_y = 108;
	
	for (int i = 64; i >= 0; i--) {
		
		if (Notes[i].flag == true) {
			
			//������邯�ǐ���ɍ쓮
			Notes[i].x = Notes[i].x_ini - Notes_Area * (NowTime - Notes[i].create_time) / (60 /Notes[i].bpm * tempo);
			//Notes[i].x = Notes[i].x_ini - (NowTime - Notes[i].create_time) * Notes[i].speed;

			//Notes[i].x_ini or speed ���o�O�̌����I�H

			snprintf(buf_notes, sizeof(buf_notes), "%d", Notes[i].kind);
			debug_draw(Notes[i].x, notes_y-5, buf_notes);
			snprintf(buf_notes, sizeof(buf_notes), "%d", i);
			debug_draw(Notes[i].x, notes_y+23, buf_notes);
			//�����Ńm�[�c�`���switch

			//sftd_draw_textf(font, Notes[i].x, 132, RGBA8(0, 255, 0, 255), 10, "%d", i);
			if (Notes[i].x <= 40 ) Notes[i].flag = false;
		}
	}
	//sftd_draw_textf(font, 100, 60, RGBA8(0, 255, 0, 255), 10, "%f", NowTime);
	notes_judge(NowTime, isDon, isKa);
	calc_judge(NowTime);
}