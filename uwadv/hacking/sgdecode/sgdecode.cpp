// cnvdec.cpp : Defines the entry point for the console application.
//

#include <stdio.h>

const char *fname = "..\\..\\uw1\\Save1\\player.dat";

char *statsnskills[23] =
{
   "Strength      ",
   "Dexterity     ",
   "Intelligence  ",
   "Attack        ",
   "Defense       ",
   "Unarmed       ",
   "Sword         ",
   "Axe           ",
   "Mace          ",
   "Missile       ",
   "Mana          ",
   "Lore          ",
   "Casting       ",
   "Traps         ",
   "Search        ",
   "Track         ",
   "Sneak         ",
   "Repair        ",
   "Charm         ",
   "Picklock      ",
   "Acrobat       ",
   "Appraise      ",
   "Swimming      "
};


int main()
{
   FILE *fd = fopen(fname,"rb");

   if (fd==NULL)
   {
      printf("could not open file %s\n",fname);
      return 1;
   }

   // read in data
   unsigned char data[312];
   fread(data,1,312,fd);

   // descramble data
   unsigned char xorvalue = data[0];
   unsigned char incrnum = 3;

   for(int i=1; i<=220; i++)
   {
      if (i==81) incrnum = 3;
      data[i] ^= (xorvalue+incrnum);
      incrnum += 3;
   }

   // print out character info
   printf("name: %s\n",data+1);
   printf("vitality: %u out of %u\n",data[221],data[55]);
   printf("mana: %u out of %u\n",data[56],data[57]);
   printf("food: %u, level %u\n",data[58],data[62]);

   unsigned int exp_points = data[79] | (unsigned(data[80])<<8) |
      (unsigned(data[81])<<16) | (unsigned(data[82])<<24);
   printf("exp: %4.1f\n",exp_points/10.f);

   int weight = data[77] | (unsigned(data[78])<<8);
   printf("weight: %2.1f\n",weight/10.f);


   printf("\nstats & skills:\n");
   for(int n=0; n<23; n++)
   {
      printf("%s: %u\t",statsnskills[n],data[n+31]);
      if (++n<23)
      printf("%s: %u\t",statsnskills[n],data[n+31]);
      if (++n<23)
      printf("%s: %u\t\t",statsnskills[n],data[n+31]);
   }
   printf("\n");

   fclose(fd);

   return 0;
}
