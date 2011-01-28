/*
 * crt0.s
 *
 *
 * Created by Liva on 10/11/29.
 *
 */

 .global _KernelStartup
 .extern KernelMain
 .extern exit
 _KernelStartup:
 call KernelMain
 call exit
 .wait: hlt
 jmp .wait
