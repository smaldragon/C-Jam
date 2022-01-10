#include "raylib.h"
#include <stdio.h>

typedef enum Gate
{
    OR,
    AND,
    NOT,
    XOR,
    NOR,
    NAND,
    NONE,
}Gate;


typedef struct Recipe
{
    char name[16];
    Gate inputA;
    int  inputAAmount;
    Gate inputB;
    int  inputBAmount;
    Gate output;
    int  outputAmount;
}Recipe;

const Recipe OR_BLUEPRINT = {
    "=1 OR",
    NOR,
    1,
    NOR,
    1,
    OR,
    1
};
const Recipe AND_BLUEPRINT = {
    "=1 AND",
    NAND,
    1,
    NAND,
    1,
    AND,
    1
};
const Recipe NOT_BLUEPRINT = {
    "=2 NOT",
    NAND,
    1,
    NOR,
    1,
    NOT,
    2
};
const Recipe XOR_BLUEPRINT = {
    "=2 XOR",
    NAND,
    1,
    NOR,
    1,
    XOR,
    2
};

typedef struct Iventory
{
    int gates[6];
    int cash;
    Recipe blueprints[4];
}Iventory;

typedef struct Shop
{
    Gate gate;
    int amount;
    int cost;
} Shop;


typedef enum State
{
    MAIN,
    BUY,
    SELL,
    CRAFT,
    SEARCH,
    TRAVEL,
}State;

//======================
//  DRAWING FUNCTIONS
//======================

const char* GateToString(Gate gate, int amount) {
    switch (gate) {
    case OR:
        return TextFormat("%i OR",amount);
    case AND:
        return TextFormat("%i AND",amount);
    case NOT:
        return TextFormat("%i NOT",amount);
    case XOR:
        return TextFormat("%i XOR",amount);
    case NOR:
        return TextFormat("%i NOR",amount);
    case NAND:
        return TextFormat("%i NAND",amount);
    default:
        return "";
    }
}

void DrawInventory(Iventory inv,Font font, int xo, int yo) {
    Color c;
    DrawTextEx(font,"-Gates-",(Vector2){20+xo,0+yo},16,1,YELLOW);
    c = DARKGRAY;if(inv.gates[OR]>0)c=GRAY;
    DrawTextEx(font,"OR:",(Vector2){16+xo,10+yo},16,1,RAYWHITE);
    DrawTextEx(font,TextFormat("%i",inv.gates[OR]),(Vector2){38+xo,10+yo},16,1,c);

    c = DARKGRAY;if(inv.gates[AND]>0)c=GRAY;
    DrawTextEx(font,"AND:",(Vector2){8+xo,20+yo},16,1,RAYWHITE);
    DrawTextEx(font,TextFormat("%i",inv.gates[AND]),(Vector2){38+xo,20+yo},16,1,c);

    c = DARKGRAY;if(inv.gates[NOT]>0)c=GRAY;
    DrawTextEx(font,"NOT:",(Vector2){8+xo,30+yo},16,1,RAYWHITE);
    DrawTextEx(font,TextFormat("%i",inv.gates[NOT]),(Vector2){38+xo,30+yo},16,1,c);

    c = DARKGRAY;if(inv.gates[XOR]>0)c=GRAY;
    DrawTextEx(font,"XOR:",(Vector2){8+xo,40+yo},16,1,RAYWHITE);
    DrawTextEx(font,TextFormat("%i",inv.gates[XOR]),(Vector2){38+xo,40+yo},16,1,c);

    c = DARKGRAY;if(inv.gates[NOR]>0)c=GRAY;
    DrawTextEx(font,"NOR:",(Vector2){8+xo,50+yo},16,1,RAYWHITE);
    DrawTextEx(font,TextFormat("%i",inv.gates[NOR]),(Vector2){38+xo,50+yo},16,1,c);

    c = DARKGRAY;if(inv.gates[NAND]>0)c=GRAY;
    DrawTextEx(font,"NAND:",(Vector2){0+xo,60+yo},16,1,RAYWHITE);
    DrawTextEx(font,TextFormat("%i",inv.gates[NAND]),(Vector2){38+xo,60+yo},16,1,c);

    c = GRAY;if(inv.cash<=0)c=RED;
    DrawTextEx(font,"CASH:",(Vector2){0+xo,70+yo},16,1,RAYWHITE);
    DrawTextEx(font,TextFormat("%i$",inv.cash),(Vector2){38+xo,70+yo},16,1,c);

    //DrawTextEx(font,"-Components-",(Vector2){100+xo,0+yo},16,1,YELLOW);
}

