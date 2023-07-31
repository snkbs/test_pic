.syntax unified
.fpu softvfp
.thumb

.word _stack
.word _flash_begin
.word _flash_end
.word _vectors_rom_begin
.word _vectors_rom_end
.word _vectors_ram_begin
.word _vectors_ram_end
.word _got_rom_begin
.word _got_rom_end
.word _got_ram_begin
.word _got_ram_end
.word _data_rom_begin
.word _data_ram_begin
.word _data_ram_end
.word _bss_ram_end

.global reset_handler
.global startup_helper
.global main

/* overwrite reset_handler presented by libopencm3 */
.section  .text.reset_handler
.type reset_handler, %function
reset_handler:

	ldr sp, =_stack

	// check how we started (bootloader or standalone)
	// bootloader should write firmware offset to r9
	// r8 should contain ~offset
	mvn r8, r8
	cmp r8, r9
	beq check_end										// (~r8 == r9) that means reset called by bootloader 
	movs r9, #0											// write zero offset if not match (standalone mode)
check_end:
	movs r8, #0

	// store offset to fw_offset
	ldr r0, =fw_offset
	str r9, [r0]

	// store vectors start addres (in ram)
	ldr r0, =vectors_ram_addr
	ldr r1, =_vectors_ram_begin
	str r1, [r0]

	//
	// copy GOT to ram and add offset for each record
	//
	ldr r0, =_got_rom_begin
	ldr r1, =_got_rom_end
	ldr r2, =_got_ram_begin
	adds r0, r0, r9										// add offset to GOT start addr (in rom)
	adds r1, r1, r9										// add offset to GOT end addr (in rom)
got_load_start:
	cmp r0, r1
	bhs got_load_end
	ldr r3, [r0], #4									// load GOT record and step next src addr
	ldr r4, =_flash_begin
	cmp r3, r4
	blo got_load_write									// if value less than rom begin addr
	ldr r4, =_flash_end
	cmp r3, r4
	bhs got_load_write									// if value greater than rom end addr
	adds r3, r3, r9										// add offset to value
got_load_write:
	str r3, [r2], #4									// store value and step next dest addr
	b got_load_start
got_load_end:
	ldr r5, =_got_ram_begin
	movs r10, r5										// set address of GOT ('pic-register' compiler option)

	//
	// copy VTOR to ram and add offset for each isr vector
	//
	ldr r0, =_vectors_rom_begin
	ldr r1, =_vectors_rom_end
	ldr r2, =_vectors_ram_begin
	adds r0, r0, r9										// add offset to VTOR start addr (in rom)
	adds r1, r1, r9										// add offset to VTOR end addr (in rom)
vect_load_start:
	cmp r0, r1
	bhs vect_load_end
	ldr r3, [r0], #4									// load VTOR record and step next
	ldr r4, =_flash_begin
	cmp r3, r4
	blo vect_load_write									// if value less than rom begin addr
	ldr r4, =_flash_end
	cmp r3, r4
	bhs vect_load_write									// if value greater than rom end addr
	adds r3, r3, r9										// add offset to value
vect_load_write:
	str r3, [r2], #4									// store value and step next dest addr
	b vect_load_start
vect_load_end:
	
	//
	// copy .data segment and fill .bss segment with zeroes
	//
	movs r0, #0
	ldr r1, =_data_rom_begin
	ldr r2, =_data_ram_begin
	ldr r3, =_data_ram_end
	ldr r4, =_bss_ram_end
	adds r1, r1, r9										// add offset to .data start addr (in rom)
data_load_start:
	cmp r2, r3
	bhs bss_fill_zero
	ldr r5, [r1], #4									// load value and step next src addr
	str r5, [r2], #4									// store value and step next dest addr
	b data_load_start
bss_fill_zero:											// fill zeroes from _data_ram_end to _bss_ram_end
	cmp r2, r4
	bhs bss_fill_end
	str r0, [r2], #4
	b bss_fill_zero
bss_fill_end:

	bl startup_helper
	bl main

infinite_loop:
	b infinite_loop

.size reset_handler, .-reset_handler

