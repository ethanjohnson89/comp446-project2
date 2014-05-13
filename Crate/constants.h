#ifndef Constants_H
#define Constants_H

//Defines to make life easier

#define Vector3 D3DXVECTOR3
#define Matrix D3DXMATRIX
#define Identity D3DXMatrixIdentity
#define Translate D3DXMatrixTranslation
#define RotateX D3DXMatrixRotationX
#define RotateY D3DXMatrixRotationY
#define RotateZ D3DXMatrixRotationZ
#define ToRadian D3DXToRadian
#define ToDegree D3DXToDegree
#define Scale D3DXMatrixScaling
#define Normalize D3DXVec3Normalize
#define TransformVec D3DXVec3Transform
#define TransformCoord D3DXVec3TransformCoord
#define VectorDot D3DXVec3Dot
#define VectorCross D3DXVec3Cross

#define Radius 25
#define REGEN_TIME 2.5
#define REGEN_TIME_LVL1 4
#define NUM_WALLS 16
//#define NUM_WALLS_LVL1 10
#define NUM_LAYERS 5
#define LASER_SPEED_LVL2 .003f
#define LASER_SPEED_LVL1 .0015f
#define WALL_SIZE 1.7
#define PLAYER_RADIUS 26.5f

#define NUM_SENTRIES_LVL3 2

#define SENTRY_RADIUS 10

enum State {intro, start, game, end, restart, nextLevel, instructions};
//// Audio stuff

// audio files
const char WAVE_BANK[]  = "audio\\Win\\Wave Bank.xwb";
// SOUND_BANK must be location of .xsb file.
const char SOUND_BANK[] = "audio\\Win\\Sound Bank.xsb";

//audio cues
const char BULLETSHOOT[] = "bulletshoot";
const char WALLHIT[] = "wallhit";
const char BOSSDYING[] = "bossdying";
const char LASER[] = "laser";
const char INTROMUSIC[] = "intromusic";
const char ENDMUSIC[] = "endmusic";

#endif