void DrawTime(int day,int time,int city,Font font, int xo,int yo) {
    DrawTextEx(font,TextFormat("Day:%i",day),(Vector2){xo+5,yo},16,1,WHITE);
    for (int i=0;i<4;i++) {
        if (i==time) {
            DrawRectangle(xo+5+10*i,yo+25,5,5,WHITE);
        } else {
            DrawRectangle(xo+7+10*i,yo+27,3,3,DARKGRAY);
        }
    }
    switch (time)
    {
    case 0:
        DrawTextEx(font,"-Morning-",(Vector2){xo-15,yo+10},16,1,WHITE);
        break;
    case 1:
        DrawTextEx(font,"-Midday-",(Vector2){xo-7,yo+10},16,1,WHITE);
        break;
    case 2:
        DrawTextEx(font,"-Afternoon-",(Vector2){xo-20,yo+10},16,1,WHITE);
        break;
    case 3:
        DrawTextEx(font,"-Midnight-",(Vector2){xo-15,yo+10},16,1,WHITE);
        break;
    default:
        break;
    }
    switch (city)
    {
    case 0:
        DrawTextEx(font,"Paris",(Vector2){xo+5,yo+30},16,1,WHITE);
    break;
    case 1:
        DrawTextEx(font,"Moscow",(Vector2){xo+5,yo+30},16,1,WHITE);
    break;
    case 2:
        DrawTextEx(font,"Tokyo",(Vector2){xo+5,yo+30},16,1,WHITE);
    break;
    case 3:
        DrawTextEx(font,"Shangai",(Vector2){xo,yo+30},16,1,WHITE);
    break;
    case 4:
        DrawTextEx(font,"NYC",(Vector2){xo+15,yo+30},16,1,WHITE);
    break;
    default:
        break;
    }
}

void DrawShop(Shop shop, Color color1, Color color2, Font font,int xo, int yo) {
    DrawTextEx(font,GateToString(shop.gate,shop.amount),(Vector2){xo,yo},16,1,color1);
    DrawTextEx(font,TextFormat("%i$",shop.cost),(Vector2){xo,10+yo},16,1,color2);
}

void DrawSelectArrow(int selected,Font font,int xo, int yo) {
    DrawTextEx(font,"^",(Vector2){xo+22+selected*60,yo+12},16,1,WHITE);
}

void DrawSelectMenu(Font font, int xo, int yo) {
    DrawTextEx(font,"Buy",(Vector2){xo+15,yo},16,1,WHITE);
    DrawTextEx(font,"Sell",(Vector2){xo+70,yo},16,1,WHITE);
    DrawTextEx(font,"Craft",(Vector2){xo+125,yo},16,1,WHITE);
    DrawTextEx(font,"Search",(Vector2){xo+180,yo},16,1,WHITE);
    DrawTextEx(font,"Travel",(Vector2){xo+240,yo},16,1,WHITE);
    //DrawRectangle(20+selected*60,yo+20,10,10,WHITE);
}

void DrawTravelMenu(Font font, int xo, int yo) {
    DrawTextEx(font,"Paris",(Vector2){xo+5,yo},16,1,WHITE);
    DrawTextEx(font,"Moscow",(Vector2){xo+60,yo},16,1,WHITE);
    DrawTextEx(font,"Tokyo",(Vector2){xo+125,yo},16,1,WHITE);
    DrawTextEx(font,"Shanghai",(Vector2){xo+175,yo},16,1,WHITE);
    DrawTextEx(font,"NYC",(Vector2){xo+255,yo},16,1,WHITE);
    //DrawRectangle(20+selected*60,yo+20,10,10,WHITE);
}

