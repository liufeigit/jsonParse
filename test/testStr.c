// clang -O ../object.c ../parse.c testStr.c -o testStr

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "../parse.h"
#include "../object.h"

void testFile(char *fileName);
void testStr();

int main(int argc,char **argv){
	char *fileName="citm_catalog.json";

	testFile(fileName);
	// testStr();

	return 0;
}



void testFile(char *fileName){
	FILE *file=fopen(fileName, "r");

	char str[128];
	char *str1=NULL;
	int len2=0;

	str1=(char *)calloc(7*1024*1024, sizeof(int ));
	while(!feof(file)) {
        if(fgets(str,128,file)!=NULL)
        
    	strncpy(str1+len2, str, strlen(str));
    	len2+=strlen(str);
        // printf("%s\n",str);
    }

    Object *obj=NULL;
	int flag=1;
	int status=0;

	char *str2=NULL;

	struct timeval t1,t2;

	gettimeofday(&t1, NULL);

	status=parseJSON(str1, &flag, &obj);

	str2=object_toString(obj);

	printf("str2 ==>%s\nstatus==>%d\n",str2,status);
	free(str2);
	object_free(obj);

	gettimeofday(&t2, NULL);

	printf("cost time is %ld\n",(t2.tv_sec-t1.tv_sec)*1000+(t2.tv_usec-t1.tv_usec)/1000);
	
	free(str1);

    fclose(file);
}

void testStr(){
	char *str1="{\"arr\":[1 ,\n \r  \"value\",-1.23e1,[{ \"sub\":1 }]],\"name\":\"希望666\"}";
	char *str2=NULL;

	Object *obj=NULL;
	int flag=1;
	int status=0;

	status=parseJSON(str1, &flag, &obj);
	str2=object_toString(obj);

	printf("str2 ==>%s \nstatus==>%d\n",str2,status);

	free(str2);
	object_free(obj);
	
}













