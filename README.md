# Cycle
A low-precision Arduino library to manage a rudimentary cycles system for automated control of individual parameters by defining a sequence of phases.

This library is not fully tested and I am not a software developer. It serves my purposes and I am glad if it serves yours too.

Every cycle object is capable of storing phases. Phases are an included subclass. Phases are primarily defined by their target value, duration, and shape. Target value is the desired value of the controlled parameter at the end of the phase. Duration is how long it takes to transition from the current value to the target value. Shape is a description of the 'order' of the change of the parameter; as in, whether the parameter should approach its target linearly, or otherwise.


As such, a cycle called *pressure_profile* could be defined and interpreted as follows:

//Over a period of 250 milliseconds, linearly transition the parameter from whatever value it may currently be to 0.
pressure_profile.add_Phase(0, 250, 1);

//Over a period of 1500 milliseconds, transition the parameter from 0 to 9 with an exponential shape.
pressure_profile.add_Phase(9, 1500, 2);

//Over a period of 3000 milliseconds, maintain a value of 9.
pressure_profile.add_Phase(9, 3000, 2);

//Over a period of 25000 milliseconds, linearly transition the parameter from 9 to 6.
pressure_profile.add_Phase(6, 25000, 1);

//Over a period of 250 milliseconds, linearly transition the parameter from 6 to 0.
pressure_profile.add_Phase(0, 250, 1);


This could be used in a piece of code with a floating point variable such as *target_pressure* so that the value stored therein corresponds to the value at the proper time within the current phase.

target_pressure = run_Cycle(target_pressure);


In the current implementation, the library assumes the time between calls to run_Cycle is significantly smaller than the duration of any single phase in the cycle. As such, it may not perform well when called in very slow loops or in loops where blocking functions such as delay() are used. As written, one phase will never transition to the next until run_Cycle is called after the completion of the previous phase. That is to say, if three phases are in place, each with a duration of 500 ms, and run_Cycle is called to begin the first phase, shortly after the end of the first phase, but then not called again until 30 minutes later, the latter phases will not be skipped; instead the second phase will begin at the next call. In this way, the library prioritizes all phases being carried out above the accuracy of their timing. This may be changed in the future.  
