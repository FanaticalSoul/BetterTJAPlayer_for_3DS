﻿#include "header.h"
#include "notes.h"
#include "tja.h"
#include "score.h"


char tja_notes[Measure_Max][Max_Notes_Measure];
int tja_cnt = 0, MeasureMaxNumber = 0;
double MainFirstMeasureTime;
bool isBranch = false;


TJA_HEADER_T Current_Header;
MEASURE_T Measure[Measure_Max];

void get_command_value(char* buf, COMMAND_T *Command);

void measure_structure_init() {

	for (int i = 0; i < Measure_Max; i++) {

		Measure[i].create_time = INT_MAX;
		Measure[i].judge_time = INT_MAX;
		Measure[i].pop_time = INT_MAX;
		Measure[i].bpm = 0;
		Measure[i].scroll = 0;
		Measure[i].notes = 0;
		Measure[i].flag = false;
		Measure[i].isDispBarLine = true;
		Measure[i].firstmeasure = -1;
		Measure[i].start_measure_count = 0;
		Measure[i].max_notes = 0;
		Measure[i].original_id = -1;
		Measure[i].notes_count = 0;
		Measure[i].command = -1;
	}
}

void tja_init() {
	measure_structure_init();
	tja_cnt = 0;
	MeasureMaxNumber = 0;
	MainFirstMeasureTime = 0;
	isBranch = false;
}

void tja_head_load(int course) {

	FILE *fp;
	char buf[128];
	bool isCourseMatch = true;

	if ((fp = fopen("sdmc:/tjafiles/" File_Name "/" File_Name ".tja", "r")) != NULL) {

		Current_Header.title = (char*)"No title";
		Current_Header.subtitle = (char*)"";
		Current_Header.level = 0;
		Current_Header.bpm = 60.0;
		Current_Header.wave = (char*)"audio.ogg";
		Current_Header.offset = 0;
		Current_Header.balloon[0] = 5;
		Current_Header.songvol = 100;
		Current_Header.sevol = 100;
		Current_Header.scoreinit = -1;
		Current_Header.scorediff = -1;
		Current_Header.course = 3;
		Current_Header.style = 1;
		Current_Header.life = -1;
		Current_Header.demostart = 0;
		Current_Header.side = 3;
		Current_Header.scoremode = 1;

		char* temp = NULL;
		while (fgets(buf, 128, fp) != NULL) {

			temp = (char *)malloc((strlen(buf) + 1));


			if (isCourseMatch == true && strstr(buf, "#START") == buf) {
				break;
			}

			if (strstr(buf, "TITLE:") == buf) {
				strlcpy(temp, buf + 6, strlen(buf) - 7);
				Current_Header.title = temp;
				continue;
			}

			if (strstr(buf, "SUBTITLE:") == buf) {
				strlcpy(temp, buf + 9, strlen(buf) - 10);
				Current_Header.subtitle = temp;
				continue;
			}

			if (strstr(buf, "LEVEL:") == buf) {
				strlcpy(temp, buf + 6, strlen(buf) - 7);
				Current_Header.level = atoi(temp);
				continue;
			}

			if (strstr(buf, "BPM:") == buf) {
				strlcpy(temp, buf + 4, strlen(buf) - 5);
				Current_Header.bpm = atof(temp);
				continue;
			}

			if (strstr(buf, "WAVE:") == buf) {
				strlcpy(temp, buf + 5, strlen(buf) - 6);
				Current_Header.wave = temp;
				continue;
			}

			if (strstr(buf, "OFFSET:") == buf) {
				strlcpy(temp, buf + 7, strlen(buf) - 8);
				Current_Header.offset = atof(temp);
				continue;
			}

			if (strstr(buf, "BALLOON:") == buf) {
				strlcpy(temp, buf + 8, strlen(buf) - 9);
				char *tp = strtok(temp, ",");
				Current_Header.balloon[0] = atoi(tp);
				int cnt = 1;
				while ((tp = strtok(NULL, ","))) {
					Current_Header.balloon[cnt] = atoi(tp);
					cnt++;
				}
				continue;
			}

			if (strstr(buf, "SONGVOL:") == buf) {
				strlcpy(temp, buf + 8, strlen(buf) - 9);
				Current_Header.songvol = atoi(temp);
				continue;
			}

			if (strstr(buf, "SEVOL:") == buf) {
				strlcpy(temp, buf + 6, strlen(buf) - 7);
				Current_Header.sevol = atoi(temp);
				continue;
			}

			if (strstr(buf, "SCOREINIT:") == buf) {
				strlcpy(temp, buf + 10, strlen(buf) - 11);
				Current_Header.scoreinit = atoi(temp);
				continue;
			}

			if (strstr(buf, "SCOREDIFF:") == buf) {
				strlcpy(temp, buf + 10, strlen(buf) - 11);
				Current_Header.scorediff = atoi(temp);
				continue;
			}


			if (strstr(buf, "COURSE:") == buf) {
				strlcpy(temp, buf + 7, strlen(buf) - 8);
				if (strlen(temp) == 1) Current_Header.course = atoi(temp);		//数字表記
				else if (strcmp(temp, "Easy") == 0) Current_Header.course = 0;	//文字表記
				else if (strcmp(temp, "Normal") == 0) Current_Header.course = 1;
				else if (strcmp(temp, "Hard") == 0) Current_Header.course = 2;
				else if (strcmp(temp, "Oni") == 0) Current_Header.course = 3;
				else if (strcmp(temp, "Edit") == 0) Current_Header.course = 4;
				
				if (Current_Header.course == course) isCourseMatch = true;
				else isCourseMatch = false;

				continue;
			}

			if (strstr(buf, "STYLE:") == buf) {
				strlcpy(temp, buf + 6, strlen(buf) - 7);
				Current_Header.style = atoi(temp);
				continue;
			}

			if (strstr(buf, "LIFE:") == buf) {
				strlcpy(temp, buf + 5, strlen(buf) - 6);
				Current_Header.life = atoi(temp);
				continue;
			}

			if (strstr(buf, "DEMOSTART:") == buf) {
				strlcpy(temp, buf + 10, strlen(buf) - 11);
				Current_Header.demostart = atof(temp);
				continue;
			}

			if (strstr(buf, "SIDE:") == buf) {
				strlcpy(temp, buf + 5, strlen(buf) - 6);
				Current_Header.side = atoi(temp);
				continue;
			}

			if (strstr(buf, "SCOREMODE:") == buf) {
				strlcpy(temp, buf + 10, strlen(buf) - 11);
				Current_Header.scoremode = atoi(temp);
				continue;
			}

			free(temp);
		}

		fclose(fp);
		free(temp);

	}
	else {
		//tjaファイルが開けなかった時
	}
}

