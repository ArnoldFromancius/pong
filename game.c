/*	TITLE: PING PONG.
	AUTHOR: ARNOLD FROMANCIUS.
	
	DATE: 06/01/2021.
	
	DESCRIPTION: COMAND LINE CONSOLE BASED GAME, USES WIN32_API FOR ITS SIMPLE
				 GRAPHICS(ALL ASCII CHARACTER).
	
	DEVELOPMENT: USES DOUBLE BUFFERING. BASICALLY _PLOT() FUNCTIONS DRAW TO 
				 A TEMPORARY BUFFER SCREEN, AND THE DRAW() FUNCTION COPIES THE 
				 TEMPORARY BUFFER SCREEN TO THE MAIN SCREEN.

*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>

#define ENTER 13
#define ESC 27
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define A_LEFT 65
#define S_RIGHT 83
#define s_RIGHT 115
#define a_LEFT 97
#define z_DOWN 90
#define Z_DOWN 122
#define MAX_FIELD_LENGTH 2171
#define PERIMETER_BREACH_ROW 1962
#define PERIMETER_BREACH_POINT 2171
#define BOTTOM_WALL 2310
#define TOP_WALL 70
#define PLAYER2_STAGE_END_POINT 140
#define PLAYER1_STAGE_END_POINT 2380
#define INFO_STAGE_END_POINT 2450
#define MAX_SHRAPNEL_TRAVEL 25
#define PLAYER2_MAX_LEFT_POS 73
#define PLAYER2_MAX_RIGHT_POS 137
#define PLAYER1_MAX_LEFT_POS 2243
#define PLAYER1_MAX_RIGHT_POS 2307
#define DEFAULT_BALL_SPEED 80

void clear(CHAR_INFO *);	//clears screen and plots game_background on screen
void draw(HANDLE *h,CHAR_INFO *ms,COORD *mss,COORD *msp,SMALL_RECT *ws);
void plot_player1(int,CHAR_INFO *);
void plot_player2(int,CHAR_INFO *);
void plot_ball(int, CHAR_INFO *);
void plot_score_board(CHAR_INFO *);
void plot_loading(int ,CHAR_INFO*);
void plot_game_over(CHAR_INFO *);
void plot_pause(CHAR_INFO *);
void compute_score(int *, char *);
void update_ball();
void launch_ball(int player);
int loading();
char p1Score[5]={'0','0','0','0'};
char p2Score[5]={'0','0','0','0'};
int p1score=0;
int p2score=0;
char Hiscore[4];
int game_mode=-1;
int ball_speed=DEFAULT_BALL_SPEED;

int score,hiscore;

//ball datastructure
typedef struct BALL{
	int cur_pos;
	int next_pos;
	int direction;	
	int trajectory_xcoord;
	int trajectory_ycoord;
	int tx;	//
	int ty; //denotes whether the trajectories are increasing or decreasing
}Ball;

Ball* ball;
 
int main(){
	HANDLE handle;
	handle=GetStdHandle(STD_OUTPUT_HANDLE);
	//setup window
	SetConsoleTitle("PING PONG...");
	//setup screen details
	SMALL_RECT WinSize={0,0,69,34};
	COORD ScreenBufferSize={70,35};
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE),TRUE,&WinSize);
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),ScreenBufferSize);
	//buffer screen
	CHAR_INFO mirror_screen[70*35];
	COORD mirror_screen_size={70,35};
	COORD mirror_screen_pos={0,0};

	//start screen
	int load,selector=1;
	plot_loading(selector,mirror_screen);
	draw(&handle,mirror_screen,&mirror_screen_size,&mirror_screen_pos,&WinSize);
	while(1){
		load=loading();
		if(load==1){
			if(selector!=1){
				selector=1;
			}
		}
		else if(load==2){
			if(selector!=2){
				selector=2;
			}
		}
		else if(load==3){
			if(selector==1){
				game_mode=1;
				break;
			}
			else if(selector==2){
				game_mode=2;	
				break;
			}
		}
		
		plot_loading(selector,mirror_screen);
		draw(&handle,mirror_screen,&mirror_screen_size,&mirror_screen_pos,&WinSize);	
	}
	//start screen ends here
	
	int p1pos=2274;
	int p2pos=106;
	int ball_update_counter=0;
	
	//initailise ball	
	ball=malloc(sizeof(Ball));
	ball->direction=0;
	ball->cur_pos=p1pos-70;
	ball->trajectory_xcoord=0;
	ball->trajectory_ycoord=0;
	ball->tx=0;
	ball->ty=0;
	ball->next_pos=-9999;
	
	while(1){
		do{		
				//update ball
				if(ball_update_counter>=ball_speed){
					update_ball(p1pos,p2pos);
					ball_update_counter=0;
				}
				ball_update_counter++;
				
				//update screen
				clear(mirror_screen);
				plot_player1(p1pos,mirror_screen);
				plot_player2(p2pos,mirror_screen);
				plot_score_board(mirror_screen);
				plot_ball(ball->cur_pos,mirror_screen);
				draw(&handle,mirror_screen,&mirror_screen_size,&mirror_screen_pos,&WinSize);
	
		}while(!kbhit());
		fflush(stdin);
		int k,key;
		k=getch();
		//if player quit
		if(k=='q'||k=='Q'||k==27){
			clear(mirror_screen);
			plot_player1(p1pos,mirror_screen);
			plot_player2(p2pos,mirror_screen);
			plot_score_board(mirror_screen);
			plot_game_over(mirror_screen);	
			draw(&handle,mirror_screen,&mirror_screen_size,&mirror_screen_pos,&WinSize);
			getch();
			exit(0);
		}/*/if player pause
		else if(k!=UP&&k!=DOWN&&k!=LEFT&&k!=RIGHT){
			if(k!=z_DOWN&&k!=Z_DOWN){
				if(k!=S_RIGHT&&k!=s_RIGHT){
					if(k!=A_LEFT&&k!=a_LEFT){
						clear(mirror_screen);
						plot_score_board(mirror_screen);
						plot_player1(p1pos,mirror_screen);
						plot_player2(p2pos,mirror_screen);
						plot_pause(mirror_screen);
						draw(&handle,mirror_screen,&mirror_screen_size,&mirror_screen_pos,&WinSize);
					}
				}
				
			}
		}*/
		
		//player 2 controls
		else if(k==A_LEFT||k==a_LEFT){
			if(p2pos>PLAYER2_MAX_LEFT_POS)
				p2pos-=2;
			else if(p2pos==PLAYER2_MAX_LEFT_POS)
				p2pos-=1;
			//make sure ball moves with player if it is not in motion
			if(ball->direction==0){
				if(ball->cur_pos<210){
					ball->cur_pos=p2pos+70;
				}
			}
			fflush(stdin);
			continue;
		}
		else if(k==S_RIGHT||k==s_RIGHT){
			if(p2pos<PLAYER2_MAX_RIGHT_POS&&p2pos+2<=PLAYER2_MAX_RIGHT_POS )
				p2pos+=2;
			else if(p1pos==PLAYER2_MAX_RIGHT_POS-1)
				p2pos+=1;
			//make sure ball moves with player if it is not in motion
			if(ball->direction==0){
				if(ball->cur_pos<210){
					ball->cur_pos=p2pos+70;
				}
			}
			fflush(stdin);
			continue;
		}
		else if(k==Z_DOWN||k==z_DOWN){
			if(ball->direction==0&&ball->cur_pos<210){
				launch_ball(2);
				continue;
			}
		}
		
		key=getch();
		//player 1 controls
		if(key==LEFT){
			if(p1pos>PLAYER1_MAX_LEFT_POS)
				p1pos-=2;
			else if(p1pos==PLAYER1_MAX_LEFT_POS)
				p1pos-=1;
			//make sure ball moves with player if it is not in motion
			if(ball->direction==0){
				if(ball->cur_pos>210){
					ball->cur_pos=p1pos-70;
				}
			}
		}
		else if(key==RIGHT){
			if(p1pos<PLAYER1_MAX_RIGHT_POS&&p1pos+2<=PLAYER1_MAX_RIGHT_POS )
				p1pos+=2;
			else if(p1pos==PLAYER1_MAX_RIGHT_POS-1)
				p1pos+=1;
			//make sure ball moves with player if it is not in motion
			if(ball->direction==0){
				if(ball->cur_pos>210){
					ball->cur_pos=p1pos-70;
				}
			}
		}
		else if(key==UP){
			if(ball->direction==0&&ball->cur_pos>210){
				launch_ball(1);
			}
		}				
	}
}

