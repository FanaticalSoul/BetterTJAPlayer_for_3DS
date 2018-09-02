#include "header.h"
#include "main.h"
#include "time.h"
#include "audio.h"
#include "tja.h"
#include "notes.h"

#define Notes_Max 512
#define BarLine_Max 64

double bpm, offset;
char buf_notes[160];

int find_notes_id(), find_line_id();
void notes_calc(bool isDon, bool isKa,double bpm, double NowTime, int cnt, C2D_Sprite sprites[12]);


NOTES_T Notes[Notes_Max];
COMMAND_T Command;
BARLINE_T BarLine[BarLine_Max];

int MeasureCount = 0;
int RendaState = 0;
int NotesCount;	
int NotesNumber = 0;	//���Ԗڂ̃m�[�c��
bool isNotesLoad= true;


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

void notes_q_sort(NOTES_T numbers[], int left, int right){

	int l_hold, r_hold,pivot;

	l_hold = left;
	r_hold = right;
	pivot = numbers[left].num;
	while (left < right)
	{
		while ((numbers[right].num >= pivot) && (left < right))
			right--;
		if (left != right)
		{
			numbers[left].num = numbers[right].num;
			left++;
		}
		while ((numbers[left].num <= pivot) && (left < right))
			left++;
		if (left != right)
		{
			numbers[right].num = numbers[left].num;
			right--;
		}
	}
	numbers[left].num = pivot;
	pivot = left;
	left = l_hold;
	right = r_hold;
	if (left < pivot)
		notes_q_sort(numbers, left, pivot - 1);
	if (right > pivot)
		notes_q_sort(numbers, pivot + 1, right);
}

void notes_structure_init() {

	for (int i = 0; i < Notes_Max; i++) {
		Notes[i].num = 0;
		Notes[i].knd = 0;
		Notes[i].notes_max = 0;
		Notes[i].x_ini = 0;
		Notes[i].x = 0;
		Notes[i].create_time = 0;
		Notes[i].judge_time = 0;
		Notes[i].pop_time = 0;
		Notes[i].bpm = 0;
		Notes[i].scroll = 0;
		Notes[i].flag = false;
	}
}

void notes_init(TJA_HEADER_T Tja_Header) {
	
	notes_structure_init();
	tja_notes_load();
	Command.data[0] = 0; Command.data[1] = 0; Command.data[2] = 0;
	Command.knd = 0;
	Command.val = 0;
	bpm = Tja_Header.bpm;
	offset = Tja_Header.offset;
	NotesNumber = 0;
	MeasureCount = 0;

}

