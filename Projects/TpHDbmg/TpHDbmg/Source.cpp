#include<stdio.h>
void writehex(unsigned char input,FILE *w){
	unsigned char second=input%16;
	input=input/16;
	for(int i=0;i<2;i++){
		switch(input){
			case 0:fputc('0',w);break;
			case 1:fputc('1',w);break;
			case 2:fputc('2',w);break;
			case 3:fputc('3',w);break;
			case 4:fputc('4',w);break;
			case 5:fputc('5',w);break;
			case 6:fputc('6',w);break;
			case 7:fputc('7',w);break;
			case 8:fputc('8',w);break;
			case 9:fputc('9',w);break;
			case 10:fputc('A',w);break;
			case 11:fputc('B',w);break;
			case 12:fputc('C',w);break;
			case 13:fputc('D',w);break;
			case 14:fputc('E',w);break;
			case 15:fputc('F',w);break;
		}
		input=second;
	}
}
void writehex2(unsigned char input,FILE *w){
	unsigned char second=input%16;
	input=input/16;
	for(int i=0;i<2;i++){
		fputc(0x00,w);
		switch(input){
			case 0:fputc('0',w);break;
			case 1:fputc('1',w);break;
			case 2:fputc('2',w);break;
			case 3:fputc('3',w);break;
			case 4:fputc('4',w);break;
			case 5:fputc('5',w);break;
			case 6:fputc('6',w);break;
			case 7:fputc('7',w);break;
			case 8:fputc('8',w);break;
			case 9:fputc('9',w);break;
			case 10:fputc('A',w);break;
			case 11:fputc('B',w);break;
			case 12:fputc('C',w);break;
			case 13:fputc('D',w);break;
			case 14:fputc('E',w);break;
			case 15:fputc('F',w);break;
		}
		input=second;
	}
}
void main(){
	//writehex(255);
	FILE *f;
	FILE *w;
	FILE *w2;
	int size=0;
	unsigned char byte;
	unsigned char save[1000000]={};
	unsigned int header_size=0;
	unsigned int header_dat1_location=0;
	unsigned short header_stringsize=0;
	unsigned short header_pointersize=0;
	unsigned int location_debug;

	int user_choose=0;
	f=fopen("zel_00.bmg","rb");
	w=fopen("extract.txt","wb");
	w2=fopen("pointer.bin","wb");
	//파일 크기 체크
	fseek(f, 0, SEEK_END);
    size = ftell(f);
	printf("파일의 크기:%d\n",size);

	//포인터를 다시 앞으로
	fseek(f, 0, SEEK_SET);
	for(int i=0;i<size;i++){
		//if(i%16==0)
		//	printf("\n%08X: ",i);
		save[i]=fgetc(f);
		//printf("%02X ",save[i]);
	}

	//헤더 계산
	header_size=((save[8] & 0xff) << 24)+((save[9] & 0xff) << 16)+((save[10] & 0xff) << 8)+(save[11]& 0xff);
	header_dat1_location=((save[0x24] & 0xff) << 24)+((save[0x25] & 0xff) << 16)+((save[0x26] & 0xff) << 8)+(save[0x27]& 0xff);
	header_dat1_location+=0x20;
	header_stringsize=((save[0x28] & 0xff) << 8)+(save[0x29]& 0xff);
	header_pointersize=((save[0x2a] & 0xff) << 8)+(save[0x2b]& 0xff);

	//분석 결과 출력
	printf("-파일 분석결과-\n");
	printf("파일 크기=%d(%08X)\n",header_size,header_size);
	printf("DAT1위치=%d(%08X)\n",header_dat1_location,header_dat1_location);
	printf("글자 갯수(?)=%d(%04X)\n",header_stringsize,header_stringsize);
	printf("항목 길이=%d(%04X)\n",header_pointersize,header_pointersize);

	//정발인지 해외판인지 질문
	printf("정발?(1=Yes,2=No):");
	scanf("%d",&user_choose);

	//포인터만 파일로 저장
	for(int i=0x20;i<header_dat1_location;i++)
		fputc(save[i],w2);

	//printf("%d",&);
	if(user_choose==1){
		fputc(0xfe,w);
		fputc(0xff,w);
	}
	for(int i=0;i<header_stringsize;i++){
		for(int j=0;j<header_pointersize;j++){
			//printf("%02X ",save[48+(i*header_pointersize)+j]);
		}
		//포인터 읽기
		location_debug=((save[48+(i*header_pointersize)] & 0xff) << 24)+((save[48+(i*header_pointersize)+1] & 0xff) << 16)+((save[48+(i*header_pointersize)+2] & 0xff) << 8)+(save[48+(i*header_pointersize)+3]& 0xff);
		location_debug+=(header_dat1_location+8);

		//정발 전용(유니코드)
		if(user_choose==1){
			for(int k=0;save[location_debug+k]!=0||save[location_debug+k+1]!=0;k+=2){
				//엔터(line feed)처리
				if(save[location_debug+k]==0x00&&save[location_debug+k+1]==0x0A){
					fputc(0x00,w);
					fputc(0x3c,w);
					fputc(0x00,w);
					fputc(0x4e,w);
					fputc(0x00,w);
					fputc(0x3e,w);
				}
				//대사 기호 처리
				else if(save[location_debug+k]==0x00&&save[location_debug+k+1]==0x1A){
					fputc(0x00,w);
					fputc('<',w);
					for(int code=0;code<save[location_debug+k+2];code++)
						writehex2(save[location_debug+k+code],w);
					fputc(0x00,w);
					fputc('>',w);
					k+=save[location_debug+k+2]-2;
					
				}
				//나머지
				else{
					//printf("%02X ",save[location_debug+k]);
					fputc(save[location_debug+k],w);
					//printf("%02X ",save[location_debug+k+1]);
					fputc(save[location_debug+k+1],w);
				}
			}
			//printf("\n");
			fputc(0x00,w);
			fputc(0x0a,w);
		}
		else if(user_choose==2){
			for(int k=0;save[location_debug+k]!=0;k++){
				//printf("%c",save[location_debug+k]);
				//엔터(line feed)처리
				if(save[location_debug+k]==0x0a){
					fputc('<',w);
					fputc('N',w);
					fputc('>',w);
				}
				//대사 기호 처리
				else if(save[location_debug+k]==0x1a){
					fputc('<',w);
					//printf("_%d_",save[location_debug+k+1]);
					for(int code=0;code<save[location_debug+k+1];code++)
						writehex(save[location_debug+k+code],w);
					fputc('>',w);
					k=k+save[location_debug+k+1]-1;
				}
				//나머지
				else
					fputc(save[location_debug+k],w);
			}
			//printf("\n");
			//다음 대사는 다음줄에...
			fputc('\n',w);
		}
	}
	fclose(f);
}
