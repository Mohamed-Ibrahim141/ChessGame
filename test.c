#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
/*this structure contains the pieces in the board and "castling" and "en passant"can be done using
"order" and "moved" in this structures*/
typedef struct{
    char piece;
    int order;
    int moved;
}board_setup;
//this structure is used for taking input from the user
//each row or column must be an integer from 0 to 7
typedef struct{
    int row1;
    int col1;
    int row2;
    int col2;
}moveStruct;
/*this structure is used to save each move details like the 2 squares selected and piece removed
and the position of each king and numer of pieces removed from each player*/
typedef struct{
    int c_player;
    moveStruct c_move;
    char c_piece_removed;
    int c_k_row;
    int c_k_col;
    int w_k_moved;
    int w_r1_moved;
    int w_r2_moved;
    int b_k_moved;
    int b_r1_moved;
    int b_r2_moved;
    int c_promotion;
    int c_castling;
    int c_en_passant;
    int c_r_col;
    int c_check;
}savedMove;
board_setup board[8][8];
//we made an assumption here that max game length is 1000 move
savedMove all_moves[1000];
int n_all_moves=0;
moveStruct player_move;
int i,j,check_player=0,valid_move,valid_promotion,last_removed=0
,valid_castling,valid_en_passant,r_col,check,check_promotion,check_undo;
int king_row[2]={7,0},king_col[2]={4,4},check_king=1,game_is_running=1,just_checking;
char promotion,w_removed[17],b_removed[17],piece_removed,board_copy[8][8];
int n_w_removed=0,n_b_removed=0,new_game=1,restart_game=0,check_draw=0,order_copy[8][8],indexes[13];
//this function is used to put each piece in its correct square
void setup()
{
    for(i=0;i<8;i++){
        board[1][i].piece='P';
        board[1][i].moved=board[6][i].moved=0;
        board[6][i].piece='p';
        board[1][i].order=i+1;
        board[6][i].order=i+1;
    }
    board[0][0].piece=board[0][7].piece='R';
    board[0][1].piece=board[0][6].piece='N';
    board[0][2].piece=board[0][5].piece='B';
    board[0][3].piece='Q';
    board[0][4].piece='K';
    board[7][0].piece=board[7][7].piece='r';
    board[7][1].piece=board[7][6].piece='n';
    board[7][2].piece=board[7][5].piece='b';
    board[7][3].piece='q';
    board[7][4].piece='k';
    board[0][6].moved=board[7][5].moved=0;
    board[0][0].moved=board[0][7].moved=0;
    board[7][0].moved=board[7][7].moved=0;
    for(i=2;i<6;i++){
        for(j=0;j<8;j++){
            board[i][j].piece=' ';
            board[i][j].order=0;
        }
    }
}
/*displaying board by clearing the last board the printing a new one with the current user,
pieces removes from each player,whether it's check or not ,if the game has ended it prints the winner
if exists or prints it's a draw and some other guides for the user*/
void display_board()
{
    system("cls");
    printf("\n\tWhite pieces are lowercase blue letters\n\tBlack pieces are uppercase red letters\n");
    printf("\n\t\t");
    for(i=0;i<8;i++){
        printf("%c ",i+65);
    }printf("\n\n");
    for(i=0;i<8;i++){
        printf("\t%d\t",8-i);
        for(j=0;j<8;j++){
            if(isalpha(board[i][j].piece)){
                (isupper(board[i][j].piece))?printf("\x1b[31m"):printf("\x1b[34m");
                printf("%c ",board[i][j].piece);
                printf("\x1b[0m");
            }else {
                ((i+j)%2==0)?printf("\x1b[34m"):printf("\x1b[31m");
                ((i+j)%2==0)?printf("- "):printf(". ");
                printf("\x1b[0m");
            }
        }printf("\t%d",8-i);
        printf("\n");
    }printf("\n");
    printf("\t\t");
    for(i=0;i<8;i++){
        printf("%c ",i+65);
    }printf("\n\n\n\tWhite removed pieces: ");
    for(i=0;i<n_w_removed;i++){
        printf("\x1b[34m");
        printf("%c ",w_removed[i]);
        printf("\x1b[0m");
    }printf("\n\tBlack removed pieces: ");
    for(i=0;i<n_b_removed;i++){
        printf("\x1b[31m");
        printf("%c ",b_removed[i]);
        printf("\x1b[0m");
    }printf("\n");
    if(game_is_running){
        if(!check_king){
        printf("\tCheck!\n");
        }
    (check_player%2==0)?printf("\tPlayer 1 turn..\n"):printf("\tplayer 2 turn..\n");
    printf("\tPress U to undo..\n\tPress N to start new game..\n");
    }
}
//this  function takes two parameters as they represent a square and returns whether this square is in check or not
/*this can be done by checking the 2 squares where paws can threaten the square which we are checking
and then it checks the 8 possible squares for a knight and check vertically, horizontally and diagonally
for queens , bishops and rocks*/
int if_check(int x,int y)
{
    int to_return=1;
    if(abs(abs(king_row[0]-king_row[1])+abs(king_col[0]-king_col[1]))==1
    ||(king_row[0]==king_row[1]&&king_col[0]==king_col[1])
    ||(abs(king_row[0]-king_row[1])==1&&abs(king_col[0]-king_col[1])==1)){
        to_return=0;
    }
    if(x-1+2*check_player>=0&&x+1-2*check_player<8&&y+1<8){
        if((!check_player&&board[x-1+2*check_player][y+1].piece=='P')||
        (check_player&&board[x-1+2*check_player][y+1].piece=='p')){
            to_return=0;
        }
    }if(x-1+2*check_player>=0&&x+1-2*check_player<8&&y-1>=0){
        if((!check_player&&board[x-1+2*check_player][y-1].piece=='P')||
        (check_player&&board[x-1+2*check_player][y-1].piece=='p')){
            to_return=0;
        }
    }if(x+2<8&&y+1<8){
        if((!check_player&&board[x+2][y+1].piece=='N')||
        (check_player&&board[x+2][y+1].piece=='n')){
            to_return=0;
        }
    }if(x+2<8&&y-1>=0){
        if((!check_player&&board[x+2][y-1].piece=='N')||
        (check_player&&board[x+2][y-1].piece=='n')){
            to_return=0;
        }
    }if(x-2>=0&&y+1<8){
        if((!check_player&&board[x-2][y+1].piece=='N')||
        (check_player&&board[x-2][y+1].piece=='n')){
            to_return=0;
        }
    }if(x-2>=0&&y-1>=0){
        if((!check_player&&board[x-2][y-1].piece=='N')||
        (check_player&&board[x-2][y-1].piece=='n')){
            to_return=0;
        }
    }if(x+1<8&&y+2<8){
        if((!check_player&&board[x+1][y+2].piece=='N')||
        (check_player&&board[x+1][y+2].piece=='n')){
            to_return=0;
        }
    }if(x+1<8&&y-2>=0){
        if((!check_player&&board[x+1][y-2].piece=='N')||
        (check_player&&board[x+1][y-2].piece=='n')){
            to_return=0;
        }
    }if(x-1>=0&&y+2<8){
        if((!check_player&&board[x-1][y+2].piece=='N')||
        (check_player&&board[x-1][y+2].piece=='n')){
            to_return=0;
        }
    }if(x-1>=0&&y-2>=0){
        if((!check_player&&board[x-1][y-2].piece=='N')||
        (check_player&&board[x-1][y-2].piece=='n')){
            to_return=0;
        }
    }i=1;
    while(x+i<8){
        if(board[x+i][y].piece==' '){
            i++;
            continue;
        }else{
            if((!check_player&&board[x+i][y].piece=='R')||
            (check_player&&board[x+i][y].piece=='r')||
            (!check_player&&board[x+i][y].piece=='Q')||
            (check_player&&board[x+i][y].piece=='q')){
                to_return=0;
            }break;
        }
    }i=1;
    while(x-i>=0){
        if(board[x-i][y].piece==' '){
            i++;
            continue;
        }else{
            if((!check_player&&board[x-i][y].piece=='R')||
            (check_player&&board[x-i][y].piece=='r')||
            (!check_player&&board[x-i][y].piece=='Q')||
            (check_player&&board[x-i][y].piece=='q')){
                to_return=0;
            }break;
        }
    }i=1;
    while(y+i<8){
        if(board[x][y+i].piece==' '){
            i++;
            continue;
        }else{
            if((!check_player&&board[x][y+i].piece=='R')||
            (check_player&&board[x][y+i].piece=='r')||
            (!check_player&&board[x][y+i].piece=='Q')||
            (check_player&&board[x][y+i].piece=='q')){
                to_return=0;
            }break;
        }
    }i=1;
    while(y-i>=0){
        if(board[x][y-i].piece==' '){
            i++;
            continue;
        }else{
            if((!check_player&&board[x][y-i].piece=='R')||
            (check_player&&board[x][y-i].piece=='r')||
            (!check_player&&board[x][y-i].piece=='Q')||
            (check_player&&board[x][y-i].piece=='q')){
                to_return=0;
            }break;
        }
    }i=1;
    while(x+i<8&&y+i<8){
        if(board[x+i][y+i].piece==' '){
            i++;
            continue;
        }else{
            if((!check_player&&board[x+i][y+i].piece=='B')||
            (check_player&&board[x+i][y+i].piece=='b')||
            (!check_player&&board[x+i][y+i].piece=='Q')||
            (check_player&&board[x+i][y+i].piece=='q')){
                to_return=0;
            }break;
        }
    }i=1;
    while(x+i<8&&y-i>=0){
        if(board[x+i][y-i].piece==' '){
            i++;
            continue;
        }else{
            if((!check_player&&board[x+i][y-i].piece=='B')||
            (check_player&&board[x+i][y-i].piece=='b')||
            (!check_player&&board[x+i][y-i].piece=='Q')||
            (check_player&&board[x+i][y-i].piece=='q')){
                to_return=0;
            }break;
        }
    }i=1;
    while(x-i>=0&&y+i<8){
        if(board[x-i][y+i].piece==' '){
            i++;
            continue;
        }else{
            if((!check_player&&board[x-i][y+i].piece=='B')||
            (check_player&&board[x-i][y+i].piece=='b')||
            (!check_player&&board[x-i][y+i].piece=='Q')||
            (check_player&&board[x-i][y+i].piece=='q')){
                to_return=0;
            }break;
        }
    }i=1;
    while(x-i>=0&&y-i>=0){
        if(board[x-i][y-i].piece==' '){
            i++;
            continue;
        }else{
            if((!check_player&&board[x-i][y-i].piece=='B')||
            (check_player&&board[x-i][y-i].piece=='b')||
            (!check_player&&board[x-i][y-i].piece=='Q')||
            (check_player&&board[x-i][y-i].piece=='q')){
                to_return=0;
            }break;
        }
    }return to_return;
    if(!just_checking&&!to_return){
        printf("\nking in danger\n");
    }
}
//if there is a valid "en passant" this function is just used to do the extra action needed to perform "en passant"
void do_en_passant()
{
    char temp;
    temp=board[player_move.row2+(int)pow(-1,check_player)][player_move.col2].piece;
    board[player_move.row2+(int)pow(-1,check_player)][player_move.col2].piece=
    board[player_move.row2][player_move.col2].piece;
    board[player_move.row2][player_move.col2].piece=temp;
}
//if there is a valid "castling" this function is just used to do the extra action needed to perform "castling"
void do_castling()
{
    char temp;
    temp=board[player_move.row1][player_move.col1+check].piece;
    board[player_move.row1][player_move.col1+check].piece=board[player_move.row1][r_col].piece;
    board[player_move.row1][r_col].piece=temp;
}
/*if the user inputs a valid move according to the board rules it will be done but if his own king
will be in check after the move he entered this function is used to undo the move and
then the user will be asked for another move*/
void forced_undo()
{
    if(toupper(board[player_move.row2][player_move.col2].piece)=='K'){
        king_row[check_player]=player_move.row1;
        king_col[check_player]=player_move.col1;
    }if(toupper(board[player_move.row2][player_move.col2].piece)=='R'){
        board[player_move.row1][player_move.col1].moved=0;
    }char temp=' ';
    if(last_removed=1){
        if(check_player){
            temp=w_removed[n_w_removed-1];
            n_w_removed--;
        }else {
            temp=b_removed[n_b_removed-1];
            n_b_removed--;
        }
    }board[player_move.row1][player_move.col1].piece=board[player_move.row2][player_move.col2].piece;
    board[player_move.row2][player_move.col2].piece=temp;
    if(valid_castling){
        do_castling();
        board[player_move.row1][player_move.col1].moved=0;
        board[player_move.row1][r_col].moved=0;
    }
}
//if the user inputs a valid move according to the board rules it will be done by this function
void do_move()
{
    if(toupper(board[player_move.row1][player_move.col1].piece)=='K'){
        king_row[check_player]=player_move.row2;
        king_col[check_player]=player_move.col2;
    }
    if(valid_en_passant){
        do_en_passant();
    }if(valid_castling){
        do_castling();
    }if(isupper(board[player_move.row2][player_move.col2].piece)){
        b_removed[n_b_removed]=board[player_move.row2][player_move.col2].piece;
        n_b_removed++;
        last_removed=1;
    }else if(islower(board[player_move.row2][player_move.col2].piece)){
        w_removed[n_w_removed]=board[player_move.row2][player_move.col2].piece;
        n_w_removed++;
        last_removed=1;
    }else{
        last_removed=0;
    }piece_removed=board[player_move.row2][player_move.col2].piece;
    board[player_move.row2][player_move.col2].piece=board[player_move.row1][player_move.col1].piece;
    board[player_move.row1][player_move.col1].piece=' ';
}
//if any pawn has reached the end of the board this function will be called to do the promotion
void do_promotion()
{
    char temp;
    temp=toupper(promotion);
    if(temp=='Q'||temp=='R'||temp=='B'||temp=='N'){
        valid_promotion=1;
        if(!check_player){
            temp=tolower(promotion);
        }board[player_move.row2][player_move.col2].piece=temp;
    }else{
        printf("Enter a valid piece for promotion..\n");
    }
}
/*this function defines the rules to move a pawn as it can be move one step forward
or one step diagonally with taking out his opponents piece
the last case valid is "en passant" but it needs checking other certain conditions*/
void p_move()
{
    if(player_move.row1==6-5*check_player){
        if((pow(-1,check_player)*(player_move.row1-player_move.row2)==1
        || pow(-1,check_player)*(player_move.row1-player_move.row2)==2)
        &&player_move.col1==player_move.col2
        &&board[player_move.row2][player_move.col2].piece==' '){
            valid_move=1;
        }else if((pow(-1,check_player)*(player_move.row1-player_move.row2)==1
        &&abs(player_move.col2-player_move.col1)==1
        &&board[player_move.row2][player_move.col2].piece!=' ')){
            valid_move=1;
        }else{
            valid_move=0;
        }
    }else {
        if((pow(-1,check_player)*(player_move.row1-player_move.row2)==1
        && player_move.col1==player_move.col2)&&
        board[player_move.row2][player_move.col2].piece==' '){
            valid_move=1;
        }else if(pow(-1,check_player)*(player_move.row1-player_move.row2)==1
        &&abs(player_move.col2-player_move.col1)==1
        &&board[player_move.row2][player_move.col2].piece!=' '){
            valid_move=1;
        }else if(pow(-1,check_player)*(player_move.row1-player_move.row2)==1
        &&board[player_move.row2][player_move.col2].piece==' '
        &&toupper(board[player_move.row2+(int)pow(-1,check_player)][player_move.col2].piece)=='P'
        &&board[player_move.row2][player_move.col2].order!=
        board[player_move.row2+(int)pow(-1,check_player)][player_move.col2].order
        &&abs(player_move.col2-player_move.col1)==1
        &&board[player_move.row2+(int)pow(-1,check_player)][player_move.col2].order==
        board[player_move.row2-(int)pow(-1,check_player)][player_move.col2].order){
            valid_move=1;
            valid_en_passant=1;
        }else{
            valid_move=0;
        }
    }if(valid_move){
        if(!just_checking){
            do_move();
            if(player_move.row2==0+7*check_player){
                valid_promotion=0;
                check_promotion=1;
            }
        }
    }
}
//this function defines the rules to move a knight as it moves one step in one axis and 2 steps in the other one
void n_move()
{
    if((abs(player_move.col1-player_move.col2)==1&&abs(player_move.row1-player_move.row2)==2)
    ||(abs(player_move.col1-player_move.col2)==2&&abs(player_move.row1-player_move.row2)==1)){
        if(!just_checking){
            do_move();
        }
    }else {
        valid_move=0;
    }
}
//this function defines the rules to move a rock as it can be moved vertically or horizontally
void r_move()
{
    if(player_move.row1==player_move.row2){
        int direction=abs(player_move.col2-player_move.col1)/(player_move.col2-player_move.col1);
        for(i=player_move.col1+1*direction;i!=player_move.col2;i=i+1*direction){
            if(board[player_move.row1][i].piece!=' '){
                valid_move=0;
            }
        }if(valid_move&&!just_checking){
            do_move();
            board[player_move.row1][player_move.col1].moved=1;
        }
    }else if(player_move.col1==player_move.col2){
        int direction=abs(player_move.row2-player_move.row1)/(player_move.row2-player_move.row1);
        for(i=player_move.row1+1*direction;i!=player_move.row2;i=i+1*direction){
            if(board[i][player_move.col2].piece!=' '){
                valid_move=0;
            }
        }if(valid_move&&!just_checking){
            do_move();
            board[player_move.row1][player_move.col1].moved=1;
        }
    }else {
        valid_move=0;
    }
}
//this function defines the rules to move a bishop as it can by moves just diagonally
void b_move()
{
    valid_move=1;
    int direction1=abs(player_move.row2-player_move.row1)/(player_move.row2-player_move.row1);
    int direction2=abs(player_move.col2-player_move.col1)/(player_move.col2-player_move.col1);
    if(abs(player_move.col2-player_move.col1)==abs(player_move.row2-player_move.row1)){
        for(i=1;i<abs(player_move.col2-player_move.col1);i++){
            if(board[player_move.row1+i*direction1][player_move.col1+i*direction2].piece!=' '){
                valid_move=0;
            }
        }
    }else{
        valid_move=0;
    }if(valid_move&&!just_checking){
        do_move();
    }
}
//the queen can be moved as a rock or a bishop
void q_move()
{
    r_move();
    if(!valid_move){
        b_move();
    }
}
/*this function defines the rules to move the king as it can be moved
one step in any direction or it can do castling after checking other certain conditions*/
void k_move()
{
    if(abs(abs(player_move.col1-player_move.col2)+abs(player_move.row1-player_move.row2))==1){
        if(!just_checking){
            board[player_move.row1][player_move.col1].moved=1;
            do_move();
        }
    }else if(abs(player_move.col1-player_move.col2)==1&&abs(player_move.row1-player_move.row2)==1){
        if(!just_checking){
            board[player_move.row1][player_move.col1].moved=1;
            do_move();
        }
    }else if(player_move.row1==player_move.row2&&abs(player_move.col2-player_move.col1)==2){
        valid_castling=1;
        if(player_move.col2>player_move.col1){
            r_col=7;
            check=1;
        }else{
            r_col=0;
            check=-1;
        }
        if(!((king_row[0]==7&&king_col[0]==4)||(king_row[1]==0&&king_col[1]==4))){
           valid_castling=0;
        }
        if(!((!check_player&&board[player_move.row1][r_col].piece=='r')
        ||(check_player&&board[player_move.row1][r_col].piece=='R'))){
            valid_castling=0;
        }
        if(board[7-7*check_player][4].moved==0
        &&board[player_move.row1][r_col].moved==0){
            for(i=1;i!=abs(r_col-4);i++){
                if(board[player_move.row1][4+i*check].piece!=' '){
                    valid_castling=0;
                }
            }
        }if(if_check(player_move.row1,player_move.col1)==0&&
        if_check(player_move.row1,r_col)==0){
            valid_castling=0;
        }
        if(valid_castling){
            if(!just_checking){
                board[player_move.row1][player_move.col1].moved=1;
                do_move();
            }
        }else{
            valid_move=0;
        }

    }else {
        valid_move=0;
    }if(valid_move){
        king_row[check_player]=player_move.row2;
        king_col[check_player]=player_move.col2;
    }
}
//this function is just a switch function to the correct piece function
void analyse_input()
{
    char temp=toupper(board[player_move.row1][player_move.col1].piece);
    switch(temp){
    case 'P':
        p_move();
        break;
    case 'R':
        r_move();
        break;
    case 'N':
        n_move();
        break;
    case 'B':
        b_move();
        break;
    case 'Q':
        q_move();
        break;
    case 'K':
        k_move();
        break;
    }
}
/*this function takes input from the user as string and then convert its characters with
ASCII code and there is an assumption that the character with index 4 is the promotion piece
but if the is not a valid promotion it's just ignored
this function check if the input is in the range of the board and
the first square contains a current player's piece and the second square doesn't*/
void taking_input()
{
    char temp[20];
    fgets(temp,20,stdin);
    printf("\n");;
    player_move.row1=7-((int)temp[0]-49);
    player_move.row2=7-((int)temp[2]-49);
    player_move.col1=(int)toupper(temp[1])-65;
    player_move.col2=(int)toupper(temp[3])-65;
    promotion=temp[4];
    if(!(toupper(temp[0])=='U'||toupper(temp[0])=='N')){
        if(player_move.col1>7 || player_move.col1<0
        || player_move.col2>7 || player_move.col2<0
        || player_move.row1>7 || player_move.row1<0
        || player_move.row2>7 || player_move.row2<0){
            valid_move=0;
        }else if(!check_player){
            if(!islower(board[player_move.row1][player_move.col1].piece)||
            islower(board[player_move.row2][player_move.col2].piece)){
                valid_move=0;
            }else{
                valid_move=1;
            }
        }else {
            if(!isupper(board[player_move.row1][player_move.col1].piece)||
            isupper(board[player_move.row2][player_move.col2].piece)){
                valid_move=0;
            }else{
                valid_move=1;
            }
        }if(valid_move){
            analyse_input();
        }
    }else if(toupper(temp[0])=='U'){
        check_undo=1;
    }else {
        restart_game=1;
    }
}
/*after every move this function is called
it tries all the combinations between the board squares as input and check if it's a valid move
if there isn't any valid moves the game is over with a checkmate if the current player's king is in check and
stalemate in the other case*/
void end_game()
{
    int r1=king_row[check_player],c1=king_col[check_player];
    int r2=king_row[(check_player+1)%2],c2=king_col[(check_player+1)%2];
    int valid_moves=0;
    int a,b,k,l;
    just_checking=1;
    for(a=0;a<8;a++){
        for(b=0;b<8;b++){
            if((!check_player&&islower(board[a][b].piece))
            ||(check_player&&isupper(board[a][b].piece))){
                for(k=0;k<8;k++){
                    for(l=0;l<8;l++){
                        if(!((!check_player&&islower(board[k][l].piece))
                        ||(check_player&&isupper(board[k][l].piece)))){
                            player_move.row1=a;
                            player_move.col1=b;
                            player_move.row2=k;
                            player_move.col2=l;
                            valid_move=1;
                            char temp=board[k][l].piece;
                            analyse_input();
                            if(valid_move){
                                board[k][l].piece=board[a][b].piece;
                                board[a][b].piece=' ';
                                valid_move=if_check(king_row[check_player],king_col[check_player]);
                                board[a][b].piece=board[k][l].piece;
                                board[k][l].piece=temp;
                                king_row[check_player]=r1;
                                king_col[check_player]=c1;
                                king_row[(check_player+1)%2]=r2;
                                king_col[(check_player+1)%2]=c2;
                            }
                            if(valid_move){
                                valid_moves++;
                            }
                        }
                    }
                }
            }
        }
    }if(valid_moves==0){
        game_is_running=0;
    }
}
/*this function checks if the current remaining pieces can't lead to check mate then the game is
over with a draw*/
void dead_position()
{
    if(n_w_removed==15&&n_b_removed==15){
        game_is_running=0;
        check_draw=1;
    }else if(n_w_removed==14&&n_b_removed==15){
        int n_kinghts=0;
        int n_bishobs=0;
        for(i=0;i<n_w_removed;i++){
            if(w_removed[i]=='n'){
                n_kinghts++;
            }else if(w_removed[i]=='b'){
                n_bishobs++;
            }
        }if(n_bishobs==1||n_kinghts==1){
            game_is_running=0;
            check_draw=1;
        }
    }else if(n_b_removed==14&&n_w_removed==15){
        int n_kinghts=0;
        int n_bishobs=0;
        for(i=0;i<n_b_removed;i++){
            if(b_removed[i]=='N'){
                n_kinghts++;
            }else if(b_removed[i]=='B'){
                n_bishobs++;
            }
        }if(n_bishobs==1||n_kinghts==1){
            game_is_running=0;
            check_draw=1;
        }
    }else if(n_w_removed==14&&n_b_removed==14){
        int n_bishobs=0,bishob_color[2]={0};
        for(i=0;i<8;i++){
            for(j=0;j<8;j++){
                if(toupper(board[i][j].piece)=='P'){
                    bishob_color[n_bishobs]=(i+j)%2;
                    n_bishobs++;
                }
            }
        }if(n_bishobs==2&&bishob_color[0]==bishob_color[1]){
            game_is_running=0;
            check_draw=1;
        }
    }
}
/*with each valid move the user enters this function save all the move details
in an array of structures containing all the moves played*/
void saving_move()
{
    all_moves[n_all_moves].c_player=check_player;
    all_moves[n_all_moves].c_move.row1=player_move.row1;
    all_moves[n_all_moves].c_move.col1=player_move.col1;
    all_moves[n_all_moves].c_move.row2=player_move.row2;
    all_moves[n_all_moves].c_move.col2=player_move.col2;
    all_moves[n_all_moves].c_piece_removed=piece_removed;
    all_moves[n_all_moves].c_k_row=king_row[check_player];
    all_moves[n_all_moves].c_k_col=king_col[check_player];
    all_moves[n_all_moves].w_k_moved=board[7][4].moved;
    all_moves[n_all_moves].b_k_moved=board[0][4].moved;
    all_moves[n_all_moves].w_r1_moved=board[7][0].moved;
    all_moves[n_all_moves].w_r2_moved=board[7][7].moved;
    all_moves[n_all_moves].b_r1_moved=board[0][0].moved;
    all_moves[n_all_moves].b_r2_moved=board[0][7].moved;
    all_moves[n_all_moves].c_r_col=r_col;
    all_moves[n_all_moves].c_promotion=check_promotion;
    all_moves[n_all_moves].c_castling=valid_castling;
    all_moves[n_all_moves].c_en_passant=valid_en_passant;
    all_moves[n_all_moves].c_check=check;
    n_all_moves++;
}
/*this function is used when the user enters 'u' and the number of moves is
greater than 0 as it can undo the moves to the first move and in each times
it's called it updates the number of moves */
void undo_move()
{
    n_all_moves--;
    check_player=all_moves[n_all_moves].c_player;
    player_move.row1=all_moves[n_all_moves].c_move.row1;
    player_move.col1=all_moves[n_all_moves].c_move.col1;
    player_move.row2=all_moves[n_all_moves].c_move.row2;
    player_move.col2=all_moves[n_all_moves].c_move.col2;
    check=all_moves[n_all_moves].c_check;
    r_col=all_moves[n_all_moves].c_r_col;
    board[7][4].moved=all_moves[n_all_moves].w_k_moved;
    board[0][4].moved=all_moves[n_all_moves].b_k_moved;
    board[7][0].moved=all_moves[n_all_moves].w_r1_moved;
    board[7][7].moved=all_moves[n_all_moves].w_r2_moved;
    board[0][0].moved=all_moves[n_all_moves].b_r1_moved;
    board[0][7].moved=all_moves[n_all_moves].b_r2_moved;
    board[player_move.row1][player_move.col1].piece=board[player_move.row2][player_move.col2].piece;
    board[player_move.row2][player_move.col2].piece=all_moves[n_all_moves].c_piece_removed;
    if(isupper(all_moves[n_all_moves].c_piece_removed)){
        n_b_removed--;
    }else if(islower(all_moves[n_all_moves].c_piece_removed)){
        n_w_removed--;
    }if(all_moves[n_all_moves].c_promotion){
        if(!check_player){
            board[player_move.row1][player_move.col1].piece='p';
        }else if(check_player){
            board[player_move.row1][player_move.col1].piece='P';
        }
    }if(all_moves[n_all_moves].c_castling){
        do_castling();
    }if(all_moves[n_all_moves].c_en_passant){
        do_en_passant();
    }
}
/*this function is called after every successful move as it updates the data of the game saved in "txt" files
one file is for board pieces , other one for the board order , another one for the indexes and the last two are
for the pieces removed from each player*/
void save()
{
    for(i=0;i<8;i++){
        for(j=0;j<8;j++){
            board_copy[i][j]=board[i][j].piece;
            order_copy[i][j]=board[i][j].order;
        }
    }FILE *fp1;
    fp1=fopen("board_pieces.txt","wb");
    fwrite(board_copy,sizeof(char),64,fp1);
    fclose(fp1);
    FILE *fp2;
    fp2=fopen("board_order.txt","wb");
    fwrite(order_copy,sizeof(int),64,fp2);
    fclose(fp2);
    indexes[0]=check_player;
    indexes[1]=n_w_removed;
    indexes[2]=n_b_removed;
    indexes[3]=board[7][4].moved;
    indexes[4]=board[0][4].moved;
    indexes[5]=board[7][0].moved;
    indexes[6]=board[7][7].moved;
    indexes[7]=board[0][0].moved;
    indexes[8]=board[0][7].moved;
    indexes[9]=king_row[0];
    indexes[10]=king_row[1];
    indexes[11]=king_col[0];
    indexes[12]=king_col[1];
    FILE *fp3;
    fp3=fopen("indexes.txt","wb");
    fwrite(indexes,sizeof(int),13,fp3);
    fclose(fp3);
    FILE *fp4;
    fp4=fopen("w_removed.txt","wb");
    fwrite(w_removed,sizeof(char),16,fp4);
    fclose(fp4);
    FILE *fp5;
    fp5=fopen("b_removed.txt","wb");
    fwrite(b_removed,sizeof(char),16,fp5);
    fclose(fp5);
}
/*if every times the user runs the "exe" file this function is called to
load the last data of the board saved in the "txt" files*/
void load()
{
    FILE *fp1;
    fp1=fopen("board_pieces.txt","rb");
    fread(board_copy,sizeof(char),64,fp1);
    fclose(fp1);
    FILE *fp2;
    fp2=fopen("board_order.txt","rb");
    fread(order_copy,sizeof(int),64,fp2);
    fclose(fp2);
    FILE *fp3;
    fp3=fopen("indexes.txt","rb");
    fread(indexes,sizeof(int),13,fp3);
    fclose(fp3);
    FILE *fp4;
    fp4=fopen("w_removed.txt","rb");
    fread(w_removed,sizeof(char),16,fp4);
    fclose(fp4);
    FILE *fp5;
    fp5=fopen("b_removed.txt","rb");
    fread(b_removed,sizeof(char),16,fp5);
    fclose(fp5);
    for(i=0;i<8;i++){
        for(j=0;j<8;j++){
            board[i][j].piece=board_copy[i][j];
            board[i][j].order=order_copy[i][j];
        }
    }check_player=indexes[0];
    n_w_removed=indexes[1];
    n_b_removed=indexes[2];
    board[7][4].moved=indexes[3];
    board[0][4].moved=indexes[4];
    board[7][0].moved=indexes[5];
    board[7][7].moved=indexes[6];
    board[0][0].moved=indexes[7];
    board[0][7].moved=indexes[8];
    king_row[0]=indexes[9];
    king_row[1]=indexes[10];
    king_col[0]=indexes[11];
    king_col[1]=indexes[12];
}
int main()
{
    //this first loop is used to restart the game at any time
    while(new_game){
        //setting up board for a new game
        check_player=0;
        restart_game=0;
        king_row[0]=7,king_row[1]=0,king_col[0]=4,king_col[1]=4,n_all_moves=0,n_w_removed=0,n_b_removed=0;
        setup();
        //loading the last saved date of the game
        load();
        display_board();
        //the second loop is for the current game loop itself
        while(game_is_running){
            //initializing variables before taking any move from the user
            valid_move=0;
            valid_castling=0;
            valid_promotion=0;
            valid_en_passant=0;
            check_promotion=0;
            just_checking=0;
            check_undo=0;
            //saving the last data of the game
            //it's called after every move
            save();
            //the third loop is for taking a move from the user and it's just
            //done when the user enters a valid move
            while(!valid_move){
                printf("\t");
                taking_input();
                if(!check_undo&&!restart_game){
                    check_king=if_check(king_row[check_player],king_col[check_player]);
                    if(valid_move&&!check_king){
                        valid_move=0;
                        forced_undo();
                    }
                    if(check_promotion&&valid_move){
                        while(!valid_promotion){
                            do_promotion();
                            if(valid_promotion){
                                break;
                            }
                            scanf(" %c",&promotion);
                        }
                    }
                    if(!valid_move){
                        printf("\tEnter a valid move ..\n");
                    }if(valid_move&&toupper(board[player_move.row2][player_move.col2].piece)=='P'){
                        board[player_move.row2][player_move.col2].order=
                        board[player_move.row1][player_move.col1].order;
                        board[player_move.row1][player_move.col1].moved=1;
                    }
                }else{
                    break;
                }
            }if(check_undo){
                if(n_all_moves>0){
                    undo_move();
                    check_king=if_check(king_row[check_player],king_col[check_player]);
                    display_board();
                    continue;
                }else{
                    printf("\tCan't undo..\n");
                    continue;
                }
            }if(restart_game){
                break;
            }
            saving_move();
            check_player=(check_player+1)%2;
            check_king=if_check(king_row[check_player],king_col[check_player]);
            end_game();
            //after calling these two functions if the game is over , if the current player's king is in check
            //then it's checkmate and else it's a draw
            dead_position();
            display_board();
            if(!game_is_running){
                if(check_king||check_draw){
                    printf("\n\tDraw!\n\tGame Over..");
                }else{
                    (check_player)?printf("\n\tPlayer 1 won"):printf("\n\tPlayer 2 won");
                    printf("\n\tGame Over..");
                }
            }
        }// after the game is over or the user restarts it manually all the variables are reinitialized
        //after that the data in the "txt" files is updated
        king_row[0]=7,king_row[1]=0,king_col[0]=4,king_col[1]=4,n_w_removed=0,n_b_removed=0;
        check_player=0,board[7][4].moved=0,board[0][4].moved=0,board[7][0].moved=0;
        board[7][7].moved=0,board[0][0].moved=0,board[0][7].moved=0,check_king=1;
        setup();
        save();
        //if the user didn't restarted the game manually then he should input 'n' to start a new game
        if(!restart_game){
            printf("\n\tPress N for new game..\n");
            char temp;
            scanf(" %c",&temp);
            if(toupper(temp)!='N'){
                new_game=0;
            }
        }
    }
    return 0;
}
