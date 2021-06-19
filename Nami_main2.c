#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

typedef struct {
	int fs; /* 標本化周波数 */
	int bits; /* 量子化精度 */
	int bitsdecimal; //bitsを10進整数に変換
	int length; /* 音データの長さ */
	double *sound; /* 音データ */
	int *sounddecimal; //音データを10進整数に変換
	double gain; //振幅 ゲイン
	double f0; //周波数
	int stackingNumber; //サイン波の重ね回数
} SOUND;

void sinewave(SOUND *ppcm) //サイン波
{
	ppcm->gain = 0.05; //振幅 ゲイン
	ppcm->f0 = 200.0; //周波数
	int n;

	for (n = 0; n < ppcm->length; n++) {
		ppcm->sound[n] = sin(2.0 * M_PI * ppcm->f0 * n / ppcm->fs);
	}
}

void sawtoothwave(SOUND *ppcm) //のこぎり波
{
	ppcm->gain = 0.025; //振幅 ゲイン
	ppcm->f0 = 400.0; //周波数
	int i;
	int n;

	for (i = 1; i <= ppcm->stackingNumber; i++) {
		for (n = 0; n < ppcm->length; n++) {
			ppcm->sound[n] += 1.0 / i
				* sin(2.0 * M_PI * i * ppcm->f0 * n / ppcm->fs);
		}
	}
}

void squarewave(SOUND *ppcm) //矩形波
{
	ppcm->gain = 0.05; //振幅 ゲイン
	ppcm->f0 = 200.0; //周波数
	int i;
	int n;

	for (i = 1; i <= ppcm->stackingNumber; i = i + 2) {
		for (n = 0; n < ppcm->length; n++) {
			ppcm->sound[n] += 1.0 / i
				* sin(2.0 * M_PI * i * ppcm->f0 * n / ppcm->fs);
		}
	}
}

void trianglewave(SOUND *ppcm) //三角波
{
	ppcm->gain = 0.05; //振幅 ゲイン
	ppcm->f0 = 200.0; //周波数
	int i;
	int n;

	for (i = 1; i <= ppcm->stackingNumber; i = i + 2)
	{
		for (n = 0; n < ppcm->length; n++)
		{
			ppcm->sound[n] += 1.0 / i
				/ i * sin(M_PI * i / 2.0)
				* sin(2.0 * M_PI * i * ppcm->f0 * n / ppcm->fs);
		}
	}
}

int main(void)
{
	SOUND pcm;
	SOUND *ppcm = &pcm;
	int n;
	int i;

	int minNumber;
	int maxNumber;
	int choice = 0;
	int loop = 1;

	while (loop) {

		choice = 0;

		while (choice < 1 || 5 < choice) {
			printf("波形を選択してください。終了する場合は5を入力\n");
			printf("1:サイン波 2:のこぎり波 3:矩形波 4:三角波 5:終了\n");
			printf("番号:");
			scanf_s("%d", &choice);
		}

		if (choice == 5) { //5の場合はループ終了
			loop = 0;
			continue;
		}
		else if (!(choice == 1)) {  //1のサイン波以外は重ね回数を入力
			printf("波形の重ね合わせ回数を入力してください(1 - 10000)\n");
			printf("回数:");
			scanf_s("%d", &ppcm->stackingNumber);
		}

		ppcm->fs = 8000;  //標本化周波数
		ppcm->bits = 8;  //量子化精度
		ppcm->length = ppcm->fs * 0.01; //音データの長さ(= コンソールに表示する長さ)
		ppcm->sound = calloc(ppcm->length, sizeof(double)); //音データのメモリ確保
		ppcm->sounddecimal = calloc(ppcm->length, sizeof(int)); //10進数にした音データのメモリ確保

		ppcm->bitsdecimal = 1; //pcm.bitsを10進整数に変換
		for (n = 0; n < ppcm->bits; n++) {
			ppcm->bitsdecimal *= 2;
		}


		switch (choice) {
		case 1:
			sinewave(ppcm);
			break;
		case 2:
			sawtoothwave(ppcm);
			break;
		case 3:
			squarewave(ppcm);
			break;
		case 4:
			trianglewave(ppcm);
			break;
		default:
			//なにもしない
			break;
		}

		//ゲイン調整
		for (n = 0; n < ppcm->length; n++) {
			ppcm->sound[n] *= ppcm->gain;

			//printf("%d\t%f\n", n, pcm.sound[n]);
		}

		//量子化精度に合わせ整数値に変換
		//設定したビット数を超える数値になった場合は最大値に丸め込む(クリッピング)
		for (n = 0; n < ppcm->length; n++) {
			ppcm->sounddecimal[n] = (int)((ppcm->bitsdecimal - 1) * ppcm->sound[n]);
			if (ppcm->bitsdecimal <= ppcm->sounddecimal[n]) { //プラスのクリッピング
				ppcm->sounddecimal[n] = ppcm->bitsdecimal - 1;
			}
			if (ppcm->sounddecimal[n] <= -(ppcm->bitsdecimal)) { //マイナスのクリッピング
				ppcm->sounddecimal[n] = -(ppcm->bitsdecimal) + 1;
			}
		}

		//波形の最小値minNumberを導出
		for (n = 0, minNumber = 0; n < ppcm->length; n++) {
			if (ppcm->sounddecimal[n] < minNumber) {
				minNumber = ppcm->sounddecimal[n];
			}
		}
		//printf("minNumber:%d\n", minNumber);

		//波形の最大値maxNumberを導出
		for (n = 0, maxNumber = 0; n < ppcm->length; n++) {
			if (maxNumber < ppcm->sounddecimal[n]) {
				maxNumber = ppcm->sounddecimal[n];
			}
		}
		//printf("maxNumber:%d\n", maxNumber);

/*
		//xy変換していない素の出力
		//マイナス部分に下駄を履かせてすべて0以上になるようにし、コンソールに表示
		for (n = 0; n < ppcm->length; n++) {
			for (i = 0; i < (ppcm->sounddecimal[n] - minNumber); i++) {
				printf(" ");
			}
			printf("*\n");
		}
//*/

//表示する波形のxy軸変換
		int graphY = -(minNumber)+maxNumber + 1;
		int graphY2 = graphY;

		char **xyFlip;
		xyFlip = calloc(graphY, sizeof(char *)); //xyFlipのメモリ確保
		for (i = 0; i < graphY; i++) {
			xyFlip[i] = calloc(ppcm->length, sizeof(char));
		}

		for (i = 0; i < ppcm->length; i++) { //Y軸の中心に棒線(-)を描画
			xyFlip[graphY2 / 2][i] = '-';
		}

		for (i = 0; i < graphY; i++) {
			for (n = 0; n < ppcm->length; n++) {
				if ((ppcm->sounddecimal[n] - minNumber + 1) == graphY2) {
					xyFlip[i][n] = '*';
				}
				else if (!i == (graphY2 / 2)) {
					xyFlip[i][n] = ' ';
				}
			}
			graphY2 -= 1;
		}

		for (i = 0; i < graphY; i++) {
			for (n = 0; n < ppcm->length; n++) {
				printf("%c", xyFlip[i][n]);
			}
			printf("\n");
		}

		//メモリ解放
		free(ppcm->sound);
		free(ppcm->sounddecimal);
		free(xyFlip);

	}

	return 0;
}