void launch_ball(int player){
	if(player==1){
		ball->direction=2;
	}
	else if(player==2){
		ball->direction=1;
	}
}

void update_ball(int p1pos, int p2pos){	//compute the balls trajectory	
		//check if ball is not in motion
		if(ball->direction==0)
			return;
			
		int flag=0,trajectory;
		//check if ball hits player 1 paddle
		if(ball->cur_pos==p1pos-2){
			flag==1;
			ball->direction=2;
			ball->trajectory_xcoord=2;
			ball->trajectory_ycoord=70;
			
			trajectory=ball->cur_pos
			-ball->trajectory_xcoord
			-ball->trajectory_ycoord;
			ball->tx=-1;
			ball->ty=-1;
			
			ball->next_pos=trajectory;
		}
		if(ball->cur_pos==p1pos-1){
			flag==1;
			ball->direction=2;
			ball->trajectory_xcoord=1;
			ball->trajectory_ycoord=70;
			
			trajectory=ball->cur_pos
			-ball->trajectory_xcoord
			-ball->trajectory_ycoord;
			ball->tx=-1;
			ball->ty=-1;
			
			ball->next_pos=trajectory;
		}
		if(ball->cur_pos==p1pos){
			//if ball hits center, deflect depending on angle
			flag==1;
			ball->direction=2;			
			ball->trajectory_ycoord=70;
			if(ball->tx==-1){
				ball->trajectory_xcoord=1;
				trajectory=ball->cur_pos
				-ball->trajectory_xcoord
				-ball->trajectory_ycoord;
			}
			else if(ball->tx==1){
				ball->trajectory_xcoord=1;
				trajectory=ball->cur_pos
				-ball->trajectory_xcoord
				-ball->trajectory_ycoord;	
			}
			else 
				ball->trajectory_xcoord=0;
				
			
			ball->ty=-1;
			ball->next_pos=trajectory;
		}
		if(ball->cur_pos==p1pos+1){
			flag==1;
			ball->direction=2;
			ball->trajectory_xcoord=1;
			ball->trajectory_ycoord=70;
			
			trajectory=ball->cur_pos
			+ball->trajectory_xcoord 
			-ball->trajectory_ycoord;
			ball->tx=1;
			ball->ty=-1;
			
			ball->next_pos=trajectory;
		}
		if(ball->cur_pos==p1pos+2){
			flag==1;
			ball->direction=2;
			ball->trajectory_xcoord=2;
			ball->trajectory_ycoord=70;
			
			trajectory=ball->cur_pos
			+ball->trajectory_xcoord 
			-ball->trajectory_ycoord;
			ball->tx=1;
			ball->ty=-1;
			
			ball->next_pos=trajectory;
		}
		//check if ball hits player 2 paddle
		if(ball->cur_pos==p2pos-2){
			flag==1;
			ball->direction=1;
			ball->trajectory_xcoord=2;
			ball->trajectory_ycoord=70;
			
			trajectory=ball->cur_pos
			-ball->trajectory_xcoord
			+ball->trajectory_ycoord;
			ball->tx=-1;
			ball->ty=1;
			
			ball->next_pos=trajectory;
		}
		if(ball->cur_pos==p2pos-1){
			flag==1;
			ball->direction=1;
			ball->trajectory_xcoord=1;
			ball->trajectory_ycoord=70;
			
			trajectory=ball->cur_pos
			-ball->trajectory_xcoord
			+ball->trajectory_ycoord;
			ball->tx=-1;
			ball->ty=1;
			
			ball->next_pos=trajectory;
		}
		if(ball->cur_pos==p2pos){
			//if ball hits center, deflect depending on angle
			flag==1;
			ball->direction=1;
			ball->trajectory_ycoord=70;
			if(ball->tx==-1){
				ball->trajectory_xcoord=1;	
				trajectory=ball->cur_pos
				-ball->trajectory_xcoord
				+ball->trajectory_ycoord;
			}
			else if(ball->tx==1){
				ball->trajectory_xcoord=1;
				trajectory=ball->cur_pos
				+ball->trajectory_xcoord
				+ball->trajectory_ycoord;
			}
			else 
				ball->trajectory_xcoord=0;
				
			ball->ty=1;
			ball->next_pos=trajectory;
		}
		if(ball->cur_pos==p2pos+1){
			flag==1;
			ball->direction=1;
			ball->trajectory_xcoord=1;
			ball->trajectory_ycoord=70;
			
			trajectory=ball->cur_pos
			+ball->trajectory_xcoord
			+ball->trajectory_ycoord;
			ball->tx=1;
			ball->ty=1;
			
			ball->next_pos=trajectory;
		}
		if(ball->cur_pos==p2pos+2){
			flag==1;
			ball->direction=1;
			ball->trajectory_xcoord=2;
			ball->trajectory_ycoord=70;
			
			trajectory=ball->cur_pos
			+ball->trajectory_xcoord
			+ball->trajectory_ycoord;
			ball->tx=1;
			ball->ty=1;
			
			ball->next_pos=trajectory;
		}
			
		//check if ball hits player 1 wall
		if(ball->cur_pos>=PLAYER1_STAGE_END_POINT-70){
			ball->direction=0;
			ball->trajectory_xcoord=0;
			ball->trajectory_ycoord=0;
			ball->tx=0;
			ball->ty=0;
			p2score++;
			compute_score(&p2score,p2Score);	
		}
		//check if ball hits player 2 wall
		else if(ball->cur_pos<=70){
			ball->direction=0;
			ball->trajectory_xcoord=0;
			ball->trajectory_ycoord=0;
			ball->tx=0;
			ball->ty=0;
			p1score++;
			compute_score(&p1score,p1Score);
		}
	
		//check if ball hits left wall
		if((ball->cur_pos-2)%70==0){
			ball->tx=1;
		}
		//check if ball hits right wall
		if((ball->cur_pos+2)%70==0){
			ball->tx=-1;
		}
		
		if(flag==0){
			if(ball->direction==1){
				if(ball->tx==1&&ball->ty==1){
					trajectory=ball->cur_pos
					+ball->trajectory_xcoord
					+ball->trajectory_ycoord;
				}
				else if(ball->tx==1&&ball->ty==-1){
					trajectory=ball->cur_pos
					+ball->trajectory_xcoord
					-ball->trajectory_ycoord;
				}				
				else if(ball->tx==-1&&ball->ty==1){
					trajectory=ball->cur_pos
					-ball->trajectory_xcoord
					+ball->trajectory_ycoord;
				}
				else if(ball->tx==-1&&ball->ty==-1){
					trajectory=ball->cur_pos
					-ball->trajectory_xcoord
					-ball->trajectory_ycoord;	
				}
				else if(ball->tx==0&&ball->ty==0){
					trajectory=ball->cur_pos+70;	
				}
				else if(ball->tx==0&&ball->ty==1){
					trajectory=ball->cur_pos
					+ball->trajectory_ycoord;
				}
				else if(ball->tx==0&&ball->ty==-1){
					trajectory=ball->cur_pos
					+ball->trajectory_ycoord;
				}
				
				ball->cur_pos=trajectory;
			}
			else if(ball->direction==2){
				if(ball->tx==1&&ball->ty==1){
					trajectory=ball->cur_pos
					+ball->trajectory_xcoord
					+ball->trajectory_ycoord;
				}
				else if(ball->tx==1&&ball->ty==-1){
					trajectory=ball->cur_pos
					+ball->trajectory_xcoord
					-ball->trajectory_ycoord;
				}				
				else if(ball->tx==-1&&ball->ty==1){
					trajectory=ball->cur_pos
					-ball->trajectory_xcoord
					+ball->trajectory_ycoord;
				}
				else if(ball->tx==-1&&ball->ty==-1){
					trajectory=ball->cur_pos
					-ball->trajectory_xcoord
					-ball->trajectory_ycoord;	
				}
				
				else if(ball->tx==0&&ball->ty==0){
					trajectory=ball->cur_pos-70;	
				}
				else if(ball->tx==0&&ball->ty==1){
					trajectory=ball->cur_pos
					-ball->trajectory_ycoord;
				}
				else if(ball->tx==0&&ball->ty==-1){
					trajectory=ball->cur_pos
					-ball->trajectory_ycoord;
				}
				
				ball->cur_pos=trajectory;
			}
		}
		else{
			ball->cur_pos=ball->next_pos;
		}
		
}

