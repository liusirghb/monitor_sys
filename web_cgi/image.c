#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cgic.h>
#include <dirent.h>

struct pic_list
{
     char name[30];	
}g_img[2000];


void print_file(FILE *dst_fp, const char *src)
{
	FILE *src_fp;
	char line[1024];	
	
	src_fp=fopen(src, "r");
		
	while (NULL != fgets(line, 1024, src_fp)) 
		fputs(line, dst_fp);
	
	fclose(src_fp);		
}

int cmp_sfc(const char *fn, const char* sf)
{
    char *tfn = (char *)fn;
    
    for(;*tfn!='.' && *tfn!=0;tfn++);
      ;
      
    if(*tfn==0)
       return -1;
    
    tfn++;
    
    return strcmp(tfn,sf);
}


int list_pic(const char* path, const char *sfx)
{
   //����/mnt/sdĿ¼�������е�ͼƬ�ļ������ּ����б�
   DIR *dir;
   struct dirent *ptr;
   int i = 0;
   
   //1. ��Ŀ¼
   dir = opendir(path);

   
   //2. ��ȡĿ¼�е�һ���ļ�
  while( (ptr = readdir(dir))!=NULL)
  { 
      //3. �жϸ��ļ��ĺ�׺�Ƿ�Ϊjpg
   
      if( 0==cmp_sfc(ptr->d_name,sfx))
      {
   	 //4. �����ͼƬ�ļ��������ļ��������б�
   	 strcpy(g_img[i].name,ptr->d_name);
   	 i++;
      }
  }	
  
  return i;
}

void show_pic(int start,int end,int total)
{
    int i;
    int j=0;
    
    fprintf(cgiOut,"<tr>");
    fprintf(cgiOut,"<td align=center>");
    fprintf(cgiOut,"<table border=\"1\">"); 
    
    for(i=start;i<end+1;i++)
    {
    	if ((j%4)==0)
    	{
    	  fprintf(cgiOut,"<tr>");
    	  fprintf(cgiOut,"<td>");    	
    	}
    	j++;
        
        fprintf(cgiOut, "<img src=\"/sd/%s\" width=\"160\" height=\"120\" />",g_img[i].name);
    
        if ((j%4)==0)
    	{
    	  fprintf(cgiOut,"<tr>");
    	  fprintf(cgiOut,"<td>");  	
    	}	
    }
    fprintf(cgiOut,"</table>"); 
    fprintf(cgiOut, "<p class=\"little\">-- ��%dҳ ��%dҳ --</p>", start/16 + 1, total%16 ? total/16+1 : total/16); 
    
    fprintf(cgiOut,"</td>");
    fprintf(cgiOut,"</tr>");

}


void show_select_form(int total)
{
	int i, pgn;
	
	pgn = total/16;
	
	if (total % 16)
		pgn++;			
	
	fprintf(cgiOut, "<tr>");	
	fprintf(cgiOut, "<td align=center>");
	
	fprintf(cgiOut, "<br><form>");			
	fprintf(cgiOut, "ת����");	
	fprintf(cgiOut, "<select name=\"selectpage\">");	
	for (i = 0; i < pgn; i++) 
		fprintf(cgiOut, "<option value=\"opt%d\">%d</option>", i, i+1);

	fprintf(cgiOut, "</select>");		
	fprintf(cgiOut, "ҳ&emsp;");	
	fprintf(cgiOut, "<input type=\"submit\" name=\"gopage\" value=\"go\"/>");
	fprintf(cgiOut, "<form>");
	
	fprintf(cgiOut, "</td>");
	fprintf(cgiOut, "</tr>");	
} 

int cgiMain()
{
    int fd;
    int led_control,led_state;
    char *data;
    int start,end,total;
    start = 0;
    end = 15;

    
    //��ͷ
    print_file(cgiOut,"../top.html");
    
    //��ͼƬ������ʾ�б�
    total=list_pic("/mnt/sd","jpg");
    
    //�����û���ѡ������
    	if (cgiFormSubmitClicked("gopage") == cgiFormSuccess) {
			
		int i, sel;
		char **optlist;
		char tmp[10];
		
		optlist = (char**)malloc(sizeof(char*) * total);
		
		for (i = 0; i < total; i++) {
			sprintf(tmp, "opt%d", i);
			optlist[i] = strdup(tmp);
		}
		
		cgiFormSelectSingle("selectpage", optlist, total, &sel, 0);

		start = sel * 16;
		end = start + 15;
		end = total < end ? total : end;
		//fprintf(cgiOut, "<p>s=%d, e=%d</p>", start, end);	
				
		free(optlist);
	} 
    
    //��ʾͼƬ
    show_pic(start,end,total);
   
    show_select_form(total);
    
    //��β��
    print_file(cgiOut,"../bottom.html");
    
    return 0;
}