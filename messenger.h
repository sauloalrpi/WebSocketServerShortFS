#ifndef _messenger_t_H_
#define _messenger_t_H_

class message;

typedef std::vector<message*>            messenger_t;

typedef std::function<void()>            func_void_t;
typedef std::function<void(String&)>     func_string_t;
typedef std::function<void(message*)>    func_publish_t;

/*
static void   voidTester()    { DBG_SERIAL.println( "Void Tester"    ); DBG_SERIAL.flush(); };
static void   voidIniter()    { DBG_SERIAL.println( "Void Initer"    ); DBG_SERIAL.flush(); };
static void   voidPrinter()   { DBG_SERIAL.println( "Void Printer"   ); DBG_SERIAL.flush(); };
static void   voidPublisher() { DBG_SERIAL.println( "Void Publisher" ); DBG_SERIAL.flush(); };
static void   voidUpdater()   { DBG_SERIAL.println( "Void Updater"   ); DBG_SERIAL.flush(); };
static void   voidLooper()    { DBG_SERIAL.println( "Void Looper"    ); DBG_SERIAL.flush(); };
*/

void   msgTester(    message* ) { DBG_SERIAL.println( "Void Tester"    ); DBG_SERIAL.flush(); };
void   msgIniter(    message* ) { DBG_SERIAL.println( "Void Initer"    ); DBG_SERIAL.flush(); };
void   msgPrinter(   message* ) { DBG_SERIAL.println( "Void Printer"   ); DBG_SERIAL.flush(); };
void   msgPublisher( message* ) { DBG_SERIAL.println( "Void Publisher" ); DBG_SERIAL.flush(); };
void   msgUpdater(   message* ) { DBG_SERIAL.println( "Void Updater"   ); DBG_SERIAL.flush(); };
void   msgLooper(    message* ) { DBG_SERIAL.println( "Void Looper"    ); DBG_SERIAL.flush(); };


struct message_funcs_t {
    func_publish_t tester    = msgTester;
    func_publish_t initer    = msgIniter;
    func_publish_t printer   = msgPrinter;
    func_publish_t publisher = msgPublisher;
    func_publish_t updater   = msgUpdater;
    func_publish_t looper    = msgLooper;
};

class message {
  private:
    uint32_t           last_update  =     0;
    uint32_t           last_loop    =     0;
    uint32_t           num_updates  =     0;
    String             nickname     =    "";
    bool               initted      = false;
        
    bool               updated      = false;
    int32_t            update_every =    -1;
    int32_t            loop_every   =    -1;
    String             c_message    =    "";

    message_funcs_t    funcs;
  public:
    message(){}
    message(String n, int32_t u, int32_t l, message_funcs_t& dfuncs, bool run_init=false): nickname(n), update_every(u), loop_every(l), funcs(dfuncs) { 
      if ( run_init ) {
        init();
      }
    }

    void     set_message(      String&         n_message   ) { this->init();        this->c_message    = n_message.c_str(); this->updated = true ; this->num_updates += 1;  }
    void     pop_message(      String&         o_message   ) { this->init();              o_message    = c_message.c_str(); this->updated = false; this->c_message    = ""; }
    void     get_message(      String&         o_message   ) { this->init();              o_message    = c_message.c_str();                                                 }
    void     set_update_every( int32_t u                   ) { this->init();        this->update_every = u; }
    void     set_loop_every(   int32_t l                   ) { this->init();        this->loop_every   = l; }
    bool     is_updated()                                    { this->init(); return this->updated;          }
    uint32_t get_num_updates()                               { this->init(); return this->num_updates;      }

    void     test()                                          { this->init(); funcs.tester(    this ); }
    void     init(bool force = false)                        { 
      if (( ! this->initted ) || ( force )){
        DBG_SERIAL.print   ( "Initializing Messenger: " );
        DBG_SERIAL.println ( this->repr() );
  
        funcs.initer(    this ); 
        this->last_loop   = millis(); 
        this->last_update = millis(); 
        this->initted     = true;
      }
    }
    void     print()                                         { this->init(); funcs.printer(   this ); }
    void     publish()                                       { this->init(); funcs.publisher( this ); }
    void     loop()                                          {
      this->init(); 
      if ( this->loop_every >= 0 ) {
        if ( (millis() - this->last_loop) >= this->loop_every ) {
          this->funcs.looper( this );
          this->last_loop = millis();
          
          //DBG_SERIAL.println ( this->repr() );
        }
      }
    }
    void     update()                                        {
      this->init();
      if ( this->update_every >= 0 ) {
        if ( ! this->updated ) {
          if ( (millis() - this->last_update) >= this->update_every ) {
            this->funcs.updater(   this );
            
            if ( this->is_updated() ) {
              this->print();
              this->publish();
              DBG_SERIAL.println ( this->repr() );
            }
            
            this->last_update = millis();
          }
        }
      }
    }
    void     go()                                            {
      //test();
      loop();
      update();
    }
    String   repr()                                          {
      String  msg;
      msg += "Name: "   + nickname     +
      " Updated: "      + updated      +
      " Num Updates: "  + num_updates  +
      " Update Every: " + update_every +
      " Loop Every: "   + loop_every   +
      " Now: "          + millis()     +
      " Last Updated: " + last_update  +
      " (" + (millis()  - last_update) + ") " +
      " Last Looped: "  + last_loop    +
      " (" + (millis()  - last_loop  ) + ") " +
      " Message: "      + c_message;
      return msg;
    }
};



messenger_t messages;


#endif //ifndef _messenger_t_H_
