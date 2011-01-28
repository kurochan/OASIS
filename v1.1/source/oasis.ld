
/*出力フォーマット
"binary"でtext,data,bssだけのシンプルな構成になる。*/
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
SEARCH_DIR("../gcc/i386-elf/lib")


SECTIONS
{
	_kernel = 0x280000 ;
	_stack = 0x10000 ;

	.head 0x0:
	AT(0x0)
	{
		LONG((ADDR(.bss) + SIZEOF(.bss) + 0xfff) & ~ 0xfff)
		BYTE(0x48) BYTE(0x61) BYTE(0x72) BYTE(0x69)
		LONG(0x0)
		LONG(ADDR(.data))
		LONG(SIZEOF(.data)+SIZEOF(.bss))
		LONG(LOADADDR(.data))
		LONG(0xe9000000)
		LONG( _KernelStartup - 0x20 )
		LONG((ADDR(.bss) + SIZEOF(.bss) + 0xf) & ~ 0xf)
	}

	.text  :
	{
		*(.text*)
	}


	.ctors          :
	{
		/* gcc uses crtbegin.o to find the start of
			the constructors, so we make sure it is
			first.  Because this is a wildcard, it
			doesn't matter if the user does not
			actually link against crtbegin.o; the
			linker won't look for a file to match a
			wildcard.  The wildcard also means that it
			doesn't matter which directory crtbegin.o
			is in.  */
		KEEP (*crtbegin.o(.ctors))
		KEEP (*crtbegin?.o(.ctors))
		/* We don't want to include the .ctor section from
			the crtend.o file until after the sorted ctors.
			The .ctor section from the crtend file contains the
			end of ctors marker and it must be last */
		KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
	}

	.data ADDR(.ctors) + SIZEOF(.ctors) + _kernel + _stack :
	AT(ADDR(.ctors)+SIZEOF(.ctors))
	{
		*(.rodata*)
		*(.data*)
		SORT(CONSTRUCTORS)

	}

	.bss :
	{
		*(.bss*)
	}
	end = .;

	.comment 0 : { *(.comment)}

	/DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) }
}
