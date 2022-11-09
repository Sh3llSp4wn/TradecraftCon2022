---
title: Waiter there is a shellcode compiler in my linker!
author: Shellspawn
## 2022-06-28
...

# Shellcode Generation for Offensive Development
Presented at
TradecraftCon by K54

---

## `whoami`

```python
>>> import os
>>> os.uname()
posix.uname_result(
    sysname='Human',
    nodename='Luke Harding',
    release='26.172.99',
    version='1.0',
    machine=False)

```


* _Applied Threat Researcher_ @ *VMware Carbon Black*
 * My views and statements are my own. Standard legal stuff, etc etc.

* Prior _TSMO Red Team_ @ *Millenium Corp*

* _Journeyman Operator_ Course Beta Tester @ *K54*

* I also worked for the UNC Charlotte SoC for a bit...

---

-> Itinerary <-
===============

1. Shellcode Basics
2. Current Solutions
3. A quick aside
4. Compiler and Linker Internals
5. Advanced shellcode tradecraft
6. Questions, Comments, Concerns, and Cries of Outrage


---

-> Shellcode <-
===============

_Shellcode:_
* Forign *position independent code* that is executed in a target execution context. 
 * This is not limited to userspace processes!
* Generally *hand crafted*, lovingly, by your local exploit bakery.
 * Not always, as we will show in the subsequent slides.
* Used in exploits frequently as the *"Abritrary Code"* in *"Arbitrary Code Execution"*
* In modern Red Teaming: 
 * _Used to stage tools and capibilities into a remote process or execution context._


---

-> Challenges in Shellcode Creation <-
============

* Assembly is *hard* and *non-portable*
 * Anything beyond simple linear programs adds *signifigant complexity*
* Many tools provide "shellcode versions"
 * This is often a *simple loader* wrapped around *another binary*
 * This can have non-obvious side effects and requirements
  * i.e. *sRDI* and *Donut* must be on a `RWX` page for self staging or relocating

---


-> Current Technology <-
========================

* Shellcode is often a *simple loader* around a full binary (_large_).
* Large "pure shellcode" binaries are often incredibly specific 
(i.e. DoublePulsar or other exploit staged cabibilities).
 * *Expensive* to develop, *difficult* to debug, and requiring specific development 
 environments that may not be a good representation of a target system.
 * Gods help you if the target is not a *common architecture*.


What's a humble lil' red teamer to do?

---

-> Shellcode Compilers <-
=========================

"I'll write a compiler" 
~ an offensive developer, about a week before a mental breakdown

There are several open source and commercial options for "compiling shellcode" in a reasonable manner. 

1. _NytroRST/ShellcodeCompiler_
 1. *Pros:*
  1. It actually works!
  2. Access to system calls and Win32API functions
 2. *Cons:*
  1. No logic, just lists of desired calls
  2. Windows and Linux x86 and x64 only
2. _Binary Ninja_
 1. *Pros:*
  1. Actually takes C!
 2. *Cons:*
  1. Requires a Licence
  2. Only works for extreamly small examples
  3. Not a development priority for it's developers

---

-> Shellcode Compilers cont. <-
=========================
3. _pwntools_
 1. *Pros:*
  1. Massive library for offensive development in python
  2. Large number of architectires and OSs supported
 2. *Cons:*
  1. No logic, just chaining together of templates
  2. Relies on using an assembler to create an elf
  and then pulls the shellcode out of it.
  3. Admittedly buggy, but an amazing project


---

-> Woe and Dispair <-
=====================

What are we going to do about the state of shellcode development?

Maybe it should be *abandoned*?

What are we missing out on if we don't have better solutions to this problem?

_Assume we have a capibility that allows for easier complex shellcode creation_
* _Better stagers_
 * On target prelem checks for VM/Sandbox
 * Clean-up initial access vector
 * Stage the implant with cooler^H^H^H^H^H^H more threat representitive techniques
* _In-Process Capibilities_
 * _Hooks!_
  * Pull SSL keys from a target process and decrypt sessions with WireShark
  * Intercept Logs before they hit the file
  * Inject a webshell into a server process
 * Remote Exploit staged implants
 * Maybe an entire RAT written in shellcode?
