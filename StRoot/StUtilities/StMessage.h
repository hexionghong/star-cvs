// $Id: StMessage.h,v 1.3 1999/06/29 17:37:31 genevb Exp $
// $Log: StMessage.h,v $
// Revision 1.3  1999/06/29 17:37:31  genevb
// Lots of fixes...
//
// Revision 1.2  1999/06/24 16:30:41  genevb
// Fixed some memory leaks
//
// Revision 1.1  1999/06/23 15:17:47  genevb
// Introduction of StMessageManager
//
//
// Revision 1.1 1999/01/27 10:28:29 genevb
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StMessage                                                            //
//                                                                      //
// This is the class of messages used by StMessageManager in STAR.      //
// Messages have a type and message specified at instantiation,         //
// and also include a time-date stamp and options for printing.         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ClassStMessage
#define ClassStMessage

class TDatime;

class StMessage {

 private:

 protected:
   const char* type;
//   char* location;
//   unsigned long runNumber;
//   pair<long, long> eventId;
   char* option;
   const TDatime* messTime;
   char* message;

 public:
   StMessage(char* mess="", char* ty="I", char* opt="O");
   StMessage(const StMessage&);
   virtual ~StMessage();
   virtual           void PrintInfo();
   virtual          int Print(int nChars=0);
   virtual const TDatime* GetTime() const {return messTime;}
   virtual const  char* GetType() const {return type;}
   virtual        char* GetMessage() {return message;}
   virtual        char* GetOptions() {return option;}
   virtual           void SetOption(char* opt) {option = opt;}
#ifdef __ROOT__
   ClassDef(StMessage,1)
#endif
};

#endif
