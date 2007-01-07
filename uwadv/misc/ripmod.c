/*
 * ripmod.c -
 *  Rip the model definitions out of the exe
 */


#include <stdio.h>
#include <stdlib.h>



/*
 * Typedefs and structs
 */
struct exe_header
{
  unsigned short magic;              /* 0x5a4d 'MZ' */
  unsigned short no_bytes_last_page;
  unsigned short no_pages;           /* 512 bytes per page */
  unsigned short no_reloc_entries;
  unsigned short header_paragraphs;  /* 16 bytes per paragraph */
  unsigned short min_extra_paras;
  unsigned short max_extra_paras;
  unsigned short initial_ss;         /* stack segment */
  unsigned short initial_sp;         /* stack pointer */
  unsigned short checksum;           /* Or zero if not present */
  unsigned short initial_ip;         /* instruction pointer */
  unsigned short initial_cs;         /* code segment */
  unsigned short offs_reloc_table;   /* from the start of the exe */
  unsigned short overlay_no;         /* zero for main program */
  unsigned short stuff [3];          /* 3 unknown words */
  unsigned char  reserved [26];
  unsigned short offs_pe_header;     /* Windows protected mode exe or zero */
};

struct exe_header UW_exe_head;


/*
 * Information about the 2 different Underworld games
 */
struct
{
  unsigned short offs_idstr;        /* A string to identify the exe */
  const char *idstr;
  unsigned short offs_mod_colinfo;  /* in the data segment */
  unsigned short size_mod_colinfo;
  unsigned short offs_mod_baseaddr;
  unsigned short size_mod_vars;     /* size of 3d variable space */
  const char *name;
} UW_layout [2] =
{ { 0x1bc6, "f32.tr",      0x060a, 4, 0x247c, 0x18, "Underworld   : SA " },
  { 0x148c, "CRIT\\as.an", 0x05fa, 5, 0x215c, 0x24, "Underworld II: LoW" } };

int UW_game = -1;


#define MAGIC_EXE 0x5a4d

#define READ_SHORT(f,w)do { (w) = fgetc (f); (w) |= fgetc (f) << 8; } while (0)


void read_exe_header (FILE *f, struct exe_header *head)
{

  fseek (f, 0, SEEK_SET); /* rewind the file */
  READ_SHORT (f, head->magic);
  if (head->magic != MAGIC_EXE)
  {
    fprintf (stderr, "read_exe_header(): not MZ exe\n");
    exit (1);
  }
  READ_SHORT (f, head->no_bytes_last_page);
  READ_SHORT (f, head->no_pages);
  READ_SHORT (f, head->no_reloc_entries);
  READ_SHORT (f, head->header_paragraphs);
  READ_SHORT (f, head->min_extra_paras);
  READ_SHORT (f, head->max_extra_paras);
  READ_SHORT (f, head->initial_ss);
  READ_SHORT (f, head->initial_sp);
  READ_SHORT (f, head->checksum);
  READ_SHORT (f, head->initial_ip);
  READ_SHORT (f, head->initial_cs);
  READ_SHORT (f, head->offs_reloc_table);
  READ_SHORT (f, head->overlay_no);
  READ_SHORT (f, head->stuff [0]);
  READ_SHORT (f, head->stuff [1]);
  READ_SHORT (f, head->stuff [2]);
  fread (head->reserved, 1, 26, f);
  READ_SHORT (f, head->offs_pe_header);

}


void dump_exe_header (struct exe_header *head)
{

  printf ("\nEXE information follows\n\n");

  printf ("%u pages in executable, %u bytes last page\n", head->no_pages,
	  head->no_bytes_last_page);
  printf ("%u entries in relocation table\n", head->no_reloc_entries);
  printf ("Header size 0x%04x bytes\n", head->header_paragraphs * 16);
  printf ("Min. extra paragraphs %u\n", head->min_extra_paras);
  printf ("Max. extra paragraphs %u\n", head->max_extra_paras);
  printf ("Initial SS:SP %04x:%04x\n", head->initial_ss, head->initial_sp);
  printf ("Checksum = %04x\n", head->checksum);
  printf ("Initial CS:IP %04x:%04x\n", head->initial_cs, head->initial_ip);
  printf ("Relocation table at file offset 0x%04x\n", head->offs_reloc_table);
  printf ("Overlay no. %u\n", head->overlay_no);
  if (head->offs_pe_header == 0) printf ("PE header not present\n");
  else printf ("PE header at file offset 0x%04x\n", head->offs_pe_header);
  printf ("\n");

}


