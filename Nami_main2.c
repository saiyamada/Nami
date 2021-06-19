#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

typedef struct {
	int fs; /* �W�{�����g�� */
	int bits; /* �ʎq�����x */
	int bitsdecimal; //bits��10�i�����ɕϊ�
	int length; /* ���f�[�^�̒��� */
	double *sound; /* ���f�[�^ */
	int *sounddecimal; //���f�[�^��10�i�����ɕϊ�
	double gain; //�U�� �Q�C��
	double f0; //���g��
	int stackingNumber; //�T�C���g�̏d�ˉ�
} SOUND;

void sinewave(SOUND *ppcm) //�T�C���g
{
	ppcm->gain = 0.05; //�U�� �Q�C��
	ppcm->f0 = 200.0; //���g��
	int n;

	for (n = 0; n < ppcm->length; n++) {
		ppcm->sound[n] = sin(2.0 * M_PI * ppcm->f0 * n / ppcm->fs);
	}
}

void sawtoothwave(SOUND *ppcm) //�̂�����g
{
	ppcm->gain = 0.025; //�U�� �Q�C��
	ppcm->f0 = 400.0; //���g��
	int i;
	int n;

	for (i = 1; i <= ppcm->stackingNumber; i++) {
		for (n = 0; n < ppcm->length; n++) {
			ppcm->sound[n] += 1.0 / i
				* sin(2.0 * M_PI * i * ppcm->f0 * n / ppcm->fs);
		}
	}
}

void squarewave(SOUND *ppcm) //��`�g
{
	ppcm->gain = 0.05; //�U�� �Q�C��
	ppcm->f0 = 200.0; //���g��
	int i;
	int n;

	for (i = 1; i <= ppcm->stackingNumber; i = i + 2) {
		for (n = 0; n < ppcm->length; n++) {
			ppcm->sound[n] += 1.0 / i
				* sin(2.0 * M_PI * i * ppcm->f0 * n / ppcm->fs);
		}
	}
}

void trianglewave(SOUND *ppcm) //�O�p�g
{
	ppcm->gain = 0.05; //�U�� �Q�C��
	ppcm->f0 = 200.0; //���g��
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
			printf("�g�`��I�����Ă��������B�I������ꍇ��5�����\n");
			printf("1:�T�C���g 2:�̂�����g 3:��`�g 4:�O�p�g 5:�I��\n");
			printf("�ԍ�:");
			scanf_s("%d", &choice);
		}

		if (choice == 5) { //5�̏ꍇ�̓��[�v�I��
			loop = 0;
			continue;
		}
		else if (!(choice == 1)) {  //1�̃T�C���g�ȊO�͏d�ˉ񐔂����
			printf("�g�`�̏d�ˍ��킹�񐔂���͂��Ă�������(1 - 10000)\n");
			printf("��:");
			scanf_s("%d", &ppcm->stackingNumber);
		}

		ppcm->fs = 8000;  //�W�{�����g��
		ppcm->bits = 8;  //�ʎq�����x
		ppcm->length = ppcm->fs * 0.01; //���f�[�^�̒���(= �R���\�[���ɕ\�����钷��)
		ppcm->sound = calloc(ppcm->length, sizeof(double)); //���f�[�^�̃������m��
		ppcm->sounddecimal = calloc(ppcm->length, sizeof(int)); //10�i���ɂ������f�[�^�̃������m��

		ppcm->bitsdecimal = 1; //pcm.bits��10�i�����ɕϊ�
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
			//�Ȃɂ����Ȃ�
			break;
		}

		//�Q�C������
		for (n = 0; n < ppcm->length; n++) {
			ppcm->sound[n] *= ppcm->gain;

			//printf("%d\t%f\n", n, pcm.sound[n]);
		}

		//�ʎq�����x�ɍ��킹�����l�ɕϊ�
		//�ݒ肵���r�b�g���𒴂��鐔�l�ɂȂ����ꍇ�͍ő�l�Ɋۂߍ���(�N���b�s���O)
		for (n = 0; n < ppcm->length; n++) {
			ppcm->sounddecimal[n] = (int)((ppcm->bitsdecimal - 1) * ppcm->sound[n]);
			if (ppcm->bitsdecimal <= ppcm->sounddecimal[n]) { //�v���X�̃N���b�s���O
				ppcm->sounddecimal[n] = ppcm->bitsdecimal - 1;
			}
			if (ppcm->sounddecimal[n] <= -(ppcm->bitsdecimal)) { //�}�C�i�X�̃N���b�s���O
				ppcm->sounddecimal[n] = -(ppcm->bitsdecimal) + 1;
			}
		}

		//�g�`�̍ŏ��lminNumber�𓱏o
		for (n = 0, minNumber = 0; n < ppcm->length; n++) {
			if (ppcm->sounddecimal[n] < minNumber) {
				minNumber = ppcm->sounddecimal[n];
			}
		}
		//printf("minNumber:%d\n", minNumber);

		//�g�`�̍ő�lmaxNumber�𓱏o
		for (n = 0, maxNumber = 0; n < ppcm->length; n++) {
			if (maxNumber < ppcm->sounddecimal[n]) {
				maxNumber = ppcm->sounddecimal[n];
			}
		}
		//printf("maxNumber:%d\n", maxNumber);

/*
		//xy�ϊ����Ă��Ȃ��f�̏o��
		//�}�C�i�X�����ɉ��ʂ𗚂����Ă��ׂ�0�ȏ�ɂȂ�悤�ɂ��A�R���\�[���ɕ\��
		for (n = 0; n < ppcm->length; n++) {
			for (i = 0; i < (ppcm->sounddecimal[n] - minNumber); i++) {
				printf(" ");
			}
			printf("*\n");
		}
//*/

//�\������g�`��xy���ϊ�
		int graphY = -(minNumber)+maxNumber + 1;
		int graphY2 = graphY;

		char **xyFlip;
		xyFlip = calloc(graphY, sizeof(char *)); //xyFlip�̃������m��
		for (i = 0; i < graphY; i++) {
			xyFlip[i] = calloc(ppcm->length, sizeof(char));
		}

		for (i = 0; i < ppcm->length; i++) { //Y���̒��S�ɖ_��(-)��`��
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

		//���������
		free(ppcm->sound);
		free(ppcm->sounddecimal);
		free(xyFlip);

	}

	return 0;
}
