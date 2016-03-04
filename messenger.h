#ifndef _messenger_t_H_
#define _messenger_t_H_

class   message;

typedef std::vector<message*>            messenger_t;

typedef std::function<void()>            func_void_t;
typedef std::function<void(String&)>     func_string_t;
typedef std::function<void(message*)>    func_publish_t;


void   msgTester(    message* ) {};
void   msgIniter(    message* ) {};
void   msgUpdater(   message* ) {};
void   msgLooper(    message* ) {};
void   msgPrinter(   message* ) {};
void   msgPublisher( message* ) {};


struct message_funcs_t {
    func_publish_t tester    = msgTester;
    func_publish_t initer    = msgIniter;
    func_publish_t updater   = msgUpdater;
    func_publish_t looper    = msgLooper;
    func_publish_t printer   = msgPrinter;
    func_publish_t publisher = msgPublisher;
};

class message {
  private:
    uint32_t           last_update  =     0;
    uint32_t           last_loop    =     0;
    uint32_t           num_updates  =     0;
    String             nickname     =    "";
    bool               initted      = false;

    bool               keep_updated = false;
    bool               erase_on_pop = false;
    bool               updated      = false;
    int32_t            update_every =    -1;
    int32_t            loop_every   =    -1;
    String             c_message    =    "";

    message_funcs_t    funcs;
  public:
    message(){}
    message(String n, int32_t u, int32_t l, message_funcs_t& dfuncs, bool run_init=false, bool ku=false, bool ep=false): nickname(n), update_every(u), loop_every(l), keep_updated(ku), erase_on_pop(ep), funcs(dfuncs), c_message("") { 
      if ( run_init ) {
        this->init();
      }
    }
    void     set_message(      String&         n_message   ) { this->init();        this->c_message    = n_message.c_str(); this->updated = true ; this->num_updates += 1;  DBG_SERIAL.print( "Setting var: " ); DBG_SERIAL.print( nickname ); DBG_SERIAL.print( " val: " ); DBG_SERIAL.println( n_message ); DBG_SERIAL.flush(); }
    void     pop_message(      String&         o_message   ) { this->init();              o_message    = c_message.c_str(); this->updated = false; if (erase_on_pop) { this->c_message    = ""; }; }
    void     get_message(      String&         o_message   ) { this->init();              o_message    = c_message.c_str(); }
    void     set_update_every( int32_t ue                  ) { this->init();        this->update_every = ue;   }
    void     set_loop_every(   int32_t le                  ) { this->init();        this->loop_every   = le;   }
    void     set_keep_updated( bool    ku                  ) { this->init();        this->keep_updated = ku;   }
    void     set_erase_on_pop( bool    ep                  ) { this->init();        this->erase_on_pop = ep;   }
    void     get_nickname(     String& nk                  ) { this->init(); nk =   this->nickname.c_str();    }
    int32_t  get_update_every()                              { this->init(); return this->update_every;        }
    int32_t  get_loop_every()                                { this->init(); return this->loop_every;          }
    uint32_t get_num_updates()                               { this->init(); return this->num_updates;         }
    uint32_t get_last_update()                               { this->init(); return this->last_update;         }
    uint32_t get_last_loop()                                 { this->init(); return this->last_loop;           }
    bool     get_keep_updated()                              { this->init(); return this->keep_updated;        }
    bool     get_erase_on_pop()                              { this->init(); return this->erase_on_pop;        }
    bool     is_initted()                                    { this->init(); return this->initted;             }
    bool     is_updated()                                    { this->init(); return this->updated;             }
    void     test()                                          { this->init(); funcs.tester(    this );          }
    void     init(bool force = false)                        { 
      if (( ! this->initted ) || ( force )){
        DBG_SERIAL.print( "Initializing Messenger: "); DBG_SERIAL.println( this->nickname ); DBG_SERIAL.flush();
                
        String msg; this->repr(msg); DBG_SERIAL.println ( msg ); DBG_SERIAL.flush();

        DBG_SERIAL.print( "Calling init: "); DBG_SERIAL.println( this->nickname ); DBG_SERIAL.flush();

        this->initted     = true;        
        this->funcs.initer( this );
        this->last_loop   = millis();
        this->last_update = millis();

        DBG_SERIAL.print( "Finished initializing Messenger: " ); DBG_SERIAL.println( this->nickname ); DBG_SERIAL.flush();
      }
    }
    void     print()                                         { this->init(); funcs.printer(   this ); }
    void     publish()                                       { this->init(); funcs.publisher( this ); }
    void     loop()                                          {
//      DBG_SERIAL.print( "Looping Messenger: " ); DBG_SERIAL.println( nickname ); DBG_SERIAL.flush();
      
      this->init(); 
      
      if ( this->loop_every >= 0 ) {
        if ( (millis() - this->last_loop) >= this->loop_every ) {
          this->funcs.looper( this );
          this->last_loop = millis();
          
          //String  msg; this->repr(msg); DBG_SERIAL.println ( msg ); DBG_SERIAL.flush();
        }
      }
      
//      DBG_SERIAL.print( "Loopped Messenger: " ); DBG_SERIAL.println( nickname ); DBG_SERIAL.flush();
    }
    void     update()                                        {
//      DBG_SERIAL.print( "Updating Messenger: " ); DBG_SERIAL.println( nickname ); DBG_SERIAL.flush();
      
      this->init();
      
      if ( this->update_every >= 0 ) {
        if (( ! this->updated ) || ( this->keep_updated )) {
          if ( (millis() - this->last_update) >= this->update_every ) {
            this->funcs.updater(   this );
            
            if ( this->is_updated() ) {
              this->print();
              this->publish();
              String  msg; this->repr(msg); DBG_SERIAL.println ( msg ); DBG_SERIAL.flush(); DBG_SERIAL.flush();
            }
            
            this->last_update = millis();
          }
        }
      }
      
//      DBG_SERIAL.print( "Updated Messenger: " ); DBG_SERIAL.println( nickname ); DBG_SERIAL.flush();
    }
    void     go()                                            {
      //test();
      loop();
      update();
      delay(0);
    }
    void     repr(String  &msg)                              {
      msg += "Name: "   + this->nickname                  +
      " Updated: "      + this->updated                   +
      " Keep Updated: " + this->keep_updated              +
      " Num Updates: "  + this->num_updates               +
      " Update Every: " + this->update_every              +
      " Loop Every: "   + this->loop_every                +
      " Now: "          + millis()                        +
      " Last Updated: " + this->last_update               +
      " ("              + (millis()  - this->last_update) + ") " +
      " Last Looped: "  + this->last_loop                 +
      " ("              + (millis()  - this->last_loop  ) + ") " +
      " Message: "      + this->c_message;
    }
};



