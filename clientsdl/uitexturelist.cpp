#include "stdafx.h"
#include "uitexturelist.h"

void InitializeUiTextureList(UiTextureList* utl)
{
	memset(utl->entities, 0, sizeof(utl->entities));
	utl->head = 0;
	utl->tail = 0;
}

void AddUiTextureEntity(UiTextureList* utl, UiTextureEntity* e)
{
	memcpy(utl->entities + utl->tail, e, sizeof(UiTextureEntity));
	++utl->tail;
}

void RenderUiTextureList(UiTextureList* utl)
{
}
