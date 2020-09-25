#define FUSE_USE_VERSION 30

#define _FILE_OFFSET_BITS  64
#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

char directories[100][256];
int dirCount = 0;

int ownerId;
int ownerGouprId;


static int is_dir(char* path)
{
	path++;

	printf("Checking if dir %s\n dircount =  %d\n", path,dirCount);

  for(int i = 0; i < dirCount;i++){
  	printf("i =  %d\n", i);
    if(strcmp(directories[i],path) == 0){
    	printf("Is dir!\n");
       return 1;
    } 
   }
printf("Is not dir\n");
return  0;
}

static int do_getattr( const char* path, struct stat *st)
{

printf( "getattr called\n");
printf ( "\tAttributes of %s requested\n", path);

st->st_uid = ownerId;
st->st_gid = ownerGouprId;
st->st_atime = time(NULL);
st->st_mtime = time(NULL);

if(strcmp("/",path) == 0 || is_dir(path) == 1){
st->st_nlink = 2;

if(strcmp("bar",path) == 0){

st->st_mode = S_IFDIR | 0705;

}
else if(strcmp("bar/bin",path) == 0){
st->st_mode = S_IFDIR | 0700;

}
else if(strcmp("bar/baz",path) == 0){

st->st_mode = S_IFDIR | 0644;

}
else if(strcmp("foo",path) == 0){ 

st->st_mode = S_IFDIR | 0644;

}
else{
//default value
st->st_mode = S_IFDIR | 0744;
}

}
else{
st->st_nlink = 1;
st->st_size =  1024;
if(strcmp("/bar/bin/echo",path) == 0){
st->st_mode = S_IFREG | 0555;
st->st_size =  150000;

}
else if(strcmp("/bar/bin/readme.txt",path) == 0){ 
st->st_mode = S_IFREG | 0400;
}
else  if(strcmp("/bar/baz/example",path) == 0){ 
st->st_mode = S_IFREG | 0222;
}
else
 if(strcmp("/foo/test.txt",path) == 0){ 
st->st_mode = S_IFREG | 0007;
}
// else if(strcmp("/newFile",path) == 0){
// st->st_mode = S_IFREG | 0700;
// }
else
{

//default value
	printf("Not exist!\n");
return -ENOENT;

}
printf("file found. Size = %d\n", st->st_size);
}


return 0;
}

static int do_readddir( const char* path,  void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
printf("getting files of %s \n", path);

filler(buffer, ".", NULL, 0);
filler(buffer, "..", NULL, 0);


if(strcmp("/",path) == 0){
filler(buffer, "bar", NULL, 0);
filler(buffer, "foo", NULL, 0);
// filler(buffer, "newFile", NULL, 0);

for(int i = 5; i < dirCount;i++){
	printf("Showing  dir: %s\n", directories[i]);
	filler(buffer,directories[i],NULL,0);
}

}
else
if(strcmp("/bar",path) == 0){
	printf("Got bar\n");
	if(ownerGouprId== getgid() && ownerId!=getuid()){
		return -EACCES;
	}
filler(buffer, "bin", NULL, 0);
filler(buffer, "baz", NULL, 0);

}
else
if(strcmp("/bar/bin",path) == 0){
if(ownerId!=getuid()){
	return -EACCES;
}
filler(buffer, "echo", NULL, 0);
filler(buffer, "readme.txt", NULL, 0);

}
else if(strcmp("/bar/baz",path) == 0){
	return -EACCES;

filler(buffer, "example", NULL, 0);

}
else if(strcmp("/foo",path) == 0){ 

filler(buffer, "test.txt", NULL, 0);

}
printf("finished getting files of %s \n", path);


return 0;
}

static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{

printf( "--> Trying to read %s, %u, %u\n", path, offset, size );


  char* echoText = (char*)calloc(150000,sizeof(char));

  strcpy(echoText,"#!/bin/bash\necho $1");
 char readmeText[] ="student Ivan Shishlyannikov, 1823383\n";
 char testText[] = "ob\nabiding\nstupid\npainstaking\nliterate\naquatic\ndiscover\ncomplex\nterrify\naction\nwonder\nsour\nearn\nrustic\nwriter\nencouraging\nenthusiastic\nknowing\ntidy\nplay\nstone\ndrain\ninvincible\nstep\nunite\ndust\nsteadfast\nfilthy\nnine\nsearch\nludicrous\nbone\nwatch\ntesty\nefficient\nunnatural\nruddy\ntime\nlocket\nscatter\ndeafening\nstaking\nnaughty\nbelief\nbuilding\ndeadpan\ngrip\nzephyr\nbabies\nflavor\nlist\ndriving\ntrashy\ntawdry\njump\npage\nquack\nstupendous\ntail\nhandle\nberry\ncast\nworried\npsychotic\ncrayon\ninteresting\ncracker\nmelted\ncreepy\ncurl\nlacking\nadd\nlicense\nhumorous\nmeasly\nreign\ndigestion\nquick\ndecide\nneat\nshoes\ncamera\nplease";



char exampleText[] = "Hello world! Student Ivan Shishlyannikov group 13, task 0\n";
char *textToReturn = NULL;

if(strcmp("/bar/bin/echo",path) == 0)
{

textToReturn=echoText;

}
else if(strcmp("/bar/bin/readme.txt",path) == 0){

	if(getuid()!=ownerId){
		return -EACCES;
	}
textToReturn=readmeText;
}
else if(strcmp("/bar/baz/example",path) == 0){
	return -EACCES;
textToReturn=exampleText;
}
else if(strcmp("/foo/test.txt",path) == 0){

	if(getuid() == ownerId || getgid() == ownerGouprId){
		return -EACCES;
	}
	textToReturn=testText;
}
else{
	return -1;
}

// if(strcmp("/newFile",path) == 0){
// 	textToReturn = testText;
// }
// else{
// 	return -1;
// }

memcpy( buffer,textToReturn +offset,size);

// printf("return text: %s\n", textToReturn);

return strlen(textToReturn) - offset;

}

static int addDir(char* path){

	strcpy( directories[dirCount],path);
dirCount++;
}

static int do_mkdir(const char *path, mode_t mode){

path++;

if(strstr(path,"/")!=NULL){
system("echo 'folders can only be created in the root folder'");
	return -1;
	}

addDir(path);

return 0;

}

static struct  fuse_operations operations = {
	.getattr = do_getattr,
	.readdir = do_readddir,
	.read = do_read,
	.mkdir = do_mkdir,
};

int main(int argc, char *argv){

addDir("/");
addDir("bar");
addDir("foo");
addDir("bar/baz");
addDir("bar/bin");

ownerId = getuid();
ownerGouprId = getgid();

	return fuse_main(argc,argv, &operations, NULL);
}

