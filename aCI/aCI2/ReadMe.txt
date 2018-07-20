

- This makes use of wcrt.lib instead of the standard C library reducing the dll size ....

- The Developer mode will build a aCI2.dll with a seperate console window 
  The Non Developer version will not include any used DEBUGprintf strings and will again be smaller

- seperate thread for scanning memory locations and watching framehook activity

- all hooks set are automatically added to the scanning engine

- doesn't use a linked libnp, instead at runtime it does a getprocaddress for the NP_ function

- CI messages are only sent once every 30 seconds, to prevent flooding the npserver

- Code which isn't used will be randomized, keeping as little code intact as possible


Always encrypt your string......use the string crypting feature !!


In order to use the Developer version, take the special build libnp.dll version  (SEE  aCI.cpp in libnp)
This version checks when it receives the current aCI version, if there is a local aCI2.dll in the MWx directory
and will load that instead.
This way you can test new versions..
