// needed includes
#include "SDL.h"

// globals
bool can_exit = false;
const int tickrate = 8;
SDL_Surface *screen;


#pragma pack(push, 1)

// from showanim.h:

/* structure declarations for deluxe animate large page files */

typedef struct {

 Uint32 id;    /* 4 character ID == "LPF " */

 Uint16 maxLps;    /* max # largePages allowed. 256 FOR NOW.   */

 Uint16 nLps;    /* # largePages in this file. */

 Uint32 nRecords;  /* # records in this file.  65534 is current limit plus */
      /* one for last-to-first delta for looping the animation */

 Uint16 maxRecsPerLp;  /* # records permitted in an lp. 256 FOR NOW.   */

 Uint16 lpfTableOffset;  /* Absolute Seek position of lpfTable.  1280 FOR NOW.
      The lpf Table is an array of 256 large page structures
      that is used to facilitate finding records in an anim
      file without having to seek through all of the Large
       Pages to find which one a specific record lives in. */

 Uint32 contentType;  /* 4 character ID == "ANIM" */

 Uint16 width;    /* Width of screen in pixels. */
 Uint16 height;    /* Height of screen in pixels. */
 Uint8 variant;  /*   0==ANIM. */
 Uint8 version;  /*   0==frame rate is multiple of 18 cycles/sec.
       1==frame rate is multiple of 70 cycles/sec.  */

 Uint8 hasLastDelta;  /* 1==Last record is a delta from last-to-first frame. */

 Uint8 lastDeltaValid;  /* 0==The last-to-first delta (if present) hasn't been
      updated to match the current first&last frames,  so it
       should be ignored. */

 Uint8 pixelType;  /* 0==256 color. */

 Uint8 CompressionType;  /* 1==(RunSkipDump) Only one used FOR NOW. */

 Uint8 otherRecsPerFrm;  /* 0 FOR NOW. */

 Uint8 bitmaptype;  /*   1==320x200, 256-color.  Only one implemented so far. */

 Uint8 recordTypes[32];  /* Not yet implemented. */

 Uint32 nFrames;   /*   In case future version adds other records at end of 
      file, we still know how many actual frames.
       NOTE: DOES include last-to-first delta when present. */

 Uint16 framesPerSecond;  /* Number of frames to play per second. */

 Uint16 pad2[29];  /* 58 bytes of filler to round up to 128 bytes total. */
} lpfileheader;


/* this is the format of an large page structure */
typedef struct {
 Uint16 baseRecord;  /* Number of first record in this large page. */

 Uint16 nRecords;  /* Number of records in lp.
            bit 15 of "nRecords" == "has continuation from previous lp".
              bit 14 of "nRecords" == "final record continues on next lp". */

 Uint16 nBytes;    /* Total number of bytes of contents, excluding header. */

} lp_descriptor;

#pragma pack(pop)


// more globals
lpfileheader lpheader;
Uint8 palette[256][3];
lp_descriptor lparray[256];
Uint8 *pages = NULL;
Uint8 *outbuffer = NULL;

int curlpnum=-1; // current large page
lp_descriptor *curlp; // current large page
Uint8 *thepage; // current page

int currentframe;

// functions

bool load_anim(const char *filename)
{
   FILE *fd = fopen(filename,"rb");
   if (fd==NULL)
      return false;

   // find out file length
   fseek(fd,0,SEEK_END);
   long filesize = ftell(fd);
   fseek(fd,0,SEEK_SET);

   // read in anim file header
   fread(&lpheader,sizeof(lpfileheader),1,fd);

   // skip color cycling structures
   fseek(fd,128,SEEK_CUR);

   // read in color palette
   for(int i=0; i<256; i++)
   {
      palette[i][2] = fgetc(fd); // red
      palette[i][1] = fgetc(fd); // green
      palette[i][0] = fgetc(fd); // blue

      // extra pad byte
      fgetc(fd);
   }

   // read in large page descriptors
   fread(lparray,sizeof(lp_descriptor),256,fd);

   // the file pointer now points to the first large page structure

   // load remaining pages into memory
   filesize -= ftell(fd);
   pages = new Uint8[filesize];
   fread(pages,filesize,1,fd);

   fclose(fd);

   // alloc memory for the outbuffer
   outbuffer = new Uint8[lpheader.width*lpheader.height+1000];

   return true;
}

// given a frame number return the large page number it resides in
void retrieve_page(int framenumber)
{
   int i=0;
   for(;i<lpheader.nLps; i++)
      if(lparray[i].baseRecord <= framenumber && lparray[i].baseRecord + lparray[i].nRecords > framenumber)
         break;

   // calculate large page descriptor pointer and large page pointer
   curlp = reinterpret_cast<lp_descriptor*>(pages+0x10000*i);
   thepage = reinterpret_cast<Uint8*>(curlp)+sizeof(lp_descriptor)+2;
   // page length: curlp.nBytes+(curlp.nRecords*2)
}