void plot_ball(int pos, CHAR_INFO *mirror_screen){
		
		mirror_screen[pos].Char.AsciiChar='0';
		mirror_screen[pos].Attributes=BACKGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_RED|FOREGROUND_RED;
}

void compute_score(int *s, char *score){
	
	int i=0,j,x,tmp,temp[4];
	tmp=*s;
	while(i<4){
		x=(*s)%10;
		(*s)/=10;
		score[i++]=x+'0';
	}
	score[i]='\0';

	i=0,j=3;
	while(i<4){
		temp[i++]=score[j--];
	}
	i=0;
	while(i<4){
		score[i]=temp[i];
		i++;
	}
	*s=tmp;
	
}

int loading(){
	int key;
	fflush(stdin);
	key=getch();
	
	if(key=='Q'||key=='q'||key==ESC){
		exit(0);
	}
	else if(key==UP){
		return 1;
	}
	else if(key==DOWN){
		return 2;
	}
	else if(key==ENTER){
		return 3;
	}
}

void plot_loading(int selector, CHAR_INFO *mirror_screen){

	int i=0;
	//set bg color
	while(i<2450){
		mirror_screen[i].Char.AsciiChar=' ';
		mirror_screen[i].Attributes=FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_RED;
		i++;
	}
	i=70;
	
	//set coorp trademark
	{
		i+=55;
		mirror_screen[i].Char.AsciiChar='B';
		mirror_screen[i].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+1].Char.AsciiChar='i';
		mirror_screen[i+1].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+2].Char.AsciiChar='n';
		mirror_screen[i+2].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+3].Char.AsciiChar='a';
		mirror_screen[i+3].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+4].Char.AsciiChar='r';
		mirror_screen[i+4].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+5].Char.AsciiChar='y';
		mirror_screen[i+5].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
	    mirror_screen[i+6].Char.AsciiChar='B';
		mirror_screen[i+6].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+7].Char.AsciiChar='r';
		mirror_screen[i+7].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+8].Char.AsciiChar='o';
		mirror_screen[i+8].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+9].Char.AsciiChar='s';
		mirror_screen[i+9].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+10].Char.AsciiChar='.';
		mirror_screen[i+10].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+11].Char.AsciiChar='i';
		mirror_screen[i+11].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+12].Char.AsciiChar='n';
		mirror_screen[i+12].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+13].Char.AsciiChar='c';
		mirror_screen[i+13].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
	}
    
	//set game title
	{
		i=((35*70)/3)-22;
		mirror_screen[i].Char.AsciiChar='-';
		mirror_screen[i].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
		mirror_screen[i+1].Char.AsciiChar='=';
		mirror_screen[i+1].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
	
		mirror_screen[i+2].Char.AsciiChar='P';
		mirror_screen[i+2].Attributes=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+3].Char.AsciiChar=' ';
		mirror_screen[i+3].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+4].Char.AsciiChar='I';
		mirror_screen[i+4].Attributes=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+5].Char.AsciiChar=' ';
		mirror_screen[i+5].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+6].Char.AsciiChar='N';
		mirror_screen[i+6].Attributes=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+7].Char.AsciiChar=' ';
		mirror_screen[i+7].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+8].Char.AsciiChar='G';
		mirror_screen[i+8].Attributes=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
	    
		mirror_screen[i+9].Char.AsciiChar=' ';
		mirror_screen[i+9].Attributes=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+10].Char.AsciiChar='-';
		mirror_screen[i+10].Attributes=FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+11].Char.AsciiChar=' ';
		mirror_screen[i+11].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		
		mirror_screen[i+12].Char.AsciiChar='P';
		mirror_screen[i+12].Attributes=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+13].Char.AsciiChar=' ';
		mirror_screen[i+13].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		
		mirror_screen[i+14].Char.AsciiChar='O';
		mirror_screen[i+14].Attributes=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+15].Char.AsciiChar=' ';
		mirror_screen[i+15].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+16].Char.AsciiChar='N';
		mirror_screen[i+16].Attributes=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+17].Char.AsciiChar=' ';
		mirror_screen[i+17].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+18].Char.AsciiChar='G';
		mirror_screen[i+18].Attributes=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+19].Char.AsciiChar=' ';
		mirror_screen[i+19].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE;
		
		mirror_screen[i+20].Char.AsciiChar='=';
		mirror_screen[i+20].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;
	    mirror_screen[i+21].Char.AsciiChar='-';
		mirror_screen[i+21].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE;;
	}
	
	//set game instructiions
    {
    	i+=140+7;
		mirror_screen[i].Char.AsciiChar='C';
		mirror_screen[i].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+1].Char.AsciiChar='O';
		mirror_screen[i+1].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+2].Char.AsciiChar='N';
		mirror_screen[i+2].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+3].Char.AsciiChar='T';
		mirror_screen[i+3].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+4].Char.AsciiChar='R';
		mirror_screen[i+4].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+5].Char.AsciiChar='O';
		mirror_screen[i+5].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+6].Char.AsciiChar='L';
		mirror_screen[i+6].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+7].Char.AsciiChar='S';
		mirror_screen[i+7].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
	
		i+=140-7;
	    mirror_screen[i+10].Char.AsciiChar='^';
		mirror_screen[i+10].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;	
		mirror_screen[i+17].Char.AsciiChar='Z';
		mirror_screen[i+17].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_GREEN;
		mirror_screen[i+18].Char.AsciiChar='\\';
		mirror_screen[i+18].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+19].Char.AsciiChar='B';
		mirror_screen[i+19].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+20].Char.AsciiChar='A';
		mirror_screen[i+20].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+21].Char.AsciiChar='L';
		mirror_screen[i+21].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+22].Char.AsciiChar='L';
		mirror_screen[i+22].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		
		
		i+=70-1;
		mirror_screen[i].Char.AsciiChar='A';
		mirror_screen[i].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_GREEN;
		mirror_screen[i+1].Char.AsciiChar='\\';
		mirror_screen[i+1].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+2].Char.AsciiChar='L';
		mirror_screen[i+2].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+3].Char.AsciiChar='E';
		mirror_screen[i+3].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+4].Char.AsciiChar='F';
		mirror_screen[i+4].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+5].Char.AsciiChar='T';
		mirror_screen[i+5].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		
		mirror_screen[i+7].Char.AsciiChar='<';
		mirror_screen[i+7].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
	
		
		mirror_screen[i+15].Char.AsciiChar='>';
		mirror_screen[i+15].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
		mirror_screen[i+17].Char.AsciiChar='S';
		mirror_screen[i+17].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_GREEN;
		mirror_screen[i+18].Char.AsciiChar='\\';
		mirror_screen[i+18].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+19].Char.AsciiChar='R';
		mirror_screen[i+19].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+20].Char.AsciiChar='I';
		mirror_screen[i+20].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+21].Char.AsciiChar='G';
		mirror_screen[i+21].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+22].Char.AsciiChar='H';
		mirror_screen[i+22].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+23].Char.AsciiChar='T';
		mirror_screen[i+23].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		
		i+=140;
	    mirror_screen[i+1].Char.AsciiChar='Q';
		mirror_screen[i+1].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
		mirror_screen[i+2].Char.AsciiChar='/';
		mirror_screen[i+2].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
		mirror_screen[i+3].Char.AsciiChar='E';
		mirror_screen[i+3].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
		mirror_screen[i+4].Char.AsciiChar='S';
		mirror_screen[i+4].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
		mirror_screen[i+5].Char.AsciiChar='C';
		mirror_screen[i+5].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
		
		mirror_screen[i+17].Char.AsciiChar='Q';
		mirror_screen[i+17].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_GREEN;
		mirror_screen[i+18].Char.AsciiChar='U';
		mirror_screen[i+18].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+19].Char.AsciiChar='I';
		mirror_screen[i+19].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+20].Char.AsciiChar='T';
		mirror_screen[i+20].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+21].Char.AsciiChar='!';
		mirror_screen[i+21].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		
		i+=140;
	    mirror_screen[i+1].Char.AsciiChar='[';
		mirror_screen[i+1].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
		mirror_screen[i+2].Char.AsciiChar='A';
		mirror_screen[i+2].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
		mirror_screen[i+3].Char.AsciiChar='O';
		mirror_screen[i+3].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
		mirror_screen[i+4].Char.AsciiChar='K';
		mirror_screen[i+4].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
		mirror_screen[i+5].Char.AsciiChar=']';
		mirror_screen[i+5].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_RED;
		
		mirror_screen[i+17].Char.AsciiChar='P';
		mirror_screen[i+17].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_GREEN;
		mirror_screen[i+18].Char.AsciiChar='A';
		mirror_screen[i+18].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+19].Char.AsciiChar='U';
		mirror_screen[i+19].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+20].Char.AsciiChar='S';
		mirror_screen[i+20].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+21].Char.AsciiChar='E';
		mirror_screen[i+21].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
	}
	
	
	//set game info
	{
		i+=148;
		mirror_screen[i].Char.AsciiChar='*';
		mirror_screen[i].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+1].Char.AsciiChar='M';
		mirror_screen[i+1].Attributes=FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+2].Char.AsciiChar='O';
		mirror_screen[i+2].Attributes=FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+3].Char.AsciiChar='D';
		mirror_screen[i+3].Attributes=FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+4].Char.AsciiChar='E';
		mirror_screen[i+4].Attributes=FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		mirror_screen[i+5].Char.AsciiChar='*';
		mirror_screen[i+5].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE;
		
		i+=137;
		mirror_screen[i].Char.AsciiChar='S';
		mirror_screen[i].Attributes=FOREGROUND_RED|FOREGROUND_INTENSITY;
		mirror_screen[i+1].Char.AsciiChar='e';
		mirror_screen[i+1].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+2].Char.AsciiChar='l';
		mirror_screen[i+2].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+3].Char.AsciiChar='e';
		mirror_screen[i+3].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+4].Char.AsciiChar='c';
		mirror_screen[i+4].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+5].Char.AsciiChar='t';
		mirror_screen[i+5].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+6].Char.AsciiChar=' ';
		mirror_screen[i+6].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+7].Char.AsciiChar='m';
		mirror_screen[i+7].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+8].Char.AsciiChar='o';
		mirror_screen[i+8].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+9].Char.AsciiChar='d';
		mirror_screen[i+9].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+10].Char.AsciiChar='e';
		mirror_screen[i+10].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+11].Char.AsciiChar=':';
		mirror_screen[i+11].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		
		i+=143;
		int x=i-3;	//used when selecting mode
		mirror_screen[i].Char.AsciiChar='1';
		mirror_screen[i].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+1].Char.AsciiChar=':';
		mirror_screen[i+1].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+3].Char.AsciiChar=' ';
		mirror_screen[i+3].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+4].Char.AsciiChar='C';
		mirror_screen[i+4].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+5].Char.AsciiChar='P';
		mirror_screen[i+5].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+6].Char.AsciiChar='U';
		mirror_screen[i+6].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		
		i+=70;
		int y=i-3;	//used when selecting mode
		mirror_screen[i].Char.AsciiChar='2';
		mirror_screen[i].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+1].Char.AsciiChar=':';
		mirror_screen[i+1].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+3].Char.AsciiChar=' ';
		mirror_screen[i+3].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+4].Char.AsciiChar='2';
		mirror_screen[i+4].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+5].Char.AsciiChar='P';
		mirror_screen[i+5].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[i+6].Char.AsciiChar='L';
		mirror_screen[i+6].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		
		//generic message to keep screen looking busy
		i=(70*35)-13;
		//print data loaded to screen
		{
			mirror_screen[i].Char.AsciiChar='D';
			mirror_screen[i].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
			mirror_screen[i+1].Char.AsciiChar='a';
			mirror_screen[i+1].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
			mirror_screen[i+2].Char.AsciiChar='t';
			mirror_screen[i+2].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
			mirror_screen[i+3].Char.AsciiChar='a';
			mirror_screen[i+3].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
			mirror_screen[i+4].Char.AsciiChar='L';
			mirror_screen[i+4].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
			mirror_screen[i+5].Char.AsciiChar='o';
			mirror_screen[i+5].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
		    mirror_screen[i+6].Char.AsciiChar='a';
			mirror_screen[i+6].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
			mirror_screen[i+7].Char.AsciiChar='d';
			mirror_screen[i+7].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
			mirror_screen[i+8].Char.AsciiChar='e';
			mirror_screen[i+8].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
			mirror_screen[i+9].Char.AsciiChar='d';
			mirror_screen[i+9].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
			mirror_screen[i+10].Char.AsciiChar='.';
			mirror_screen[i+10].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
			mirror_screen[i+11].Char.AsciiChar='.';
			mirror_screen[i+11].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
			mirror_screen[i+12].Char.AsciiChar='.';
			mirror_screen[i+12].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
		}	
		if(selector==1){
			mirror_screen[x].Char.AsciiChar='-';
			mirror_screen[x].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY|BACKGROUND_RED;
			mirror_screen[x+1].Char.AsciiChar='>';
			mirror_screen[x+1].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY|BACKGROUND_BLUE;	
		}
		else if(selector==2){
			mirror_screen[y].Char.AsciiChar='-';
			mirror_screen[y].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY|BACKGROUND_RED;
			mirror_screen[y+1].Char.AsciiChar='>';
			mirror_screen[y+1].Attributes=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY|BACKGROUND_BLUE;
		
		}
	}
}

