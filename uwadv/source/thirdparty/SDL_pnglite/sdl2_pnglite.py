from ctypes import POINTER, c_int, c_char_p
from sdl2 import SDL_Surface, SDL_RWops
import sdl2.dll
import sdl2.rwops

pnglite = sdl2.dll.DLL("SDL_pnglite", ["SDL_pnglite"], os.getenv("PYSDL2_DLL_PATH"))
SDL_LoadPNG_RW = pnglite.bind_function("SDL_LoadPNG_RW", [POINTER(SDL_RWops), c_int], POINTER(SDL_Surface))
SDL_SavePNG_RW = pnglite.bind_function("SDL_SavePNG_RW", [POINTER(SDL_Surface), POINTER(SDL_RWops), c_int], c_int)

def load_png(fname):
    fobj = open(fname, "rb")
    rwops = sdl2.rw_from_object(fobj)
    surf = SDL_LoadPNG_RW(rwops, 0)
    sdl2.SDL_RWclose(rwops)
    fobj.close()
    return surf

def save_png(surf, fname):
    fobj = open(fname, "wb")
    rwops = sdl2.rw_from_object(fobj)
    rv = SDL_SavePNG_RW(surf, rwops, 0)
    sdl2.SDL_RWclose(rwops)
    fobj.close()
    return rv