void myPlayRunSkipDump(Uint8 *srcP,Uint8 *dstP)
{
   while(true)
   {
      Sint8 cnt = (Sint8)*srcP++;

      if (cnt>0)
      {
         // dump
         while (cnt>0)
         {
            *dstP++ = *srcP++;
            cnt--;
         }
      }
      else if (cnt==0)
      {
         // run
         Uint8 wordCnt = *srcP++;
         Uint8 pixel = *srcP++;
         while(wordCnt>0)
         {
            *dstP++ = pixel;
            wordCnt--;
         }
      }
      else
      {
         cnt &= 0x7f; // cnt -= 0x80;
         if (cnt != 0)
         {
            // shortSkip
            dstP += cnt;
         }
         else
         {
            // longOp
            Uint16 wordCnt = *((Uint16*)srcP);
            srcP+=2;

            if ((Sint16)wordCnt <= 0)
            {
               // notLongSkip
               if (wordCnt == 0)
               {
                  break; // end loop
               }

               wordCnt &= 0x7fff; // wordCnt -= 0x8000; // Remove sign bit.
               if (wordCnt >= 0x4000)
               {
                  // longRun
                  wordCnt -= 0x4000; // Clear "longRun" bit
                  Uint8 pixel = *srcP++;
                  while(wordCnt>0)
                  {  
                     *dstP++ = pixel;
                     wordCnt--;
                  }
//                  dstP += wordCnt;
               }
               else
               {
                  // longDump
                  while(wordCnt>0)
                  {
                     *dstP++ = *srcP++;
                     wordCnt--;
                  }

//                  dstP += wordCnt;
//                  srcP += wordCnt;
               }
            }
            else
            {
               // longSkip
               dstP += wordCnt;
            }
         }
      }
   }
}

/* draw the frame sepcified from the large page in the buffer pointed to */
void get_frame_bytes(int framenumber)
{
   int destframe = framenumber - curlp->baseRecord;

   Uint16 offset=0;
   Uint16 *pagepointer = (Uint16*)thepage;
   for(Uint16 i = 0; i < destframe; i++)
      offset += pagepointer[i];

   Uint8 *ppointer = thepage + curlp->nRecords*2+offset;

   Uint16 *ppointer16 = (Uint16*)(ppointer);

   if(ppointer[1])
      ppointer += (4 + (ppointer16[1] + (ppointer16[1] & 1) ));
   else
      ppointer+=4;

   // extract data to the output buffer
//   CPlayRunSkipDump(ppointer, outbuffer);
   myPlayRunSkipDump(ppointer, outbuffer);
}

void DrawPixel(SDL_Surface *screen, int x, int y,
                                    Uint8 R, Uint8 G, Uint8 B)
{
  Uint32 color = SDL_MapRGB(screen->format, R, G, B);
  switch (screen->format->BytesPerPixel)
  {
    case 1: // Assuming 8-bpp
      {
        Uint8 *bufp;
        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
        *bufp = color;
      }
      break;
    case 2: // Probably 15-bpp or 16-bpp
      {
        Uint16 *bufp;
        bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
        *bufp = color;
      }
      break;
    case 3: // Slow 24-bpp mode, usually not used
      {
        Uint8 *bufp;
        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x * 3;
        if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
        {
          bufp[0] = color;
          bufp[1] = color >> 8;
          bufp[2] = color >> 16;
        } else {
          bufp[2] = color;
          bufp[1] = color >> 8;
          bufp[0] = color >> 16;
        }
      }
      break;
    case 4: // Probably 32-bpp
      {
        Uint32 *bufp;
        bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
        *bufp = color;
      }
      break;
  }
}

void draw_screen()
{
   retrieve_page(currentframe);
   get_frame_bytes(currentframe);

   if(SDL_MUSTLOCK(screen))
      SDL_LockSurface(screen);

   // draw pixels to screen
   int size=lpheader.width*lpheader.height;
   for(int i=0; i<size; i++)
   {
      int x=i%lpheader.width;
      int y=i/lpheader.width;

      Uint8 index = outbuffer[i];

      DrawPixel(screen,x,y,palette[index][0],palette[index][1],palette[index][2]);
   }

   if(SDL_MUSTLOCK(screen))
      SDL_UnlockSurface(screen);

   SDL_Flip(screen);
}

void done_anim()
{
   delete pages;
   delete outbuffer;
}

void process_events()
{
   SDL_Event event;

   // check for new event
   while(SDL_PollEvent(&event))
   {
      switch(event.type)
      {
      case SDL_KEYDOWN:
         if (event.key.keysym.sym == SDLK_ESCAPE)
            can_exit = true;
         break;

      case SDL_QUIT:
         can_exit = true;
         break;
      }
   }
}

int main(int argc, char* argv[])
{
   // check for proper number of arguments
   if (argc!=2)
   {
      printf("syntax: cutsdec <cutscene-file>\n");
      printf("example: cutsdec cs011.n01\n");
      return 1;
   }

   // try to load animation
   if (!load_anim(argv[1]))
   {
      printf("could not open/process file %s\n",argv[1]);
      return 1;
   }

   // init SDL
   if(SDL_Init(SDL_INIT_VIDEO)<0)
   {
      fprintf(stderr,"error initializing video: %s\n", SDL_GetError());
      return 1;
   }

   // get info about video
   const SDL_VideoInfo* info = SDL_GetVideoInfo();
   if(!info)
   {
      fprintf(stderr,"error getting video info: %s\n", SDL_GetError());
      return 1;
   }

   int width = lpheader.width;
   int height = lpheader.height;
   int bpp = info->vfmt->BitsPerPixel;

   // set video mode
   if((screen=SDL_SetVideoMode(width,height,bpp,SDL_HWSURFACE|SDL_DOUBLEBUF))==0)
   {
      fprintf(stderr,"failed to set video mode: %s\n", SDL_GetError());
      return 1;
   }

   Uint32 now,then;
   then = SDL_GetTicks();

   currentframe=0;

   // main loop
   while(!can_exit)
   {
      process_events();

      now = SDL_GetTicks();
      while ((now - then) > (1000.f/tickrate))
      {
         then += Uint32(1000.f/tickrate);

         draw_screen();

         // next tick
         currentframe++;
         if (unsigned(currentframe)>=lpheader.nRecords)
            currentframe=0;
      }
      SDL_Delay(0);
   }

   done_anim();

   // finish off SDL
   SDL_Quit();

   return 0;
}