void notes_main(bool isDon,bool isKa, char tja_notes[Measure_Max][Max_Notes_Measure],MEASURE_T Measure[Measure_Max], int cnt, C2D_Sprite  sprites[12]) {

	//�ŏ��̏��߂�create_time���}�C�i�X���������p�ɒ���
	double NowTime = time_now(0) + Measure[0].create_time;

	snprintf(buf_notes, sizeof(buf_notes), "time0:%.2f", NowTime);
	debug_draw(0, 10, buf_notes);

	bool isEnd = false;

	if ( cnt >= 0 && isNotesLoad== true) {

		if (Measure[MeasureCount].create_time <= NowTime) {

			int BarLineId = find_line_id();
			if (BarLineId != -1) {
				BarLine[BarLineId].flag = true;
				BarLine[BarLineId].scroll = Measure[MeasureCount].scroll;
				BarLine[BarLineId].x_ini = 
					Notes_Judge_Range*BarLine[BarLineId].scroll + Notes_Judge;
				BarLine[BarLineId].x = BarLine[BarLineId].x_ini;
				BarLine[BarLineId].measure = MeasureCount;
				BarLine[BarLineId].create_time = NowTime;
				BarLine[BarLineId].isDisp = Measure[MeasureCount].isDispBarLine;
			}

			NotesCount = 0;
			
			while (tja_notes[Measure[MeasureCount].notes][NotesCount] != ',') {

				//����
				if (NotesCount == 0 && tja_notes[Measure[MeasureCount].notes][0] == '#') {

					get_command_value(tja_notes[Measure[MeasureCount].notes],&Command);
					Command.notes = tja_notes[Measure[MeasureCount].notes];

					switch (Command.knd) {
					case ENd:
						isNotesLoad= false;						
						isEnd = true;
						break;

					default:
						break;

					}

					if (isEnd == true) break;

					NotesCount = 0;
					MeasureCount++;
					continue;
				}
				if (NotesCount == 0 && strstr(tja_notes[Measure[MeasureCount].notes], ",") == NULL) {
					NotesCount = 0;
					MeasureCount++;
					continue;
				}
				NotesCount++;
			}

			for (int i = 0; i < NotesCount; i++) {

				int id = find_notes_id();
				
				if (id != -1 && tja_notes[Measure[MeasureCount].notes][i] != '0') {

					if (RendaState == 0 || tja_notes[Measure[MeasureCount].notes][i] == '8') {

						Notes[id].flag = true;
						Notes[id].notes_max = NotesCount;	//�p�r�s��
						Notes[id].num = NotesNumber;
						Notes[id].scroll = Measure[MeasureCount].scroll;
						Notes[id].x_ini = ((Notes_Area*Measure[MeasureCount].measure / NotesCount)*i + Notes_Judge_Range)*Notes[id].scroll+ Notes_Judge;
						Notes[id].x = Notes[id].x_ini;
						Notes[id].bpm = Measure[MeasureCount].bpm;
						Notes[id].knd = ctoi(tja_notes[Measure[MeasureCount].notes][i]);
						//Notes[id].create_time = NowTime;
						Notes[id].pop_time = Measure[MeasureCount].pop_time;
						Notes[id].judge_time = Measure[MeasureCount].judge_time + 60.0 / Measure[MeasureCount].bpm * 4 * Measure[MeasureCount].measure * i / NotesCount;
						switch (Notes[id].knd) {

						case Renda:
							RendaState = Renda;
							break;
						case BigRenda:
							RendaState = BigRenda;
							break;
						case RendaEnd:
							switch (RendaState) {
							case Renda:
								Notes[id].knd = RendaEnd;
								break;
							case BigRendaEnd:
								Notes[id].knd = BigRendaEnd;
								break;
							default:
								Notes[id].flag = false;
								break;
							}
							RendaState = 0;
							break;
						}

						NotesNumber++;
					}
				}
			}
			MeasureCount++;

			notes_q_sort(Notes, 0, Notes_Max - 1);
			//notes_sort();	//�\�[�g
		}

	}

	
	for (int i = 0; i < (BarLine_Max - 1); i++) {

		if (BarLine[i].flag == true) {

			BarLine[i].x = BarLine[i].x_ini -
				Notes_Area * BarLine[i].scroll * (NowTime - Measure[BarLine[i].measure].pop_time) / (60 / Measure[BarLine[i].measure].bpm * 4);
			if (BarLine[i].isDisp == true)
				C2D_DrawRectangle(BarLine[i].x, 86, 0, 1, 46, C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1));
			
			if (BarLine[i].x < 62) BarLine[i].flag = false;
			//snprintf(buf_notes, sizeof(buf_notes), "%.1f", Measure[BarLine[i].measure].judge_time);
			//debug_draw(BarLine[i].x-10, 133, buf_notes);
		}
	}

	notes_calc(isDon,isKa,bpm, NowTime, cnt, sprites);
	C2D_DrawRectangle(0 ,86, 0, 62, 58, C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1));
	debug_draw(0, 40, tja_notes[Measure[MeasureCount].notes - 1]);


	snprintf(buf_notes, sizeof(buf_notes), "time1:%.2f", NowTime);
	debug_draw(0, 0, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "cnt :%d", cnt);
	debug_draw(150, 0, buf_notes);

	snprintf(buf_notes, sizeof(buf_notes), "judge :%.1f", Measure[MeasureCount].judge_time);
	debug_draw(100, 30, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "bpm :%.1f", Measure[MeasureCount].bpm);
	debug_draw(0, 30, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "create :%.1f", Measure[MeasureCount].create_time);
	debug_draw(200, 30, buf_notes);

	snprintf(buf_notes, sizeof(buf_notes), "knd:%d", Command.knd);
	debug_draw(0, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "val:%.2f", Command.val);
	debug_draw(100, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "COM:%s", Command.command_s);
	debug_draw(200, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "VAL:%s", Command.value_s);
	debug_draw(200, 180, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "%s", Command.notes);
	debug_draw(0, 180, buf_notes);
}

