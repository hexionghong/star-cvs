//:Copyright 1995, Lawrence Berkeley Laboratory
//:>--------------------------------------------------------------------
//:FILE:        socClasses.C
//:DESCRIPTION: Service and Object Catalog
//:AUTHOR:      cet - Craig E. Tull, cetull@lbl.gov
//:BUGS:        -- STILL IN DEVELOPMENT --
//:HISTORY:     21jul95-v000a-cet- creation
//:<--------------------------------------------------------------------

//:----------------------------------------------- INCLUDES           --
#include <stream.h>
#include <math.h>

#include "asuAlloc.h"
#include "asuLib.h"
#include "emlLib.h"
#include "socClasses.hh"
#include "soc_globals.h"

#define VALID_IDREF(A)  ( (0 <= A && A < count() && A < maxCount()) \
	&& ( myObjs[A] != NULL ) )

//:----------------------------------------------- PROTOTYPES         --
extern "C" char *id2name(char *base, long id);

//:#####################################################################
//:=============================================== CLASS              ==
// socObject

//:----------------------------------------------- CTORS & DTOR       --
socObject:: socObject() {
   EML_MESSAGE("Warning -- This CTOR should never be called.");
   myPtr = NULL; // This must be set in derived CTOR !!!
   myName = new string("UNNAMED");
   myType = new string("UNKNOWN");
   myLock = FALSE;
   soc->signIn(this,myIdRef);
}

//----------------------------------
socObject:: socObject(long id) {
// EML_MESSAGE("Alert -- This CTOR called only by soc.");
   myPtr = NULL; // This must be set in derived CTOR !!!
   myName = new string("soc");
   myType = new string("socCatalog");
   myLock = TRUE;
   myIdRef = id;
}

//----------------------------------
socObject:: socObject(const char* name) {
   myPtr = (SOC_PTR_T)this;
   myName = new string(name);
   myType = new string("socObject");
   myLock = FALSE;
   soc->signIn(this,myIdRef);
}

//----------------------------------
socObject:: socObject(const char* name, const char* type) {
   myPtr = NULL; // This must be set in derived CTOR !!!
   myName = new string(name);
   myType = new string(type);
   myLock = FALSE;
   soc->signIn(this,myIdRef);
}

//----------------------------------
socObject:: socObject(long n, const char* type) {
   myPtr = NULL; // This must be set in derived CTOR !!!
   	char *name = id2name((char*)type,n);
   myName = new string(name);
   	free(name);
   myType = new string(type);
   myLock = FALSE;
   soc->signIn(this,myIdRef);
}

//----------------------------------
socObject:: ~socObject() {
}

//:----------------------------------------------- ATTRIBUTES         --
IDREF_T socObject::  idRef () {
   return myIdRef;
}

//----------------------------------
char * socObject::  name () {
   char *c = (char*)ASUALLOC(strlen(myName->show())+1);
   strcpy(c,myName->show());
   return c;
}

//----------------------------------
char * socObject::  type () {
   char *c = (char*)ASUALLOC(strlen(myType->show())+1);
   strcpy(c,myType->show());
   return c;
}

//----------------------------------
char * socObject::  version () {
   char *myVersion="dev";
   char *c = (char*)ASUALLOC(strlen(myVersion)+1);
   strcpy(c,myVersion);
   return c;
}

//----------------------------------
SOC_PTR_T socObject::  ptr () {
   return myPtr;
}

//----------------------------------
char * socObject::  soRef () {
   char *c = (char*)ASUALLOC(strlen(mySOR->show())+1);
   strcpy(c,mySOR->show());
   return c;
}

//----------------------------------
void socObject:: lock (unsigned char lock) {
   if(lock){
      myLock = 1;
   }else{
      myLock = 0;
   }
}

//----------------------------------
unsigned char socObject::  lock () {
   return (myLock>0);
}

//:----------------------------------------------- PUB FUNCTIONS      --
STAFCV_T socObject:: attach () {
   myLock++;
   EML_SUCCESS(STAFCV_OK);
}

//----------------------------------
STAFCV_T socObject:: release () {
   if(myLock>0)myLock--;
   EML_SUCCESS(STAFCV_OK);
}

//:----------------------------------------------- PRIV FUNCTIONS     --
// **NONE**

