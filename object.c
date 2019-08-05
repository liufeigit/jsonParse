// clang -g 

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "object.h"

int __array_capacity=5;
int __dict_capacity=5;
int __hash_capacity=1000;

static int __arr_getIndex(char **arr,int length,char *value);
static unsigned long __hashValue(char *key);

Array *array_create(int *capacity){
	Array *array=NULL;
	int _capacity=__array_capacity; 

	if(capacity){
		_capacity=(*capacity>0?*capacity:_capacity);
	}

	array=(Array *)malloc(1*sizeof(Array ));
	array->capacity=_capacity;
	array->length=0;
	array->dataArr=(Object **)malloc(_capacity*sizeof(Object *));

	return array;
}

void array_set(Array *array,Object *value){
	Object **dataArr=NULL;
	int length=0;
	int capacity=0;
	int _capacity=__array_capacity; 

	dataArr=array->dataArr;
	length=array->length;
	capacity=array->capacity;

	if(length>=capacity){
		capacity+=_capacity;
		dataArr=(Object **)realloc(dataArr, capacity*sizeof(Object *));

		array->dataArr=dataArr;
		array->capacity=capacity;
	}
	
	value->_referNum++;
	dataArr[length]=value;
	length++;

	array->length=length;
}

Object *array_get(Array *array,int index){
	Object *obj=NULL;

	if(index<array->length){
		obj=array->dataArr[index];
	}

	return obj;
}

int array_length(Array *array){
	int length=0;

	length=array->length;
	return length;
}

void array_free(Array *array){
	int length=0;
	Object **dataArr=NULL;

	length=array->length;
	dataArr=array->dataArr;
	for(int i=0;i<length;i++){
		object_free(dataArr[i]);
	}

	free(dataArr);
	free(array);
}

char *array_toString(Array *array){
	char *str=NULL;
	int _cap=100;
	int len=0;

	Object **dataArr=NULL;
	int length=0;

	if(!array){
		return NULL;
	}

	str=(char *)malloc(_cap*sizeof(char ));
	length=array->length;
	dataArr=array->dataArr;

	str[len]='[';
	len++;

	for(int i=0;i<length;i++){
		char *_str=NULL;

		_str=object_toString(dataArr[i]);
		if(_str){
			int _len=0;

			_len=strlen(_str);
			if(len+_len+5>_cap){
				_cap=len+_len+5+100;
				str=(char *)realloc(str, _cap*sizeof(char ));
			}
			strncpy(str+len, _str, _len);
			len+=_len;

			if(i<length-1){
				str[len]=',';
				len++;
			}
			
			free(_str);
		}
	}

	str[len]=']';
	str[len+1]='\0';

	return str;
}

Dict *dict_create(int *capacity){
	Dict *dict=NULL;
	int _capacity=__dict_capacity; 

	if(capacity){
		_capacity=(*capacity>0?*capacity:_capacity);
	}

	dict=(Dict *)malloc(1*sizeof(Dict ));
	dict->capacity=_capacity;
	dict->length=0;

	dict->keyArr=(char **)malloc(_capacity*sizeof(char *));
	dict->valueArr=(Object **)malloc(_capacity*sizeof(Object *));

	return dict;
}

void dict_set(Dict *dict,char *key,Object *value){
	Object **valueArr=NULL;
	char **keyArr=NULL;
	int length=0;
	int capacity=0;
	int _capacity=__dict_capacity; 

	valueArr=dict->valueArr;
	keyArr=dict->keyArr;
	length=dict->length;
	capacity=dict->capacity;

	if(length>=capacity){
		capacity+=_capacity;
		keyArr=(char **)realloc(keyArr, capacity*sizeof(char *));
		valueArr=(Object **)realloc(valueArr, capacity*sizeof(Object *));

		dict->keyArr=keyArr;
		dict->valueArr=valueArr;
		dict->capacity=capacity;
	}
	
	value->_referNum++;

	keyArr[length]=key;
	valueArr[length]=value;
	length++;

	dict->length=length;
}

Object *dict_get(Dict *dict,char *key){
	Object *obj=NULL;
	int length=0;
	int index=-1;

	length=dict->length;
	index=__arr_getIndex(dict->keyArr, length, key);
	if(index!=-1){
		obj=dict->valueArr[index];
	}

	return obj;
}

int dict_length(Dict *dict){
	int length=0;

	length=dict->length;
	return length;
}

void dict_free(Dict *dict){
	int length=0;
	char **keyArr=NULL;
	Object **valueArr=NULL;

	if(!dict){
		return;
	}

	length=dict->length;
	keyArr=dict->keyArr;
	valueArr=dict->valueArr;
	for(int i=0;i<length;i++){
		free(keyArr[i]);
		object_free(valueArr[i]);
	}
	free(keyArr);
	free(valueArr);

	free(dict);
}

