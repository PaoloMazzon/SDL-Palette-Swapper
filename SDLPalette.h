// Copyright (c) 2020 Paolo Mazzon
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// 
// This is a tool for applying interchangable palettes to surfaces to achieve
// an effect similar to that of palette swapping in the game Downwell. This
// tool uses SDL2 and is licensed under the zlib license. 
// 
// Example
// 
//     RGBA baseColours[] = {
//         {255, 255, 255, 255},
//         {000, 000, 000, 255}
//     }
//     RGBA palette1[] = {
//         {000, 000, 255, 255},
//         {255, 000, 000, 255}
//     }
//     /// More palettes
//     
//     Palette myPalette1;
//     myPalette1.palette = palette1;
//     myPalette1.baseColours = baseColours;
//     myPalette1.numColours = 2;
//     Palette myPalette2;
//     myPalette2.palette = palette2;
//     myPalette2.baseColours = baseColours;
//     myPalette2.numColours = 2;
//     ...
//     SDL_Surface* appliedPaletteSurface = applyPalette(mySurface, &myPalette1);
//     
// For every different palette you can (and likely will) use the same base colours
// but will need a new palette of colours to swap to. You then just call applyPalette
// and it will return a new surface which is a copy of the original but with the new
// palette applied to it (and then of course convert it to a texture or what have you).

#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/// \brief A basic RGBA struct for transporting data
typedef struct {
	Uint8 red;   ///< Red component
	Uint8 green; ///< Green component
	Uint8 blue;  ///< Blue component
	Uint8 alpha; ///< Alpha component
} RGBA;

/// \brief Converts a colour in a colour vector to a given surface's format
/// \param vec Vector to grab the colour from
/// \param i Index of the colour to convert
/// \param surf The surface to use the colour format of
/// \return Returns a Uint32 colour that fits the format of the given surface
static inline Uint32 mapToSurfaceColour(RGBA* vec, int i, SDL_Surface* surf) {
	return SDL_MapRGBA(surf->format, vec[i].red, vec[i].green, vec[i].blue, vec[i].alpha);
}

/// \brief Contains all of the data necessary for applyPalette to swap a surface's colours
typedef struct {
	RGBA* palette;     ///< A vector of colours that represent what the corresponding baseColours index will be changed to
	RGBA* baseColours; ///< A vector of colours that are to be changed into the corresponding palette index
	int numColours;      ///< How many colours are in this palette (doesn't matter how many; needs not be power of 2)
} Palette;

/// \brief Applies a palette on a copy of the source surface and returns it
/// \param source Surface that will be copied and have the palette applied to it
/// \param palette Palette to apply to the copied surface
/// \return Returns a copy of source but with the palette applied to it (source will remain unchanged)
SDL_Surface* applyPalette(SDL_Surface* source, Palette* palette) {
	SDL_Surface *dest, *newSource;
	Uint32 *srcPixels, *dstPixels;
	dest = NULL;
	int x, y, i;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const Uint32 rmask = 0xff000000;
    const Uint32 gmask = 0x00ff0000;
    const Uint32 bmask = 0x0000ff00;
    const Uint32 amask = 0x000000ff;
#else
	const Uint32 rmask = 0x000000ff;
	const Uint32 gmask = 0x0000ff00;
	const Uint32 bmask = 0x00ff0000;
	const Uint32 amask = 0xff000000;
#endif

	if (source != NULL && palette != NULL) {
		dest = SDL_CreateRGBSurface(0, source->w, source->h, 32, rmask, gmask, bmask, amask);

		if (dest != NULL) {
			newSource = SDL_ConvertSurface(source, dest->format, 0);

			if (newSource != NULL) {
				SDL_LockSurface(source);
				srcPixels = newSource->pixels;
				dstPixels = dest->pixels;

				for (y = 0; y < source->h; y++)
					for (x = 0; x < source->w; x++)
						for (i = 0; i < palette->numColours; i++)
							if (mapToSurfaceColour(palette->baseColours, i, dest) == srcPixels[y * source->w + x])
								dstPixels[y * source->w + x] = mapToSurfaceColour(palette->palette, i, dest);

				SDL_UnlockSurface(source);
				SDL_FreeSurface(newSource);
			} else {
				fprintf(stderr, "Failed to convert surface, SDL error=%s\n", SDL_GetError());
			}
		} else {
			fprintf(stderr, "Failed to create destination surface, SDL error=%s\n", SDL_GetError());
		}
	} else {
		if (source == NULL)
			fprintf(stderr, "Source does not exist\n");
		else
			fprintf(stderr, "Palette does not exist\n");
	}

	return dest;
}

#ifdef __cplusplus
}
#endif