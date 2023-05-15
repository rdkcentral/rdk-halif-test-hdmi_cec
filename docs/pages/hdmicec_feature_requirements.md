# Snag list / Upgrade for ut-core
 
|Version|Comment|
|--------|-------|
|1.0|Initial release|
 

# Table Of Contents
 
- [Building Skeleton Library](Building-Skeleton-Library)
- [Key Value Pair](Key-Value-Pair)
- [Doxygen](Doxygen())
- [UT-Log](UT-Log)
- [C++ Support](C++-Support)
 
 See also [featureRequirements](ut-core-featureRequirements.md)

## Building Skeleton Library
 
Change the makefile, to that SKELETON_SRC is build into a library in `UT-CORE` makefiles, so no other dependancies area required.
 
It should detect if the image is already there, and not build it again, so to rebuild the library you could have to perform a clean.
 
It's upto the caller, to replace the library with the target one in question as required, this can be done by copuying the library in the lib directory, or by not copying it to the final target ,and using the one from rootfs or `LD_LIBRARY_PATH`

## Key Value Pair
 
- In UT-Core we're going to support the reading and decoding of a YML / JSON file.
- This will be read into a internal KVP database, which can be accessed by calling the UT-Core
 
E.g run a specific platform test.
 
The configuration file is passed into ut-core via the '--config' parameter
 
`./run_test --config xione.de.hdmicec_configuration.yml`
 
 
## Doxygen

- Upgrade doxygen to support mermaid from .md files into the HTML output, needs to be corrected.
                - It currently works in markdown displayed images, but not in the output for the HTML
- Check the latest version of doxygen, and check the patch level, does it support the above correctly?

## UT-Log

Enhance the support for UT-Log by under pinning the system with xfw_log. UT-Log will still exist just that the lower .c file will be a wrapper layer for xfw_log.

- Modules must have consistent logging, and a global system for ut-core to support module logging as required.
- All configuration should be driven by either switches to the command line, or incoming configuration files.

## C++ Support

Need to upgrade UT to support C++ Test

Compile Switch should switch the whole header file and makefile to download and compile the CPP Test Suite, rather than CUnit