* _Better exploits_
 * Steal socket 
 * Reverse Shell with encryption 
 * Something more complex than bin\_bash(`nc -e ...`)
 * Repair process as to not kill single threaded applications
* And much much more!


---

-> A quick aside on Bootloaders <-
========

Wait, what?

Wern't we just talking about *shellcode*, why *bootloaders*?


_Consider the following_

How is firmware written? Those often use one-off binary
formats. How are bootloaders designed?

The *Master Boot Record* (MBR) is written on the first block of a disk as plain
executable code. The BIOS's job is just to find a disk with the valid bootloader
signature and run the code. 

That is probably not written as an *ELF* or a *PE* and then stripped out right?

Well hopefully not. 

As it turns out this intuition is *correct*. Firmware designers and bootloader authors do not
use such, admittedly *crude*, methodologies. 

They use *Linker Scripts*.


---

-> Linker Scripts for Red Teams <-
========


_Linker Script:_
* A set of instructions provided to the *linker* so it knows 
how to create the requested binary
* Used to provide the linker with the following information
 * How to *order* the *sections*
 * How to designate *internal* vs *external symbols*
 * Where to store read-only *data*

Awesome! Does this mean I know how to write Linker Scripts?


---

no

---


-> Linker Scripts, plagerized <-
======

During the research for this I found the documentation 
for the GNU core utils linker `ld`. 

To say these docs are terse is a *vast* understatement.
I understand why this feature is not well known. 

The linker script I am about to show is an amalgomation of 
a linker script I found for a bootloader and a ESP32U...[something]...

I understand how it works. It technically uses a cursor and a set
of keywords that instruct the linker to create a binary where the 
`start_external` entrypoint is the first byte in the binary.

---


-> Breathe, we'll get through it together <-
==================

```
ENTRY(start_external)
INCLUDE formats/linkcmds.memory
SECTIONS
  {
    .text :
      {
        /*
         * Align on 1 may cause breakage.
         * SO, don't say I didn't warn you.
         */
        . = ALIGN(1);
        /*
         * hmm yes, the text segment address
         * is made out of text segment address
         */
        *(.text)
      } > REGION_TEXT
    .rodata :
      {
        *(.rodata)
       } > REGION_RODATA
   }
```

---

-> ??? <-
=========

```
MEMORY
  {
    RAM : ORIGIN = 0, LENGTH = 4M
  }
REGION_ALIAS("REGION_TEXT", RAM);
REGION_ALIAS("REGION_RODATA", RAM);
REGION_ALIAS("REGION_DATA", RAM);
REGION_ALIAS("REGION_BSS", RAM);

```


---

-> Linker Scripts, Cont. <-
=

As it turns out, if you have ever built anything with *GCC* 
you have used a *linker script*. There is a default script
included in standard invocations of GCC. 

A custom script can be provided with the `-T` argument 
(you know, T for Tcript...)


---

-> Now for our example <-
=========================

Given that we are writing this in "Pure C" we will need a few things. 

1. A custom entry point to replace *main*
2. A way to directly call *syscalls*
3. Probably a *Makefile* to keep everything straight

---

-> Custom Entry Point <-
========================

```assembly
.text
.intel_syntax noprefix
.extern _start
.global start_external
.equ SYS_exit, 60
start_external:
call _start
mov rax, SYS_exit
syscall
```

That's not too bad. Basically just create a *start_external*  symbol for the 
linker script and require a *\_start* be the entry point of our C code. 

---

-> Syscall Interface <-
=======================

Now we create a small interface for the syscalls we will use
```assembly
.text
.intel_syntax noprefix
.global _write
.global _exit
.equ SYS_write, 1
.equ SYS_exit, 60
# just load rax with the number and syscall
# and define the specified symbol as this syscall invocation
.macro direct_syscall syscall_number, symbol
\symbol:
mov rax, \syscall_number
syscall
ret
.endm
# define a symbol called _write calling the #1 syscall
direct_syscall SYS_write, _write
_exit:
mov rax, SYS_exit
syscall
```

---

-> Now the core of our project <-
=======

