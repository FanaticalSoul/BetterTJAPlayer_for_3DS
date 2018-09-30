﻿#include "header.h"
#include "notes.h"
#include "tja.h"
#include "main.h"
#include "score.h"

bool isGOGO;
int combo,init,diff,DiffMul,scoremode,HitScore,ScoreDiff,BaseCeilingPoint;
int CurrentScore,TotalScore,CurrentRollCount, TotalRollCount,TotalRyouCount,TotalKaCount,TotalFukaCount, CurrentRyouCount, CurrentKaCount, CurrentFukaCount;
double tmp,Perfect,CurrentPerfect;
TJA_HEADER_T TJA_Header;
char buf_score[160];
double A, B, C, D;

void score_init() {

	combo = 0;
	get_tja_header(&TJA_Header);
	init = TJA_Header.scoreinit;
	diff = TJA_Header.scorediff;
	DiffMul = 0;
	scoremode = TJA_Header.scoremode;
	HitScore = 0;
	ScoreDiff = 0;
	BaseCeilingPoint = 0;
	CurrentScore = 0;
	TotalScore = 0;
	CurrentRollCount = 0;
	TotalRollCount = 0;
	TotalRyouCount = 0;
	TotalKaCount = 0;
	TotalFukaCount = 0;
	CurrentRyouCount = 0;
	CurrentKaCount = 0;
	CurrentFukaCount = 0;
	Perfect = 0;
	CurrentPerfect = 0;
}


void score_update(int knd) {

	int PreScore = TotalScore;
	bool isCombo = false;

	//if (scoremode == 1 || scoremode == 2) {

		double GOGOMul;
		if (isGOGO == true) GOGOMul = 1.2;
		else GOGOMul = 1.0;

		if (scoremode == 0) {	//ドンダフル配点
			if (combo < 200) DiffMul = 0;
			else DiffMul = 1;
		}
		else if (scoremode == 2) {	//新配点
			if (0 <= combo && combo < 9) DiffMul = 0;
			else if (9 <= combo && combo < 29) DiffMul = 1;
			else if (29 <= combo && combo < 49) DiffMul = 2;
			else if (49 <= combo && combo < 99) DiffMul = 4;
			else if (99 <= combo) DiffMul = 8;
		}
		else if (scoremode == 1) {	//旧配点
			DiffMul = (combo+1)/10;
			if (combo > 100) DiffMul = 10;
		}

		HitScore = init + diff * DiffMul;

		switch (knd) {

		case RYOU:
			TotalScore += round_down(HitScore*GOGOMul);
			CurrentScore += round_down(HitScore*GOGOMul);
			combo++;
			isCombo = true;
			TotalRyouCount++;
			CurrentRyouCount++;
			break;

		case SPECIAL_RYOU:
			TotalScore += round_down(HitScore * GOGOMul) * 2;
			CurrentScore += round_down(HitScore * GOGOMul) * 2;
			combo++;
			isCombo = true;
			TotalRyouCount++;
			CurrentRyouCount++;
			break;

		case KA:
			TotalScore += round_down(HitScore / 2);
			CurrentScore += round_down(HitScore / 2);
			combo++;
			isCombo = true;
			TotalKaCount++;
			CurrentKaCount++;
			break;

		case SPECIAL_KA:
			TotalScore += round_down(HitScore - 10);
			CurrentScore += round_down(HitScore - 10);
			combo++;
			isCombo = true;
			TotalKaCount++;
			CurrentKaCount++;
			break;

		case FUKA:
			combo = 0;
			TotalFukaCount++;
			CurrentFukaCount++;
			break;

		case THROUGH:
			combo = 0;
			TotalFukaCount++;
			CurrentFukaCount++;
			break;

		case BALLOON:
		case ROLL:
			if (scoremode == 0 || scoremode == 1) {	//旧配点
				if (isGOGO == true) {
					TotalScore += 120;
					CurrentScore += 120;
				}
				else {
					TotalScore += 100;
					CurrentScore += 100;
				}
			}
			else if (scoremode == 2) {	//新配点
				if (isGOGO == true) {
					TotalScore += 360;
					CurrentScore += 360;
				}
				else {
					TotalScore += 300;
					CurrentScore += 300;
				}
			}
			CurrentRollCount++;
			TotalRollCount++;
			break;

		case BIG_ROLL:
			if (scoremode == 0 || scoremode == 1) {	//旧配点
				if (isGOGO == true) {
					TotalScore += 430;
					CurrentScore += 430;
				}
				else {
					TotalScore += 360;
					CurrentScore += 360;
				}
			}
			else if (scoremode == 2) {	//新配点
				if (isGOGO == true) {
					TotalScore += 240;
					CurrentScore += 240;
				}
				else {
					TotalScore += 200;
					CurrentScore += 200;
				}
			}
			CurrentRollCount++;
			TotalRollCount++;
			break;

		case BALLOON_BREAK:
			if (isGOGO == true) {
				TotalScore += 6000;
				CurrentScore += 6000;
			}
			else {
				TotalScore += 5000;
				CurrentScore += 5000;
			}
			break;

		default:
			break;
		}
	//}

	if (scoremode == 2) {	//100コンボ毎のボーナス(新配点のみ)
		if (isCombo == true && combo % 100 == 0) {
			TotalScore += 10000;
			CurrentScore += 10000;
		}
	}
	ScoreDiff = TotalScore - PreScore;
	if ((TotalRyouCount + TotalKaCount + TotalFukaCount) != 0) Perfect = (double)TotalRyouCount / (TotalRyouCount + TotalKaCount + TotalFukaCount)*100.0;
	else Perfect = 0;
	if ((CurrentRyouCount + CurrentKaCount + CurrentFukaCount) != 0) CurrentPerfect = (double)CurrentRyouCount / (CurrentRyouCount + CurrentKaCount + CurrentFukaCount)*100.0;
	else CurrentPerfect = 0;
}

