# TelltaleModdingLib - Telltale Tool Modding Library
A Library which implements the Meta part of the Telltale Tool Engine. 
This is a very strict library, and requires that you understand the fundamentals.

### Library Information
This is a 64bit WINDOWS C++ library and is only meant to be built for windows. It should ideally be linked as a static library; however, if you are going to built it into your application then the header and source files are all in the same directories. Ideally you would copy the whole source into your project (note, credit must be given), and use it like that; this is also a template library for some types so this would benefit. This library should be shipped with/in your applications alongside the oo2core Oodle compression library DLL.
This library can also be compiled as an application, the CLI folder should be deleted to not compile the CLI translation unit.
Also note that Telltale Games for functions and classes and members does not use camelCase by default, most words are Capitalised. However they do use hungarian notation (a very c++ style) for all globals variables and member fields.
Please note that the games this supports are the PC versions of Telltale's games. It may well work for other platforms, but I have not tested and do not aim to. If you are using this library you are most likely wanting to mod the games, and that is best done on PC. <br> IMPORTANT: One thing also to note is that Batman: Season 1 can only serialize D3DMeshes (read and write) that are from the LATEST PC version. The library will raise an error if its not. Also only D3DMeshes in games newer than (and including) TWD: Michonne can be serialized. Games older than and including MCSM: Season 1 cannot be, at the moment.
#### Important Compiler options
Must be compiled with /bigobj on the MSVC compiler, other compilers may have a similar option to this.

