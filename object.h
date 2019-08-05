#ifndef OBJECT_H
#define OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef enum {
	DataType_InValid=0,
	
	DataType_Int,
	DataType_Float,
	DataType_String,

	DataType_True,
	DataType_False,
	DataType_Null,

	DataType_Array,
	DataType_Dict

} DataType;

typedef struct {
	DataType type;
	char _nextFlag;
	int _referNum;

	union {
		int i;
		double d;
	} value;

	void *data;
	void *_data1; // key weak
	void *_data2; // next strong
} Object;

typedef struct {
	int length;
	int capacity;
	
	Object **dataArr;
} Array;

typedef struct {
	int length;
	int capacity;

	char **keyArr;
	Object **valueArr;
} Dict;

typedef struct {
	int length;
	int capacity;

	char **keyArr;
	Object **valueArr;
	int *lenArr;
} Hash;


Array *array_create(int *capacity);
void array_set(Array *array,Object *value);
Object *array_get(Array *array,int index);
int array_length(Array *array);
void array_free(Array *array);
char *array_toString(Array *array);

Dict *dict_create(int *capacity);
void dict_set(Dict *dict,char *key,Object *value);
Object *dict_get(Dict *dict,char *key);
int dict_length(Dict *dict);
void dict_free(Dict *dict);
char *dict_toString(Dict *dict);

Object *object_create();
void object_free(Object *object);
char *object_toString(Object *object);

Hash *hash_create(int *capcity);
void hash_set(Hash *dict,char *key,Object *value);
Object *hash_get(Hash *dict,char *key);
int hash_length(Hash *dict);
void hash_free(Hash *dict);
char *hash_toString(Hash *dict);


#ifdef __cplusplus
}
#endif

#endif

