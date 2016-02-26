#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <dirent.h>
#include <unistd.h>

#include <cgic.h>

#define MAX_FILE_NO  20000

struct img_list_t {
	char   name[30];
	time_t mtime;
} g_avi[MAX_FILE_NO];

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

int list_avi(const char *path, const char *sfx)
{
    DIR *dir;  
    struct dirent *ptr; 
    int i = 0;

	
        dir =opendir(path);  
	while((ptr=readdir(dir))!=NULL) {
		if (0 == cmp_sfc(ptr->d_name, sfx)) {
			strcpy(g_avi[i].name, ptr->d_name);
			i++;
		}
	} 	
	
	return i;
}
void show_select_form(int total)
{
	int i, pgn;
	
	pgn = total;
				
	fprintf(cgiOut, "<tr>");	
	fprintf(cgiOut, "<td align=center>");
	
	fprintf(cgiOut, "<br><form>");			
	fprintf(cgiOut, "转到第");	
	fprintf(cgiOut, "<select name=\"selectpage\">");	
	for (i = 0; i < pgn; i++) 
		fprintf(cgiOut, "<option value=\"opt%d\">%d</option>", i, i+1);

	fprintf(cgiOut, "</select>");		
	fprintf(cgiOut, "页&emsp;");	
	fprintf(cgiOut, "<input type=\"submit\" name=\"gopage\" value=\"go\"/>");
	fprintf(cgiOut, "<form>");
	
	fprintf(cgiOut, "</td>");
	fprintf(cgiOut, "</tr>");	
} 

void print_file(FILE *dst_fp, const char *name)
{
	FILE *src_fp;
	char line[1024];	
	
	src_fp=fopen(name, "r");
		
	while (NULL != fgets(line, 1024, src_fp)) 
		fputs(line, dst_fp);
	
	fclose(src_fp);		
}
void show_movie(int nr, int total) 
{	
	int i, j = 0;
	
	if (nr > total - 1)
		return;
		
	fprintf(cgiOut, "<tr>");	
	fprintf(cgiOut, "<td align=center>");	
	
	fprintf(cgiOut, "<embed src=\"/sd/%s\" autostart=true loop=true width=\"640\" height=\"480\">", g_avi[nr].name);
	
	fprintf(cgiOut, "<p class=\"little\">-- 第%d页 共%d页 --</p>", 
	                nr+1, total);
	
	fprintf(cgiOut, "</td>");
	fprintf(cgiOut, "</tr>");
}

int cgiMain() 
{
	int total, nr;
	
	nr = 0;
		
	print_file(cgiOut, "../top.html");
	
	total = list_avi("/mnt/sd","avi");
	
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

		nr = sel;
		//fprintf(cgiOut, "<p>s=%d, e=%d</p>", start, end);	
				
		free(optlist);
	} 
	
	show_movie(nr, total);
	
	show_select_form(total);

	print_file(cgiOut, "../bottom.html");
	
	return 0;
}