void DrawRecipe(Iventory inv,Recipe r,Font font, int xo, int yo) {
    
    Color c = WHITE;
    if (inv.gates[r.inputA]<r.inputAAmount || inv.gates[r.inputB]<r.inputBAmount) c = DARKGRAY;
    DrawTextEx(font,r.name,(Vector2){xo,yo},16,1,c);
    c = GREEN;
    if (inv.gates[r.inputA]<r.inputAAmount) c = RED;
    DrawTextEx(font,GateToString(r.inputA,r.inputAAmount),(Vector2){xo,yo+10},16,1,c);
    c = GREEN;
    if (inv.gates[r.inputB]<r.inputBAmount) c = RED;
    DrawTextEx(font,GateToString(r.inputB,r.inputBAmount),(Vector2){xo,yo+20},16,1,c);
}

void ExecuteRecipe(Iventory *inv,Recipe r) {
    if (inv->gates[r.inputA]>=r.inputAAmount && inv->gates[r.inputB]>=r.inputBAmount) {
        inv->gates[r.inputA] -= r.inputAAmount;
        inv->gates[r.inputB] -= r.inputBAmount;
        inv->gates[r.output] += r.outputAmount;
    }
}

//======================
//  GAMEPLAY FUNCTIONS
//======================
int MoveSelectArrow(int *selected,Sound fx) {
    if (IsKeyPressed(KEY_LEFT)) {
        if (--*selected < 0) {
            *selected = 4;
        }
        PlaySoundMulti(fx);
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        if (++*selected >= 5) {
            *selected = 0;
        }
        PlaySoundMulti(fx);
    }
    if (IsKeyPressed(KEY_ONE)||IsKeyPressed(KEY_KP_1)){
        *selected=0;
        return 1;
    }
    if (IsKeyPressed(KEY_TWO)||IsKeyPressed(KEY_KP_2)){
        *selected=1;
        return 1;
    }
    if (IsKeyPressed(KEY_THREE)||IsKeyPressed(KEY_KP_3)){
        *selected=2;
        return 1;
    }
    if (IsKeyPressed(KEY_FOUR)||IsKeyPressed(KEY_KP_4)){
        *selected=3;
        return 1;
    }
    if (IsKeyPressed(KEY_FIVE)||IsKeyPressed(KEY_KP_5)){
        *selected=4;
        return 1;
    }
    return 0;
}
Shop RollShop(int cost_multiplier) {
    Shop shop;
    shop.gate = GetRandomValue(0,5);
    shop.amount = 1+GetRandomValue(0,5)+GetRandomValue(0,5)+GetRandomValue(0,5);
    shop.cost = 5+(GetRandomValue(0,5)+GetRandomValue(0,5)+GetRandomValue(0,5))*cost_multiplier;
    return shop;
}

void IncrementTime(int *time, int *day, Iventory *inv, Shop shop_buy_inv[5][4], Shop shop_sell_inv[5][4]) {
    *time=*time+1;
    if (*time >= 4) {
        *time = 0;
        *day = *day+1;
        inv->cash-=30;
        for(int c=0;c<5;c++){
        for(int i=0;i<3;i++){
            shop_buy_inv[c][i]=shop_buy_inv[c][i+1];
            shop_sell_inv[c][i]=shop_sell_inv[c][i+1];
        }
        shop_buy_inv[c][3]=RollShop(6);
        shop_sell_inv[c][3]=RollShop(5);
        }
    }
}

