# SuperSCAIANN
Waterloo experimental ANN Starcraft bot

## "What do I do with this?"
Open the solution is VS2013. Mess with the source files. Build the thing in Release or Debug. Release mode hides all the debugging related shenanigans (like debugdrawing for showing where units are moving to at the moment) so you probably won't use it, but it's going to be nice for seeing the end product without debugdraws slowing you down (drawing every frame is EXPENSIVE!)

## "Okay, I see the DLL outputs in Release / Debug folder. What do I do with this?"
Throw it in \{Starcraft installation}\bwapi-data\AI . You might need to make the AI folder within bwapi-data. Once you do that, launch ChaosLauncher and set:
>BWAPI {versionnumber} Injector [RELEASE]

or

>BWAPI {versionnumber} Injector [DEBUG]
Based on which mode you're intending on using.

After that, highlight the injector and go into its Config. Match the target DLL names to the one you generated, for example:

>ai     = bwapi-data/AI/SuperSCAIANN.dll

>ai_dbg = bwapi-data/AI/SuperSCAIANN.dll

Having same name for both of them is probably a bad idea (might cause a crash when a Release injector loads a Debug DLL?), but whatever.
