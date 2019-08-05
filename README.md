# jsonParse
A simple and fast JSON parser in ANSI C.

* [Overview](#overview)
* [Build](#build)
	* [copying the source](#copying-the-source)
	* [compile](#compile)
* [Usage](#usage)
   * [Include jsonParse](#include-jsonParse)
   * [Parse JSON ](#parse-JSON )
   * [Print JSON](#print-JSON)
	* [Free object](#free-object)
	* [Data Structure](#data-Structure)
* [Example](#example)
* [License](#license)
* [Enjoy jsonParse](#enjoy-jsonParse)


### Overview
--
`jsonParse` implements a reference counting object model that  easily construct JSON objects in C.

`jsonParse` is faster than [cJSON](https://github.com/DaveGamble/cJSON) and [json-c](https://github.com/json-c/json-c) as implements in C.

`jsonParse` is simple ,only 1600+ lines code.

### Build 
--
#####copying the source
You can copy source `parse.c` and `object.c`,header `parse.h` and `object.h` to your projects source and start using it.

`jsonParse` is written in ANSI C in order to support as many platforms and compilers as possible.

##### compile
Use clang or gcc compile share or static library.

for clang example

```sh
$ clang -O parse.c object.c -fPIC -shared -o libjsonParse.so
```

```sh
$ clang -c parse.c object.c
$ ar -r libjsonParse.a parse.o object.o
```

### Usage
--
##### Include jsonParse
```c
#include <parse.h>
```
##### Parse JSON 
```c
int parseJSON(char *str,int *flag,Object **data);
```
> `str`: 
> is JSON data.
> 
> `flag` :
> `NULL` or `0`, is only parse;
> `1`, create C JSON objects ;
> `2`, as 1 but parse ignore the redundant semicolons of arrays and dicts.
> 
> `data`:
>  out  C JSON objects.
> 
> `return`:
> `0`,is error;
> `str length` , success.
> 
> 

##### Print JSON
```c
char *object_toString(Object *object);
```
##### Free object
```c
void object_free(Object *object);
```


##### Data Structure
`jsonParse` represents JSON data using the `object` struct :

```c
typedef struct {
	DataType type;
	char _nextFlag;
	int _referNum;

	union {
		int i;
		double d;
	} value;

	void *data;
	void *_data1; 
	void *_data2; 
} Object;
```

JSON data type:

```c
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
```
An item of this type represents a JSON value. 

`true/false/null` is `object struct ` **type** field determine

`Int/Float` use `object struct ` **value** field.

`String` use  `object struct ` **data** field as **char pointer**.

`Array/Dict` use  `object struct ` **data** field as **Array/Dict struct**.

### Example
simple data as

```c
	char *str="{\"arr\":[1,\n \r  \"value\",-1.23e1,[{ \"sub\":1 }] ],\"name\":\"希望666\"}";

```
usage as

```c
	char *printStr=NULL;

	Object *obj=NULL;
	int flag=1;
	int status=0;
	
	// parse
	status=parseJSON(str, &flag, &obj);
	
	// print
	printStr=object_toString(obj);
	
	if(status){
		printf("printStr ==>%s\n", printStr);
	}
	
	// free 
	free(printStr);
	object_free(obj);

```


test demo

`testStr.c ` is demo example, usage **parse/print/free**

### License
MIT License
> Copyright (c) 2019 liufei and jsonParse contributors
>
>  Permission is hereby granted, free of charge, to any person obtaining a copy
>  of this software and associated documentation files (the "Software"), to deal
>  in the Software without restriction, including without limitation the rights
>  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
>  copies of the Software, and to permit persons to whom the Software is
>  furnished to do so, subject to the following conditions:
>
>  The above copyright notice and this permission notice shall be included in
>  all copies or substantial portions of the Software.
>
>  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
>  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
>  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
>  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
>  THE SOFTWARE.


### Enjoy jsonParse
  It's not perfect, it's just the beginning.

  Welcome to contribute to improve the code.


-  **Although it is cross-platform, it lacks rigorous multi-platform testing**.
- **The library itself currently supports reading and parsing related operations, but writing related operations are not yet perfect**.