void MeasureInsertionSort(MEASURE_T t[], int array_size) {

	for (int i = 1; i < array_size; i++) {

		MEASURE_T temp = t[i];
		if (t[i - 1].create_time > temp.create_time) {

			int j = i;
			do {
				t[j] = t[j - 1];
				--j;
			} while (j > 0 && t[j - 1].create_time > temp.create_time);
			t[j] = temp;
		}
	}
}

void tja_notes_load(int course) {

	int FirstMultiMeasure = -1,	//複数行の小節の最初の小節id 複数出ない場合は-1
		NotesCount = 0, BranchCourse = -1;
	bool isStart = false, isEnd = false, isDispBarLine = true, isNoComma = false,isCourseMatch = false;
	FILE *fp;
	COMMAND_T Command;
	double bpm = Current_Header.bpm,
		NextBpm = bpm,
		measure = 1,
		scroll = 1,
		NextMeasure = 1,
		delay = 0,
		percent = 1,
		BeforeBranchJudgeTime = 0,
		BeforeBranchCreateTime = 0,
		BeforeBranchPreJudge = 0,
		BeforeBranchBpm = 0,
		BeforeBranchDelay = 0,
		BeforeBranchMeasure = 0,
		BeforeBranchScroll = 1;

	if ((fp = fopen("sdmc:/tjafiles/" File_Name  "/" File_Name ".tja", "r")) != NULL) {

		tja_cnt = 0;
		int MeasureCount = 0,CurrentCourse = -1;
		double PreJudge = 0, FirstMeasureTime = 0;

		FirstMeasureTime = (60.0 / bpm * 4 * measure)*(Notes_Judge_Range / Notes_Area) - 60.0 / bpm * 4 * measure;
		PreJudge = FirstMeasureTime;


		while (
			(fgets(tja_notes[tja_cnt], Max_Notes_Measure, fp) != NULL || tja_cnt < Measure_Max) &&
			isEnd == false
			) {

			if (strstr(tja_notes[tja_cnt], "COURSE:") == tja_notes[tja_cnt]) {

				char* temp = NULL;
				temp = (char *)malloc((strlen(tja_notes[tja_cnt]) + 1));

				strlcpy(temp, tja_notes[tja_cnt] + 7, strlen(tja_notes[tja_cnt]) - 8);
				if (strlen(temp) == 1) CurrentCourse = atoi(temp);		//数字表記
				else if (strcmp(temp, "Easy") == 0) CurrentCourse = 0;	//文字表記
				else if (strcmp(temp, "Normal") == 0) CurrentCourse = 1;
				else if (strcmp(temp, "Hard") == 0) CurrentCourse = 2;
				else if (strcmp(temp, "Oni") == 0) CurrentCourse = 3;
				else if (strcmp(temp, "Edit") == 0) CurrentCourse = 4;

				free(temp);

				if (course == CurrentCourse) isCourseMatch = true;

				continue;
			}

			if (isStart == false && isCourseMatch == true && strstr(tja_notes[tja_cnt], "#START") == tja_notes[tja_cnt]) {

				isStart = true;
				continue;
			}

			if (isStart == true && isCourseMatch == true) {

				//一文字目がコメントアウトの時スキップ
				if (strstr(tja_notes[tja_cnt], "//") == tja_notes[tja_cnt] || strstr(tja_notes[tja_cnt], "\r") == tja_notes[tja_cnt]) {

					tja_cnt++;
					continue;
				}

				if (strstr(tja_notes[tja_cnt], ",") == NULL && tja_notes[tja_cnt][0] != '#') {
					isNoComma = true;

					if (FirstMultiMeasure == -1) {

						FirstMultiMeasure = MeasureCount;
						Measure[FirstMultiMeasure].original_id = FirstMultiMeasure;	//ソート前のidを格納
					}
				}
				else {
					isNoComma = false;
				}

				if (tja_notes[tja_cnt][0] == '#') {

					get_command_value(tja_notes[tja_cnt], &Command);
					Measure[MeasureCount].command = Command.knd;
					switch (Command.knd) {
					case BPmchange:
						NextBpm = Command.val[0];
						break;
					case MEasure:
						NextMeasure = Command.val[0];
						break;
					case SCroll:
						scroll = Command.val[0];
						break;
					case DElay:
						delay = Command.val[0];
						break;
					case BArlineon:
						isDispBarLine = true;
						break;
					case BArlineoff:
						isDispBarLine = false;
						break;
					case N:
						BranchCourse = N;
						break;
					case E:
						BranchCourse = E;
						break;
					case M:
						BranchCourse = M;
						break;
					case BRanchstart:
						isBranch = true;
						break;
					case BRanchend:
						BranchCourse = -1;
						break;
					case SEction:
						Measure[MeasureCount].command = SEction;
						break;
					case ENd:
						isEnd = true;
						break;
					default:
						break;
					}
				}
				else {

					if (isNoComma == true || NotesCount != 0) {	//複数小節

						Measure[MeasureCount].start_measure_count = NotesCount;
						int i = 0;
						while (tja_notes[tja_cnt][i] != '\n' && tja_notes[tja_cnt][i] != ',' && tja_notes[tja_cnt][i] != '/') i++;
						NotesCount += i - 1;
						if (tja_notes[tja_cnt][i] == '/') NotesCount++;
						if (tja_notes[tja_cnt][i] != ',' && tja_notes[tja_cnt][i] != '/') i--;
						Measure[MeasureCount].notes_count = i;

					}
				}

				Measure[MeasureCount].flag = true;
				Measure[MeasureCount].notes = tja_cnt;
				Measure[MeasureCount].firstmeasure = FirstMultiMeasure;
				Measure[MeasureCount].bpm = NextBpm;
				Measure[MeasureCount].measure = NextMeasure;
				Measure[MeasureCount].scroll = scroll;
				Measure[MeasureCount].judge_time = 60.0 / bpm * 4 * measure * percent + PreJudge + delay;
				Measure[MeasureCount].pop_time = Measure[MeasureCount].judge_time - (60.0 / Measure[MeasureCount].bpm * 4)*(Notes_Judge_Range / Notes_Area);
				Measure[MeasureCount].create_time = Measure[MeasureCount].judge_time - (60.0 / Measure[MeasureCount].bpm * 4)*(Notes_Judge_Range / (Notes_Area*scroll));
				Measure[MeasureCount].isDispBarLine = isDispBarLine;
				Measure[MeasureCount].branch = BranchCourse;

				if (tja_notes[tja_cnt][0] == '#') {

					if (MeasureCount > 0) {
						Measure[MeasureCount].judge_time = Measure[MeasureCount - 1].judge_time;
						Measure[MeasureCount].create_time = Measure[MeasureCount - 1].create_time;
						//Measure[MeasureCount].isDispBarLine = false;
					}
					switch (Command.knd) {
					case BRanchstart:
						BeforeBranchJudgeTime = Measure[MeasureCount].judge_time;
						BeforeBranchCreateTime = Measure[MeasureCount].create_time;
						BeforeBranchBpm = bpm;
						BeforeBranchDelay = delay;
						BeforeBranchMeasure = measure;
						BeforeBranchPreJudge = PreJudge;
						BeforeBranchScroll = scroll;
						break;
					case N:
					case E:
					case M:
						bpm = BeforeBranchBpm;
						measure = BeforeBranchMeasure;
						delay = BeforeBranchDelay;
						scroll = BeforeBranchScroll;
						Measure[MeasureCount].judge_time = BeforeBranchJudgeTime;
						Measure[MeasureCount].create_time = BeforeBranchCreateTime;
						PreJudge = BeforeBranchPreJudge;
						break;
					default:
						break;
					}
				}
				else {
					if (isNoComma == false) PreJudge = Measure[MeasureCount].judge_time;
					bpm = NextBpm;
					measure = NextMeasure;
					delay = 0;
				}


				if (isNoComma == false && NotesCount != 0 && tja_notes[tja_cnt][0] != '#') {	//複数行小節の最後の行

					Measure[Measure[MeasureCount].firstmeasure].max_notes = NotesCount + 1;
					FirstMultiMeasure = -1;
					NotesCount = 0;

					for (int i = 1; i < MeasureCount - Measure[MeasureCount].firstmeasure + 1; i++) {	//judge_timeの調整

						if (tja_notes[Measure[MeasureCount].notes][0] != '#') {	//複数行小節の最初の小節以外

							Measure[Measure[MeasureCount].firstmeasure + i].judge_time =
								Measure[Measure[MeasureCount].firstmeasure + i - 1].judge_time +
								(60.0 / Measure[Measure[MeasureCount].firstmeasure + i - 1].bpm * 4 * Measure[Measure[MeasureCount].firstmeasure + i - 1].measure)
								* Measure[Measure[MeasureCount].firstmeasure + i - 1].notes_count / Measure[Measure[MeasureCount].firstmeasure].max_notes;	//delayはとりあえず放置
							Measure[Measure[MeasureCount].firstmeasure + i].pop_time = Measure[Measure[MeasureCount].firstmeasure + i].judge_time - (60.0 / Measure[Measure[MeasureCount].firstmeasure + i].bpm * 4)*(Notes_Judge_Range / (Notes_Area));
							Measure[Measure[MeasureCount].firstmeasure + i].create_time = Measure[Measure[MeasureCount].firstmeasure + i].judge_time - (60.0 / Measure[Measure[MeasureCount].firstmeasure + i].bpm * 4)*(Notes_Judge_Range / (Notes_Area*Measure[Measure[MeasureCount].firstmeasure + i].scroll));
							percent = (double)Measure[Measure[MeasureCount].firstmeasure + i].notes_count / (double)Measure[Measure[MeasureCount].firstmeasure].max_notes;

							Measure[Measure[MeasureCount].firstmeasure + i].isDispBarLine = false;	//最初の小節は小節線をオフにしない
						}
					}

					PreJudge = Measure[MeasureCount].judge_time;
				}
				else if (tja_notes[tja_cnt][0] != '#') {
					percent = 1;
				}


				if (isEnd == true) {
					break;
				}

				tja_cnt++;
				MeasureCount++;
			}
		}

		MeasureMaxNumber = tja_cnt;

		for (int i = 0; i < MeasureMaxNumber; i++) {	//次の小節の判定時に発動する命令の調整

			if (Measure[i].command == SEction) {
				int n = i + 1;
				while (n <= MeasureMaxNumber && tja_notes[n][0] == '#') n++;
				Measure[i].judge_time = Measure[n].judge_time;
			}
		}

		//基本天井点を計算

		calc_base_score(Measure, tja_notes);

		fclose(fp);
		MeasureInsertionSort(Measure, Measure_Max);
		MainFirstMeasureTime = Measure[0].judge_time - Measure[0].create_time;
	}
}

