#include <limits.h>
#include "header.h"
#include "main.h"
#include "time.h"
#include "audio.h"
#include "tja.h"
#include "notes.h"

#define Notes_Area 400.0
#define Notes_Judge 93
#define Notes_Max 512

char buf_notes[160];

int notes_id_check(); 
void notes_calc(bool isDon, bool isKa,double bpm, double tempo, double NowTime, int cnt, C2D_Sprite sprites[12]);


NOTES_T Notes[512];
COMMAND_T Command;

int bpm_count = 0;
int bpm_count2 = 1;	//����bpm�̎��Ԍv���p
int sec_count = 0;
int notes_main_count = 0;
int renda_flag = 0;
int notes_count;	
int notes_number = 0;	//���Ԗڂ̃m�[�c��
double bar_x,
speed,
bpm_time,
NextMeasureTime,
PreMeasureTime,
PreBpmTime;
double tempo_time[2], sec_time;
double SecTime[4] = { 0,0,0,0 };	//�f�o�b�O�p
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

int tempo;
double bpm,offset;

int cmp(const void *p, const void *q) {	//��r�p

	int pp = ((NOTES_T*)p)->num;
	int qq = ((NOTES_T*)q)->num;

	if (((NOTES_T*)p)->flag == false) pp = INT_MAX;
	if (((NOTES_T*)p)->flag == false) qq = INT_MAX;

	return qq - pp;
}

void noted_sort() {	//�m�[�c���o�����Ƀ\�[�g

	int n = sizeof Notes / sizeof(NOTES_T);
	qsort(Notes, n, sizeof(NOTES_T), cmp);
}

void notes_init(TJA_HEADER_T Tja_Header) {

	Command.data[0] = 0; Command.data[1] = 0; Command.data[2] = 0;
	Command.knd = 0;
	Command.val = 0;
	bpm = Tja_Header.bpm;
	offset = Tja_Header.offset;
	tempo = 4;
	PreBpmTime = 0;
	notes_number = 0;

}

void notes_main(bool isDon,bool isKa, char tja_notes[2048][Max_Notes_Measure], int cnt, C2D_Sprite  sprites[12]) {

	double NowTime = time_now(0);

	snprintf(buf_notes, sizeof(buf_notes), "NowTime0:%.2f", NowTime);
	debug_draw(0, 10, buf_notes);

	if (notes_main_count == 0) {

		bpm_time = 60.0 / bpm;
		PreBpmTime = bpm_time;
		NextMeasureTime = 60.0 / bpm;
		PreMeasureTime = NextMeasureTime;
	}

	//sftd_draw_textf(font, 100, 20, RGBA8(0, 255, 0, 255), 10, "BpmTime:%f", bpm_time);
	//sftd_draw_textf(font, 100, 30, RGBA8(0, 255, 0, 255), 10, "BpmCount2:%d", bpm_count2);

	if ( cnt >= 0 && notes_load_flag == true) {
		

		if (NextMeasureTime <= NowTime) {

			PreMeasureTime = NextMeasureTime;
			NextMeasureTime = 60.0 / bpm * tempo + PreMeasureTime;

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

				//����
				if (notes_count == 0 && tja_notes[sec_count][0] == '#') {

					bool isEnd = false,isBpmChange= false;
					get_command_value(tja_notes[sec_count],&Command);
					Command.notes = tja_notes[sec_count];

					switch (Command.knd) {
					case End:
						notes_load_flag = false;						
						isEnd = true;
						break;

					case Bpmchange:
						//bpm = Command.val;
						isBpmChange = true;
						break;

					default:
						break;

					}

					if (isEnd == true) break;
					//if (isBpmChange == true) break;

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
						Notes[id].num = notes_number;
						Notes[id].x_ini = (Notes_Area / notes_count)*i + 400.0;
						Notes[id].x = Notes[id].x_ini;
						Notes[id].bpm = bpm;
						Notes[id].kind = ctoi(tja_notes[sec_count][i]);
						Notes[id].sec = sec_count;
						Notes[id].create_time = NowTime;
						Notes[id].judge_time = Notes[id].create_time + (Notes[id].x_ini - Notes_Judge) / (Notes_Area / (60 / bpm * tempo));

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

						notes_number++;
					}
				}
			}
			bar_x = 400.0;
			sec_count++;

			noted_sort();	//�\�[�g
		}

		//bpm_count++;
	}

	notes_main_count++;

	if (bpm_tempo_flag == true) bar_x = Notes_Area - Notes_Area * (NowTime - tempo_time[0]) / (60.0*tempo / bpm);
	

	C2D_DrawRectangle(bar_x,86,0,1,46, C2D_Color32f(1,1,1,1), C2D_Color32f(1,1,1,1), C2D_Color32f(1,1,1,1), C2D_Color32f(1,1,1,1));
	notes_calc(isDon,isKa,bpm, tempo, NowTime, cnt, sprites);
	C2D_DrawRectangle(0 ,86, 0, 62, 58, C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1));
	snprintf(buf_notes, sizeof(buf_notes), "%s", tja_notes[sec_count - 1]);
	debug_draw(0, 40, tja_notes[sec_count - 1]);


	snprintf(buf_notes, sizeof(buf_notes), "NowTime1:%.2f", NowTime);
	debug_draw(0, 0, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "cnt :%d", cnt);
	debug_draw(150, 0, buf_notes);
	//snprintf(buf_notes, sizeof(buf_notes), "firstsec :%.2f", FirstSecTime);
	//debug_draw(0, 20, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "offset :%.2f", offset);
	debug_draw(100, 20, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "bpm :%.1f", bpm);
	debug_draw(0, 20, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "nextmes :%.2f", NextMeasureTime);
	debug_draw(200, 20, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "Diff :%.2f", NextMeasureTime - PreMeasureTime);
	debug_draw(330, 20, buf_notes);

	snprintf(buf_notes, sizeof(buf_notes), "knd:%d", Command.knd);
	debug_draw(0, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "val:%.2f", Command.val);
	debug_draw(100, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "Com:%s", Command.command);
	debug_draw(200, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "VAL:%s", Command.value);
	debug_draw(200, 180, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "%s", Command.notes);
	debug_draw(0, 180, buf_notes);
	
	snprintf(buf_notes, sizeof(buf_notes), "notes_cnt:%d", notes_count);
	debug_draw(300, 30, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "speed:%.2f", Notes_Area / (3600 / bpm * tempo));
	debug_draw(300, 50, buf_notes);
}

