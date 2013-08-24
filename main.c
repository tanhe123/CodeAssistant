//大注释问题

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FILENAME_BLANK "out.txt"
#define MAXSIZE 10000
#define INDENTATION 4


char *filename;

int file_exist(char *filename);	   /* 检测文件是否存在 */
void remove_blank(char *filename); /* 取出空格、TAB、空行 **/
void put_blank(char *buf, int *pos, int ind);
int instr(char *str, char ch);
void ferr(char *str);		/* 错误输出函数 */
void remove_blank_line(char **p);

int main(int argc, char *argv[])
{
    if(argc != 2)  		/*  检测参数 */
    {
        ferr("缺少源文件!\n");
        exit(1);
    }

    filename = argv[1];		/* 获取源文件名 */

    if( !file_exist(filename) )   /* 判断文件是否存在 */
    {
        ferr("文件不存在!\n");
        exit(1);
    }

    remove_blank(filename);	/* 除去多余空格 、TAB、空行*/
    exit(0);
}

int file_exist(char *filename)
{
    FILE *fp = fopen(filename, "r");
    fclose(fp);
    if(fp) return 1;
    else return 0;
}

void remove_blank(char *filename)
{
    FILE *ifp, *ofp;
    char *p;
    short flag;		/* 原样输出模式 */
    int ind = 0, tem_ind = 0;
    char s[MAXSIZE], buf[MAXSIZE];
    int buf_t, new_line;

    if((ifp = fopen(filename, "r")) == NULL)
    {
        ferr("打开文件失败\n");
        exit(1);
    }

    if((ofp = fopen(FILENAME_BLANK, "w")) == NULL)
    {
        ferr("打开中转文件失败\n");
        exit(1);
    }

    while(fgets(s, MAXSIZE-1, ifp) != NULL)
    {
        p = s;
        buf_t = 0;
        while(*p == ' ' || *p == '\t') p++;	/* 去掉开头多余空格、TAB*/

        /* 检测第一个字符  */
        if(*p == '#' || (*p == '/' && *(p+1) == '/')) //行注释
            flag = 1;
	else if(*p == '/' && *(p+1) == '*') /* 大范围注释 */
	    flag = 2;
        else
            flag = 0;

        new_line = 1;

        while(*p)
        {
            while(!flag && *p == ' ' && (new_line || (*(p+1) == ' ' || *(p+1) == '\t')))
                p++;

	    if(*p == '*' && *(p+1) == '/') { /* 注释取消 */
		buf[buf_t++] = '*';
		buf[buf_t++] = '/';
		new_line = 1;
		p += 2;
		continue;
	    }

	    if(flag == 2) {	/* 大注释,原样输出 */
		buf[buf_t++] = *p++;
		continue;
	    }
		
            if(*p == '{')
            {
                tem_ind = 0;

                while(buf[buf_t-1] == ' ')
                {
                    buf_t--;
                }

                if(!new_line)
                    buf[buf_t++] = '\n';

                put_blank(buf, &buf_t, ind);

                buf[buf_t++] = *p++;
                buf[buf_t++] = '\n';
                ind++;		/* 要放在put_blank前 */
                new_line = 1;
                remove_blank_line(&p);
            }
            else if(*p == '}')
            {
                tem_ind = 0;
                ind--;
                if(!new_line)
                    buf[buf_t++] = '\n';
                while(buf[buf_t-1] == ' ')
                {
                    buf_t--;
                }
                put_blank(buf, &buf_t, ind);
                buf[buf_t++] = *p++;
                buf[buf_t++] = '\n';
                new_line = 1;
                remove_blank_line(&p);
            }
            else if(*p == '\n' ||
                    (*p == ';' && ((*(p+1) == '\n' || (*(p+1) == ' ' && *(p+2) == ' ')
                    || *(p+1) == '\t') || strstr(s, "for") == NULL)))
            {
                if(*p == '\n')
                {
                    buf[buf_t++] = *p++;

                    remove_blank_line(&p);

                    if(!flag && !new_line)
                    {
                        if(tem_ind == 0)
                            tem_ind = ind+1;
                        else tem_ind++;
                    }

                    new_line = 1;
                }
                else if(*p == ';')
                {
                    buf[buf_t++] = *p++;
                    buf[buf_t++] = '\n';
                    new_line = 1;
                    remove_blank_line(&p);
                    tem_ind = 0;
                }
            }
            else
            {
                if(new_line)
                {
                    if(tem_ind == 0)
                        put_blank(buf, &buf_t, ind);
                    else
                        put_blank(buf, &buf_t, tem_ind);
                }

                buf[buf_t++] = *p++;
                new_line = 0;
            }
        }

        /* 把多余的空格去掉 */
        while(buf[buf_t-1] == ' ')
        {
            buf_t--;
        }

        buf[buf_t] = '\0';

        printf("%s", buf);

        fprintf(ofp, "%s", buf);
    }

    fclose(ifp);
    fclose(ofp);
}

void ferr(char *str)
{
    fprintf(stderr, "%s", str);
}

int instr(char *str, char ch)
{
    if(strchr(str, ch) != NULL) return 1;
    else return 0;
}

void put_blank(char *buf, int *pos, int ind)
{
    int i;

    for(i = 0; i < ind*INDENTATION; i++)
        buf[(*pos)++] = ' ';
}

void remove_blank_line(char **p)
{
    while(**p == ' ' || **p == '\t' || **p == '\n') (*p)++;
}
