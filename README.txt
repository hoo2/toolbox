toolbox
=======

This is a toolbox for embedded systems. it contains
1. Automatic control system functions like PID, Fuzzy logic and TnE (trial n error) controller
2. Cryptographic functions like sha1/2 md5, des and aes.
3. Target indepentend device drivers like i2c polling, eeprom, spi polling, sd cards (spi), ptc/ntc sensors etc.
4. DSP functions like moving average leaky integrator filters
5. stdio prinf/scanf functionality to replace libc/newlib. The API is far much smaller and has floating point support.
6. semaphores/mutexes, timer/code based (non interrupt) delay, FAT32 file system.
7. TUI, with menu, combo box, text box, time box, value box API (also in demonized version).

ACS - PID
---------

This is a PID controller implementation. The API can be used for mode than one PID. Each must have a pid_c_t item.

example:
<pre>
...
pid_c_t pid;
float out, fb, sp;

pid_deinit (&pid);
pid_init (&pid, 1, 0.25, 0.15, 0.1, 0.01); 
   // P:,1 I:0.,25 D:0.15, dt=0.1sec, db=0.01
pid_sat (&pid, 1, 0);	// saturate output to 0..1

sp = 0.3;
for ( ;  ; )
{
   fb = read_input ();
   out = pid_out(&pid, sp-fb);
   //delay 0.1 sec
}
...
</pre>
The above example create a pid with P:1,I:0.25,D:0.1 and saturation to output in range [0, 1].
After that each time pid_out called with the error, a new output value is produced. The pid_out ()
must be called in periodic way, every 0.1 sec.
