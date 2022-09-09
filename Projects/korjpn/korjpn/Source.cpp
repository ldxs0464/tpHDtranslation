#include<stdio.h>
void writehex(unsigned char input,unsigned char input2,FILE *w){
	unsigned char sum=0;
		switch(input){
			case 0x30:sum=sum+0x00;break;
			case 0x31:sum=sum+0x10;break;
			case 0x32:sum=sum+0x20;break;
			case 0x33:sum=sum+0x30;break;
			case 0x34:sum=sum+0x40;break;
			case 0x35:sum=sum+0x50;break;
			case 0x36:sum=sum+0x60;break;
			case 0x37:sum=sum+0x70;break;
			case 0x38:sum=sum+0x80;break;
			case 0x39:sum=sum+0x90;break;
			case 0x41:sum=sum+0xA0;break;
			case 0x42:sum=sum+0xB0;break;
			case 0x43:sum=sum+0xC0;break;
			case 0x44:sum=sum+0xD0;break;
			case 0x45:sum=sum+0xE0;break;
			case 0x46:sum=sum+0xF0;break;
		}
		switch(input2){
			case 0x30:sum=sum+0x00;break;
			case 0x31:sum=sum+0x01;break;
			case 0x32:sum=sum+0x02;break;
			case 0x33:sum=sum+0x03;break;
			case 0x34:sum=sum+0x04;break;
			case 0x35:sum=sum+0x05;break;
			case 0x36:sum=sum+0x06;break;
			case 0x37:sum=sum+0x07;break;
			case 0x38:sum=sum+0x08;break;
			case 0x39:sum=sum+0x09;break;
			case 0x41:sum=sum+0x0A;break;
			case 0x42:sum=sum+0x0B;break;
			case 0x43:sum=sum+0x0C;break;
			case 0x44:sum=sum+0x0D;break;
			case 0x45:sum=sum+0x0E;break;
			case 0x46:sum=sum+0x0F;break;
		}
	//printf("%X",sum);
	fputc(sum,w);
}
unsigned char hex2char(unsigned char input){
	switch(input){
			case 0x30:return 0;break;
			case 0x31:return 1;break;
			case 0x32:return 2;break;
			case 0x33:return 3;break;
			case 0x34:return 4;break;
			case 0x35:return 5;break;
			case 0x36:return 6;break;
			case 0x37:return 7;break;
			case 0x38:return 8;break;
			case 0x39:return 9;break;
			case 0x41:return 10;break;
			case 0x42:return 11;break;
			case 0x43:return 12;break;
			case 0x44:return 13;break;
			case 0x45:return 14;break;
			case 0x46:return 15;break;
	}
}
void main(){
	FILE *un;
	FILE *ja;
	FILE *ko;
	FILE *in;
	FILE *ou;
	//FILE *po;
	FILE *pw;
	un=fopen("unicode.txt","rb");
	ja=fopen("sheet.txt","rb");
	ko=fopen("ks1001.txt","rb");
	in=fopen("input.txt","rb");
	//po=fopen("pointer.bin","rb");
	pw=fopen("editpointer","wb");
	ou=fopen("output.txt","wb");
	unsigned char jpn[20000]={};
	unsigned char kor[20000]={};
	unsigned char save[800000]={};
	unsigned int countpointer=0;
	int countstring=0;
	for(int i=0;i<20000;i++){
		jpn[i]=fgetc(ja);
		kor[i]=fgetc(un);
	}

	fseek(in, 0, SEEK_END);
    unsigned int size = ftell(in);
	printf("파일의 크기:%d\n",size);

	//포인터를 다시 앞으로
	fseek(in, 0, SEEK_SET);
	for(int i=0;i<size;i++){
		save[i]=fgetc(in);
		//printf("%02X ",save[i]);
	}
	printf("\n");
	countstring++;
	countpointer++;

	//첫번째 포인터 작성
			fputc((countpointer >> 24) & 0xFF,pw);
			fputc((countpointer >> 16) & 0xFF,pw);
			fputc((countpointer >> 8) & 0xFF,pw);
			fputc(countpointer & 0xFF,pw);
			for(int de=0;de<16;de++)
				fputc(0x00,pw);
	printf("포인터=%d(%X),%d\n",countpointer,countpointer,countstring);
	fputc(0x00,ou);
	countpointer++;
	countstring++;
	unsigned int findout=0;
	for(int i=0;i<size;i+=2){
		if(save[i]>=0xac&&save[i]<0xee){
			for(int search=0;search<20000;search+=2){
				if(save[i]==kor[search]&&save[i+1]==kor[search+1]){
					//printf("found ");
					findout=search;
				}
			}
			fputc(jpn[findout],ou);
			fputc(jpn[findout+1],ou);
			countpointer++;
			countpointer++;
		}

		//엔터 처리(다음 대사)
		else if(save[i]==0x00&&save[i+1]==0x0d){
			fputc(0x00,ou);
			fputc((countpointer >> 24) & 0xFF,pw);
			fputc((countpointer >> 16) & 0xFF,pw);
			fputc((countpointer >> 8) & 0xFF,pw);
			fputc(countpointer & 0xFF,pw);
			for(int de=0;de<16;de++)
				fputc(0x00,pw);
			printf("포인터=%d(%X),%d\n",countpointer,countpointer,countstring);
			countstring++;
			countpointer++;
			
		}
		//0a삭제
		else if(save[i]==0x00&&save[i+1]==0x0a){
			
		}
		//<기호 처리
		else if(save[i]==0x00&&save[i+1]==0x3c){
			//엔터 기호 처리
			if(save[i+3]==0x4E){
				fputc('\n',ou);
				i+=4;
				countpointer++;
			}
			//일반 기호
			else if(save[i+3]==0x31&&save[i+5]==0x41){
				unsigned char number=0;
				number=hex2char(save[i+7])*0x10+hex2char(save[i+9]);
				for(int cnt=0;cnt<number*2;cnt+=2){
					writehex(save[i+(cnt*2)+3],save[i+(cnt*2)+5],ou);
					countpointer++;
				}
				i+=number*4+2;
			}
		}
		/*else if(save[i]==0x00&&save[i+1]==0x0d&&save[i+2]==0x00&&save[i+3]==0x0a){
			fputc(save[i+3],ou);
			i+=2;
			printf("포인터%d\n",countstring);
			countstring++;
			//countpointer++;
		}*/

		//아스키 코드 부분에 해당하는 문자일 때 00을 삭제한다
		else if(save[i]==0x00){
			fputc(save[i+1],ou);
			countpointer++;
		}
		//나머지
		else{
			//printf("unknown letter found\n");
			//countpointer++;
			//countpointer++;
		}
	}
}