### Setting up
Once you have the code included or static library added to the linker arguments then you can start by including TelltaleToolLib.h first.
This header contains all the library exported functions and functions which will should be used to perform all operations in the library (although, other headers can be included and functions in them called, but unless they are in the Types folder then they arent documented for you to be using them.

#### Fundamental structures
Telltale Games store all 'files' in a sectioned wrapping file format called a `MetaStream`. A Meta Stream is a file stream, which can be written to and read from, and has three main sections: the `header` section, `default` section, `debug` section, and `async` section.
The header section contains the section sizes in the meta stream, the meta version, and a list of type symbol CRC64s (with their version CRC32s) for all types serialized using the Meta namespace default serialization (more on that later). The default section contains the main chunk of serialized data, most data read in files is from this section. The debug section, as the name implies, contains debug data used when making games in Tool. The last section, the asynchronous section, contains data which would need to be accessed by other threads in the runtime Game Engine. This is normally data such as texture data. All of this can be found inside the Meta.hpp header, so get that included.
The following snippet of code shows you how to initialize a MetaStream for reading.
<br>
```
#include "TelltaleToolLib.h"
#include "Meta.hpp"

int MyExample() {
  //Use _OpenDataStreamFromDisc to open a file stream. The returned instance is a DataStream, a custom stream of data the engine uses. This returns a pointer,
  //but MetaStreams will consume it and delete it for you on destruction.
  MetaStream myStream;
  //The last parameter can be kept as empty, since no parameters are usually needed.
  myStream.Open(_OpenDataStreamFromDisc("C:/Users/User/Desktop/Input_File.d3dtx", READ), MetaStreamMode::eMetaStream_Read, { 0 });
  
  // Read file specific data...
  
  //This gets called by default on destructor, and essentially what this does is if the stream mode is closed, nothing. If its in read mode, returns the complete stream size,
  //and if its in write mode then this will write the data currently written to this MetaStream to the current output stream. This is why you way want to set the mode to 
  //closed before letting it get destructed.
  myStream.Close();
}
```

The Open function reads the header section. This will read every header version from any game. However, the actual data inside the meta streams in this library only supports games newer than and including the Wolf Among Us (games that use .ttarch2, an updated version of the game engine). Another thing to note is that since only games newer than TWAU are supported, only meta versions 5 & 6 can be written (and read, in terms of reading the payload data in the sections). These versions you would commonly know as 'MSV5/MSV6' or '5VSM/6VSM' in little endian. These mean 'Meta Stream Version X'.

Writing MetaStreams is easy aswell:

```
//...
//Initializes all sections, makes this meta stream ready to be written to.
myStream.Open(_OpenDataStreamFromDisc("C:/Users/User/Desktop/File_To_Write_To.font",WRITE), MetaStreamMode::eMetaStream_Write, {0});

//Write data to meta stream...

//Flush the data in the meta stream to the output stream (writes to the file).
myStream.Close();
```

#### Serializing types of data
Ok, so we can write data to MetaStreams. But what data? This is where MetaClassDescriptions come into play. A MetaClassDescription is a view of a class/struct or any intrinsic type of data, a meta view, which holds a list of the member variables in the class (names, and their offsets etc) and is used to present these types programmatically in the game engine. In each meta class description is also a linked list of meta member descriptions which describe a list of member variables in the class/struct. These contain the variable names, variable offsets in the struct layed out in memory and its flags. They also contain enum and flag descriptions, which if the member is an enum or has flags that can be set, can contain enum names and values, and flag names and values. 

Meta classes also store information about specialized meta operations. A meta operation is a function declared as `MetaOpResult MetaOperation_<funcName>(void* pObj, MetaClassDescription* pObjDescription, MetaMemberDescription* pContextDescription, void* pUserData)`. Returns a result to return if the operation succeeded. pObj is an instance of the meta class description type. pContextDescription is most of the time NULL, if not its the member this type belongs to in another type (meta members all have a meta class description member, the type of the member). 

The Meta namespace contains all default meta operations (not all, lot more in the actual engine). However meta classes can have specialized meta operations (static functions in the actual type of the meta class description). These provide a specilialized function instead of the default function in the Meta namespace. This is mainly useful for serialization functions, for the specific type to be read or written in a specific way. 

The main meta operation is `MetaOperation_SerializeAsync`. This serializes (reading and writing in this one function) the type to and from the contained metastream. The default version of this serialization function (in the Meta namespace) serializes sequentially all members in the meta class description one by one. For each member, if it has a specialized meta operation then this is called on that member, else recursively this same function is called again.

On the API side of things, to actually serialize a specific type (all types in are in the Types folder, most types have one file for it), you call (from Meta.hpp) `PerformMetaSerializeAsync` or `PerformMetaSerializeFull`. Most safe is to call serialize full. Call async if you are not using the library as a prebuilt static library, since its a template function. Otherwise, using serialize full you need to get the meta class description of the given type. This can be done by calling either `GetMetaClassDescription<T>` (again, a template), or via the suggested `TelltaleToolLib_FindMetaClassDescription(string, bynameorextension)`.

Pass in the name of the type you want to get, or the extension of the type, to this function. If searching by extension then set the second parameter to `false`, else if its by its type name then `true`. When calling the PerformMetaSerialize function, pass in a pointer to an instance of it and a pointer to the meta class. Also, important that you DO NOT delete any meta class or meta member descriptions. They are all in static memory.

Here is an example of how you would now read a .LANDB (LanguageDB). This same format continues for all other types.

```
#include "TelltaleToolLib.h"
#include "Meta.hpp"
#include "Types/LanguageDB.h"

//Assuming we are including all source code, so can use template functions safely
int MyExample(){
  MetaStream stream;
  LanguageDB db;
  stream.Open(_OpenDataStreamFromDisc("C:/Path/To/Input.landb", READ), MetaStreamMode::eMetaStream_Read, {0});
  //Implicity gets the meta class description for you, instead of PerformMetaSerializeFull(&stream, &db, GetMetaClassDescription<LanguageDB>()) or 
  //PerformMetaSerializeFull(&stream, &db, TelltaleToolLib_FindMetaClassDescription("landb", false))
  PerformMetaSerializeAsync<LanguageDB>(&stream, &db);
  //Now all data from the .landb is in the instance!
  
  //Now we can write it back to another file (deletes old stream)
  stream.SwitchToMode(MetaStreamMode::eMetaStream_Write,_OpenDataStreamFromDisc("C:/Path/To/Output.landb",WRITE));
  //Write the new .landb to the metastream
  PerformMetaSerializeAsync(&stream, &db);
  //Destructor calls Close(), so it will automatically flush at the end of this scope! Note that when calling Close(), after the call the mode is set to closed to
  //prevent it from being written again.
}
```