char *dict_toString(Dict *dict){
	char *str=NULL;
	int _cap=100;
	int len=0;

	Object **valueArr=NULL;
	char **keyArr=NULL;
	int length=0;

	if(!dict){
		return NULL;
	}

	str=(char *)malloc(_cap*sizeof(char ));
	length=dict->length;
	keyArr=dict->keyArr;
	valueArr=dict->valueArr;

	str[len]='{';
	len++;

	for(int i=0;i<length;i++){
		char *_str1=NULL;
		char *_str2=NULL;

		_str1=keyArr[i];
		_str2=object_toString(valueArr[i]);
		if(_str2){
			int _len1=0;
			int _len2=0;

			_len1=strlen(_str1);
			_len2=strlen(_str2);
			if(len+_len1+_len2+10>_cap){
				_cap=len+_len1+_len2+10+100;
				str=(char *)realloc(str, _cap*sizeof(char ));
			}

			str[len]='\"';
			len++;
			if(_len1){
				strncpy(str+len, _str1, _len1);
				len+=_len1;
			}
			str[len]='\"';
			len++;

			str[len]=':';
			len++;

			strncpy(str+len, _str2, _len2);
			len+=_len2;

			if(i<length-1){
				str[len]=',';
				len++;
			}

			free(_str2);
		}
	}

	str[len]='}';
	str[len+1]='\0';

	return str;
}

static void hash_extend(Hash *hash,int capacity){
	int length=0;
	char **keyArr=NULL;
	Object **valueArr=NULL;
	int *lenArr=NULL;

	Hash *hash1=NULL;
	Object *obj=NULL;

	length=hash->length;
	keyArr=hash->keyArr;
	valueArr=hash->valueArr;
	lenArr=hash->lenArr;

	hash1=hash_create(&capacity);
	for(int i=0;i<length;i++){
		obj=hash_get(hash, keyArr[i]);
		hash_set(hash1, keyArr[i], obj);
	}

	for(int i=0;i<length;i++){
		free(keyArr[i]);
		object_free(valueArr[i]);
	}
	free(keyArr);
	free(valueArr);
	free(lenArr);

	hash->capacity=capacity;
	hash->keyArr=hash1->keyArr;
	hash->valueArr=hash1->valueArr;
	hash->lenArr=hash1->lenArr;

	free(hash1);
}

Hash *hash_create(int *capacity){
	Hash *hash=NULL;
	int _capacity=__hash_capacity; 

	if(capacity){
		_capacity=(*capacity>0?*capacity:_capacity);
	}

	hash=(Hash *)calloc(1, sizeof(Hash ));
	hash->capacity=_capacity;
	hash->keyArr=(char **)calloc(_capacity, sizeof(char *));
	hash->valueArr=(Object **)calloc(_capacity, sizeof(Object *));
	hash->lenArr=(int *)calloc(_capacity, sizeof(int ));

	return hash;
}

void hash_set(Hash *hash,char *key,Object *value){
	Object **valueArr=NULL;
	char **keyArr=NULL;
	int *lenArr=NULL;
	int length=0;
	int capacity=0;

	Object *obj=NULL;
	int index=0;
	unsigned long code=0;
	int _len=0;

	if(!key){
		return;
	}

	length=hash->length;
	capacity=hash->capacity;
	if(length>=3*capacity/4){
		hash_extend(hash,2*capacity);
	}

	valueArr=hash->valueArr;
	keyArr=hash->keyArr;
	lenArr=hash->lenArr;
	capacity=hash->capacity;

	code=__hashValue(key);
	index=code%capacity;

	_len=lenArr[index];
	value->_referNum++;
	if(_len==0){ // new
		keyArr[length]=key;
		valueArr[index]=value;
		valueArr[index]->_data1=key;
		lenArr[index]=1;
		length++;
	}
	else{
		int _len1=0;

		_len1=strlen(key);
		obj=valueArr[index];
		if(strncmp(obj->_data1, key, _len1)==0){ // rewrite
			object_free(valueArr[index]);
			valueArr[index]=value;
		}
		else{
			int _flag=0;

			for(int i=0;i<_len-1;i++){
				if(strncmp(((Object *)obj->_data2)->_data1,key,_len1)==0){ // rewrite
					_flag=1;
					object_free(obj->_data2);
					obj->_data2=value;
					break;
				}
				obj=obj->_data2;
			}

			if(!_flag){ // new
				keyArr[length]=key;
				obj->_data2=value;
				((Object *)obj->_data2)->_data1=key;
				lenArr[index]+=1;
				length++;
			}
		}
	}

	hash->length=length;
}

Object *hash_get(Hash *hash,char *key){
	Object *obj=NULL;
	Object *value=NULL;

	int capacity=0;
	unsigned long code=0;
	int index=0;
	int len=0;

	if(!key){
		return NULL;
	}

	capacity=hash->capacity;
	len=strlen(key);
	code=__hashValue(key);
	index=code%capacity;

	value=hash->valueArr[index];
	while(value){
		if(strncmp(value->_data1, key, len)==0){
			obj=value;
			break;
		}
		value=value->_data2;
	}

	return obj;
}

int hash_length(Hash *hash){
	int length=0;

	length=hash->length;

	return length;
}