Now we write the payload in C

```c
static const char msg[] = "hello, friend\n";
extern int _write(int fd, char* buff, int len);
int _start(){
    _write(0, msg, 15);
    return 0;
}
```

---

-> Compile it <-
================

```bash
gcc -c hello.c
as -o start.o start.s
as -o syscalls.o syscalls.s
ld -o hello.bin start.o syscalls.o hello.o -T shellcode.ld
./tools/runsc hello.bin
hello, friend
```

---

-> what just happened? <-
=========================

We just created shellcode using only *gcc*, *as*, and *ld*. 

This process at first appears complex, but... it scales well. 
There is very little difference in the level of difficulty of this
*hello world* example and a *fully featured loader* that creates
shellcode for all architectures that you can get a cross compiler for.

That means, with just a single *start.s* and *syscalls.s* file per architecure 
you can write shellcode in C that will compile on all platforms.

Yeah, that's nice, but what about windows?

---

-> calling library code <-
=========================

Gotta give it to *Windows* this time actually...

That's where this gets really interesting. This technique, as long as you
can resolve *Win32API* calls, can use GCC to *compile shellcode for windows*. 

The process is a little bit more complicated, but once the Win32API loader
is implemented, this technique *just works*. The details and an implementation 
of this will by in my blog at *shellspawn.github.io* after this talk.

I will also be publishing a dlsym based locator for POSIX based systems as well
once I figure out if the thread local storage negative offsets work "portably"
(This is needed to use *malloc* and friends on a linux system.)

```c
GetProcessHeap_t get_proc_heap_fp = locate_fp("GetProcessHeap");
HeapAlloc_t heap_alloc_fp = locate_fp("HeapAlloc");
...
unsigned char *data = heap_alloc_fp(get_proc_heap_fp(), 0, 1337);
```

---

-> Benefits of this technique <-
======

* Ability to use *known headers* to access data structures in programs
(this is good for modifying arbitrary data in a target process)
* Able to use the entirety of C (without stdlib that is)
* Able to use reverse engineered structures to cleanly modify target process state
* Unions
* Complex logic such as switch statements, loops, and recursion


---

-> Limits of this technique <-
======

* No libraries, unless they can be staticly built... but then the shellcode is huge.
* No `main` (argument parsing) - why you'd need that in shellcode, however...

---

-> Hooks <-
======

* A hook is an interupt in *code flow* for the purpose of modifying the behavior of a function. 
  * Hooks can return control back to the function with modified arguments or return to the end of the function with a modified return value.
* There are a few techniques for implementing this technique
  * I made these terms up, so YMMV

* 1. `Pure Hooks`
* 2. `Breakpoint Hooks`
* 3. `Hybrid Hooks`

---

-> 1. Pure Hooks <-
======

* Pure hooks do not require a controller process.
* They are defined by all of the hook code *existing in the target process*.

* In-Line function hooks
  * return from
  * call back to
* SO hijacking/preload

---

-> 2. Breakpoint Hooks <-
======

* The controller proceess injects *breakpoints* into the start of the hooked functions
* Once the breakpoint is hit, the controller examines the *R/EIP* to detect which hook fired
* Controller process has full control on the *target process* execution flow and registers

* Blackbone <- Windows Memory hacking library
* break at call
* break at return

---

-> 3. Hybrid Hooks <-
======

* Breakpoint hooks that rely on code *injected into the target process*

* OK I lied in the last slide, *blackbone* goes here.
* Controller can redirect execution flow to injected code dynamically

---

-> Real World Uses for Hooks <-
======

* Personal Example:
  * Hooking *allocation* and *execution* primatives to dump "memory modules" from Target Toolkits
  * Catching *credentials* sent to an SSH server
* Other Real World Examples
  * Speed Run clocks
  * Aim Bots
  * X-Ray Hacks
* Red Team Examples
  * ???

---

-> That is up to you all <-
======

In Summary, we discussed *shellcoding* and advanced use cases for shellcode in an *offensive context*.

The rest is up to your creativity and how far you are willing to dig into the targets for your mission. 

---

-> Questions, Comments, Concerns? <-
======

Thank you for your time!

---
