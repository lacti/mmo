#pragma once

#ifndef LAME_API
#	ifdef LAME_EXPORTS
#		define LAME_API __declspec(dllexport)
#	else
#		define LAME_API __declspec(dllimport)
#	endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

LAME_API int
lame_decoder(lame_t gfp, FILE * outf, char *inPath, char *outPath);

LAME_API int
lame_main_ex(lame_t gf, int argc, char **argv);

#ifdef __cplusplus
}
#endif