void plot_score_board(CHAR_INFO *mirror_screen){
	int x=2382;
	//plot player 1 score
	{
		mirror_screen[x].Char.AsciiChar='P';
		mirror_screen[x].Attributes=FOREGROUND_RED|FOREGROUND_INTENSITY;
		mirror_screen[x+1].Char.AsciiChar='1';
		mirror_screen[x+1].Attributes=FOREGROUND_RED|FOREGROUND_INTENSITY;
		mirror_screen[x+2].Char.AsciiChar='S';
		mirror_screen[x+2].Attributes=FOREGROUND_RED|FOREGROUND_INTENSITY;
		mirror_screen[x+3].Char.AsciiChar='C';
		mirror_screen[x+3].Attributes=FOREGROUND_RED|FOREGROUND_INTENSITY;
		mirror_screen[x+4].Char.AsciiChar='O';
		mirror_screen[x+4].Attributes=FOREGROUND_RED|FOREGROUND_INTENSITY;
		mirror_screen[x+5].Char.AsciiChar='R';
		mirror_screen[x+5].Attributes=FOREGROUND_RED|FOREGROUND_INTENSITY;
		mirror_screen[x+6].Char.AsciiChar='E';
		mirror_screen[x+6].Attributes=FOREGROUND_RED|FOREGROUND_INTENSITY;
		mirror_screen[x+7].Char.AsciiChar=':';
		mirror_screen[x+7].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[x+8].Char.AsciiChar=p1Score[1];
		mirror_screen[x+8].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[x+9].Char.AsciiChar=p1Score[2];
		mirror_screen[x+9].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[x+10].Char.AsciiChar=p1Score[3];
		mirror_screen[x+10].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
	}
	
	x+=28;
	//plot title
	{
		mirror_screen[x].Char.AsciiChar='-';
		mirror_screen[x].Attributes=FOREGROUND_RED|FOREGROUND_BLUE;
		mirror_screen[x+1].Char.AsciiChar='=';
		mirror_screen[x+1].Attributes=FOREGROUND_RED|FOREGROUND_GREEN;
		mirror_screen[x+2].Char.AsciiChar=' ';
		mirror_screen[x+2].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN;
		mirror_screen[x+3].Char.AsciiChar='P';
		mirror_screen[x+3].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[x+4].Char.AsciiChar='O';
		mirror_screen[x+4].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[x+5].Char.AsciiChar='N';
		mirror_screen[x+5].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[x+6].Char.AsciiChar='G';
		mirror_screen[x+6].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		mirror_screen[x+7].Char.AsciiChar=' ';
		mirror_screen[x+7].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN;
		mirror_screen[x+8].Char.AsciiChar='=';
		mirror_screen[x+8].Attributes=FOREGROUND_RED|FOREGROUND_GREEN;
		mirror_screen[x+9].Char.AsciiChar='-';
		mirror_screen[x+9].Attributes=FOREGROUND_RED|FOREGROUND_BLUE;
	}
	
	//plot player 2 score
	{
	
		if(game_mode==2){
			x+=28;
			mirror_screen[x].Char.AsciiChar='P';
			mirror_screen[x].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+1].Char.AsciiChar='2';
			mirror_screen[x+1].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+2].Char.AsciiChar='S';
			mirror_screen[x+2].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+3].Char.AsciiChar='C';
			mirror_screen[x+3].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+4].Char.AsciiChar='O';
			mirror_screen[x+4].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+5].Char.AsciiChar='R';
			mirror_screen[x+5].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+6].Char.AsciiChar='E';
			mirror_screen[x+6].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+7].Char.AsciiChar=':';
			mirror_screen[x+7].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
			mirror_screen[x+8].Char.AsciiChar=p2Score[1];
			mirror_screen[x+8].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
			mirror_screen[x+9].Char.AsciiChar=p2Score[2];
			mirror_screen[x+9].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
			mirror_screen[x+10].Char.AsciiChar=p2Score[3];
			mirror_screen[x+10].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		}
		else if(game_mode==1){
			x+=28;
			mirror_screen[x].Char.AsciiChar='C';
			mirror_screen[x].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+1].Char.AsciiChar='P';
			mirror_screen[x+1].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+2].Char.AsciiChar='U';
			mirror_screen[x+2].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+3].Char.AsciiChar='S';
			mirror_screen[x+3].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+4].Char.AsciiChar='C';
			mirror_screen[x+4].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+5].Char.AsciiChar='O';
			mirror_screen[x+5].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+6].Char.AsciiChar='R';
			mirror_screen[x+6].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+7].Char.AsciiChar='E';
			mirror_screen[x+7].Attributes=FOREGROUND_BLUE|FOREGROUND_INTENSITY;
			mirror_screen[x+8].Char.AsciiChar=':';
			mirror_screen[x+8].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
			mirror_screen[x+9].Char.AsciiChar=p2Score[1];
			mirror_screen[x+9].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
			mirror_screen[x+10].Char.AsciiChar=p2Score[2];
			mirror_screen[x+10].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
			mirror_screen[x+11].Char.AsciiChar=p2Score[3];
			mirror_screen[x+11].Attributes=FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		}
	}
}