int find_notes_id() {

	for (int i = 0; i < Notes_Max-1; i++) {

		if (Notes[i].flag == false) return i;
	}
	return -1;
}

int find_line_id() {

	for (int i = 0; i < BarLine_Max - 1; i++) {

		if (BarLine[i].flag == false) return i;
	}
	return -1;
}

bool isJudgeDisp = false;	//�v������
double JudgeMakeTime;		//
int JudgeDispknd;			//

void make_judge(int knd,double NowTime) {
	isJudgeDisp = true;
	JudgeMakeTime = NowTime;
	JudgeDispknd = knd;
}

void calc_judge(double NowTime) {

	if (isJudgeDisp == true) {
		switch (JudgeDispknd) {
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

			if (Notes[i].knd == Don ||
				Notes[i].knd == BigDon ||
				Notes[i].knd == Renda ||
				Notes[i].knd == BigRenda ||
				Notes[i].knd == Balloon ||
				Notes[i].knd == RendaEnd ||
				Notes[i].knd == Potato) {	//�h��

				if (CurrentJudgeNotesLag[0] > fabs(Notes[i].judge_time - NowTime) ||
					CurrentJudgeNotesLag[0] == -1) {

					CurrentJudgeNotes[0] = i;
					CurrentJudgeNotesLag[0] = fabs(Notes[i].judge_time - NowTime);
				}
			}
			else if (
				Notes[i].knd == Ka ||
				Notes[i].knd == BigKa) {	//�J�c

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
					Notes[i].knd == Don ||
					Notes[i].knd == BigDon ||
					Notes[i].knd == Renda ||
					Notes[i].knd == BigRenda ||
					Notes[i].knd == Balloon ||
					Notes[i].knd == RendaEnd ||
					Notes[i].knd == Potato) {
					
					isSe[0] = true;
					music_play(0);
				}
				else if (
					isSe[1] == false ||
					Notes[i].knd == Ka ||
					Notes[i].knd == BigKa) {

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

void notes_calc(bool isDon,bool isKa,double bpm, double NowTime, int cnt, C2D_Sprite sprites[12]) {
	
	//int small_y = 95, big_y = 90;
	int notes_y = 109;
	
	for (int i = 0; i < Notes_Max; i++) {	//�v�Z
		
		if (Notes[i].flag == true) {
			
			Notes[i].x = Notes[i].x_ini -
				Notes_Area* Notes[i].scroll * (NowTime - Notes[i].pop_time) / (60 / Notes[i].bpm * 4);


		}
			
		if (Notes[i].x <= 40) Notes[i].flag = false;
	}

	for (int i = 0; i < Notes_Max; i++) {	//�`��

		if (Notes[i].flag == true) {

			switch (Notes[i].knd) {
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
				C2D_SpriteSetPos(&sprites[bAlloon], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bAlloon]);
				break;
			case RendaEnd:
				C2D_SpriteSetPos(&sprites[rEnda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[rEnda_fini]);
				break;
			case BigRendaEnd:
				C2D_SpriteSetPos(&sprites[bIg_renda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda_fini]);
				break;
			default:
				break;
			}
			snprintf(buf_notes, sizeof(buf_notes), "%d:%d",i,(Notes[i].flag == true && Notes[i].judge_time <= NowTime));
			debug_draw(Notes[i].x-5, 132, buf_notes);
		}
	}
	notes_judge(NowTime, isDon, isKa);
	calc_judge(NowTime);
}