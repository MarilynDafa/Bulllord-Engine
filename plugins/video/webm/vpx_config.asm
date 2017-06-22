%ifidn __OUTPUT_FORMAT__,elf32
%include "vpx_config_32.asm"
%elifidn __OUTPUT_FORMAT__,elf64
%include "vpx_config_64.asm"
%elifidn __OUTPUT_FORMAT__,macho32
%include "vpx_config_32.asm"
%elifidn __OUTPUT_FORMAT__,macho64
%include "vpx_config_64.asm"
%elifidn __OUTPUT_FORMAT__,win32
%include "vpx_config_32.asm"
%elifidn __OUTPUT_FORMAT__,win64
%include "vpx_config_64.asm"
%elifidn __OUTPUT_FORMAT__,obj
%endif