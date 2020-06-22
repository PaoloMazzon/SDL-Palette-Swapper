SDL Palette Swapper
===================

This is a tool for applying interchangable palettes to surfaces to achieve
an effect similar to that of palette swapping in the game Downwell. This
tool uses SDL2 and is licensed under the zlib license. 

Example

    RGBA baseColours[] = {
        {255, 255, 255, 255},
        {000, 000, 000, 255}
    }
    RGBA palette1[] = {
        {000, 000, 255, 255},
        {255, 000, 000, 255}
    }
    /// More palettes
    
    Palette myPalette1;
    myPalette1.palette = palette1;
    myPalette1.baseColours = baseColours;
    myPalette1.numColours = 2;
    Palette myPalette2;
    myPalette2.palette = palette2;
    myPalette2.baseColours = baseColours;
    myPalette2.numColours = 2;
    ...
    SDL_Surface* appliedPaletteSurface = applyPalette(mySurface, &myPalette1);
    
For every different palette you can (and likely will) use the same base colours
but will need a new palette of colours to swap to. You then just call applyPalette
and it will return a new surface which is a copy of the original but with the new
palette applied to it (and then of course convert it to a texture or what have you).

Usage
=====

Include the header in your project (C or C++) and use it from whatever file you need it in.