messenger_t messages;



void message_to_json( message& msg, String& res ) {
  bool     is_updated   = msg.is_updated();
  bool     is_initted   = msg.is_initted();
  bool     erase_on_pop = msg.get_erase_on_pop();
  bool     keep_updated = msg.get_keep_updated();
  uint32_t num_updates  = msg.get_num_updates();
  uint32_t last_update  = msg.get_last_update();
  uint32_t last_loop    = msg.get_last_loop();
  int32_t  loop_every   = msg.get_loop_every();
  int32_t  update_every = msg.get_update_every();

  String nickname;
  msg.get_nickname( nickname );
  
  msg.get_message( res       );

  res = String()                                                         + \
        "{"                                                              + \
        "\"_type\""          + ":" + "\"message_wrapper\""         + "," + \
        "\"_id\""            + ":" + String(millis()    )          + "," + \
        "\"data\""           + ":" + "{"                           + ""  + \
          "\"nickname\""     + ":" + "\"" + nickname + "\""        + "," + \
          "\"is_updated\""   + ":" + (is_updated  ?"true":"false") + "," + \
          "\"is_initted\""   + ":" + (is_initted  ?"true":"false") + "," + \
          "\"keep_updated\"" + ":" + (keep_updated?"true":"false") + "," + \
          "\"erase_on_pop\"" + ":" + (erase_on_pop?"true":"false") + "," + \
          "\"loop_every\""   + ":" + String(loop_every  )          + "," + \
          "\"update_every\"" + ":" + String(update_every)          + "," + \
          "\"last_loop\""    + ":" + String(last_loop   )          + "," + \
          "\"last_update\""  + ":" + String(last_update )          + "," + \
          "\"num_updates\""  + ":" + String(num_updates )          + "," + \
          "\"data\""         + ":" + (res.length()==0?"{}":res)    + ""  + \
        "}" + \
       "}";
}


#endif //ifndef _messenger_t_H_