void get_tja_header(TJA_HEADER_T *TJA_Header) {

	*TJA_Header = Current_Header;
}

void tja_to_notes(bool isDon, bool isKatsu, int count, C2D_Sprite sprites[Sprite_Number]) {

	notes_main(isDon, isKatsu, tja_notes, Measure, count, sprites);

}

//コマンドと値を取り出す
void get_command_value(char* buf, COMMAND_T *Command) {

	bool isComment = false;
	int comment, space, length;

	if (buf[0] == '#') {

		length = strlen(buf);
		comment = 0;

		char* command = (char *)malloc((strlen(buf) + 1));
		char* value = (char *)malloc((strlen(buf) + 1));

		Command->notes = buf;

		if (strstr(buf, "//") != NULL) {	//コメント処理

			comment = strstr(buf, "//") - buf - 1;
			strlcpy(command, buf + 1, comment);
			isComment = true;
		}

		if (strstr(buf, " ") != NULL) {		//値処理

			space = strstr(buf, " ") - buf;

			if (space < comment && isComment == true) {	//値ありコメントあり

				strlcpy(command, buf + 1, space);
				strlcpy(value, buf + 1 + strlen(command), comment - strlen(command) + 1);

			}
			else {	//値ありコメントなし
				strlcpy(command, buf + 1, space);
				strlcpy(value, buf + 1 + strlen(command), length - strlen(command));
			}
		}
		else {	//値なし

			//コメントあり
			if (isComment == true) strlcpy(command, buf + 1, comment + 1);
			//コメントなし 改行あり
			else if (strstr(buf, "\n") != NULL) strlcpy(command, buf + 1, length - 2);
			//コメントなし　改行なし
			else strlcpy(command, buf + 1, length);

			strlcpy(value, "0", 1);
		}


		Command->command_s = command;
		Command->value_s = value;
		Command->val[0] = 0;
		Command->val[1] = 0;
		Command->val[2] = 0;

		if (strcmp(command, "START") == 0) Command->knd = STart;
		else if (strcmp(command, "END") == 0) Command->knd = ENd;
		else if (strcmp(command, "BPMCHANGE") == 0) {
			Command->knd = BPmchange;
			Command->val[0] = strtod(value, NULL);
		}
		else if (strcmp(command, "GOGOSTART") == 0) Command->knd = GOgostart;
		else if (strcmp(command, "GOGOEND") == 0) Command->knd = GOgoend;

		else if (strcmp(command, "MEASURE") == 0) {
			Command->knd = MEasure;
			if (strstr(value, "/") != NULL) {

				int srash = strstr(value, "/") - value;
				char *denominator = (char *)malloc((strlen(buf) + 1)),
					*molecule = (char *)malloc((strlen(buf) + 1));
				strlcpy(molecule, value + 1, srash);
				strlcpy(denominator, value + srash + 1, strlen(buf) - srash);
				Command->val[0] = strtod(molecule, NULL) / strtod(denominator, NULL);
				free(denominator);
				free(molecule);
			}
			else {
				if (strtod(value, NULL) != 0) Command->val[0] = strtod(value, NULL);
				else Command->val[0] = 1.0;
			}
		}
		else if (strcmp(command, "SCROLL") == 0) {
			Command->knd = SCroll;
			Command->val[0] = strtod(value, NULL);
		}
		else if (strcmp(command, "DELAY") == 0) {
			Command->knd = DElay;
			Command->val[0] = strtod(value, NULL);
		}
		else if (strcmp(command, "SECTION") == 0) Command->knd = SEction;
		else if (strcmp(command, "BRANCHSTART") == 0) {
			Command->knd = BRanchstart;
			char* tp;
			tp = strtok(value, ",");
			switch (value[1]) {
			case 'r':Command->val[0] = 0; break;	//連打
			case 'p':Command->val[0] = 1; break;	//精度
			case 's':Command->val[0] = 2; break;	//スコア
			default: break;
			}
			int i = 1;
			while ((tp = strtok(NULL, ","))) {
				Command->val[i] = strtod(tp, NULL);
				i++;
			}
		}
		else if (strcmp(command, "BRANCHEND") == 0) Command->knd = BRanchend;
		else if (strcmp(command, "N") == 0) Command->knd = N;
		else if (strcmp(command, "E") == 0) Command->knd = E;
		else if (strcmp(command, "M") == 0) Command->knd = M;
		else if (strcmp(command, "LEVELHOLD") == 0) Command->knd = LEvelhold;
		else if (strcmp(command, "BMSCROLl") == 0) Command->knd = BMscroll;
		else if (strcmp(command, "HBSCROLL") == 0) Command->knd = HBscroll;
		else if (strcmp(command, "BARLINEOFF") == 0) Command->knd = BArlineoff;
		else if (strcmp(command, "BARLINEON") == 0) Command->knd = BArlineon;
		else Command->knd = -1;

		free(command);
		free(value);
	}

	else Command->knd = -1;
}

double get_FirstMeasureTime() {
	return MainFirstMeasureTime;
}

int MeasureIdFromOriginalId(int id) {

	for (int i = 0; i < Measure_Max; i++) {

		if (Measure[i].original_id == id) return i;
	}
	return -1;
}

bool get_isBranch() {
	return isBranch;
}