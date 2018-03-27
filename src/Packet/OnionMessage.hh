#ifndef __ONION_MESSAGE__
#define __ONION_MESSAGE_
namespace message {
    class MsgBody
    {
        private:
            string body;
            enum { ONION, MSG, IMG } type;
        public:
            MsgBody(string msg);
    };

}
#endif
