#pragma once

static const int FPS = 60;
static const float FPSf = static_cast<float>(FPS);
static const float SPF = 1.0f/FPSf;
static const float TICK_SEC = SPF;//1–Ú·‚è‚ ‚½‚è‚Ì•b”
static const int TICK_WDTH = 5;//1–Ú·‚è‚ ‚½‚è‚Ì•
static const float MAX_TIME = 5.0f;//Å‘å•b”
static const int MAX_TICK_NUM = static_cast<int>(MAX_TIME / TICK_SEC);
static const int HEIGHT_BAR = 24;//ƒo[‚Ì‚‚³