void hash_free(Hash *hash){
	int length=0;
	char **keyArr=NULL;
	Object **valueArr=NULL;
	int *lenArr=NULL;

	if(!hash){
		return;
	}

	length=hash->length;
	keyArr=hash->keyArr;
	valueArr=hash->valueArr;
	lenArr=hash->lenArr;
	for(int i=0;i<length;i++){
		free(keyArr[i]);
		object_free(valueArr[i]);
	}
	free(keyArr);
	free(valueArr);
	free(lenArr);

	free(hash);
}

char *hash_toString(Hash *hash){
	char *str=NULL;
	int _cap=100;
	int len=0;

	char **keyArr=NULL;
	int length=0;

	if(!hash){
		return NULL;
	}

	str=(char *)calloc(_cap, sizeof(char ));
	length=hash->length;
	keyArr=hash->keyArr;

	str[len]='{';
	len++;

	for(int i=0;i<length;i++){
		char *_str1=NULL;
		char *_str2=NULL;

		int _len1=0;
		int _len2=0;

		_str1=keyArr[i];
		_str2=object_toString(hash_get(hash,keyArr[i]));
		_len1=strlen(_str1);
		_len2=strlen(_str2);
		if(_str2){
			if(len+_len1+_len2+10>_cap){
				_cap=len+_len1+_len2+10+_cap;
				str=(char *)realloc(str, _cap*sizeof(char ));
			}

			str[len]='\"';
			len++;
			if(_len1){
				strncpy(str+len, _str1, _len1);
				len+=_len1;
			}
			str[len]='\"';
			len++;

			str[len]=':';
			len++;

			strncpy(str+len, _str2, _len2);
			len+=_len2;

			if(i<length-1){
				str[len]=',';
				len++;
			}

			free(_str2);
		}
	}

	str[len]='}';

	return str;
}

Object *object_create(){
	Object *obj=NULL;

	obj=(Object *)malloc(1*sizeof(Object ));
	obj->type=0;
	obj->_nextFlag=0;
	obj->_referNum=1;

	return obj;
}

void object_free(Object *object){
	int type=0;
	void *data=NULL;

	if(!object){
		return;
	}

	if(object->_nextFlag){ // link ???
		object_free(object->_data2);
	}

	if(object->_referNum>1){
		object->_referNum--;
		return;
	}

	type=object->type;
	data=object->data;
	if(type==DataType_String||
		type==DataType_Int||
		type==DataType_Float){ // type==DataType_Number||

		free(data);
	}
	else if(type==DataType_Array){
		array_free(data);
	}
	else if(type==DataType_Dict){
		dict_free(data);
	}

	free(object);
}

char *__toTrueString(){
	char *str=NULL;

	str=(char *)calloc(10, sizeof(char ));
	strncpy(str, "true", 4);

	return str;
}

char *__toFalseString(){
	char *str=NULL;

	str=(char *)calloc(10, sizeof(char ));
	strncpy(str, "false", 5);

	return str;
}

char *__toNullString(){
	char *str=NULL;

	str=(char *)malloc(5*sizeof(char ));
	str[4]='\0';
	strncpy(str, "null", 4);

	return str;
}

char *__toNumberString(char *data){
	char *str=NULL;
	int len=0;

	len=strlen(data);
	str=(char *)malloc((len+1)*sizeof(char ));
	str[len]='\0';
	strncpy(str, data, len);

	return str;
}

char *__toIntString(int i){
	char *str=NULL;

	str=(char *)calloc(30, sizeof(char ));
	sprintf(str, "%d",i);

	return str;
}

char *__toFloatString(double d){
	char *str=NULL;

	str=(char *)calloc(30, sizeof(char ));
	gcvt(d, 30, str);

	return str;
}

char *__toStringString(char *data){
	char *str=NULL;
	int len=0;

	len=strlen(data);
	str=(char *)malloc((len+3)*sizeof(char ));

	str[0]='\"';
	if(len){
		strncpy(str+1, data, len);
	}
	str[1+len]='\"';
	str[2+len]='\0';

	return str;
}

char *object_toString(Object *object){
	char *str=NULL;

	int type=0;
	void *data=NULL;

	if(!object){
		return NULL;
	}

	type=object->type;
	data=object->data;
	if(type==DataType_True){
		str=__toTrueString();
	}
	else if(type==DataType_False){
		str=__toFalseString();
	}
	else if(type==DataType_Null){
		str=__toNullString();
	}
	else if(type==DataType_Int){
		str=__toIntString(object->value.i);
	}
	else if(type==DataType_Float){
		str=__toFloatString(object->value.d);
	}
	else if(type==DataType_String){
		str=__toStringString(data);
	}
	else if(type==DataType_Array){
		str=array_toString(data);
	}
	else if(type==DataType_Dict){
		str=dict_toString(data);
	}
	else{
		printf("fuck object_toString !!!!\n");
	}

	return str;
}

int __arr_getIndex(char **arr,int length,char *value){
	int index=-1;

	if(arr&&length&&value){
		for(int i=0;i<length;i++){
			if(strcmp(arr[i], value)==0){
				index=i;
				break;
			}
		}
	}

	return index;
}

static unsigned long __hashValue(char *key){
	unsigned long h=0;
	int len=0;

	len=strlen(key);
	for(int i=0;i<len;i++){
		h=31*h+key[i];
	}

	return h;
}






