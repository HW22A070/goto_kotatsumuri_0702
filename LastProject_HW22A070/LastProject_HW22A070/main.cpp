#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
using namespace std;


enum ModeScreen{
    Title,
    Game,
    Result,
    End
};

enum Direction{
    UP,Right,Down,Left,None
};

ModeScreen _screenMode=Title;
SDL_Window* _windowNow;
SDL_Event _eventFlag;
SDL_Renderer* _windowRenderer = NULL;

SDL_Texture* LoadTextureBMP( const char* filename){
    SDL_Surface* image = SDL_LoadBMP(filename);
    SDL_SetColorKey( image, SDL_TRUE, SDL_MapRGB(image->format, 0, 0, 0));
    SDL_Texture* text = SDL_CreateTextureFromSurface(_windowRenderer, image);
    SDL_FreeSurface( image );
    return text;
}

SDL_Color _clWhite = {255, 255, 255},_clIce = {200, 200, 255},_clOrange={200,170,0},_clBrack={0,0,0};

bool _isPlaying=true;
bool _isSet=false;

int _countOrange=0;
int orangeMax;
int _timer=0,_timeResult,_timerCount;
int _valueWarm=0,_valueWarmMax=200;
bool _isFreezing=false;

TTF_Font* font;

SDL_Rect _imageRect={0,0,640,480};

vector<const char*> _resultTextList={
    "体の芯まで冷えました",
    "体温が-273度未満だ！",
    "冷たい人間になってしまった",
    "さむいど〜！体が凍るど〜！",
    "人間も冬眠するんですなぁ",
    "あなたもわたしも永久凍土",
    "1000年後までコールドスリープだ！",
    "これが次世代の熱中症対策"
};

const char* _resultText;

int level,levelMax=2;

int _mapData[15][15];