//:#####################################################################
//:=============================================== CLASS              ==
// socFactory
//:----------------------------------------------- CTORS & DTOR       --
socFactory :: socFactory(long maxCount)
		: socObject() {
   myCount = 0;
   myMaxCount = maxCount;
   idRefs = new IDREF_T [myMaxCount];
}

//----------------------------------
socFactory :: socFactory(const char * name, const char * type)
		: socObject(name, type) {
   myCount = 0;
   idRefs = new IDREF_T [OBJ_MAX_COUNT];
}

//----------------------------------
socFactory :: ~socFactory() {
   delete[] idRefs;
}

//:----------------------------------------------- ATTRIBUTES         --
long socFactory :: count () {
   return myCount;
}

//----------------------------------
long socFactory :: maxCount () {
   return myMaxCount;
}

//:----------------------------------------------- PUB FUNCTIONS      --
char * socFactory :: list () {
   for( int i=0;i<maxCount();i++ ){
      printf("%d (%d)\n",i,entry(i));
   }
   return "";	// TEMPORARY HACK
}

//----------------------------------
STAFCV_T socFactory :: addEntry (IDREF_T idRef) {
   if(myCount >= myMaxCount)return FALSE;
   idRefs[myCount++] = idRef;
   EML_SUCCESS(STAFCV_OK);
}

//----------------------------------
STAFCV_T socFactory :: deleteEntry (IDREF_T idRef) {
   for( int i=0;i<myCount;i++ ){
      if( idRef == idRefs[i] ){
	 idRefs[i]=-1; /*-SOC_E_INVALID_IDREF-*/
         soc->deleteID(idRef);
	 EML_SUCCESS(STAFCV_OK);
      }
   }
   EML_ERROR(INVALID_IDREF);
}

//----------------------------------
STAFCV_T socFactory :: unaddEntry (IDREF_T idRef) {
   for( int i=0;i<myCount;i++ ){
      if( idRef == idRefs[i] ){
	 idRefs[i]=-1; /*-SOC_E_INVALID_IDREF-*/
	 EML_SUCCESS(STAFCV_OK);
      }
   }
   EML_ERROR(INVALID_IDREF);
}

//----------------------------------
IDREF_T socFactory :: entry (long n) {
   if(n<myCount && idRefs[n]>=0){
      return idRefs[n];
   }
   else{
      return -1; /*-SOC_E_INVALID_IDREF-*/
   }
}

//:----------------------------------------------- PROT FUNCTIONS     --
//:**NONE**

//:----------------------------------------------- PRIV FUNCTIONS     --
//:**NONE**

//:#####################################################################
//:=============================================== CLASS              ==
// socCatalog

//:----------------------------------------------- CTORS & DTOR       --
socCatalog:: socCatalog() 
        : socFactory(OBJ_MAX_COUNT)
	, socObject((IDREF_T)0) {
   myPtr = (SOC_PTR_T)this;
   myObjs = new socObject* [maxCount()];
   nextIDRef = FIRST_IDREF;
   IDREF_T id;
   signIn(this,id);
}

//----------------------------------
socCatalog:: ~socCatalog() {
	for( long i=count()-1; i>0; i-- ){
  		if(myObjs[i])delete myObjs[i];
	}
  	delete[] myObjs;
}

//:----------------------------------------------- ATTRIBUTES         --
char * socCatalog:: version() {
	char * myVersion="$Header: /scratch/smirnovd/star-bnl-readonly/star-cvs-local/cvs/asps/staf/soc/src/Attic/socClasses.cc,v 1.9 1997/01/21 20:35:24 tull Exp $";
	char *c=(char*)ASUALLOC(strlen(myVersion) +1);
	strcpy(c,myVersion);
	return c;
}

//:----------------------------------------------- PUB FUNCTIONS      --
STAFCV_T socCatalog:: deleteID (IDREF_T id) {
   if( !VALID_IDREF(id) ){
      EML_ERROR(INVALID_IDREF);
   }
   if(myObjs[id]->lock()){
      EML_ERROR(OBJECT_LOCKED);
   }
   delete myObjs[id];
   myObjs[id] = NULL;
   EML_SUCCESS(STAFCV_OK);
}

