#ifndef WATCH_DOG_H_
#define WATCH_DOG_H_

#define WATCH_DOG port4000
extern volatile ioport unsigned WATCH_DOG; 

#ifndef _NO_WATCHDOG_
#define watchdog_feed WATCH_DOG = 0;
#else
#define watchdog_feed
#endif

#endif // WATCH_DOG_H_