int notes_id_check() {

	for (int i = 0; i < Notes_Max-1; i++) {

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
			debug_draw(80, 80, "ryou");
			break;
		case 1:		//��
			debug_draw(80, 80, "ka");
			break;
		case 2:		//�s��
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

	for (int i = 0; i < Notes_Max-1; i++) {

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

	if (isAuto == true) {	//�I�[�g

		bool isSe[2];

		for (int i = 0; i < Notes_Max-1; i++) {

			if (Notes[i].flag == true && Notes[i].judge_time <= NowTime) {

				if (isSe[0] == false ||
					Notes[i].kind == Don ||
					Notes[i].kind == BigDon ||
					Notes[i].kind == Renda ||
					Notes[i].kind == BigRenda ||
					Notes[i].kind == Balloon ||
					Notes[i].kind == RendaEnd ||
					Notes[i].kind == Potato) {
					
					isSe[0] = true;
					music_play(0);
				}
				else if (
					isSe[1] == false ||
					Notes[i].kind == Ka ||
					Notes[i].kind == BigKa) {

					isSe[1] = true;
					music_play(1);
				}

				Notes[i].flag = false;
			}
		}
	}

	else if (isAuto == false) {			//�蓮
		
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

}

void notes_calc(bool isDon,bool isKa,double bpm, double tempo, double NowTime, int cnt, C2D_Sprite sprites[12]) {
	
	//int small_y = 95, big_y = 90;
	int notes_y = 108;
	
	for (int i = 0; i < Notes_Max; i++) {	//�v�Z
		
		if (Notes[i].flag == true) {
			
			Notes[i].x = Notes[i].x_ini - Notes_Area * (NowTime - Notes[i].create_time) / (60 /Notes[i].bpm * tempo);

			snprintf(buf_notes, sizeof(buf_notes), "%d", i);
			debug_draw(Notes[i].x, notes_y+23, buf_notes);

		}
			
		if (Notes[i].x <= 40 ) Notes[i].flag = false;
	}

	for (int i = 0; i < Notes_Max; i++) {	//�`��

		if (Notes[i].flag == true) {

			switch (Notes[i].kind) {
			case Don:
				C2D_SpriteSetPos(&sprites[dOn], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[dOn]);
				break;
			case Ka:
				C2D_SpriteSetPos(&sprites[kA], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[kA]);
				break;
			case BigDon:
				C2D_SpriteSetPos(&sprites[bIg_don], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_don]);
				break;
			case BigKa:
				C2D_SpriteSetPos(&sprites[bIg_ka], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_ka]);
				break;
			case Renda:
				C2D_SpriteSetPos(&sprites[rEnda], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[rEnda]);
				break;
			case BigRenda:
				C2D_SpriteSetPos(&sprites[bIg_renda], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda]);
				break;
			case Balloon:
				C2D_SpriteSetPos(&sprites[bIg_renda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda_fini]);
				break;
			case RendaEnd:		//8:�A�ŏI�� 9:��A�ŏI��
				C2D_SpriteSetPos(&sprites[rEnda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[rEnda_fini]);
				break;
			case RendaEnd + 1:
				C2D_SpriteSetPos(&sprites[bIg_renda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda_fini]);
				break;
			default:
				break;
			}
		}
	}
	notes_judge(NowTime, isDon, isKa);
	calc_judge(NowTime);
}