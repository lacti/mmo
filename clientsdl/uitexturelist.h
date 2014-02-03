#pragma once

typedef struct UiTextureEntity
{
	int x, y; // 화면상 위치. 좌상단이 (0,0)
	float age; // 화면에 보인 뒤 지난 시간 (초)
	float lifetime; // 얼마나 오랫동안 보일지 (초)
	float alpha; // 투명도
	unsigned int texId; // OpenGL 텍스쳐 ID
} UiTextureEntity;

typedef struct UiTextureList
{
	UiTextureEntity entities[64];
	size_t head, tail;
} UiTextureList;

void InitializeUiTextureList(UiTextureList* utl);
void AddUiTextureEntity(UiTextureList* utl, UiTextureEntity* e);