void plot_player1(int x, CHAR_INFO *mirror_screen){
	
		mirror_screen[x-2].Char.AsciiChar='*';
		mirror_screen[x-2].Attributes=FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|BACKGROUND_INTENSITY|BACKGROUND_RED;
		
		
		mirror_screen[x-1].Char.AsciiChar='*';
		mirror_screen[x-1].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|BACKGROUND_INTENSITY|BACKGROUND_RED;
		
		mirror_screen[x].Char.AsciiChar='=';
		mirror_screen[x].Attributes=FOREGROUND_INTENSITY|FOREGROUND_BLUE|BACKGROUND_INTENSITY|BACKGROUND_RED;
		
		mirror_screen[x+1].Char.AsciiChar='*';
		mirror_screen[x+1].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|BACKGROUND_INTENSITY|BACKGROUND_RED;
		
		mirror_screen[x+2].Char.AsciiChar='*';	
		mirror_screen[x+2].Attributes=FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|BACKGROUND_INTENSITY|BACKGROUND_RED;
		
}

void plot_player2(int x, CHAR_INFO *mirror_screen){
	
		mirror_screen[x-2].Char.AsciiChar='*';
		mirror_screen[x-2].Attributes=FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|BACKGROUND_INTENSITY|BACKGROUND_BLUE;
		
		
		mirror_screen[x-1].Char.AsciiChar='*';
		mirror_screen[x-1].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|BACKGROUND_INTENSITY|BACKGROUND_BLUE;
		
		mirror_screen[x].Char.AsciiChar='=';
		mirror_screen[x].Attributes=FOREGROUND_INTENSITY|FOREGROUND_RED|BACKGROUND_INTENSITY|BACKGROUND_BLUE;
		
		mirror_screen[x+1].Char.AsciiChar='*';
		mirror_screen[x+1].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|BACKGROUND_INTENSITY|BACKGROUND_BLUE;
		
		mirror_screen[x+2].Char.AsciiChar='*';	
		mirror_screen[x+2].Attributes=FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|BACKGROUND_INTENSITY|BACKGROUND_BLUE;

}

