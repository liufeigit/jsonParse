// clang 

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "parse.h"
#include "object.h"

int __parse_strLen; 

static int __arr_getIndex(char *arr,int length,char value);
static int __isHex(char c);

static int parseFilter(char *str,int cur,char *arr,int length);
static int parseFilterWs(char *str,int cur);

static int parseDigit(char *str,int cur);
static int parseDotDigit(char *str,int cur);
static int parseEDigit(char *str,int cur);

static int parseUniHex(char *str,int cur);
static int parseEscape(char *str,int cur);

// flag 0 parse 1 严格 2 array/object宽松、首次解析
int parseNumber(char *str,int cur,int *flag,Object **data);
int parseString(char *str,int cur,int *flag,Object **data,char **strData);
int parseTrue(char *str,int cur,int *flag,Object **data);
int parseFalse(char *str,int cur,int *flag,Object **data);
int parseNull(char *str,int cur,int *flag,Object **data);

int parseArray(char *str,int cur,int *flag,Object **data);
int parseObject(char *str,int cur,int *flag,Object **data);
int parseValue(char *str,int cur,int *flag,Object **data);

// 基本类型转换
static Object *convertNumber(char *str,int index,int length,int floatFlag);
static char *_convertString(char *str,int index,int length);
static Object *convertString(char *str,int index,int length);
static Object *convertTrue();
static Object *convertFalse();
static Object *convertNull();

static int conertUTF8(char *str,int index,char *str1,int index1);

int parseJSON(char *str,int *flag,Object **data){
	int offset=0;
	int cur=0;

	if(!str){
		return 0;
	}

	__parse_strLen=strlen(str);
	if(!__parse_strLen){
		return 0;
	}

	cur=parseFilterWs(str, cur);
	cur=parseValue(str, cur,flag,data);
	if(cur){
		cur=parseFilterWs(str, cur);
		if(cur==__parse_strLen){
			offset=cur;
		}
	}
	
	return offset;
}

int parseArray(char *str,int cur,int *flag,Object **data){
	int offset=0;

	int _flag=0;
	Object *obj=NULL;
	Array *arrayObj=NULL;

	if(flag){
		_flag=*flag;
	}
	if(_flag&&data){
		arrayObj=array_create(NULL);
	}

	cur++;
	cur=parseFilterWs(str, cur);
	if(cur>=__parse_strLen){
		array_free(arrayObj);
		return 0;
	}
	while(str[cur]!=']'){
		Object *valueObj=NULL;

		cur=parseValue(str, cur,flag,arrayObj?&valueObj:NULL);
		if(cur==0){
			break;
		}

		cur=parseFilterWs(str, cur);
		if(cur>=__parse_strLen){
			object_free(valueObj);
			array_free(arrayObj);
			return 0;
		}

		if(str[cur]==']'){
			if(arrayObj){
				array_set(arrayObj, valueObj);
			}

			object_free(valueObj);
			break;
		}

		if(str[cur]==','){
			cur++;
		}
		else{
			cur=0;
			object_free(valueObj);
			break;
		}

		cur=parseFilterWs(str, cur);
		if(cur>=__parse_strLen){
			object_free(valueObj);
			array_free(arrayObj);
			return 0;
		}

		// strict mode
		if(_flag<2){
			if(str[cur]==']'){
				cur=0;
				object_free(valueObj);
				break;
			}
		}

		if(arrayObj){
			array_set(arrayObj, valueObj);
		}

		object_free(valueObj);
	}

	if(cur&&str[cur]==']'){
		offset=cur+1;
	}

	if(_flag&&data){
		if(offset){
			obj=object_create();
			obj->type=DataType_Array;
			obj->data=arrayObj;
			*data=obj;
		}
		else{
			array_free(arrayObj);
		}
	}

	return offset;
}