void scoer_debug() {

	snprintf(buf_score, sizeof(buf_score), "Scoremode:%d   Init:%d   Diff:%d", TJA_Header.scoremode, init, diff);
	debug_draw(0, 10, buf_score);
	snprintf(buf_score, sizeof(buf_score), "%s %s    Course:%d    Level:%d    %s" ,TJA_Header.title, TJA_Header.subtitle,TJA_Header.course, TJA_Header.level,TJA_Header.wave);
	debug_draw(0, 30, buf_score);
	snprintf(buf_score, sizeof(buf_score), "Score:%d    %dCombo    diff:%d",TotalScore, combo, ScoreDiff);
	debug_draw(0, 150, buf_score);
	snprintf(buf_score, sizeof(buf_score), "Current   Score:%d    Roll:%d    Perfect:%.1f", CurrentScore, CurrentRollCount, CurrentPerfect);
	debug_draw(0, 160, buf_score);
	snprintf(buf_score, sizeof(buf_score), "%.0f:%.0f:%.0f:%.0f",A,B,C,D);
	debug_draw(0, 170, buf_score);
	if (isGOGO == true) {
		snprintf(buf_score, sizeof(buf_score), "GOGOTIME");
		debug_draw(0, 190, buf_score);
	}
}

int branch_start(int knd,double x,double y) {	//分岐

	A = knd;
	B = x;
	C = y;
	int branch;
	switch (knd) {
	case 0:	//連打
		if (y <= CurrentRollCount) branch = M;
		else if (x <= CurrentRollCount) branch = E;
		else branch = N;
		break;
	case 1:	//精度
		if (y <= CurrentPerfect) branch = M;
		else if (x <= CurrentPerfect) branch = E;
		else branch = N;
		break;
	case 2:	//スコア
		if (y <= CurrentScore) branch = M;
		else if (x <= CurrentScore) branch = E;
		else branch = N;
		break;
	default:
		branch = N;
		break;
	}
	D = (double)branch;
	return branch;
}

void branch_section_init() {	//#SECTION

	CurrentRollCount = 0;
	CurrentRyouCount = 0;
	CurrentKaCount = 0;
	CurrentFukaCount = 0;
	CurrentScore = 0;
	CurrentPerfect = 0;
}

void send_gogotime(bool arg) {
	isGOGO = arg;
}

int round_down(int arg) {
	int temp = arg % 10;
	return arg - temp;
}