void plot_game_over(CHAR_INFO *mirror_screen){
	int x;
	//print game over
	{
		x=(70*35)-220;
		x+=65;
		mirror_screen[x].Char.AsciiChar='G';
		mirror_screen[x].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
		mirror_screen[x+1].Char.AsciiChar='A';
		mirror_screen[x+1].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
		mirror_screen[x+2].Char.AsciiChar='M';
		mirror_screen[x+2].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
		mirror_screen[x+3].Char.AsciiChar='E';
		mirror_screen[x+3].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
		mirror_screen[x+4].Char.AsciiChar=' ';
		mirror_screen[x+4].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
		mirror_screen[x+5].Char.AsciiChar='O';
		mirror_screen[x+5].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
		mirror_screen[x+6].Char.AsciiChar='V';
		mirror_screen[x+6].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
		mirror_screen[x+7].Char.AsciiChar='E';
		mirror_screen[x+7].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
		mirror_screen[x+8].Char.AsciiChar='R';
		mirror_screen[x+8].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
		mirror_screen[x+9].Char.AsciiChar=' ';
		mirror_screen[x+9].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
		mirror_screen[x+10].Char.AsciiChar='.';
		mirror_screen[x+10].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
		mirror_screen[x+11].Char.AsciiChar='!';
		mirror_screen[x+11].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
		mirror_screen[x+12].Char.AsciiChar='!';
		mirror_screen[x+12].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
		mirror_screen[x+13].Char.AsciiChar='!';
		mirror_screen[x+13].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
	}
	//if new hiscore
	if(hiscore<score){
		FILE *fp=fopen("game.dat","w");
		if(fp==NULL){
			getch();
			exit(0);
		}
		fprintf(fp,"%d",score);
		fclose(fp);
		//High score
		{
			x+=70;
			mirror_screen[x].Char.AsciiChar='H';
			mirror_screen[x].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+1].Char.AsciiChar='i';
			mirror_screen[x+1].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+2].Char.AsciiChar='g';
			mirror_screen[x+2].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+3].Char.AsciiChar='h';
			mirror_screen[x+3].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+4].Char.AsciiChar=' ';
			mirror_screen[x+4].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+5].Char.AsciiChar='S';
			mirror_screen[x+5].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
			mirror_screen[x+6].Char.AsciiChar='c';
			mirror_screen[x+6].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
			mirror_screen[x+7].Char.AsciiChar='o';
			mirror_screen[x+7].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
			mirror_screen[x+8].Char.AsciiChar='r';
			mirror_screen[x+8].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
			mirror_screen[x+9].Char.AsciiChar='e';
			mirror_screen[x+9].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
			mirror_screen[x+10].Char.AsciiChar=':';
			mirror_screen[x+10].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+11].Char.AsciiChar=p1Score[1];
			mirror_screen[x+11].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+12].Char.AsciiChar=p1Score[2];
			mirror_screen[x+12].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+13].Char.AsciiChar=p1Score[3];
			mirror_screen[x+13].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;	
			
		}
	
	}
	else{
		//print score
		{
			x+=70;
			mirror_screen[x].Char.AsciiChar='C';
			mirror_screen[x].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+1].Char.AsciiChar='u';
			mirror_screen[x+1].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+2].Char.AsciiChar='r';
			mirror_screen[x+2].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+3].Char.AsciiChar='r';
			mirror_screen[x+3].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+4].Char.AsciiChar=' ';
			mirror_screen[x+4].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+5].Char.AsciiChar='S';
			mirror_screen[x+5].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
			mirror_screen[x+6].Char.AsciiChar='c';
			mirror_screen[x+6].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
			mirror_screen[x+7].Char.AsciiChar='o';
			mirror_screen[x+7].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
			mirror_screen[x+8].Char.AsciiChar='r';
			mirror_screen[x+8].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
			mirror_screen[x+9].Char.AsciiChar='e';
			mirror_screen[x+9].Attributes=FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
			mirror_screen[x+10].Char.AsciiChar=':';
			mirror_screen[x+10].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+11].Char.AsciiChar=p2Score[1];
			mirror_screen[x+11].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+12].Char.AsciiChar=p2Score[2];
			mirror_screen[x+12].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;
			mirror_screen[x+13].Char.AsciiChar=p2Score[3];
			mirror_screen[x+13].Attributes=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE;	
			
		}		
	}
}