int parseObject(char *str,int cur,int *flag,Object **data){
	int offset=0;

	int _flag=0;
	Object *obj=NULL;
	Dict *dictObj=NULL;

	if(flag){
		_flag=*flag;
	}
	if(_flag&&data){
		dictObj=dict_create(NULL);
	}

	cur++;
	cur=parseFilterWs(str, cur);
	if(cur>=__parse_strLen){
		dict_free(dictObj);
		return 0;
	}
	while(str[cur]!='}'){
		char *strObj=NULL;
		Object *valueObj=NULL;

		if(str[cur]=='\"'){
			int _flag=0;

			_flag=dictObj?1:0;
			cur=parseString(str, cur,&_flag,NULL,dictObj?&strObj:NULL);
		}
		else{
			cur=0;
		}

		if(cur==0){
			break;
		}

		cur=parseFilterWs(str, cur);
		if(cur>=__parse_strLen){
			free(strObj);
			dict_free(dictObj);
			return 0;
		}

		if(str[cur]==':'){
			cur++;
		}
		else{
			cur=0;
			free(strObj);
			break;
		}

		cur=parseFilterWs(str, cur);
		if(cur>=__parse_strLen){
			free(strObj);
			dict_free(dictObj);
			return 0;
		}

		cur=parseValue(str,cur,flag,dictObj?&valueObj:NULL);
		if(cur==0){
			free(strObj);
			break;
		}

		cur=parseFilterWs(str, cur);
		if(cur>=__parse_strLen){
			free(strObj);
			object_free(valueObj);
			dict_free(dictObj);
			return 0;
		}

		if(str[cur]=='}'){
			if(dictObj){
				dict_set(dictObj, strObj, valueObj);
			}
			else{
				free(strObj);
			}
			
			object_free(valueObj);
			break;
		}

		if(str[cur]==','){
			cur++;
		}
		else{
			cur=0;
			free(strObj);
			object_free(valueObj);
			break;
		}

		cur=parseFilterWs(str, cur);
		if(cur>=__parse_strLen){
			free(strObj);
			object_free(valueObj);
			dict_free(dictObj);
			return 0;
		}

		// strict mode
		if(_flag<2){
			if(str[cur]=='}'){
				cur=0;
				free(strObj);
				object_free(valueObj);
				break;
			}
		}
		
		if(dictObj){
			dict_set(dictObj, strObj, valueObj);
		}
		else{
			free(strObj);
		}
		
		object_free(valueObj);
	}

	if(cur&&str[cur]=='}'){
		offset=cur+1;
	}

	if(_flag&&data){
		if(offset){
			obj=object_create();
			obj->type=DataType_Dict;
			obj->data=dictObj;
			*data=obj;
		}
		else{
			dict_free(dictObj);
		}
	}

	return offset;
}

int parseValue(char *str,int cur,int *flag,Object **data){
	int offset=0;

	if(cur>=__parse_strLen){
		return 0;
	}

	if(str[cur]=='\"'){
		cur=parseString(str,cur,flag,data,NULL);
	}
	else if(str[cur]=='-'||(str[cur]>='0'&&str[cur]<='9')){
		cur=parseNumber(str, cur,flag,data);
	}
	else if(str[cur]=='{'){
		cur=parseObject(str, cur,flag,data);
	}
	else if(str[cur]=='['){
		cur=parseArray(str, cur,flag,data);
	}
	else if(str[cur]=='t'){
		cur=parseTrue(str, cur,flag,data);
	}
	else if(str[cur]=='f'){
		cur=parseFalse(str, cur,flag,data);
	}
	else if(str[cur]=='n'){
		cur=parseNull(str, cur,flag,data);
	}
	else {
		cur=0;
	}

	offset=cur;
	return offset;
}

int parseTrue(char *str,int cur,int *flag,Object **data){
	int offset=0;

	int _flag=0;
	Object *obj=NULL;

	if(flag){
		_flag=*flag;
	}

	if(cur+3>=__parse_strLen){
		return 0;
	}

	if(strncmp("true", str+cur, 4)==0){
		offset=cur+4;
	}

	if(_flag&&data&&offset){
		obj=convertTrue();
		*data=obj;
	}

	return offset;
}

int parseFalse(char *str,int cur,int *flag,Object **data){
	int offset=0;

	int _flag=0;
	Object *obj=NULL;

	if(flag){
		_flag=*flag;
	}

	if(cur+4>=__parse_strLen){
		return 0;
	}

	if(strncmp("false", str+cur, 5)==0){
		offset=cur+5;
	}

	if(_flag&&data&&offset){
		obj=convertFalse();
		*data=obj;
	}

	return offset;
}

int parseNull(char *str,int cur,int *flag,Object **data){
	int offset=0;

	int _flag=0;
	Object *obj=NULL;

	if(flag){
		_flag=*flag;
	}

	if(cur+3>=__parse_strLen){
		return 0;
	}

	if(strncmp("null", str+cur, 4)==0){
		offset=cur+4;
	}

	if(_flag&&data&&offset){
		obj=convertNull();
		*data=obj;
	}

	return offset;
}