int main(void)
{   

    // Initialization
    //--------------------------------------------------------------------------------------
    const int canvasWidth = 300;
    const int canvasHeight = 160;
    
    const int screenWidth = 300*3;
    const int screenHeight = 160*3;

    InitWindow(screenWidth, screenHeight, "jam game");
    InitAudioDevice();

    // Load Assets
    RenderTexture2D canvas = LoadRenderTexture(canvasWidth,canvasHeight);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_POINT);

    Font font = LoadFont("assets/monogram-extended.ttf");
    Sound fxMove = LoadSound("assets/move.wav");
    Sound fxSelect = LoadSound("assets/select.wav");
    Music bgm = LoadMusicStream("assets/track-00.ogg");
    PlayMusicStream(bgm);
    SetSoundVolume(fxMove, 0.5f);
    SetSoundVolume(fxSelect, 0.5f);
    Texture2D background = LoadTexture("assets/background_2.png");

    Iventory inv;
    inv.cash = 100;
    for (int i=0;i<6;i++){
        inv.gates[i]=0;
    }
    inv.blueprints[0]=OR_BLUEPRINT;
    inv.blueprints[1]=AND_BLUEPRINT;
    inv.blueprints[2]=NOT_BLUEPRINT;
    inv.blueprints[3]=XOR_BLUEPRINT;

    //Shop shop_inv[4];
    Shop shop_buy_inv[5][4];
    Shop shop_sell_inv[5][4];
    
    for (int c=0;c<5;c++) {
    for (int i=0;i<4;i++) {
        shop_buy_inv[c][i]=RollShop(6);
    }
    for (int i=0;i<4;i++) {
        shop_sell_inv[c][i]=RollShop(5);
    }
    }

    int search_roll = 0;

    int selected = 0;

    int day  = 1;
    int time = 0;
    int city = 2;

    State state = MAIN;

    int square_offset[canvasWidth/10];
    for (int i=0;i<canvasWidth/10;i++) {
        square_offset[i]=GetRandomValue(0,canvasHeight);
    }

    while (!WindowShouldClose()) {
        UpdateMusicStream(bgm);
        /*
            Gameplay
        */
        int shortcut = MoveSelectArrow(&selected,fxMove);
        switch (state)
        {
        case MAIN:
            if (IsKeyPressed(KEY_ENTER) || shortcut) {
                PlaySoundMulti(fxSelect);
                switch (selected)
                {
                case 0:
                    state = BUY;
                    selected = 0;
                    break;
                case 1:
                    state = SELL;
                    selected = 0;
                    break;
                case 2:
                    state = CRAFT;
                    selected = 0;
                    break;
                case 3:
                    search_roll = GetRandomValue(0,5)+GetRandomValue(0,5)+GetRandomValue(0,5)+GetRandomValue(0,5);
                    state = SEARCH;
                    selected = 0;
                    break;
                case 4:
                    state = TRAVEL;
                    selected = city;
                    break;
                default:
                    break;
                }
            }
            break;
        case BUY:
            if (IsKeyPressed(KEY_ENTER) || shortcut) {
                PlaySoundMulti(fxSelect);
                if (selected == 4) {
                    state = MAIN;
                    selected = 0;
                    //IncrementTime(&time,&day,&inv,shop_buy_inv,shop_sell_inv);
                } else {
                Shop s = shop_buy_inv[city][selected];
                if (inv.cash>=s.cost) {
                    IncrementTime(&time,&day,&inv,shop_buy_inv,shop_sell_inv);
                    inv.cash-=s.cost;
                    inv.gates[s.gate]+=s.amount;
                    state = MAIN;
                    shop_buy_inv[city][selected] = RollShop(6);
                    selected = 0;
                }
                }
            }
            break;
        case SELL:
            if (IsKeyPressed(KEY_ENTER) || shortcut) {
                PlaySoundMulti(fxSelect);
                if (selected == 4) {
                    state=MAIN;
                    selected = 0;
                    //IncrementTime(&time,&day,&inv,shop_buy_inv,shop_sell_inv);
                } else {
                Shop s = shop_sell_inv[city][selected];
                if (inv.gates[s.gate]>=s.amount) {
                    IncrementTime(&time,&day,&inv,shop_buy_inv,shop_sell_inv);
                    inv.cash+=s.cost;
                    inv.gates[s.gate]-=s.amount;
                    state = MAIN;
                    shop_sell_inv[city][selected] = RollShop(5);
                    selected = 0;
                }
                }
                
            }
            break;
        case CRAFT:
            if (IsKeyPressed(KEY_ENTER) || shortcut) {
                PlaySoundMulti(fxSelect);
                if (selected == 4) {
                    state=MAIN;
                    selected = 0;
                    //IncrementTime(&time,&day,&inv,&shop_buy_inv,&shop_sell_inv);
                } else {
                    ExecuteRecipe(&inv,inv.blueprints[selected]);
                }
            }
            break;
        case SEARCH:
            if (IsKeyPressed(KEY_ENTER) || shortcut) {
                PlaySoundMulti(fxSelect);
                IncrementTime(&time,&day,&inv,shop_buy_inv,shop_sell_inv);
                state = MAIN;
                switch (search_roll)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    inv.gates[GetRandomValue(0,4)] += GetRandomValue(1,50);
                    break;
                default:
                    break;
                }
            }
            break;
        case TRAVEL:
            if (IsKeyPressed(KEY_ENTER) || shortcut) {
                PlaySoundMulti(fxSelect);
                if (selected != city)
                    IncrementTime(&time,&day,&inv,shop_buy_inv,shop_sell_inv);
                city = selected;
                selected = 0;
                state = MAIN;
            }
            break;
        default:
            printf("we shouldn't be here!");
            break;
        }
        /*
            Drawing
        */
        BeginDrawing();
        BeginTextureMode(canvas);
            ClearBackground(BLACK);
            //DrawTexture(background,0,0,WHITE);
            double t = t = GetTime();
            for (int i=0;i<canvasWidth;i+=10) {
                Color c[5] = {
                    BLUE,
                    GREEN,
                    PURPLE,
                    RED,
                    ORANGE
                };
                int y = ((int)(t*15)+square_offset[i/10])%(canvasHeight+10);
                int trans = 255-y*2;
                if (trans<0)trans=0;
                DrawRectangle(i,canvasHeight-y,10,10,(Color){c[city].r,c[city].g,c[city].b,trans});
            }
            DrawInventory(inv,font,0,0);
            DrawTime(day,time,city,font,230,0);
            switch (state)
            {
            case MAIN:
                DrawSelectMenu(font,0,135);
                DrawSelectArrow(selected,font,0,135);
            break;
            case BUY:
                DrawSelectArrow(selected,font,0,135);
                DrawTextEx(font,"Buying...",(Vector2){115,100},16,1,ORANGE);
                for(int i=0;i<4;i++){
                    Color color1 = WHITE;
                    Color color2 = GREEN;
                    if (shop_buy_inv[city][i].cost > inv.cash) {color1 = DARKGRAY; color2 = RED;} 
                    DrawShop(shop_buy_inv[city][i],color1,color2,font,i*60,125);
                }
                DrawTextEx(font,"Cancel",(Vector2){240,130},16,1,LIGHTGRAY);
            break;
            case SELL:
                DrawSelectArrow(selected,font,0,135);
                DrawTextEx(font,"Selling...",(Vector2){115,100},16,1,ORANGE);
                for(int i=0;i<4;i++){
                    Color color1 = WHITE;
                    Color color2 = GREEN;
                    if (shop_sell_inv[city][i].amount > inv.gates[shop_sell_inv[city][i].gate]) {color1 = DARKGRAY; color2 = RED;} 
                    DrawShop(shop_sell_inv[city][i],color1,color2,font,i*60,125);
                }
                DrawTextEx(font,"Cancel",(Vector2){240,130},16,1,LIGHTGRAY);
            break;
            case CRAFT:
                DrawSelectArrow(selected,font,0,135);
                DrawTextEx(font,"Crafting...",(Vector2){115,100},16,1,ORANGE);
                for (int i=0;i<4;i++) {
                DrawRecipe(inv,inv.blueprints[i],font,5+i*60,115);
                }
                DrawTextEx(font,"Cancel",(Vector2){240,130},16,1,LIGHTGRAY);
            break;
            case SEARCH:
                switch (search_roll)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    DrawTextEx(font,"You find some logic chips",(Vector2){45,100},16,1,ORANGE);
                break;
                default:
                    DrawTextEx(font,"You find nothing...",(Vector2){80,100},16,1,ORANGE);
                break;
                }
            break;
            case TRAVEL:
                DrawSelectArrow(selected,font,0,135);
                DrawTravelMenu(font,0,135);
            break;
            }
        EndTextureMode();
        float scale = 2.0f;
        DrawTexturePro(
        canvas.texture, 
        (Rectangle){ 0.0f, 0.0f,(float)canvasWidth,(float)-canvasHeight },
        (Rectangle){0,0,(float)screenWidth,(float)screenHeight},
        (Vector2){0,0},
        0.0f,
        WHITE
        );
        EndDrawing();
    }
    // Unload Assets
    UnloadFont(font);
    UnloadSound(fxMove);
    UnloadSound(fxSelect);
    UnloadTexture(background);
    CloseWindow();

    return 0;
}