void calc_base_score(MEASURE_T Measure[Measure_Max], char notes[Measure_Max][Max_Notes_Measure]) {	//初項と公差を計算
	
	int NotesCount = 0, i = 0, combo = 0, DiffTmp = 0, BalloonCnt = 0, TmpBaseCeilingPoint = 0,NotesCountMax = 0,RollCnt=0,RollKnd=0;
	bool isEND = false;
	double init_cnt=0,diff_cnt=0,gogo = 1,special = 1,roll_start_time=0, roll_end_time = 0;
	COMMAND_T Command;

	int level = TJA_Header.level;
	if (level > 10) level = 10;

	switch (TJA_Header.course) {	//基本天井点を設定
	case 0:	//かんたん
		BaseCeilingPoint = 280000 + level * 20000;
		break;
	case 1:	//ふつう
		BaseCeilingPoint = 350000 + level * 50000;
		break;
	case 2:	//むずかしい
		BaseCeilingPoint = 500000 + level * 50000;
		break;
	case 3:	//おに
	case 4:
		if (level == 10) BaseCeilingPoint = 1200000;
		else BaseCeilingPoint = 650000 + level * 50000;
		break;
	}
	TmpBaseCeilingPoint = BaseCeilingPoint;

	if ((TJA_Header.scoreinit == -1 || TJA_Header.scorediff == -1) && (scoremode == 1 || scoremode == 2)) {	//新配点と旧配点

		while (isEND == false && i < Measure_Max && Measure[i].flag == true) {	//小節

			NotesCount = 0;

			if (NotesCount == 0 && notes[i][0] == '#') {

				get_command_value(notes[i], &Command);

				switch (Command.knd) {
				case GOgostart:
					gogo = 1.2;
					break;
				case GOgoend:
					gogo = 1.0;
					break;
				case ENd:
					isEND = true;
					break;
				default:
					break;

				}
				NotesCount = 0;
				i++;
				continue;
			}

			while (notes[i][NotesCount] != ',' && notes[i][NotesCount] != '\n' && notes[i][NotesCount] != '/') {

				NotesCount++;
			}
			if (Measure[i].firstmeasure != -1) NotesCountMax = Measure[Measure[i].firstmeasure].max_notes;
			else NotesCountMax = NotesCount;

			for (int j = 0; j < NotesCount; j++) {	//ノーツ

				int knd = ctoi(notes[i][j]);

				if (knd != 0) {

					if (knd == Don || knd == Ka || knd == BigDon || knd == BigKa) {
						if (knd == BigDon || knd == BigKa) special = 2.0;
						else special = 1.0;
						combo++;
						init_cnt += 1 * gogo * special;

						if (scoremode == 1) {		//旧配点

							if (combo > 100) DiffTmp = 10;
							else DiffTmp = combo / 10;
						}
						else if (scoremode == 2) {	//新配点

							if (combo >= 1 && combo <= 9) DiffTmp = 0;
							else if (combo >= 10 && combo <= 29) DiffTmp = 1;
							else if (combo >= 30 && combo <= 49) DiffTmp = 2;
							else if (combo >= 50 && combo <= 99) DiffTmp = 4;
							else if (combo >= 100) DiffTmp = 8;
						}

						diff_cnt += DiffTmp * gogo * special;
					}
					else if (knd == Balloon) {	//風船

						TmpBaseCeilingPoint -= (TJA_Header.balloon[BalloonCnt] * 300 + 5000) * gogo;
						BalloonCnt++;
					}
					else if (knd == Roll) { //連打
					
						roll_start_time = Measure[i].judge_time + 60.0 / Measure[i].bpm * 4 * Measure[i].measure * i / NotesCountMax;
						RollKnd = Roll;
					}
					else if (knd == BigRoll) { //大連打

						roll_start_time = Measure[i].judge_time + 60.0 / Measure[i].bpm * 4 * Measure[i].measure * i / NotesCountMax;
						RollKnd = BigRoll;
					}
					else if (knd == RollEnd) {

						if (roll_start_time != 0) {

							roll_end_time = Measure[i].judge_time + 60.0 / Measure[i].bpm * 4 * Measure[i].measure * i / NotesCountMax;
							RollCnt = (int)((roll_end_time - roll_start_time) / (1.0 / 12.0));

							if (RollKnd == Roll) {
								if (scoremode == 1) {
									if (gogo == true) TmpBaseCeilingPoint -= RollCnt * 360;
									else TmpBaseCeilingPoint -= RollCnt * 300;
								}
								if (scoremode == 2) {
									if (gogo == true) TmpBaseCeilingPoint -= RollCnt * 120;
									else TmpBaseCeilingPoint -= RollCnt * 100;
								}
							}
							else if (RollKnd == BigRoll) {
								if (scoremode == 1) {
									if (gogo == true) TmpBaseCeilingPoint -= RollCnt * 430 * gogo;
									else TmpBaseCeilingPoint -= RollCnt * 360 * gogo;
								}
								if (scoremode == 1) {
									if (gogo == true) TmpBaseCeilingPoint -= RollCnt * 240 * gogo;
									else TmpBaseCeilingPoint -= RollCnt * 200 * gogo;
								}
							}
							roll_start_time = 0;
							roll_end_time = 0;
							RollCnt = 0;
						}
					}
				}
			}
			i++;
		}
		diff = (TmpBaseCeilingPoint - (int)(combo / 100) * 10000) / (init_cnt * 4 + diff_cnt);
		init = diff * 4;
	}
	else if (scoremode == 0) {
		init = 1000;
		diff = 1000;
	}
	//score_init_after();
}