unsigned long seg_offs_to_file_offs (unsigned short seg, unsigned short offs)
{
  return seg * 16 + offs + 16 * UW_exe_head.header_paragraphs;
}



int main (int argc, char *argv [])
{

  FILE *uwexe;
  unsigned long offs_code;
  unsigned long offs_data;
  unsigned short main_data_seg;
  unsigned short mod_seg;
  unsigned short mod_base;
  char turboc [0x2a];
  int i;

  if (argc != 2)
  {
    fprintf (stderr, "%s: syntax\n", argv [0]);
    exit (1);
  }

  uwexe = fopen (argv [1], "rb");
  if (uwexe == NULL)
  {
    fprintf (stderr, "%s: couldn't open file `%s'\n", argv [0], argv [1]);
    exit (1);
  }

  read_exe_header (uwexe, &UW_exe_head);
  dump_exe_header (&UW_exe_head);

  /*
   * The very first thing all Underworld executables do is to load up the
   *  value they're going to be using for the main data segment and store it
   *  somewhere safe
   */
  printf ("Looking for the main data segment ...\n");
  offs_code = seg_offs_to_file_offs (UW_exe_head.initial_cs,
				     UW_exe_head.initial_ip);
  printf ("Start of executable code at file offset 0x%08x\n", offs_code);
  fseek (uwexe, offs_code, SEEK_SET);
  if (fgetc (uwexe) != 0xba) /* mov dx, immed16 */
  {
    fprintf (stderr, "%s: didn't find data segment load\n", argv [0]);
    exit (1);
  }
  READ_SHORT (uwexe, main_data_seg);
  printf ("Found what looks like a mov dx, $%04x ... \n", main_data_seg);

  /*
   * Now we check for the Turbo C++ copyright string at data:0008
   */
  offs_data = seg_offs_to_file_offs (main_data_seg, 0);
  printf ("Main data segment at file offset 0x%08x\n", offs_data);
  fseek (uwexe, offs_data + 8, SEEK_SET);
  fread (turboc, 1, 0x29, uwexe);
  if (strcmp (turboc, "Turbo C++ - Copyright 1990 Borland Intl.") != 0)
  {
    fprintf (stderr, "%s: didn't find Turbo C++ copyright string\n");
    exit (1);
  }
  printf ("Found copyright string \"%s\"\n", turboc);
  for (i = 0; i < 2; ++i)
  {
    int len = strlen (UW_layout [i].idstr) + 1;
    offs_data = seg_offs_to_file_offs (main_data_seg,
				       UW_layout [i].offs_idstr);
    fseek (uwexe, offs_data, SEEK_SET);
    fread (turboc, 1, len, uwexe);
    turboc [len] = '\0';
    if (strcmp (turboc, UW_layout [i].idstr) == 0)
    {
      printf ("Found UW identifying string \"%s\"\n", turboc);
      UW_game = i;
      break;
    }
  }
  if (UW_game < 0)
  {
    fprintf (stderr, "%s: didn't find characteristic string\n", argv [0]);
    exit (1);
  }
  printf ("Exe belongs to %s\n\n", UW_layout [UW_game].name);

  /*
   * Now that we know what game we are dealing with, we can go look for the
   *  models
   */
  offs_data = seg_offs_to_file_offs (main_data_seg,
				     UW_layout [UW_game].offs_mod_baseaddr);
  fseek (uwexe, offs_data, SEEK_SET);
  READ_SHORT (uwexe, mod_base);
  READ_SHORT (uwexe, mod_seg);
  offs_data = seg_offs_to_file_offs (mod_seg, mod_base);
  printf ("Model base address is %04x:%04x (file offset 0x%08x)\n",
	  mod_seg, mod_base, offs_data);

  /*
   * There are 64 model slots, immediately preceding the model variables, which
   *  in turn immediately precede the models themselves
   */
  for (i = 0; i < 64; ++i)
  {
    unsigned short mod_rel;
    unsigned long offs_mod;
    fseek (uwexe, offs_data - UW_layout [UW_game].size_mod_vars - 0x80 + 2*i,
	   SEEK_SET);
    READ_SHORT (uwexe, mod_rel);
    offs_mod = seg_offs_to_file_offs (mod_seg, mod_base + mod_rel);
    printf ("Model %d at model offset %04x (file offset 0x%08x)\n", i,
	    mod_rel, offs_mod);
  }

  fclose (uwexe);
  return 0;

}
