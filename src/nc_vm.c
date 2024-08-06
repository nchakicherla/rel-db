#include <stdio.h>

#include "nc_common.h"
#include "nc_debug.h"
#include "nc_vm.h"

VM vm;

void init_vm() {

}

void free_vm() {

}

Interpret_Result interpret(Chunk *chunk) {
	vm.chunk = chunk;
	vm.ip = vm.chunk->code;
	return run();
}

static Interpret_Result run() {
	#define READ_BYTE() (*vm.ip++)
	#define READ_CONSTANT() (vm.chunk->constant.values[READ_BYTE()])

	while (true) {

	#ifdef DEBUG_TRACE_EXECUTION
		disassemble_instruction(vm.chunk, (size_t)(vm.ip - vm.chunk->code));
	#endif

		uint8_t instruction;
		switch (instruction = READ_BYTE()) {
			case OP_CONSTANT: {
				Value constant = READ_CONSTANT();
				print_value(constant);
				printf("\n");
				break;	
			}
			case OP_RETURN: {
				return INTERPRET_OK;
			}
		}
	}
	#undef READ_BYTE
	#undef READ_CONSTANT
}