void draw(HANDLE *h,CHAR_INFO *ms,COORD *mss,COORD *msp,SMALL_RECT *ws){
		WriteConsoleOutputA(*h,ms,*mss,*msp,ws);
}

void clear(CHAR_INFO *mirror_screen){
	int i=0,row=1,j,l=0;
	//draw player2 stage
	while(i<70){
		mirror_screen[i].Char.AsciiChar='-';
		mirror_screen[i].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_GREEN;
		
		i++;
	}
	while(i<PLAYER2_STAGE_END_POINT){
		mirror_screen[i].Char.AsciiChar=' ';
		mirror_screen[i].Attributes=BACKGROUND_INTENSITY;
		i++;	
	}
	
	while(i<(MAX_FIELD_LENGTH+69)){
		mirror_screen[i].Char.AsciiChar=' ';
		mirror_screen[i].Attributes=FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_RED|
		FOREGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
		i++;
	}
	//draw player1 stage
	while(i<PLAYER1_STAGE_END_POINT-70){
			mirror_screen[i].Char.AsciiChar=' ';
			mirror_screen[i].Attributes=BACKGROUND_INTENSITY;
			i++;
	}
	while(i<PLAYER1_STAGE_END_POINT){
			mirror_screen[i].Char.AsciiChar='-';
			mirror_screen[i].Attributes=FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_GREEN;
		
			i++;
	}
	//draw info stage
	while(i<INFO_STAGE_END_POINT){
		mirror_screen[i].Char.AsciiChar=' ';
		mirror_screen[i].Attributes=FOREGROUND_INTENSITY;
		i++;
	}
}