int parseNumber(char *str,int cur,int *flag,Object **data){
	int offset=0;

	int _flag=0;
	int _index=0;
	Object *obj=NULL;

	int floatFlag=0;

	if(flag){
		_index=cur;
		_flag=*flag;
	}

	if(str[cur]=='-'){
		cur++;

		if(cur>=__parse_strLen){ // lex error
			return 0;
		}

		if(!(str[cur]>='0'&&str[cur]<='9')){
			return offset;
		}
	}

	if(str[cur]=='0'){
		cur++;
	}
	else{ // [1-9]
		cur=parseDigit(str,cur);
	}

	if(cur>=__parse_strLen){ // right
		return cur;
	}

	if(str[cur]=='.'){
		floatFlag=1;
		cur=parseDotDigit(str,cur);
		if(cur==0){
			return offset;
		}
	}

	if(cur>=__parse_strLen){ // right
		return cur;
	}

	if(str[cur]=='E'||str[cur]=='e'){
		cur=parseEDigit(str,cur);
		if(cur==0){
			return offset;
		}
	}

	offset=cur;

	if(_flag&&data&&offset){
		obj=convertNumber(str, _index, offset-_index,floatFlag);
		*data=obj;
	}

	return offset;
}

int parseString(char *str,int cur,int *flag,Object **data,char **strData){
	int offset=0;

	int _flag=0;
	int _index=0;
	Object *obj=NULL;

	if(flag){
		_index=cur;
		_flag=*flag;
	}

	cur++;
	if(cur>=__parse_strLen){
		return 0;
	}
	while(str[cur]!='\"'){
		if(str[cur]=='\\'){
			cur=parseEscape(str,cur);
			if(cur==0){
				break;
			}
		}
		else{
			cur++;
		}

		if(cur>=__parse_strLen){
			return 0;
		}
	}

	if(cur&&str[cur]=='\"'){
		offset=cur+1;
	}

	if(_flag&&offset){
		if(data){
			obj=convertString(str, _index, offset-_index);
			*data=obj;
			
		}
		
		if(strData){
			*strData=_convertString(str, _index, offset-_index);
		}
	}

	return offset;
}

inline int parseUniHex(char *str,int cur){
	int offset=0;
	int num=0;

	cur++;
	if(cur>=__parse_strLen){
		return 0;
	}
	while(__isHex(str[cur])){
		cur++;
		num++;
		if(num==4){
			break;
		}

		if(cur>=__parse_strLen){
			return 0;
		}
	}

	if(num==4){
		offset=cur;
	}

	return offset;
}

inline int parseEscape(char *str,int cur){
	int offset=0;

	cur++;
	if(cur>=__parse_strLen){
		return 0;
	}
	if(str[cur]=='\"'||
		str[cur]=='\\'||
		str[cur]=='/'||
		str[cur]=='b'||
		str[cur]=='f'||
		str[cur]=='n'||
		str[cur]=='r'||
		str[cur]=='t'){
		cur++;
	}
	else if(str[cur]=='u'){
		cur=parseUniHex(str,cur);
	}
	else{
		cur=0;
	}

	offset=cur;
	return offset;
}

inline int parseDigit(char *str,int cur){
	int offset=0;

	cur++;
	if(cur>=__parse_strLen){
		return cur;
	}
	while(str[cur]>='0'&&str[cur]<='9'){
		cur++;
		if(cur>=__parse_strLen){
			break;
		}
	}

	offset=cur;
	return offset;
}

inline int parseDotDigit(char *str,int cur){
	int offset=0;

	cur++;
	if(cur>=__parse_strLen){
		return 0;
	}
	if(str[cur]>='0'&&str[cur]<='9'){
		offset=parseDigit(str,cur);
	}

	return offset;
}

inline int parseEDigit(char *str,int cur){
	int offset=0;

	cur++;
	if(cur>=__parse_strLen){
		return 0;
	}

	if(str[cur]=='+'||str[cur]=='-'){
		cur++;
		if(cur>=__parse_strLen){ // error
			return 0;
		}
	}

	if(str[cur]>='0'&&str[cur]<='9'){
		cur=parseDigit(str,cur);
	}
	else{
		cur=0;
	}

	offset=cur;
	return offset;
}

inline int parseFilterWs(char *str,int cur){
	int offset=0;
	int length=4;
	char arr[]={'\x09','\x0a','\x0d','\x20','\0'};

	if(cur>=__parse_strLen){
		return cur;
	}

	offset=parseFilter(str,cur,arr,length);

	return offset;
}

inline int parseFilter(char *str,int cur,char *arr,int length){
	int offset=0;
	int index=-1;

	index=__arr_getIndex(arr, length,str[cur]);
	while(index!=-1){
		cur++;
		if(cur>=__parse_strLen){
			break;
		}
		index=__arr_getIndex(arr, length,str[cur]);
	}

	offset=cur;
	return offset;
}	