//----------------------------------
STAFCV_T socCatalog:: deleteObject (
		const char * name, const char * type) {
   IDREF_T id;
   idObject(name,type,id);
   if( !VALID_IDREF(id) ){
      EML_ERROR(INVALID_IDREF);
   }
   if(myObjs[id]->lock()){
      EML_ERROR(OBJECT_LOCKED);
   }
   deleteID(id);
   EML_SUCCESS(STAFCV_OK);
}

//----------------------------------
socObject* socCatalog:: findObject (
		const char * name, const char * type) {
   socObject *obj=NULL;
   IDREF_T id;
   idObject(name,type,id);
   if( !VALID_IDREF(id) ){
      obj = NULL;
      return NULL; //BUG-TOO VERBOSE      EML_ERROR(INVALID_IDREF);
   }
   obj = myObjs[id];
   return obj;
}

//----------------------------------
socObject* socCatalog:: getObject (IDREF_T id) {
   socObject *obj=NULL;
   if( !VALID_IDREF(id) ){
      obj = NULL;
      return NULL;
   }
   obj = myObjs[id];
   return obj;
}

//----------------------------------
STAFCV_T socCatalog:: idObject (const char * name
		, const char * type, IDREF_T& id) {
// THIS COULD BE IMPLEMENTED BY A HASHTABLE LOOKUP !!
   char *n, *t;
   for( int i=0; i<count(); i++ ){
      if( myObjs[i] ) {
	 if(  0 == strcmp(n=myObjs[i]->name(),name) ){
	    if( 0 == strcmp(t=myObjs[i]->type(),type) 
	    ||  0 == strcmp("-",type) ){
	       id = i;
	       if(i != myObjs[i]->idRef()){	//- HACK???
printf("MAJOR ERROR REPORT!!! (%d,%d)\n",i,id=myObjs[i]->idRef());
	       }
	       ASUFREE(n); ASUFREE(t);
	       EML_SUCCESS(STAFCV_OK);
	    }
	    ASUFREE(t);
	 }
	 ASUFREE(n);
      }
   }
   id = -1; /*- SOC_E_IDREF_NOTFOUND -*/
// EML_ERROR(OBJECT_NOT_IDED);
   return FALSE;	//- HACK: This error is too sensitive
}

//----------------------------------
//- OVER-RIDE socFactory:: list()
char * socCatalog :: list () {

// char *c, *cc;
// c = (char*)ASUALLOC(70*(count()+5));	// Best guess

   printf("\n"
"+---------------------------------------------------------------------"
   "\n"
"|************** SOC - Service & Object Catalog listing ***************"
   "\n"
"+-------+-----------------+-----------------+-------------------------"
   "\n"
"| IDREF | NAME            | TYPE            | POINTER                 "
    "\n"
"+-------+-----------------+-----------------+-------------------------"
    "\n");
   char *n,*t;
   for( int i=0;i<count();i++ ){
      if( myObjs[i] ){
	 printf("| %5d | %-15s | %-15s | %p \n"
	 		,i,n=(myObjs[i])->name(),t=(myObjs[i])->type()
			,myObjs[i]);
	 delete[] n; delete[] t;
      } else {
/*- Do not printout any info about deleted objects. -**
  	 printf("| %5d | %-15s | %-15s | - \n"
	 		,i,"**DELETED**","**DELETED**");
-*/
      }
   }
   printf(
"+-------+-----------------+-----------------+-------------------------"
   "\n\n");

   return ""; // TEMPORARY HACK
}

//----------------------------------
socObject* socCatalog:: newObject (const char * name) {
   static socObject* p;
   p = new socObject(name,"socObject");
   return p;
}

//----------------------------------
STAFCV_T socCatalog:: signIn (socObject* obj, IDREF_T& id) {
   myObjs[count()] = obj;
   id = nextIDRef++;
   addEntry(id);
   EML_SUCCESS(STAFCV_OK);
}

//----------------------------------
STAFCV_T socCatalog:: signOut (IDREF_T id) {
   if( !VALID_IDREF(id) ){
      EML_ERROR(INVALID_IDREF);
   }
   if(myObjs[id]->lock()){
      EML_ERROR(OBJECT_LOCKED);
   }
   myObjs[id] = NULL;
   EML_SUCCESS(STAFCV_OK);
}
 
//:----------------------------------------------- PRIV FUNCTIONS     --
//:**NONE**

