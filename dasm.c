/*************************************************************************************

   Z8001 disassembler

   Copyright Matt Knoth 2008 (p8000@earthlink.net)

   based on Juergen Buchmueller's Z8002 MAME model
 
   This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 *************************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define OPCALL(x) NULL
#define Z8K1_DASM 1

typedef unsigned int   UINT32;
typedef unsigned short UINT16;
typedef unsigned char  UINT8;
typedef unsigned int   offs_t;
typedef char           INT8;

/* structure for the opcode definition table */
typedef struct {
        int     beg, end, step;
        int     size, cycles;
        void    (*opcode)(void);
        const char      *dasm;
    UINT32 dasmflags;
}       Z8000_init;

/* structure for the opcode execution table / disassembler */
typedef struct {
    void    (*opcode)(void);
    int     cycles;
        int     size;
    const char    *dasm;
    UINT32 dasmflags;
}       Z8000_exec;

#define DASMFLAG_STEP_OVER                0x20000000      /* this instruction should be stepped over by setting a breakpoint afterwards */
#define DASMFLAG_STEP_OUT                 0x40000000      /* this instruction should be the end of a step out sequence */
#define DASMFLAG_SUPPORTED                0x80000000      /* are disassembly flags supported? */

#define F_PV    0x0010                          /* parity/overflow flag */
#define F_S     0x0020                          /* sign flag */
#define F_Z     0x0040                          /* zero flag */

static Z8000_exec *z8000_exec = NULL;
static Z8000_exec *z8000_exec_seg = NULL;
static UINT8 z8000_zsp[256];
static UINT8 segdisp32hex=0;

static char version[]= { 'v', '0', '.', '9', 'b', 'e', 't', 'a', '\0' };

static void  zinvalid(void)
{
        fprintf(stderr,"Z8000 invalid opcode\n");
        exit(1);
}

#include"8000dasm.c"
#include"z8000tbl.c"

void usage(char* arg)
{
     fprintf(stderr,"\nZ8001 Disassembler %s. (C) Matt Knoth 2008 (p8000@earthlink.net), based on Juergen Buchmueller's Z8002 MAME model\n\n",version);
     fprintf(stderr,"USAGE: %s [-s] [-a] [-t 1/2] [-p <start_pc>] [-o <file_offset>] [-n <n_bytes>] <Z8001_binary>\n",arg);
     fprintf(stderr,"               number prefix '0x'-hex, '0'-octal, else decimal\n");
     fprintf(stderr,"           -s  segmented mode, non-segmented default\n");
     fprintf(stderr,"           -a  display pc and segment number as 32bit hex, rather than <<%%seg>>%%base\n");
     fprintf(stderr,"           -t  test mode: -t1 hex encoding, 1 instruction/line, -t2 hex encoding consecutively - newlines accepted\n");
     fprintf(stderr,"           -p  preset PC to argument value\n");
     fprintf(stderr,"           -n  stop disassemble after n bytes\n");
     fprintf(stderr,"           -o  preset file offset to argument value\n\n");
}

// Z8001 disassembler - testmode 1: each input line contains at least 16bit hex code which denotes a primary Z8001 opcode.
// Opcode lines are appended by zeroes as needed by each instruction length. Encodings >16bit can be added to the primary
// opcode. Additionally, comment lines are accepted, marked with '#' at line start. Comments are copied into the output stream
// Address bytes 0-7 are interpreted as Z8001 PSW codes
// I.E a file containing 65536 lines with hex codes 0000-ffff will be disassembled as (almost) complete Z8001 instruction list

void test_mode1(FILE* fp, offs_t pc, int segmented, int len)
{
    char  inbuf[256];
    char  oprom[256];
    char dasmbuf[256];
    int  cnt;

    cnt=0;
    while(fgets(inbuf,255,fp)) {
        int i,n,sz;
        char c,*p;

        if(inbuf[0]=='#') {            /// comment
           printf("%s",inbuf);
           continue;
        }

        p=strchr(inbuf,'\n');
        if(p) *p='\0';

        n=strlen(inbuf)>>1;
        memset(oprom,0,256);
        for(i=0; i<n; i++) {
            c=inbuf[(i<<1)+2];
            inbuf[(i<<1)+2]=0;
            sscanf(&inbuf[(i<<1)],"%02x",(int*)&oprom[i]);
            inbuf[(i<<1)+2]=c;
        }

        sz=z8000_dasm(dasmbuf, pc, oprom, NULL, segmented? z8000_exec_seg : z8000_exec) & 0x0ffff;
        strcat(inbuf,"000000000000000000000000000000000000000000000000000000000000000000000000");
        inbuf[sz<<1]='\0';
        printf("%04x %20s  %s\n",pc,inbuf,dasmbuf);
        pc+=sz;
        cnt+=sz;
        if(len && (cnt>=len)) return;
    }
}

// Z8001 disassembler - testmode 2: all opcodes are hexadecimal. Newlines are permitted at word boundaries.
// Comments/Spaces are not supported.