inline int __arr_getIndex(char *arr,int length,char value){
	int index=-1;

	length=strlen(arr);
	for(int i=0;i<length;i++){
		if(arr[i]==value){
			index=i;
			break;
		}
	}

	return index;
}

inline int __isHex(char c){
	int status=0;

	if((c>='0'&&c<='9')||
		(c>='A'&&c<='F')||
		(c>='a'&&c<='f')){
		status=1;
	}

	return status;
}

inline Object *convertNumber(char *str,int index,int length,int floatFlag){
	Object *obj=NULL;
	char *data=NULL;

	int i=0;
	double d=0;

	obj=object_create();

	data=(char *)malloc(length+1*sizeof(char ));
	strncpy(data,str+index,length);
	data[length]='\0';
	obj->data=data;

	if(floatFlag){
		d=strtod(data,NULL);
		obj->value.d=d;
		obj->type=DataType_Float;
	}
	else{
		i=strtol(data,NULL,10);
		obj->value.i=i;
		obj->type=DataType_Int;
	}

	return obj;
}

/***
	0000~007f --> 0xxxxxxx
	0080~07ff --> 110xxxxx 10xxxxxx
	0800~ffff --> 1110xxxx 10xxxxxx 10xxxxxx
	.....
	>=2 取位存码 第一字节补0按8位掩码 其它字节移位后6位掩码取位
****/
inline int conertUTF8(char *str,int index,char *str1,int index1){
	int len=0;
	int value=0;
	int fact=1;
	char a='\0';

	for(int i=index+3;i>=index;i--){
		if(str[i]>='0'&&str[i]<='9'){
			a=str[i]-'0';
		}
		else if(str[i]>='a'&&str[i]<='f'){
			a=10+str[i]-'a';
		}
		else if(str[i]>='A'&&str[i]<='F'){
			a=10+str[i]-'A';
		}

		if(i==index+3){
			value=value+a;
		}
		else{
			fact*=16;
			value=value+a*fact;
		}
	}

	if(value>=0x0000&&value<=0x007f){
		str1[index1]=value;
		len++;
	}
	else if(value>=0x0080&&value<=0x07ff){
		str1[index1]=0xc0|((value>>6)&0xff); // 110
		str1[index1+1]=0x80|(value&0x3f); // 10
		len+=2;
	}
	else if(value>=0x0800&&value<=0xffff){
		str1[index1]=0xe0|((value>>12)&0xff); // 1110
		str1[index1+1]=0x80|((value>>6)&0x3f); // 10
		str1[index1+2]=0x80|(value&0x3f); // 10
		len+=3;
	}

	return len;
}

inline char *_convertString(char *str,int index,int length){
	char *data=NULL;
	int _length=0;

	data=(char *)malloc((length+1)*sizeof(char));
	for(int i=index+1,j=0;i<index+length-1;i++,j++){
		if(str[i]=='\\'){
			if(str[i+1]=='\"'){
				data[j]='\"';
				i++;
			}
			else if(str[i+1]=='\\'){
				data[j]='\\';
				i++;
			}
			else if(str[i+1]=='/'){
				data[j]='/';
				i++;
			}
			else if(str[i+1]=='b'){
				data[j]='\b';
				i++;
			}
			else if(str[i+1]=='f'){
				data[j]='\f';
				i++;
			}
			else if(str[i+1]=='n'){
				data[j]='\n';
				i++;
			}
			else if(str[i+1]=='r'){
				data[j]='\r';
				i++;
			}
			else if(str[i+1]=='t'){
				data[j]='\t';
				i++;
			}
			else if(str[i+1]=='u'){ 
				int _len=0;

				_len=conertUTF8(str,i+2,data,j);
				j+=(_len-1);
				i+=5;
			}
			else{ // error
				data[j]=str[i];
			}
		}
		else{
			data[j]=str[i];
		}

		_length=j;
	}
	data[_length+1]='\0';

	return data;
}

inline Object *convertString(char *str,int index,int length){
	Object *obj=NULL;
	char *data=NULL;

	obj=object_create();
	data=_convertString(str, index, length);

	obj->type=DataType_String;
	obj->data=data;

	return obj;
}

inline Object *convertTrue(){
	Object *obj=NULL;

	obj=object_create();
	obj->type=DataType_True;

	return obj;
}

inline Object *convertFalse(){
	Object *obj=NULL;

	obj=object_create();
	obj->type=DataType_False;

	return obj;
}

inline Object *convertNull(){
	Object *obj=NULL;

	obj=object_create();
	obj->type=DataType_Null;

	return obj;
}