int _map[15][15]={
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,1,0,8,0,0,0,0,0,1},
    {1,0,0,0,0,0,1,0,3,0,4,0,0,0,1},
    {1,0,9,0,2,0,1,1,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,0,0,1,1,1,0,0,0,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
    {1,0,0,1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,9,0,0,0,1},
    {1,0,0,1,0,0,0,0,0,0,4,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
    {1,0,0,4,0,0,1,0,0,1,1,1,0,0,1},
    {1,0,0,9,0,0,0,0,0,9,9,9,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
},

_map2[15][15]={
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,8,1,0,1,1,1,1,0,0,9,0,1},
    {1,0,0,0,2,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,0,0,1,0,1,0,9,4,0,1},
    {1,4,1,0,0,0,0,1,0,1,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,0,1,0,0,0,0,1,9,1},
    {1,3,3,3,3,1,0,1,0,0,0,0,0,0,1},
    {1,3,3,3,3,0,0,0,0,0,4,0,1,1,1},
    {1,3,8,3,3,0,0,0,0,0,0,0,0,0,1},
    {1,3,3,3,3,1,0,1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
},
_map3[15][15]={
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,3,3,3,3,3,3,1,3,8,3,3,3,8,1},
    {1,3,3,3,3,3,3,1,3,3,3,3,3,3,1},
    {1,8,3,3,2,3,3,1,1,1,0,0,1,1,1},
    {1,3,3,3,3,3,3,0,0,0,0,4,0,0,1},
    {1,3,3,3,3,3,3,1,1,1,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,0,0,0,0,1},
    {1,3,3,0,0,0,9,3,3,1,0,0,0,0,1},
    {1,1,1,0,0,0,0,3,3,1,0,0,0,0,1},
    {1,0,1,3,1,1,1,1,1,1,0,0,0,0,1},
    {1,4,0,0,3,3,3,3,3,3,0,0,0,0,1},
    {1,0,1,3,1,1,1,1,1,1,0,9,9,0,1},
    {1,1,1,0,0,0,0,3,3,1,9,0,0,9,1},
    {1,9,0,0,0,0,0,3,3,1,9,9,9,9,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

//文字生成
void SetSDLTextKotatsu(SDL_Color color,int posX,int posY,int textSize,const char *message){
    SDL_Surface *text;
    SDL_Texture *textureA;
    SDL_Rect txtRect,pasteRect;
    
    text = TTF_RenderUTF8_Blended(TTF_OpenFont("/Library/Fonts/Arial Unicode.ttf", textSize),  message, color);
    
    textureA=SDL_CreateTextureFromSurface(_windowRenderer, text);
    
    int iw,ih;
    SDL_QueryTexture(textureA, NULL, NULL, &iw, &ih);
    txtRect=(SDL_Rect){
        0,0,iw,ih
    };
    pasteRect=(SDL_Rect){
        posX,posY,iw,ih
    };
    SDL_RenderCopy(_windowRenderer, textureA, &txtRect, &pasteRect);
    
    SDL_FreeSurface(text);
    SDL_DestroyTexture(textureA);
}

//======================================================================
// Class 停止機能を持つすべてのオブジェクト
//======================================================================
class Object{
protected:
    vector<SDL_Texture*> textures;
    long int count;
public:
    int px, py, w, h;
    bool isMoving;
    int sizeXY;
    
    ~Object(){
        for( int i = 0 ; i < textures.size() ;i++ ){
            SDL_DestroyTexture(textures[i]);
        }
    }
    
    void stop(){
        isMoving=false;
    }
};


//======================================================================
// Class Furniture　家に固定され、プレイヤーに対して動く
//======================================================================
class Furniture:public Object {
public:
    void move(int x,int y){
        if(x==0&&y==0){
            stop();
        }
        else{
            px = px + x;
            py = py + y;
            isMoving=true;
        }
    }
    
    virtual void redraw(){
        SDL_Rect imageRect={0,0,w,h};
        SDL_Rect drawRect={px,py,w,h};
        
        count = ( count + 1 ) % textures.size();
        SDL_RenderCopyEx(_windowRenderer, textures[count], &imageRect, &drawRect,NULL,NULL,SDL_FLIP_HORIZONTAL);
    }
};

//======================================================================
// Class floor　プレイヤーが上を通れる
//======================================================================
class Floor:public Furniture {
public:
    Floor( int x, int y ){
        sizeXY=64;
        textures = {
            LoadTextureBMP( "Floor.bmp" )
        };
        SDL_QueryTexture(textures[0], NULL, NULL, &(this->w), &(this->h));
        px = x;
        py = y;
        count = 0;
    }
};

vector<Floor*> floorInfos;

//======================================================================
// Class Orange　取るとポイントがもらえるアイテム
//======================================================================
class Orange:public Furniture {
public:
    Orange( int x, int y ){
        sizeXY=32;
        textures = {
            LoadTextureBMP( "Orange.bmp" ),
            LoadTextureBMP( "Orange2.bmp" )
        };
        SDL_QueryTexture(textures[0], NULL, NULL, &(this->w), &(this->h));
        px = x;
        py = y;
        count = 0;
    }
};

vector<Orange*> orangeInfos;

//======================================================================
// Class Wall　プレイヤーの行手を阻む
//======================================================================
class Wall:public Furniture{
public:
    Wall( int x, int y ){
        sizeXY=64;
        textures = {
            LoadTextureBMP( "wall.bmp" )
        };
        SDL_QueryTexture(textures[0], NULL, NULL, &(this->w), &(this->h));
        px = x;
        py = y;
        count = 0;
    }
    
    void redraw() override{
        SDL_Rect imageRect={0,0,w,h};
        SDL_Rect drawRect={px,py-32,w,h};
        
        count = ( count + 1 ) % textures.size();
        SDL_RenderCopyEx(_windowRenderer, textures[count], &imageRect, &drawRect,NULL,NULL,SDL_FLIP_HORIZONTAL);
    }
};

vector<Wall*> wallInfos;

//======================================================================
// Class Kotastu　こたつ。あったかメーター回復！拠点になります
//======================================================================
class Kotatsu:public Furniture {
public:
    int warmValue;
    
    Kotatsu( int x, int y ,int warm){
        warmValue=warm;
        sizeXY=64;
        textures = {
            LoadTextureBMP( "kotatsu.bmp" )
        };
        SDL_QueryTexture(textures[0], NULL, NULL, &(this->w), &(this->h));
        px = x;
        py = y;
        count = 0;
    }
};
vector<Kotatsu*> kotatsuInfos;

//======================================================================
// Class Kotastu　ストーブ。あったかメーター回復！
//======================================================================
class Stove:public Furniture {
public:
    int warmValue;
    
    Stove( int x, int y ,int warm){
        warmValue=warm;
        sizeXY=64;
        textures = {
            LoadTextureBMP( "stove.bmp" )
        };
        SDL_QueryTexture(textures[0], NULL, NULL, &(this->w), &(this->h));
        px = x;
        py = y;
        count = 0;
    }
    
    void redraw() override{
        SDL_Rect imageRect={0,0,w,h};
        SDL_Rect drawRect={px,py-16,w,h};
        
        count = ( count + 1 ) % textures.size();
        SDL_RenderCopyEx(_windowRenderer, textures[count], &imageRect, &drawRect,NULL,NULL,SDL_FLIP_HORIZONTAL);
    }
    
    
};
vector<Stove*> stoveInfos;

//======================================================================
// Class Ice　凍った床。あったかメーター激減！
//======================================================================
class Ice:public Furniture {
public:
    Ice( int x, int y){
        sizeXY=64;
        textures = {
            LoadTextureBMP( "FloorCold.bmp" )
        };
        SDL_QueryTexture(textures[0], NULL, NULL, &(this->w), &(this->h));
        px = x;
        py = y;
        count = 0;
    }
};

vector<Ice*> iceInfos;

//======================================================================
// Class Player　プレイヤー自身。画面中央にとどまる
//======================================================================

int moveSpeedX=0,moveDeltaX=0;
int moveSpeedY=0,moveDeltaY=0;

class Player:public Object {
    vector<SDL_Texture*> texturesWarm,texturesCold;
public:
    bool isLeft;
    
    Player( int x, int y ){
        sizeXY=32;
        textures = {
            LoadTextureBMP( "pl_front1.bmp" )
        };
        texturesWarm={
            LoadTextureBMP("pl_front_warm1.bmp")
        };
        texturesCold={
            LoadTextureBMP("pl_front_cold1.bmp")
        };
        SDL_QueryTexture(textures[0], NULL, NULL, &(this->w), &(this->h));
        px = x;
        py = y;
        count = 0;
    }
    
    void redraw(int mode){
        SDL_Rect imageRect={0,0,w,h};
        SDL_Rect drawRect={px,py-48,w,h};
        
        if(isMoving){
            count = ( count + 1 ) % textures.size();
            count=( count + 1 ) % texturesWarm.size();
        }
        else{
            count=textures.size()-1;
            count=texturesWarm.size()-1;
        }
        
        switch (mode) {
            case 0:
                SDL_RenderCopyEx(_windowRenderer, textures[count], &imageRect, &drawRect,NULL,NULL,SDL_FLIP_NONE);
                break;
            case -3:
                SDL_RenderCopyEx(_windowRenderer, texturesCold[count], &imageRect, &drawRect,NULL,NULL,SDL_FLIP_HORIZONTAL);
                break;
                
            case 2:
            case 4:
                SDL_RenderCopyEx(_windowRenderer, texturesWarm[count], &imageRect, &drawRect,NULL,NULL,SDL_FLIP_HORIZONTAL);
                break;
        }
        
        
    }
    
    void move(int x,int y){
        if(x==0&&y==0){
            stop();
        }
        else{
            isMoving=true;
        }
    }
    
    void look(int x,int y){
        //向き調節
        if(x<0)isLeft=true;
        else if(x>0) isLeft=false;
    }
    
    bool isHit(Object* wall){
        
        int dpx=px+moveSpeedX;
        int dpy=py+moveSpeedY;
        
        bool XHit=wall->px< dpx+sizeXY && dpx < (wall->px + wall->sizeXY);
        bool YHit=wall->py < dpy+sizeXY && dpy < (wall->py + wall->sizeXY);
        
        return (XHit&&YHit);
    }
    
};

//game定義
Player *playerInfo=new Player( 0 , 0 );

//＝＝＝＝＝＝＝＝Main＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
//＝＝＝＝＝＝＝＝Main＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
//＝＝＝＝＝＝＝＝Main＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

int main(int argc, char* argv[]) {
    
    //まずタイトル設定
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    _windowNow = SDL_CreateWindow("test",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,640,480,SDL_WINDOW_SHOWN);
    _windowRenderer = SDL_CreateRenderer(_windowNow, -1, SDL_RENDERER_ACCELERATED);
    
    font = TTF_OpenFont("ipag-mona.ttf", 24);
    
    while(_isPlaying){
        SDL_Delay(60);
        switch (_screenMode) {
                //タイトルーーーーーーーーーーーーーーーーーーーーーーーーーーー
            case Title:
                if(!_isSet){

                    SDL_RenderCopyEx(_windowRenderer, LoadTextureBMP("Title.bmp"), &_imageRect, &_imageRect,NULL,NULL,SDL_FLIP_HORIZONTAL);
                    SDL_RenderPresent(_windowRenderer);
                    _isSet=true;
                }
                else{
                    if(SDL_PollEvent(&_eventFlag)){
                        switch (_eventFlag.type) {
                                //終了
                            case SDL_QUIT:
                                _screenMode=End;
                                _isPlaying=false;
                                break;
                                
                            case SDL_KEYDOWN:
                                switch( _eventFlag.key.keysym.sym ){
                                        //スペースでスタート
                                    case SDLK_SPACE :
                                        level=0;
                                        _isSet=false;
                                        _screenMode=Game;
                                        std::cout<<"ゲーム\n";
                                        break;
                                        
                                    case SDLK_LSHIFT :
                                        level=999;
                                        _isSet=false;
                                        _screenMode=Game;
                                        std::cout<<"ゲーム\n";
                                        break;
                                        
                                    case SDLK_ESCAPE:
                                        
                                        _screenMode=End;
                                        break;
                                        
                                    default:
                                        break;
                                }
                                break;
                        }
                    }
                }
                
                break;
                
                //ゲーム画面ーーーーーーーーーーーーーーーーーーーーーーーーーーー
            case Game:
                if(!_isSet){
                    _timer=0;
                    _timerCount=0;
                    playerInfo = new Player( 320-16 , 240-16 );
                    
                    wallInfos={};
                    kotatsuInfos={};
                    floorInfos={};
                    orangeInfos={};
                    iceInfos={};
                    stoveInfos={};
                    
                    switch (level) {
                        case 0:
                            for(int x=0;x<std::extent<decltype(_map),0>::value;x++){
                                for(int y=0;y<std::extent<decltype(_map),0>::value;y++){
                                    _mapData[x][y]=_map[x][y];;
                                }
                            }
                            break;
                            
                        case 1:
                            for(int x=0;x<std::extent<decltype(_map2),0>::value;x++){
                                for(int y=0;y<std::extent<decltype(_map2),0>::value;y++){
                                    _mapData[x][y]=_map2[x][y];
                                }
                            }
                            break;
                        case 2:
                            for(int x=0;x<std::extent<decltype(_map3),0>::value;x++){
                                for(int y=0;y<std::extent<decltype(_map3),0>::value;y++){
                                    _mapData[x][y]=_map3[x][y];
                                }
                            }
                            break;
                        case 999:
                            for(int x=0;x<std::extent<decltype(_map3),0>::value;x++){
                                for(int y=0;y<std::extent<decltype(_map3),0>::value;y++){
                                    _mapData[x][y]=rand()%10;
                                }
                            }
                            _mapData[3][4]=2;
                            _mapData[4][4]=0;
                            _mapData[3][5]=0;
                            _mapData[4][5]=0;
                            
                            break;
                    }
                    
                    //マップ生成
                    for(int x=0;x<std::extent<decltype(_mapData),0>::value;x++){
                        for(int y=0;y<std::extent<decltype(_mapData),0>::value;y++){
                            switch (_mapData[y][x]) {
                                case 0:
                                    floorInfos.push_back(new Floor(x*64,y*64));
                                    break;
                                    
                                case 1:
                                    wallInfos.push_back(new Wall(x*64,y*64));
                                    break;
                                    
                                case 2:
                                    floorInfos.push_back(new Floor(x*64,y*64));
                                    kotatsuInfos.push_back(new Kotatsu(x*64,y*64,2));
                                    break;
                                    
                                case 3:
                                    iceInfos.push_back(new Ice(x*64,y*64));
                                    break;
                                    
                                case 4:
                                    floorInfos.push_back(new Floor(x*64,y*64));
                                    stoveInfos.push_back(new Stove(x*64,y*64,1));
                                    break;
                                    
                                case 8:
                                    iceInfos.push_back(new Ice(x*64,y*64));
                                    orangeInfos.push_back(new Orange((x*64)+16,(y*64)+16));
                                    break;
                                    
                                case 9:
                                    floorInfos.push_back(new Floor(x*64,y*64));
                                    orangeInfos.push_back(new Orange((x*64)+16,(y*64)+16));
                                    break;
                                    
                                default:
                                    iceInfos.push_back(new Ice(x*64,y*64));
                                    break;
                            }
                        }
                    }
                    moveSpeedX=0;
                    moveDeltaX=0;
                    moveSpeedY=0;
                    moveDeltaY=0;
                    
                    _isFreezing=false;
                    _valueWarm=_valueWarmMax;
                    _countOrange=0;
                    orangeMax=(int)orangeInfos.size();
                    
                    _isSet=true;
                }
                else{
                    _timerCount++;
                    if(_timerCount>1000/60){
                        _timerCount=0;
                        _timer++;
                    }
                    
                    if(SDL_PollEvent(&_eventFlag)){
                        switch (_eventFlag.type) {
                                //終了
                            case SDL_QUIT:
                                _screenMode=End;
                                break;
                                
                            case SDL_KEYDOWN:
                                switch( _eventFlag.key.keysym.sym ){
                                        //左
                                    case SDLK_RIGHT :
                                        moveDeltaX=3;
                                        break;
                                        //右
                                    case SDLK_LEFT :
                                        moveDeltaX=-3;
                                        break;
                                        //上
                                    case SDLK_UP :
                                        moveDeltaY=-3;
                                        break;
                                        //下
                                    case SDLK_DOWN :
                                        moveDeltaY=3;
                                        break;
                                        
                                        //ESCでタイトルにもどる
                                    case SDLK_ESCAPE :
                                        _isSet=false;
                                        _screenMode=Title;
                                        std::cout<<"タイトル\n";
                                        break;
                                }
                                break;
                                
                                //無反応
                            default:
                                break;
                        }
                    }
                }
                
                //摩擦
                if(moveDeltaX>1)moveDeltaX-=1;
                else if(moveDeltaX<-1) moveDeltaX+=1;
                else {
                    moveDeltaX=0;
                    moveSpeedX=0;
                }
                
                if(moveDeltaY>1)moveDeltaY-=1;
                else if(moveDeltaY<-1) moveDeltaY+=1;
                else {
                    moveDeltaY=0;
                    moveSpeedY=0;
                }
                
                moveSpeedX+=moveDeltaX;
                moveSpeedY+=moveDeltaY;
                
                //移動先を検証
                bool isHit;
                isHit=false;
                for(int i=0;i<wallInfos.size();i++){
                    if(playerInfo->isHit(wallInfos[i])){
                        //std::cout<<i<<std::endl;
                        isHit=true;
                        break;
                    }
                }
                
                
                //移動
                if(!isHit){
                    for(int i=0;i<wallInfos.size();i++){
                        wallInfos[i]->move(-moveSpeedX,-moveSpeedY);
                    }
                    for(int i=0;i<kotatsuInfos.size();i++){
                        kotatsuInfos[i]->move(-moveSpeedX,-moveSpeedY);
                    }
                    for(int i=0;i<stoveInfos.size();i++){
                        stoveInfos[i]->move(-moveSpeedX,-moveSpeedY);
                    }
                    for(int i=0;i<floorInfos.size();i++){
                        floorInfos[i]->move(-moveSpeedX,-moveSpeedY);
                    }
                    for(int i=0;i<orangeInfos.size();i++){
                        orangeInfos[i]->move(-moveSpeedX,-moveSpeedY);
                    }
                    for(int i=0;i<iceInfos.size();i++){
                        iceInfos[i]->move(-moveSpeedX,-moveSpeedY);
                    }
                    
                    playerInfo->look(moveDeltaX,moveDeltaY);
                    playerInfo->move(moveSpeedX,moveSpeedY);
                }
                
                //あったかメーター
                int Warm;
                Warm=0;
                
                for(int i=0;i<iceInfos.size();i++){
                    if(playerInfo->isHit(iceInfos[i])){
                        //std::cout<<i<<"：さむ！^^"<<std::endl;
                        Warm=-3;
                        break;
                    }
                }
                
                for(int i=0;i<stoveInfos.size();i++){
                    if(playerInfo->isHit(stoveInfos[i])){
                        //std::cout<<i<<"：あったか^^"<<std::endl;
                        Warm=2;
                        break;
                    }
                }
                
                for(int i=0;i<kotatsuInfos.size();i++){
                    if(playerInfo->isHit(kotatsuInfos[i])){
                        //std::cout<<i<<"：あったか^^"<<std::endl;
                        Warm=4;
                        break;
                    }
                }
                
                int coldx,coldy;
                coldx=(rand()%12)-6;
                coldy=(rand()%4)-2;
                
                switch (Warm) {
                    case 0:
                        _valueWarm--;
                        break;
                        
                    case -3:
                        _valueWarm-=3;
                        
                        for(int i=0;i<floorInfos.size();i++){
                            floorInfos[i]->px+=coldx;
                            floorInfos[i]->py+=coldy;
                        }
                        for(int i=0;i<iceInfos.size();i++){
                            iceInfos[i]->px+=coldx;
                            iceInfos[i]->py+=coldy;
                        }
                        for(int i=0;i<kotatsuInfos.size();i++){
                            kotatsuInfos[i]->px+=coldx;
                            kotatsuInfos[i]->py+=coldy;
                        }
                        for(int i=0;i<stoveInfos.size();i++){
                            stoveInfos[i]->px+=coldx;
                            stoveInfos[i]->py+=coldy;
                        }
                        for(int i=0;i<orangeInfos.size();i++){
                            orangeInfos[i]->px+=coldx;
                            orangeInfos[i]->py+=coldy;
                        }
                        for(int i=0;i<wallInfos.size();i++){
                            wallInfos[i]->px+=coldx;
                            wallInfos[i]->py+=coldy;
                        }
                        playerInfo->px+=coldx;
                        playerInfo->py+=coldy;
                        
                        break;
                        
                    case 2:
                        _valueWarm+=2;
                        break;
                        
                    case 4:
                        _valueWarm+=4;
                        //クリア
                        if(_countOrange>=orangeMax&&_valueWarm>=_valueWarmMax){
                            _screenMode=Result;
                            _isSet=false;
                            _timeResult=_timer;
                            std::cout<<"リザルト\n";
                        }
                        break;
                        
                    default:
                        break;
                }
                if(_valueWarm>_valueWarmMax)_valueWarm=_valueWarmMax;
                if(_valueWarm<0)_valueWarm=0;
                
                SDL_SetRenderDrawColor(_windowRenderer, 200, 200, 255, 255);
                SDL_RenderClear(_windowRenderer);
                
                //凍死判定
                if(_valueWarm<=0){
                    _isFreezing=true;
                    _isSet=false;
                    _screenMode=Result;
                    std::cout<<"凍死！リザルト\n";
                }
                
                //みかん取得
                for(int i=0;i<orangeInfos.size();i++){
                    if(playerInfo->isHit(orangeInfos[i])){
                        std::cout<<"みかんゲット！"<<std::endl;
                        orangeInfos.erase(orangeInfos.begin()+i);
                        _countOrange++;
                    }
                }
                
                //描画
                
                for(int i=0;i<floorInfos.size();i++)floorInfos[i]->redraw();
                for(int i=0;i<iceInfos.size();i++)iceInfos[i]->redraw();
                for(int i=0;i<kotatsuInfos.size();i++)kotatsuInfos[i]->redraw();
                
                for(int i=0;i<orangeInfos.size();i++)orangeInfos[i]->redraw();
                
                for(int i=0;i<wallInfos.size();i++)if(wallInfos[i]->py<playerInfo->py)wallInfos[i]->redraw();
                for(int i=0;i<stoveInfos.size();i++) if(stoveInfos[i]->py<playerInfo->py)stoveInfos[i]->redraw();
                playerInfo->redraw(Warm);
                for(int i=0;i<wallInfos.size();i++)if(wallInfos[i]->py>=playerInfo->py)wallInfos[i]->redraw();
                for(int i=0;i<stoveInfos.size();i++) if(stoveInfos[i]->py>=playerInfo->py)stoveInfos[i]->redraw();
                
                if(Warm==-3){
                    for(int i=0;i<floorInfos.size();i++){
                        floorInfos[i]->px-=coldx;
                        floorInfos[i]->py-=coldy;
                    }
                    for(int i=0;i<iceInfos.size();i++){
                        iceInfos[i]->px-=coldx;
                        iceInfos[i]->py-=coldy;
                    }
                    for(int i=0;i<kotatsuInfos.size();i++){
                        kotatsuInfos[i]->px-=coldx;
                        kotatsuInfos[i]->py-=coldy;
                    }
                    for(int i=0;i<stoveInfos.size();i++){
                        stoveInfos[i]->px-=coldx;
                        stoveInfos[i]->py-=coldy;
                    }
                    for(int i=0;i<orangeInfos.size();i++){
                        orangeInfos[i]->px-=coldx;
                        orangeInfos[i]->py-=coldy;
                    }
                    for(int i=0;i<wallInfos.size();i++){
                        wallInfos[i]->px-=coldx;
                        wallInfos[i]->py-=coldy;
                    }
                    playerInfo->px-=coldx;
                    playerInfo->py-=coldy;
                }
                
                //あったかメーター描画
                //枠
                SDL_Rect rectPaddle;
                rectPaddle={5,(475-_valueWarmMax*2)-10,30,(_valueWarmMax*2)+10};
                SDL_SetRenderDrawColor(_windowRenderer,0,0,100-(_valueWarm/2),0);
                SDL_RenderFillRect( _windowRenderer , &rectPaddle );
                
                //メーター
                rectPaddle={10,470-_valueWarm*2,20,_valueWarm*2};
                SDL_SetRenderDrawColor(_windowRenderer,_valueWarm+55,128,_valueWarmMax-_valueWarm,0);
                SDL_RenderFillRect( _windowRenderer , &rectPaddle );
                
                //ステータス
                rectPaddle={0,0,38*orangeMax,48};
                SDL_SetRenderDrawColor(_windowRenderer,0,0,0,0);
                SDL_RenderFillRect( _windowRenderer , &rectPaddle );
                

                for(int i=0;i<orangeMax;i++){
                    SDL_Rect imageRect;imageRect={0,0,32,32};
                    SDL_Rect drawRect;drawRect={i*34,0,32,32};
                    if(orangeInfos.size()>i){
                        SDL_RenderCopyEx(_windowRenderer, LoadTextureBMP("OrangeEmpty.bmp"), &imageRect, &drawRect,NULL,NULL,SDL_FLIP_HORIZONTAL);
                    }
                    else{
                        SDL_RenderCopyEx(_windowRenderer, LoadTextureBMP("Orange.bmp"), &imageRect, &drawRect,NULL,NULL,SDL_FLIP_HORIZONTAL);
                    }
                    
                }
                
                
                SDL_RenderPresent(_windowRenderer);
                
                //delete &isHit;
                //delete &Warm;
                
                break;
                
                //リザルトーーーーーーーーーーーーーーーーーーーーーーーーーーー
            case Result:
                if(!_isSet){
                    //SDL_SetRenderDrawColor(_windowRenderer, 0, 100, 100, 255);
                    
                    for(int i=0;i<floorInfos.size();i++)delete floorInfos[i];
                    for(int i=0;i<iceInfos.size();i++)delete iceInfos[i];
                    for(int i=0;i<kotatsuInfos.size();i++)delete kotatsuInfos[i];
                    
                    for(int i=0;i<orangeInfos.size();i++)delete orangeInfos[i];
                    
                    for(int i=0;i<wallInfos.size();i++)delete wallInfos[i];
                    for(int i=0;i<stoveInfos.size();i++)delete stoveInfos[i];
                    delete playerInfo;
                    
                    _resultText= _resultTextList[rand()%_resultTextList.size()];
                    
                   
                    
                    if(_isFreezing){
                        SDL_RenderCopyEx(_windowRenderer, LoadTextureBMP("GameOver.bmp"), &_imageRect, &_imageRect,NULL,NULL,SDL_FLIP_HORIZONTAL);
                        SetSDLTextKotatsu(_clIce,160,180,30,_resultText);
                        
                        SetSDLTextKotatsu(_clIce,50,400,30,"スペースキーで再チャレンジ");
                        
                    }
                    else{
                        SDL_RenderCopyEx(_windowRenderer, LoadTextureBMP("Clear.bmp"), &_imageRect, &_imageRect,NULL,NULL,SDL_FLIP_HORIZONTAL);
                        SetSDLTextKotatsu(_clBrack,140,180,30,"タイム：");
                        SetSDLTextKotatsu(_clBrack,260,180,30,std::to_string(_timeResult).c_str());
                        if(levelMax<=level||_isFreezing){
                            SetSDLTextKotatsu(_clBrack,0,370,30,"全階クリアおめでとう！");
                            SetSDLTextKotatsu(_clBrack,0,410,30,"スペースキーでタイトルにもどる");
                        }
                        else{
                            SetSDLTextKotatsu(_clWhite,0,410,30,"スペースキーで次の階へ");
                        }
                        
                    }
                    
                    SDL_RenderPresent(_windowRenderer);
                    
                    _isSet=true;
                }
                else{
                    while (SDL_PollEvent(&_eventFlag)){
                        switch (_eventFlag.type) {
                            case SDL_QUIT:
                                _screenMode=End;
                                _isPlaying=false;
                                break;
                            case SDL_KEYDOWN:
                                switch( _eventFlag.key.keysym.sym ){
                                        //スペースでタイトルにもどる
                                    case SDLK_ESCAPE:
                                        _isSet=false;
                                        _screenMode=Title;
                                        std::cout<<"タイトル\n";
                                        level=0;
                                        
                                    case SDLK_SPACE :
                                        if(_isFreezing){
                                            _isSet=false;
                                            _screenMode=Game;
                                            std::cout<<"ゲーム\n";
                                        }
                                        else{
                                            if(levelMax<=level){
                                                _isSet=false;
                                                _screenMode=Title;
                                                std::cout<<"タイトル\n";
                                                level=0;
                                            }
                                            else{
                                                level++;
                                                _isSet=false;
                                                _screenMode=Game;
                                                std::cout<<"ゲーム\n";
                                            }
                                        }
                                        break;
                                }
                                break;
                        }
                    }
                }
                break;
                
            case End:
                _isPlaying=false;
                break;
        }
    }
    
    std::cout<<"終了\n";
    SDL_DestroyWindow(_windowNow);
    SDL_Quit();
    return 0;
}