void test_mode2(FILE* fp, offs_t pc, int segmented, int len)
{
    char inbuf[256];
    char oprom[256];
    char dasmbuf[256];
    int k,m,n,sz,cnt;
    char* p, c;

    cnt=0;                                               // reset byte counter
    m=n=0;                                               // init max and current index
    do {                                                 // read in full hex file

      if(((m-n)<16) && ((m-n)>=0)) {                     // oprom buffer low
          inbuf[0]='\0';                                 // reset buffer
          fgets(inbuf,255,fp);                           // get new hex line
          p=strchr(inbuf,'\n');                          // find newline
          if(p) *p='\0';                                 // remove newline

          memmove(&oprom[0],&oprom[n],1+m-n);            // move binary buffer to begin of buf
          m=m-n;                                         // correct max index
          n=0;                                           // reset current index

          while(inbuf[n] && inbuf[n+1]) {                // convert input line
            c=inbuf[n+2];                                // remember follow up
            inbuf[n+2]='\0';                             // limit string
            sscanf(&inbuf[n],"%02x",(int*)&oprom[m++]);  // convert
            n+=2;                                        // advance in string
            inbuf[n]=c;                                  // restore next entry
          }

          if(inbuf[n] && ~inbuf[n+1]) {                  // can't do single nibble hex values
             fprintf(stderr,"ERROR: odd sized input line - abort: \n\n>>%s<<\n\n",&inbuf[0]);
             return;
          }
          n=0;
      }

      if(n==m) return;                                   // file end, nothing to do

      sz=z8000_dasm(dasmbuf, pc, &oprom[n], NULL, segmented? z8000_exec_seg : z8000_exec) & 0x0ffff;
      for(k=0, inbuf[0]='\0'; k<sz; k++) sprintf(inbuf,"%s%02x",inbuf,(unsigned char)oprom[n+k]);
      printf("%04x %20s  %s\n",pc,inbuf,dasmbuf);
      n+=sz;
      pc+=sz;
      cnt+=sz;

    } while(~(len && (cnt>=len)) && (n<=m));
}

void dasm_binary(FILE* fp, offs_t pc, int segmented, int len)
{
    char inbuf[256];
    char oprom[256];
    char dasmbuf[256];
    int k,m,n,sz,cnt;

    cnt=0;                                               // reset byte counter
    m=n=0;                                               // init max and current index
    do {                                                 // read in full hex file

      if(((m-n)<16) && ((m-n)>=0)) {                     // oprom buffer low

          memmove(&oprom[0],&oprom[n],1+m-n);            // move binary buffer to begin of buf
          m=m-n;                                         // correct max index
          n=0;                                           // reset current index

          m+=fread(&oprom[m],1,256-m,fp);                // fill up buffer
      }

      if(n==m) return;                                   // file end, nothing to do

      sz=z8000_dasm(dasmbuf, pc, &oprom[n], NULL, segmented? z8000_exec_seg : z8000_exec) & 0x0ffff;
      for(k=0, inbuf[0]='\0'; k<sz; k++) sprintf(inbuf,"%s%02x",inbuf,(unsigned char)oprom[n+k]);
      printf("%04x %20s  %s\n",pc,inbuf,dasmbuf);
      n+=sz;
      pc+=sz;
      cnt+=sz;

    } while(~(len && (cnt>=len)) && (n<=m));
}

int main(int argc, char** argv)
{
    FILE* fp;
    int segmented;
    offs_t pc,offset;
    int c,n,tmode;
     
    n=0;
    pc=0;
    tmode=0;
    offset=0;
    opterr = 0;
    segmented=0;

    while ((c = getopt (argc, argv, "an:o:p:st:")) != -1)
       switch (c) {
       case 'a':
             segdisp32hex=1;
             break;
       case 's':
             segmented = 1;
             break;
       case 'n':
             n=(offs_t)strtol(optarg,NULL,0);
             break;
       case 'o':
             offset=(offs_t)strtol(optarg,NULL,0);
             break;
       case 'p':
             pc=(offs_t)strtol(optarg,NULL,0);
             break;
       case 't':
             tmode = atoi(optarg);
             if(tmode<1 || tmode>2) {
                fprintf(stderr,"ERROR: undefined -t argument '%d' - ignored\n",tmode);
                tmode=0;
             }
             break;
       default:
             usage(argv[0]);
             return 0;
    }

    if(optind >= argc) {
       fprintf(stderr,"No input file given -- abort\n");
       return 0;
    }

    if((fp=fopen(argv[optind],tmode? "r":"rb"))==NULL) {
        fprintf(stderr,"ERROR: unable to open file %s\n",argv[optind]);
        return(1);
    }

    if(tmode && offset) {
       fprintf(stderr,"WARNING: file offset ignored in -t%d mode - reset to 0\n",tmode);
       offset=0;
    }

    if(offset && fseek(fp,offset,SEEK_SET)) {
       fprintf(stderr,"ERROR: invalide file offset %d for %s - abort\n\n",offset,argv[optind]);
       return(3);
    }

    z8000_init(0, 0, NULL, NULL);

         if(tmode==1) test_mode1(fp,pc,segmented,n);
    else if(tmode==2) test_mode2(fp,pc,segmented,n);
    else              dasm_binary(fp,pc,segmented,n);

    fclose(fp);

    return 0;
}

