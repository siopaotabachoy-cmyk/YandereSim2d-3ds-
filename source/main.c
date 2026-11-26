// Starter for a 2D school-life stealth game on Nintendo 3DS (libctru + citro2d)
// Controls: Circle Pad / D-Pad = move, A = talk to crush, START = exit

#include <3ds.h>
#include <citro2d.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_W 400
#define SCREEN_H 240

typedef struct { float x, y, w, h; } Actor;

static float dist(Actor a, Actor b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

int main(void) {
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    Actor player = {40, 110, 16, 16};
    Actor crush  = {320, 110, 16, 16};
    Actor guard  = {160, 60, 16, 16};
    float guardDir = 1.0f;
    int affection = 0;     // 0..100
    int suspicion = 0;     // 0..100

    u32 bg      = C2D_Color32(40, 40, 60, 255);
    u32 cPlayer = C2D_Color32(255, 105, 180, 255);
    u32 cCrush  = C2D_Color32(120, 200, 255, 255);
    u32 cGuard  = C2D_Color32(230, 80, 80, 255);
    u32 cBar    = C2D_Color32(255, 255, 255, 255);
    u32 cAff    = C2D_Color32(255, 150, 200, 255);
    u32 cSus    = C2D_Color32(255, 200, 0, 255);

    while (aptMainLoop()) {
        hidScanInput();
        u32 held = hidKeysHeld();
        u32 down = hidKeysDown();
        if (down & KEY_START) break;

        // Movement
        circlePosition cp;
        hidCircleRead(&cp);
        float speed = 2.0f;
        if (held & KEY_DLEFT)  player.x -= speed;
        if (held & KEY_DRIGHT) player.x += speed;
        if (held & KEY_DUP)    player.y -= speed;
        if (held & KEY_DDOWN)  player.y += speed;
        if (abs(cp.dx) > 15) player.x += speed * cp.dx / 156.0f;
        if (abs(cp.dy) > 15) player.y -= speed * cp.dy / 156.0f;

        if (player.x < 0) player.x = 0;
        if (player.y < 0) player.y = 0;
        if (player.x > SCREEN_W - player.w) player.x = SCREEN_W - player.w;
        if (player.y > SCREEN_H - player.h) player.y = SCREEN_H - player.h;

        // Guard patrols left/right
        guard.x += guardDir * 1.2f;
        if (guard.x > 260 || guard.x < 100) guardDir = -guardDir;

        // Interaction: talk to crush
        if ((down & KEY_A) && dist(player, crush) < 30 && affection < 100)
            affection += 5;

        // Suspicion rises if the guard is close, falls otherwise
        if (dist(player, guard) < 70) { if (suspicion < 100) suspicion++; }
        else if (suspicion > 0 && (svcGetSystemTick() % 4 == 0)) suspicion--;

        // Draw
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, bg);
        C2D_SceneBegin(top);

        C2D_DrawRectSolid(crush.x,  crush.y,  0, crush.w,  crush.h,  cCrush);
        C2D_DrawRectSolid(guard.x,  guard.y,  0, guard.w,  guard.h,  cGuard);
        C2D_DrawRectSolid(player.x, player.y, 0, player.w, player.h, cPlayer);

        // Meters
        C2D_DrawRectSolid(10, 10, 0, 102, 8, cBar);
        C2D_DrawRectSolid(11, 11, 0, affection, 6, cAff);
        C2D_DrawRectSolid(10, 24, 0, 102, 8, cBar);
        C2D_DrawRectSolid(11, 25, 0, suspicion, 6, cSus);

        C3D_FrameEnd(0);
    }

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
