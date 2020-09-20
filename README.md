# FTLight
FTLight (Faster-Than-Light) aims to be an overall, long-term software framework being conceptually unlimited regarding information structure and size

Proposed software framework is based on a specification "FTLight" that was developed since 1997 for the European Radio Astronomy Club (ERAC). Part of the features are demonstrated by an OpenSource application that was called "InfoNet". It is part of a github repository which can be cloned from the following link: 

https://github.com/WegaLink/FTLight

A top level design goal of the FTLight (Faster-Than-Light) specification was long-term stability in order to serve the special requirements of radio astronomy applications for dealing with observation data from radio sources in the sky over a long period of time spanning decades or longer time frames. As well important was avoiding redundant data items because of the usually huge amount of data from astronomical observations.

The special requirements from radio astronomy provided to a canonical form of a software specification which contains almost no redundant data while allowing to generate conceptually unlimited information structures of arbitrary size which only limitations are bandwidth respectively data storage capabilities. This could explain why software developers usually have difficulties to get into the concepts since those are very different from known specifications dealing with data structure, transport and storage.

This information was set up to help overcoming problems when starting to work under the FTLight specification for building an application. Essential parts of the specification will be shown how they work in an application for an information processing network. Those features comprise for example:

Dynamic GUI: 
Measurement charts are shown on screen that update 10 times per second while maintaining full flexibility for GUI presentation including background, text, chart grid and the run of measurement data.

Unified configuration management:
All configuration data is defined in a hierarchical way featuring a conceptually unlimited hierarchical depth. Besides descriptive elements in that information hierarchy, there are elements which are connected to program variables that the application can efficiently work with. 

Unified information transport:
All information being exchanged between program modules either locally or remotely is defined in an identical way as configuration management data.
 
Unified data serialization and storage:
Whenever configuration management or information transport data leaves the programming space for saving it to disk or for transferring it on a network connection then it will be automatically serialized according to the FTLight specification.

Unified networking 
Programming modules are connected locally as well as remotely by a tetragonal peer-to-peer structure. Unique addresses are used to address a partner module for data exchange. Inside same program space, a pointer of the top level information element will be forwarded whereas in case of a remote connection, all information will be FTLight serialized as described before.

Unified data types
The FTLight specification assigns general data types like for example text, numeric or binary to any information element. An application will benefit from a type conversion to any of the known e.g. byte, word, double word, signed and unsigned programming data types.

Unified addresses
Universally Unique Resource Identifiers (UURI) are used for addressing the top element of a hierarchiccal FTLight information structure. This makes all data items in those structures universally unique. This applies not only on Earth but also when going into space for example when exchanging data with a space probe on the Moon or on Mars.

Concurrency
This is supported by having one background process pre-defined in each program module that is derived from a base "PlugNode" module.

More...